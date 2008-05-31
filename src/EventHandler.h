#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

NS_CUSTER_BEGIN

class EventHandler
{
public:
	virtual void handleAccept() = 0;
	virtual void handleRead() = 0;
	virtual void handleWrite() = 0;
	virtual void handleClose() = 0;
	
	socket_type getHandle() { return m_handle; };
private:
	socket_type m_handle;
};

NS_CUSTER_END

#endif