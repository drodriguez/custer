#include "KQueueDispatcher.h"
#include "EventHandler.h"
#include "utils.h"

#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <cstdio>
#include <cerrno>

using namespace custer;

/**
 * Traduce nuestros eventos a los posibles eventos disponibles en KQueue.
 *
 * @param et Los eventos a capturar.
 * @return Los eventos equivalentes en KQueue.
 */
int translateEvents(unsigned int et) {
	if (et == NO_EVENT)    return 0;
	if (et & ACCEPT_EVENT) return EVFILT_READ;
	if (et & READ_EVENT)   return EVFILT_READ;
	if (et & WRITE_EVENT)  return EVFILT_WRITE;
	if (et & CLOSE_EVENT)  return EVFILT_WRITE;
}

KQueueDispatcher::KQueueDispatcher() :
	m_eventHandlerMap(),
	m_keventSize(64), // Inicialmente suponemos 64 descriptores
	m_keventUsed(0)
{
	if ((m_kqueue = kqueue()) == -1)
		fatal("Error creando kqueue: %s", strerror(errno));
	
	// Creamos los vectores de kevents.
	m_keventChanges =
		(struct kevent*) xmalloc(m_keventSize * sizeof(struct kevent));
	m_keventList =
		(struct kevent*) xmalloc(m_keventSize * sizeof(struct kevent));
}

KQueueDispatcher::~KQueueDispatcher()
{
	free(m_keventChanges);
	free(m_keventList);
	close(m_kqueue);
}

void KQueueDispatcher::resizeKeventsVectors()
{
	// Si el vector de kevents está lleno...
	if (m_keventUsed == m_keventSize) {
		debug("Doblando el tamanio de los vectores a %d", m_keventSize << 1);
		m_keventSize <<= 1; // ...doblamos su capacidad.
		m_keventChanges = (struct kevent*) xrealloc(
			m_keventChanges, m_keventSize * sizeof(struct kevent)
		);
		m_keventList = (struct kevent*) xrealloc(
			m_keventList, m_keventSize * sizeof(struct kevent)
		);
	}
}

void KQueueDispatcher::registerHandler
	(boost::shared_ptr<EventHandler> eh, unsigned int et)
{
	debug("KQueueDispatcher::registerHandler");
	
	// Si no se piden eventos no lo añadimos.
	if (et == NO_EVENT) return;
	
	resizeKeventsVectors();
			
	modifyKevents(et, eh->getHandle(), EV_ADD, eh.get());
	
	EventHandlerMap::iterator iter;
	if ((iter = m_eventHandlerMap.find(eh.get())) != m_eventHandlerMap.end()) {
		int newEt = iter->second.second | et;
		debug("actualizando antiguo: %x, %x, %x", iter->second.second, et, newEt);
		m_eventHandlerMap[eh.get()] = std::make_pair(eh, newEt);
	} else {
		m_eventHandlerMap[eh.get()] = std::make_pair(eh, et);
	}
}

void KQueueDispatcher::modifyKevents(
	unsigned int et,
	uintptr_t ident,
	u_short flags,
	void* udata)
{
	struct kevent* newKevent;
	
	while (et != NO_EVENT) {
		// Tomamos el siguiente kevent libre
		newKevent = &m_keventChanges[m_keventUsed++];
	
		// y lo rellenamos
		newKevent->ident = ident;
		newKevent->flags = flags;
		newKevent->fflags = 0;
		newKevent->data = 0;
		newKevent->udata = udata;
		
		/*
		 * En cada uno eliminamos CLOSE_EVENT porque viene implicito con
		 * EVFILT_READ o EVFILT_WRITE. Si es el único lo incluimos con
		 * EVFILT_WRITE (no se avisará de escrituras si el EventHandler no
		 * está suscrito a WRITE_EVENT).
		 */
		if (et & ACCEPT_EVENT) {
			newKevent->filter = EVFILT_READ;
			et &= !(ACCEPT_EVENT | CLOSE_EVENT);
		} else if (et & READ_EVENT) {
			newKevent->filter = EVFILT_READ;
			et &= !(READ_EVENT | CLOSE_EVENT);
		} else if (et & WRITE_EVENT) {
			newKevent->filter = EVFILT_WRITE;
			et &= !(WRITE_EVENT | CLOSE_EVENT);
		} else if (et & CLOSE_EVENT) {
			newKevent->filter = EVFILT_WRITE;
			et &= !CLOSE_EVENT;
		} else {
			error("Evento desconocido");
			newKevent->filter = 0;
			et = NO_EVENT;
		}
	}
}

