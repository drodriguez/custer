#ifndef KQUEUE_DISPATCHER_H
#define KQUEUE_DISPATCHER_H

#include "custer.h"
#include "IDispatcher.h"

#include <boost/shared_ptr.hpp>

#include <map>

NS_CUSTER_BEGIN

/**
 * Dispatcher nativo para sistemas BSD (y Mac OS X). Utiliza kqueue como
 * sistema de E/S no bloqueante.
 */
class KQueueDispatcher : public IDispatcher
{
public:	
	/** Constructor */
	KQueueDispatcher();
	
	/** Destructor */
	virtual ~KQueueDispatcher();
	
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
	
	/** Vector de kevents por actualizar */
	struct kevent* m_keventChanges;
	
	/** Vector de kevents con eventos */
	struct kevent* m_keventList;
	
	/** Total de kevents disponibles */
	unsigned int m_keventSize;
	
	/** kevents utilizados */
	unsigned int m_keventUsed;
	
	/** Descriptor del kqueue */
	int m_kqueue;
};

typedef KQueueDispatcher NativeDispatcher;

NS_CUSTER_END

#endif