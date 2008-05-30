#include "KQueueDispatcher.h"

#include <sys/types.h>
#include <sys/event.h>
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
int translateEvents(EventType et) {
	int result = 0;
	
	if (et == NO_EVENT)    return 0;
	if (et & ACCEPT_EVENT) result &= EVFILT_READ;
	if (et & READ_EVENT)   result &= EVFILT_READ;
	if (et & WRITE_EVENT)  result &= EVFILT_WRITE;
	if (et & CLOSE_EVENT)  result &= EVFILT_WRITE;
	
	return result;
}

KQueueDispatcher::KQueueDispatcher() :
	m_eventHandlersMap(),
	m_keventArraySize(64), // Inicialmente suponemos 64 descriptores
	m_keventArrayUsed(0)
{
	if ((m_kqueue = kqueue()) == -1)
		fatal("Error creando kqueue: %s", strerror(errno));
	
	// Creamos el vector de kevents.
	m_keventArray =
		(struct kevent*) xmalloc(m_keventArraySize * sizeof(struct kevent));
}

KQueueDispatcher::~KQueueDispatcher()
{
	free(m_keventArray);
	close(m_kqueue);
}

void KQueueDispatcher::registerHandler
	(boost::shared_ptr<EventHandler> eh, EventType et)
{
	// Si no se piden eventos no lo añadimos.
	if (et == NO_EVENT) return;
	
	// Si el vector de kevents está lleno...
	if (m_keventArrayUsed == m_keventArraySize) {
		m_keventArraySize <<= 1; // ...doblamos su capacidad.
		m_keventArray = (struct kevent*) xrealloc(
			m_keventArray, m_keventArraySize * sizeof(struct kevent)
		);
	}
	
	// Tomamos el siguiente kevent libre
	struct kevent* newKevent = &m_keventArray[m_keventArrayUsed++];
	
	// y lo rellenamos
	newKevent->ident = eh->getHandle();
	newKevent->filter = translateEvents(et);	
	newKevent->flags = EV_ADD;
	newKevent->fflags = 0;
	newKevent->data = 0;
	newKevent->udata = eh.get();
	
	m_eventHandlersMap.insert(std::make_pair(eh.get(), std::make_pair(eh, et)));
}

void KQueueDispatcher::removeHandler
	(boost::shared_ptr<EventHandler> eh, EventType et)
{
	std::pair<boost::shared_ptr<EventHandler>, EventType> ehp =
		m_eventHandlersMap[(EventHandler*) eh.get()];
	
	// Primero borramos por completo el antiguo
	struct kevent* oldKevent = &m_keventArray[m_keventArrayUsed++];
	oldKevent->ident = ehp.first->getHandle();
	oldKevent->filter = 0;
	oldKevent->flags = EV_DELETE;
	oldKevent->fflags = 0;
	oldKevent->data = 0;
	oldKevent->udata = NULL;
	
	// Calculamos el resultado de los eventos antiguos y los nuevos
	EventType newEventTypes =  static_cast<EventType>(ehp.second & !et);
	
	// Si el resultado acaba siendo ningún evento, eliminados el EventHandler
	if (newEventTypes == NO_EVENT) {
		m_eventHandlersMap.erase(eh.get());
		return;
	}
	
	// Añadimos un nuevo kevent
	struct kevent* newKevent = &m_keventArray[m_keventArrayUsed++];
	newKevent->ident = eh->getHandle();
	newKevent->filter = translateEvents(newEventTypes);
	newKevent->flags = EV_ADD;
	newKevent->fflags = 0;
	newKevent->data = 0;
	newKevent->udata = eh.get();
	
	m_eventHandlersMap[eh.get()] = std::make_pair(eh, newEventTypes);
}

void KQueueDispatcher::handleEvents(long timeout)
{
	int numEvents;
	struct kevent* activeKevent;
	struct timespec keventTimeout;
	
	if (timeout != -1) {
		long secs = timeout / 1000;
		long msecs = timeout % 1000;
		
		keventTimeout.tv_sec = secs;
		keventTimeout.tv_nsec = msecs * 1000000;
	}
	
	numEvents = kevent(
		m_kqueue,
		m_keventArray,
		m_keventArrayUsed,
		m_keventArray,
		m_keventArraySize,
		timeout != -1 ? &keventTimeout : NULL
	);
	
	if (numEvents == -1)
		fatal("Error en kevent(): %s", strerror(errno));
	if (numEvents == 0)
		debug("No se han recibido eventos");
	
	for (activeKevent = m_keventArray;
		activeKevent < &m_keventArray[numEvents];
		activeKevent++) {
		std::pair<boost::shared_ptr<EventHandler>, EventType> ehp =
			m_eventHandlersMap[(EventHandler*) activeKevent->udata];
		// TODO: enviar el evento correcto
	}
}
