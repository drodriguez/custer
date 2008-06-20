#include "SelectDispatcher.h"
#include "EventHandler.h"
#include "utils.h"

#include <queue>

using namespace custer;

SelectDispatcher::SelectDispatcher() :
	m_eventHandlerMap()
#ifndef WIN32
	// Cuidado con esta "comita" de delante.
	,m_sockets()
#endif
{
	FD_ZERO(&m_readSockets);
	FD_ZERO(&m_readSocketsCopy);
	FD_ZERO(&m_writeSockets);
	FD_ZERO(&m_writeSocketsCopy);
}

SelectDispatcher::~SelectDispatcher()
{
	// Nothing
}

void SelectDispatcher::registerHandler
	(boost::shared_ptr<EventHandler> eh, unsigned int et)
{
	debug("SelectDispatcher::registerHandler");
	
	// Si no se piden eventos no lo añadimos.
	if (et == NO_EVENT) return;
	
	socket_type sckt = eh->getHandle();
	
#ifndef WIN32
	// Lo incluimos en el conjunto de sockets
	m_sockets.insert(sckt);
#endif
	
	// Iteramos por todos los eventos añadiendo el socket a las listas
	// adecuadas.
	unsigned int tempEt = et;
	while (tempEt != NO_EVENT) {
		if (tempEt & ACCEPT_EVENT) {
			FD_SET(sckt, &m_readSockets);
			tempEt &= ~ACCEPT_EVENT;
		} else if (tempEt & READ_EVENT) {
			FD_SET(sckt, &m_readSockets);
			tempEt &= ~READ_EVENT;
		} else if (tempEt & WRITE_EVENT) {
			FD_SET(sckt, &m_writeSockets);
			tempEt &= ~WRITE_EVENT;
		} else if (tempEt & CLOSE_EVENT) {
			FD_SET(sckt, &m_readSockets);
			tempEt &= ~CLOSE_EVENT;
		} else {
			error("Evento desconocido");
			tempEt = NO_EVENT;
		}
	}
		
	EventHandlerMap::iterator iter;
	if ((iter = m_eventHandlerMap.find(eh.get())) != m_eventHandlerMap.end()) {
		int newEt = iter->second.second | et;
		debug("actualizando antiguo: %x, %x, %x",
			iter->second.second, et, newEt);
		m_eventHandlerMap[eh.get()] = std::make_pair(eh, newEt);
	} else {
		m_eventHandlerMap[eh.get()] = std::make_pair(eh, et);
	}
}

void SelectDispatcher::removeHandler
	(boost::shared_ptr<EventHandler> eh, unsigned int et)
{
	debug("SelectDispatcher::removeHandler");
	
	if (et == NO_EVENT) return;
	if (m_eventHandlerMap.find(eh.get()) == m_eventHandlerMap.end()) {
		error("Intento de eliminar un EventHandler no existente");
		return;
	}
			
	EventHandlerPair ehp = m_eventHandlerMap[eh.get()];
	
	// Eliminamos el socket de las listas que se indiquen.
	socket_type sckt = eh->getHandle();
	unsigned int tempEt = et;
	while (tempEt != NO_EVENT) {
		if (tempEt & ACCEPT_EVENT) {
			FD_CLR(sckt, &m_readSockets);
			tempEt &= ~ACCEPT_EVENT;
		} else if (tempEt & READ_EVENT) {
			FD_CLR(sckt, &m_readSockets);
			tempEt &= ~READ_EVENT;
		} else if (tempEt & WRITE_EVENT) {
			FD_CLR(sckt, &m_writeSockets);
			tempEt &= ~WRITE_EVENT;
		} else if (tempEt & CLOSE_EVENT) {
			FD_CLR(sckt, &m_readSockets);
			tempEt &= ~CLOSE_EVENT;
		} else {
			error("Evento desconocido");
			tempEt = NO_EVENT;
		}
	}

#ifndef WIN32
	// Lo eliminamos del conjunto de sockets
	m_sockets.erase(sckt);
#endif
	
	// Calculamos el resultado de los eventos antiguos y los nuevos
	unsigned int newEventTypes =  ehp.second & ~et;
	
	// Si el resultado acaba siendo ningún evento, eliminados el EventHandler
	if (newEventTypes == NO_EVENT) {
		m_eventHandlerMap.erase(eh.get());
		return;
	}
	
	m_eventHandlerMap[eh.get()] = std::make_pair(eh, newEventTypes);
}

