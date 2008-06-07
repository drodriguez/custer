#ifndef SELECT_DISPATCHER_H
#define SELECT_DISPATCHER_H

#include "custer.h"
#include "IDispatcher.h"

#include <boost/shared_ptr.hpp>

#include <map>
#include <sys/types.h>

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
};

typedef SelectDispatcher NativeDispatcher;

NS_CUSTER_END

#endif