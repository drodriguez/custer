#ifndef SELECT_DISPATCHER_H
#define SELECT_DISPATCHER_H

#include "custer.h"
#include "IDispatcher.h"

#include <boost/shared_ptr.hpp>

#include <map>
#include <sys/select.h>

NS_CUSTER_BEGIN

/**
 * Dispatcher nativo para sistemas BSD (y Mac OS X). Utiliza kqueue como
 * sistema de E/S no bloqueante.
 */
class SelectDispatcher : public IDispatcher
{
public:	
	/** Constructor */
	SelectDispatcher();
	
	/** Destructor */
	virtual ~SelectDispatcher();
	
	// Heredada de IDispatcher
	void registerHandler(boost::shared_ptr<EventHandler> eh, unsigned int et);
	
	// Heredada de IDispatcher
	void removeHandler(boost::shared_ptr<EventHandler> eh, unsigned int et);
	
	// Heredada de IDispatcher
	void handleEvents(long timeout = -1);
private:
	typedef std::pair<boost::shared_ptr<EventHandler>, unsigned int>
		EventHandlerPair;
	typedef std::map<EventHandler*, EventHandlerPair> EventHandlerMap;
	
	/**
	 * Almacenamos los shared_ptr para que los punteros no sean destruidos.
	 */
	EventHandlerMap m_eventHandlerMap;
	
	/**
	 * Bitfield para sockets que serán observados para lecturas.
	 */
	fd_set m_readSockets;
	
	/**
	 * Bitfield para sockets que serán observados para lecturas (copia).
	 */
	fd_set m_readSocketsCopy;
	
	/**
	 * Bitfield para sockets que serán observador para escrituras.
	 */
	fd_set m_writeSockets;
	
	/**
	 * Bitfield para sockets que serán observados para escrituras (copia).
	 */
	fd_set m_writeSocketsCopy;
	
#ifndef WIN32
	/*
	 * Windows no hace caso del parámetro nfds de select(), a pesar de que en
	 * otras plataformas se utilizan otros dispatchers, por compatibilidad
	 * implemento como debería funcionar.
	 */
	/**
	 * Conjunto de los descriptores del ficheros utilizados.
	 */
	std::set<socket_type> m_sockets;
#endif
};

typedef SelectDispatcher NativeDispatcher;

NS_CUSTER_END

#endif