void SelectDispatcher::handleEvents(long timeout)
{
	debug("SelectDispatcher::handleEvents");
	
	struct timeval selectTimeout;	
	if (timeout != -1) {
		long secs = timeout / 1000;
		long msecs = timeout % 1000;
		
		selectTimeout.tv_sec = secs;
		selectTimeout.tv_usec = msecs * 1000;
	}
	
	FD_COPY(&m_readSockets, &m_readSocketsCopy);
	FD_COPY(&m_writeSockets, &m_writeSocketsCopy);
	
	/*
	 * Un std::set tiene sus elementos ordenados de menor a mayor, por lo
	 * tanto rbegin apunta al mayor elemento del std::set.
	 */
	int readySockets = select(
#ifndef WIN32
		*m_sockets.rbegin()+1,
#else
		0, // Windows ignora este parámetro
#endif
		&m_readSocketsCopy,
		&m_writeSocketsCopy,
		NULL,
		timeout == -1 ? NULL : &selectTimeout);
	
	if (readySockets == SOCKET_ERROR)
		fatal("Error en select(): %s", strerror(ERROR_NUM));
	if (readySockets == 0) {
		debug("No se han recibido eventos, timeout: %d", timeout);
	}
	
	boost::shared_ptr<IDispatcher> self(shared_from_this());

	// Tenemos que copiar los EventHandler.
	/*
	 * En NO-Win32 sería más facil empezar en socket = 0 y terminar en el
	 * máximo socket que le hemos pasado a select().
	 */
	std::queue<EventHandlerPair> handlerQueue;
	EventHandlerMap::iterator iter;
	for (iter = m_eventHandlerMap.begin();
		iter != m_eventHandlerMap.end();
		++iter) {
		handlerQueue.push(iter->second);
	}
	
	while (readySockets > 0 && !handlerQueue.empty()) {
		EventHandlerPair actualHandler = handlerQueue.front();
		socket_type sckt = actualHandler.first->getHandle();
		
		struct sockaddr remoteAddress;
		socklen_t remoteAddressSize = sizeof(struct sockaddr);
		memset(&remoteAddress, 0, remoteAddressSize);
		bool socketConnected = getpeername(
			sckt, &remoteAddress, &remoteAddressSize) != SOCKET_ERROR;
		
		// Comprobamos si se ha cerrado el socket
		//*
		if (socketConnected && FD_ISSET(sckt, &m_readSocketsCopy)) {
			char test;
			int result = recv(sckt, &test, sizeof(test), MSG_PEEK);
			if (result == SOCKET_ERROR) {
				debug("Parece que el socket ha desaparecido: %s",
					strerror(ERROR_NUM));
				actualHandler.first->handleClose(self);
				// Repetido xq avanzamos sin mirar más este socket.
				readySockets--;
				handlerQueue.pop();
				continue;
			}
		}
		/**/
		
		if (FD_ISSET(sckt, &m_writeSocketsCopy) &&
			actualHandler.second & WRITE_EVENT) {
			debug("El socket esta en la lista de escritura");
			
			actualHandler.first->handleWrite(self);
			
			readySockets--;
		}
		
		if (FD_ISSET(sckt, &m_readSocketsCopy)) {
			debug("El socket esta en la lista de lectura");
						
			// Comprobamos si el socket está escuchando
			if (!socketConnected &&
				actualHandler.second & ACCEPT_EVENT) {
				actualHandler.first->handleAccept(self);
			} else if (actualHandler.second & READ_EVENT) {
				actualHandler.first->handleRead(self);
			}
			
			readySockets--;
		}
				
		handlerQueue.pop();
	}
}