void KQueueDispatcher::removeHandler
	(boost::shared_ptr<EventHandler> eh, unsigned int et)
{
	debug("KQueueDispatcher::removeHandler");
	
	if (et == NO_EVENT) return;
	if (m_eventHandlerMap.find(eh.get()) == m_eventHandlerMap.end()) {
		error("Intento de eliminar un EventHandler no existente");
		return;
	}
	
	// Si nos piden todos suponenmos que van a cerrar el socket ellos
	if (et == ALL_EVENTS) {
		m_eventHandlerMap.erase(eh.get());
		return;
	}
	
	resizeKeventsVectors();
	
	EventHandlerPair ehp = m_eventHandlerMap[eh.get()];
	
	modifyKevents(et, eh->getHandle(), EV_DELETE, NULL);
	
	// Calculamos el resultado de los eventos antiguos y los nuevos
	unsigned int newEventTypes =  ehp.second & !et;
	
	// Si el resultado acaba siendo ningún evento, eliminados el EventHandler
	if (newEventTypes == NO_EVENT) {
		m_eventHandlerMap.erase(eh.get());
		return;
	}
	
	m_eventHandlerMap[eh.get()] = std::make_pair(eh, newEventTypes);
}

void debugKevents(struct kevent* k, int n)
{
	struct kevent* ki;
	
	for (ki = k; ki < &k[n]; ki++) {
		debug("kevent: ident %d, filter %x, flags %x, fflags %x, data %d, udata %p",
			ki->ident, ki->filter, ki->flags, ki->fflags, ki->data, ki->udata);
	}
}

void KQueueDispatcher::handleEvents(long timeout)
{
	int numEvents;
	struct kevent* activeKevent;
	struct timespec keventTimeout;
	
	debug("KQueueDispatcher::handleEvents");
		
	// Convertimos el timeout al formato del sistema operativo
	if (timeout != -1) {
		long secs = timeout / 1000;
		long msecs = timeout % 1000;
		
		keventTimeout.tv_sec = secs;
		keventTimeout.tv_nsec = msecs * 1000000;
	}
	
	// debug("Pre-kevent");
	// debugKevents(m_keventChanges, m_keventUsed);
	
	/* Pedimos a KQueue que actualice la lista de eventos y que nos devuelva los
	   que se hallan generado durante el timeout */
	numEvents = kevent(
		m_kqueue,
		m_keventChanges,
		m_keventUsed,
		m_keventList,
		m_keventSize,
		timeout != -1 ? &keventTimeout : NULL
	);
	m_keventUsed = 0;
	
	// debug("Post-kevent");
	// debugKevents(m_keventList, numEvents);
		
	if (numEvents == -1)
		fatal("Error en kevent(): %s", strerror(errno));
	if (numEvents == 0) {
		debug("No se han recibido eventos, timeout: %d", timeout);
		/* TODO: No definimos el evento de timeout
		EventHandlerMap::iterator iter;
		for (iter = m_eventHandlerMap.begin();
			iter != m_eventHandlerMap.end();
			++iter) {
			if (iter->second->second & TIMEOUT_EVENT) {
				iter->second->first->handleTimeout();
			}
		}
		/**/
	}
	
	boost::shared_ptr<IDispatcher> self(shared_from_this());
	
	for (activeKevent = m_keventList;
		activeKevent < &m_keventList[numEvents];
		activeKevent++) {
		
		if (activeKevent->flags == EV_ERROR) {
			error("en kevent: %s", strerror(activeKevent->data));
			continue;
		}
		
		// debug("a ver que podemos hacer...");
		
		EventHandlerPair ehp =
			m_eventHandlerMap[(EventHandler*) activeKevent->udata];
		
		if (activeKevent->flags & EV_EOF &&
			ehp.second & CLOSE_EVENT) {
			ehp.first->handleClose(self);
			continue;
		}
		
		if (activeKevent->filter == EVFILT_READ) {
			// debug("Parece que podemos leer...");
			struct sockaddr remoteAddress;
			socklen_t remoteAddressSize = sizeof(struct sockaddr);
			memset(&remoteAddress, 0, remoteAddressSize);
			
			// Comprobamos si el socket está escuchando
			// debug("!! %x & %x = %x", ehp.second, READ_EVENT, ehp.second & READ_EVENT);
			if (getpeername(
					activeKevent->ident,
					&remoteAddress,
					&remoteAddressSize) == -1) {
				// debug("%x & %x = %x", ehp.second, ACCEPT_EVENT, ehp.second & ACCEPT_EVENT);
				if (ehp.second & ACCEPT_EVENT) {
					// debug("Hemos aceptado");
					ehp.first->handleAccept(self);
					continue;
				}
			} else if (ehp.second & READ_EVENT) {
				// debug("Hemos leido");
				ehp.first->handleRead(self);
				continue;
			}
		}
		
		// debug("activeKevent->filter == EVFILT_WRITE :: %d == %d", activeKevent->filter, EVFILT_WRITE);
		// debug("ehp.second & WRITE_EVENT :: %x & %x = %x", ehp.second, WRITE_EVENT, ehp.second & WRITE_EVENT);
		if (activeKevent->filter == EVFILT_WRITE &&
			ehp.second & WRITE_EVENT) {
			debug("se pueden escribir %d bytes", activeKevent->data);
			ehp.first->handleWrite(self);
			continue;
		}
		fatal("No hemos podido hacer nada?");
	}
}
