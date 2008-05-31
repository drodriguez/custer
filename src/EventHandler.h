#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

NS_CUSTER_BEGIN

class EventHandler
{
public:
	void handleAccept();
	void handleRead();
	void handleWrite();
	void handleClose();
	
	socket_type getHandle();
private:
	socket_type m_handle;
};

NS_CUSTER_END

#endif