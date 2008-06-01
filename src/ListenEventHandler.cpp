#include "ListenEventHandler.h"

#include <cstring>
#include <cerrno>

using namespace custer;

ListenEventHandler::ListenEventHandler(
	boost::shared_ptr<CusterServer> server) :
	m_server(server)
{
	// Nothing
}

void ListenEventHandler::handleAccept(IDispatcher* dispatcher)
{
	debug("ListenEventHandler::handleAccept");
	socket_type connection;
	struct sockaddr_in* address;
	socklen_t address_len = sizeof(struct sockaddr_in);
	
	// Esto no debería bloquear...
	if ((connection = accept(m_handle, address, &address_len)) == -1)
		fatal("en accept(): %s", strerror(errno));
	
	boost::shared_ptr<ClientEventHandler> handler =
		boost::shared_ptr(new ClientEventHandler(m_server, connection));
	dispatcher->registerHandler(
		handler,
		READ_EVENT | WRITE_EVENT | CLOSE_EVENT);
}

void ListenEventHandler::handleClose(IDispatcher* dispatcher)
{
	debug("ListerEventHandler::handleClose");
}
