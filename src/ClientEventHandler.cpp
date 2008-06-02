#include "ClientEventHandler.h"

#include <cstring>
#include <cerrno>

using namespace custer;

static char* response =
"HTTP/1.0 200 OK\r\n"
"Connection: close\r\n\r\n"
"Esto es una prueba.\r\n";

ClientEventHandler::ClientEventHandler(
	boost::shared_ptr<CusterServer> server,
	socket_type connection) :
	m_server(server)
{
	debug("ClientEventHandler::constructor");
	m_handle = connection;
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
	write(m_handle, response, strlen(response));
	unregister(dispatcher);
}

void ClientEventHandler::handleClose(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleClose");
	unregister(dispatcher);
}

void ClientEventHandler::unregister(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClienteEventHandler::unregister");
	// FIX: Al cerrar un descriptor los kevents asociados se destruyen, si le
	// pedimos eliminarlo y luego lo cerramos se produce un error al invocar
	// a kevent.
	// dispatcher->removeHandler(shared_from_this(), ALL_EVENTS);
	close(m_handle);
}
