#include "Dispatcher.h"

using namespace custer;

Dispatcher::Dispatcher() :
	m_dispatcher()
{
	// Nothing
}

void Dispatcher::registerHandler
	(boost::shared_ptr<EventHandler> eh, EventType et)
{
	m_dispatcher.registerHandler(eh, et);
}

void Dispatcher::removeHandler
	(boost::shared_ptr<EventHandler> eh, EventType et)
{
	m_dispatcher.removeHandler(eh, et);	
}

void Dispatcher::handleEvents(long timeout)
{
	m_dispatcher.handleEvents(timeout);
}
