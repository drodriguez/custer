#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

NS_CUSTER_BEGIN

class EventHandler
{
public:
	socket_type getHandle();
private:
	socket_type m_handle;
};

NS_CUSTER_END

#endif