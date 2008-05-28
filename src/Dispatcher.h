#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "custer.h"
#include "IDispatcher.h"

#include <boost/shared_ptr.hpp>

NS_CUSTER_BEGIN

/**
 * Recibe todos los eventos y los envia al EventHandler adecuado cuando un
 * cliente lo solicita.
 */
class Dispatcher : public IDispatcher
{
public:
	/** Constructor */
	Dispatcher();
	
	// Heredada de IDispatcher
	void registerHandler(boost::shared_ptr<EventHandler> eh, EventType et);
	
	// Heredada de IDispatcher
	void removeHandler(boost::shared_ptr<EventHandler> eh, EventType et);
	
	// Heredada de IDispatcher
	void handleEvents(long timeout = 0);
	
private:
	NativeDispatcher m_dispatcher;
};

NS_CUSTER_END

#endif
