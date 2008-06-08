#include "SelectDispatcher.h"
#include "EventHandler.h"
#include "utils.h"

using namespace custer;

SelectDispatcher::SelectDispatcher() :
	m_eventHandlerMap()
{
	// Nothing
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
	
	// TODO
		
	EventHandlerMap::iterator iter;
	if ((iter = m_eventHandlerMap.find(eh.get())) != m_eventHandlerMap.end()) {
		int newEt = iter->second.second | et;
		debug("actualizando antiguo: %x, %x, %x", iter->second.second, et, newEt);
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
	
	// Si nos piden todos suponenmos que van a cerrar el socket ellos
	if (et == ALL_EVENTS) {
		m_eventHandlerMap.erase(eh.get());
		return;
	}
		
	EventHandlerPair ehp = m_eventHandlerMap[eh.get()];
	
	// TODO
	
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
}
