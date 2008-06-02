#include "CusterServer.h"
#include "ListenEventHandler.h"

#include <boost/filesystem/operations.hpp>

using namespace custer;

namespace bfs = boost::filesystem;

CusterServer::CusterServer(uint16_t port, std::string directory) :
	m_port(port),
	m_directory(directory)
{
	debug("Directorio '%s'",
		bfs::system_complete(m_directory).normalize().string().c_str());
	
	m_dispatcher = boost::shared_ptr<Dispatcher>(new Dispatcher());
}

void CusterServer::run()
{
	debug("CusterServer::run");
	
	boost::shared_ptr<CusterServer> cs(shared_from_this());
	boost::shared_ptr<ListenEventHandler> leh(new ListenEventHandler(cs));
	m_dispatcher->registerHandler(leh, ACCEPT_EVENT | CLOSE_EVENT);
	
	while(true)
		m_dispatcher->handleEvents();
}