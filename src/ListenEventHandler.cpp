#include "ListenEventHandler.h"
#include "ClientEventHandler.h"
#include "CusterServer.h"

#define BACKLOG 32

using namespace custer;

ListenEventHandler::ListenEventHandler(
	boost::shared_ptr<CusterServer> server) :
	m_server(server)
{
	struct in_addr listenAddress;
	struct sockaddr_in inetListenAddress;
	int one = 1;
	
	listenAddress.s_addr = htonl(INADDR_ANY);
	
	if ((m_handle = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		fatal("creando socket: %s", strerror(errno));
	
	if (socketSetOpt(
		m_handle,
		SOL_SOCKET,
		SO_REUSEADDR,
		&one,
		sizeof(one)) == -1)
		fatal("establenciendo SO_REUSEADDR: %s", strerror(errno));

	memset(&inetListenAddress, 0, sizeof(inetListenAddress));
	inetListenAddress.sin_family = AF_INET;
	inetListenAddress.sin_addr   = listenAddress;
	inetListenAddress.sin_port   = htons(server->getPort());
	
	if (bind(
		m_handle,
		(struct sockaddr*) &inetListenAddress,
		sizeof(inetListenAddress)) == -1)
		fatal("asociando socket: %s", strerror(errno));
	
	if (listen(m_handle, BACKLOG) == -1)
		fatal("escuchando en el socket: %s", strerror(errno));
	
	info("Escuchando conexiones al puerto %d", server->getPort());
}

void ListenEventHandler::handleAccept
	(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ListenEventHandler::handleAccept");
	socket_type connection;
	struct sockaddr* address;
	socklen_t address_len = sizeof(struct sockaddr_in);
	
	// Esto no debería bloquear...
	if ((connection = accept(m_handle, address, &address_len)) == -1)
		fatal("en accept(): %s", strerror(errno));
		
	debug("Conexión aceptada");
	
	boost::shared_ptr<ClientEventHandler> handler(
			new ClientEventHandler(m_server, connection));
		
	dispatcher->registerHandler(handler, READ_EVENT | CLOSE_EVENT);
}

void ListenEventHandler::handleClose
	(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ListerEventHandler::handleClose");
}
