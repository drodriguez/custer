#include "KQueueDispatcher.h"

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <cstdio>
#include <cerrno>

using namespace custer;

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
	// FIXME: newKevent->ident = eh->getHandle();
	newKevent->filter =
		et & (ACCEPT_EVENT | READ_EVENT) ? EVFILT_READ : EVFILT_WRITE;	
	newKevent->flags = EV_ADD;
	newKevent->fflags = 0;
	newKevent->data = 0;
	newKevent->udata = eh.get();
	
	m_eventHandlersMap.insert(std::make_pair(eh.get(), std::make_pair(eh, et)));
}

void KQueueDispatcher::removeHandler
	(boost::shared_ptr<EventHandler> eh, EventType et)
{
	// TODO
}

void KQueueDispatcher::handleEvents(long timeout)
{
	int numEvents;
	struct kevent* activeKevent;
	
	numEvents = kevent(
		m_kqueue,
		m_keventArray,
		m_keventArrayUsed,
		m_keventArray,
		m_keventArraySize,
		NULL // FIXME: Calcular correctamente el timeout
	);
	
	if (numEvents == -1)
		fatal("Error en kevent(): %s", strerror(errno));
	if (numEvents == 0)
		debug("No se han recibido eventos");
	
	for (activeKevent = m_keventArray;
		activeKevent < &m_keventArray[numEvents];
		activeKevent++) {
		std::pair<boost::shared_ptr<EventHandler>, int> ehp =
			m_eventHandlersMap[(EventHandler*) activeKevent->udata];
		// TODO: enviar el evento correcto
	}
}
