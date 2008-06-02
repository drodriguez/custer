#ifndef CLIENTEVENTHANDLER_H
#define CLIENTEVENTHANDLER_H

#include "custer.h"
#include "EventHandler.h"

NS_CUSTER_BEGIN

class CusterServer;

class ClientEventHandler : public EventHandler
{
public:
	/**
	 * Constructor que recibe la conexión a la que se servirá.
	 *
	 * @param server El servidor al que pertenece ClientEventHandler.
	 * @param connection Conexión que se servirá.
	 */
	ClientEventHandler(
		boost::shared_ptr<CusterServer> server,
		socket_type connection);
	
	// heredado de EventHandler
	void handleRead(boost::shared_ptr<IDispatcher>  dispatcher);
	
	// heredado de EventHandler
	void handleWrite(boost::shared_ptr<IDispatcher>  dispatcher);
	
	// heredado de EventHandler
	void handleClose(boost::shared_ptr<IDispatcher>  dispatcher);
	
private:
	/** Servidor al que pertenece el ClientEventHandler */
	boost::shared_ptr<CusterServer> m_server;
		
	/**
	 * Cierra la conexión asociada y se desregistra del dispatcher parámetro.
	 *
	 * @param dispatcher El dispatcher del que nos desregistraremos.
	 */
	void unregister(boost::shared_ptr<IDispatcher> dispatcher);
};

NS_CUSTER_END

#endif
