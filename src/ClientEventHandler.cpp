#include "ClientEventHandler.hpp"

using namespace custer;

ClientEventHadler::ClientEventHandler(
	boost::shared_ptr<CusterServer> server,
	socket_type connection) :
	m_server(server),
	m_connection(connection)
{
	// Nothing
}

void ClientEventHandler::handleRead(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleRead");
	// TODO
}

void ClientEventHandler::handleWrite(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleWrite");
	// TODO
}

void ClientEventHandler::handleClose(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleClose");
	// TODO
}
