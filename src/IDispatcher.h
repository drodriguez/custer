#ifndef IDISPATCHER_H
#define IDISPATCHER_H

#include "custer.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

NS_CUSTER_BEGIN

class EventHandler;

/**
 * Tipos de eventos que son entendidos. Los valores son compatibles para
 * realizar aritmética de bits con ellos.
 */
enum EventType {
	NO_EVENT     = 0,
	ACCEPT_EVENT = 1 << 0,
	READ_EVENT   = 1 << 1,
	WRITE_EVENT  = 1 << 2,
	CLOSE_EVENT  = 1 << 3,
	ALL_EVENTS   = ACCEPT_EVENT | READ_EVENT | WRITE_EVENT | CLOSE_EVENT
};

/**
 * Interfaz para todos los dispatchers.
 */
class IDispatcher : public boost::enable_shared_from_this<IDispatcher>
{
public:
	/** Destructor */
	virtual ~IDispatcher() {}
	
	/**
	 * Registra un EventHandler de un EventType específico.
	 *
	 * @param eh El EventHandler a registrar.
	 * @param et Los EventType a los que el EventHandler responderá. Se
	 *           puede responder a varios haciendo un OR (|) de sus valores.
	 */
	virtual void registerHandler
		(boost::shared_ptr<EventHandler> eh, unsigned int et) = 0;
		
	/**
	 * Elimina un EventHandler de un EventType específico.
	 *
	 * @param eh El EventHandler a desregistrar.
	 * @param et Los EventType que el EventHandler dejará de responder. Se puede
	 *           dejar de responder a varios haciendo un OR (|) de sus valores.
	 */
	virtual void removeHandler
		(boost::shared_ptr<EventHandler> eh, unsigned int et) = 0;
	
	/**
	 * Método que captura los eventos que suceden en el sistema operativo y los
	 * envia a los EventHandler adecuandos.
	 *
	 * @param timeout Tiempo de espera de los eventos (en milisegundos).
	 */
	virtual void handleEvents(long timeout = -1) = 0;
};

NS_CUSTER_END

#endif
