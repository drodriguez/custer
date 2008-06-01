#include "CusterServer.h"

#include <boost/filesystem/operations.hpp>

using namespace custer;

namespace bfs = boost::filesystem;

CusterServer::CusterServer(uint16_t port, std::string directory) :
	m_port(port),
	m_directory(directory)
{
	debug("Directorio '%s'",
		bfs::system_complete(m_directory).normalize().string().c_str());
}

void CusterServer::run()
{
	// Nothing
	debug("Corriendo...");
}