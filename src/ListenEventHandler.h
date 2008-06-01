#ifndef LISTENEVENTHANDLER_H
#define LISTENEVENTHANDLER_H

#include "custer.h"
#include "CusterServer.h"

NS_CUSTER_BEGIN

class ListenEventHandler : public EventHandler
{
public:
	/**
	 * Constructor.
	 *
	 * @param server El servidor que posee este ListenEventHandler.
	 */
	ListenEventHandler(boost::shared_ptr<CusterServer> server);
	
	// heradado de EventHandler
	void handleAccept(boost::shared_ptr<IDispatcher>  dispatcher);
	
	// heredado de EventHandler
	void handleClose(boost::shared_ptr<IDispatcher>  dispatcher);

private:
	/** Servidor que posee este ListenEventHandler */
	boost::shared_ptr<CusterServer> m_server;
};

NS_CUSTER_END

#endif
