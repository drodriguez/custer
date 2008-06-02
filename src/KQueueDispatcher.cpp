#include "KQueueDispatcher.h"
#include "EventHandler.h"
#include "utils.h"

#include <sys/types.h>
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
	int result = 0;
	
	if (et == NO_EVENT)    return 0;
	if (et & ACCEPT_EVENT) result |= EVFILT_READ;
	if (et & READ_EVENT)   result |= EVFILT_READ;
	if (et & WRITE_EVENT)  result |= EVFILT_WRITE;
	if (et & CLOSE_EVENT)  result |= EVFILT_WRITE;
	
	return result;
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

void KQueueDispatcher::registerHandler
	(boost::shared_ptr<EventHandler> eh, unsigned int et)
{
	debug("KQueueDispatcher::registerHandler");
	
	// Si no se piden eventos no lo añadimos.
	if (et == NO_EVENT) return;
	
	// Si el vector de kevents está lleno...
	if (m_keventUsed == m_keventSize) {
		m_keventSize <<= 1; // ...doblamos su capacidad.
		m_keventChanges = (struct kevent*) xrealloc(
			m_keventChanges, m_keventSize * sizeof(struct kevent)
		);
		m_keventList = (struct kevent*) xrealloc(
			m_keventList, m_keventSize * sizeof(struct kevent)
		);
	}
	
	// Tomamos el siguiente kevent libre
	struct kevent* newKevent = &m_keventChanges[m_keventUsed++];
	
	// y lo rellenamos
	newKevent->ident = eh->getHandle();
	newKevent->filter = translateEvents(et);	
	newKevent->flags = EV_ADD;
	newKevent->fflags = 0;
	newKevent->data = 0;
	newKevent->udata = eh.get();
	
	m_eventHandlerMap.insert(std::make_pair(eh.get(), std::make_pair(eh, et)));
}

void KQueueDispatcher::removeHandler
	(boost::shared_ptr<EventHandler> eh, unsigned int et)
{
	debug("KQueueDispatcher::removeHandler");
	
	EventHandlerPair ehp = m_eventHandlerMap[(EventHandler*) eh.get()];
	
	// Primero borramos por completo el antiguo
	struct kevent* oldKevent = &m_keventChanges[m_keventUsed++];
	oldKevent->ident = ehp.first->getHandle();
	oldKevent->filter = translateEvents(ehp.second);
	oldKevent->flags = EV_DELETE;
	oldKevent->fflags = 0;
	oldKevent->data = 0;
	oldKevent->udata = NULL;
	
	// Calculamos el resultado de los eventos antiguos y los nuevos
	unsigned int newEventTypes =  ehp.second & !et;
	
	// Si el resultado acaba siendo ningún evento, eliminados el EventHandler
	if (newEventTypes == NO_EVENT) {
		m_eventHandlerMap.erase(eh.get());
		return;
	}
	
	// Añadimos un nuevo kevent
	struct kevent* newKevent = &m_keventChanges[m_keventUsed++];
	newKevent->ident = eh->getHandle();
	newKevent->filter = translateEvents(newEventTypes);
	newKevent->flags = EV_ADD;
	newKevent->fflags = 0;
	newKevent->data = 0;
	newKevent->udata = eh.get();
	
	m_eventHandlerMap[eh.get()] = std::make_pair(eh, newEventTypes);
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
		}
		
		EventHandlerPair ehp =
			m_eventHandlerMap[(EventHandler*) activeKevent->udata];
		
		if (activeKevent->filter & EVFILT_READ) {
			struct sockaddr remoteAddress;
			socklen_t remoteAddressSize = sizeof(struct sockaddr);
			memset(&remoteAddress, 0, remoteAddressSize);
			
			// Comprobamos si el socket está escuchando
			if (getpeername(
					activeKevent->ident,
					&remoteAddress,
					&remoteAddressSize) == -1) {
				if (ehp.second & ACCEPT_EVENT) {
					ehp.first->handleAccept(self);
				}
			} else if (ehp.second & READ_EVENT) {
				ehp.first->handleRead(self);
			}
		}
		
		if (activeKevent->filter & EVFILT_WRITE &&
			ehp.second & WRITE_EVENT) {
			ehp.first->handleWrite(self);
		}
		
		if (activeKevent->flags & EV_EOF &&
			ehp.second & CLOSE_EVENT) {
			ehp.first->handleClose(self);
		}
	}
}
