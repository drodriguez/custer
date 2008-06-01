#include "ClientEventHandler.h"

#include <cstring>
#include <cerrno>

using namespace custer;

ClientEventHandler::ClientEventHandler(
	boost::shared_ptr<CusterServer> server,
	socket_type connection) :
	m_server(server),
	m_connection(connection)
{
	// Nothing
}

static unsigned int bufferSize = 1024;

void ClientEventHandler::handleRead(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleRead");
	
	char buffer[bufferSize+1];
	int n;
	
	if ((n = read(m_handle, buffer, bufferSize)) == -1) {
		error("Error leyendo de socket: %s", strerror(errno));
		unregister(dispatcher);
	} else if (n == 0) {
		warn("Recibidos 0 bytes");
		unregister(dispatcher);
	}
	
	buffer[n >= 0 ? n : 0] = '\0';
	
	info("Recibido: %s", buffer);
}

void ClientEventHandler::handleWrite(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleWrite");
	// TODO
}

void ClientEventHandler::handleClose(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleClose");
	unregister(dispatcher);
}

void ClientEventHandler::unregister(boost::shared_ptr<IDispatcher> dispatcher)
{
	close(m_handle);
	dispatcher->removeHandler(shared_from_this(), ALL_EVENTS);	
}
