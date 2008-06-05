#ifndef KQUEUE_DISPATCHER_H
#define KQUEUE_DISPATCHER_H

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
	
	/** Redimensiona los vector de kevents si es necesario */
	void resizeKeventsVectors();
	
	/**
	 * Realiza las modificaciones necesarias al vector de cambios de los
	 * kevents.
	 *
	 * @param et EventTypes a modificar.
	 * @param ident El identificador del kevent (descriptor de fichero).
	 * @param flags Los flags que se deben establecer en el kevent.
	 * @param udata Los datos de usuario que se deben establecer en el kevent.
	 */
	void modifyKevents(
		unsigned int et,
		uintptr_t ident,
		u_short flags,
		void* udata);
};

typedef KQueueDispatcher NativeDispatcher;

NS_CUSTER_END

#endif