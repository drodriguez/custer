#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "custer.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

NS_CUSTER_BEGIN

class IDispatcher;

class EventHandler : public boost::enable_shared_from_this<EventHandler>
{
public:
	virtual void handleAccept(boost::shared_ptr<IDispatcher> dispatcher) {}
	virtual void handleRead(boost::shared_ptr<IDispatcher> dispatcher) {}
	virtual void handleWrite(boost::shared_ptr<IDispatcher> dispatcher) {}
	virtual void handleClose(boost::shared_ptr<IDispatcher> dispatcher) {}
	
	virtual socket_type getHandle() { return m_handle; };
protected:
	socket_type m_handle;
};

NS_CUSTER_END

#endif