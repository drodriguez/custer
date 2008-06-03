#include "DirectorySender.h"

#include <boost/filesystem/operations.hpp>

using namespace custer;
namespace bfs = boost::filesystem;

static std::string defaultContentType("application/octet-stream");

DirectorySender::DirectorySender(
	std::string directory,
	bool allowListing = true,
	std::string indexFile) :
	m_directory(directory),
	m_allowListing(allowListing),
	m_indexFile(indexFile)
{
	m_directory = bfs::system_complete(m_directory);
}

boost::filesystem::path DirectoryServer::canServe(std::string pathInfo)
{
	std::string requestPathStr = HttpRequest.unescape(pathInfo);
	// Lo completamos con el directorio base
	bfs::path requestPath = m_directory / requestPathStr;
	requestPath = bfs::system_complete(requestPath);
	
	// Comprobamos que esté dentro del directorio base
	if (requestPath.string().index(m_directory.string()) == 0 &&
		bfs::exists(requestPath)) {
		// Existe y está en una localización permitida
		if (bfs::is_directory(requestPath)) {
			// Piden un directorio
			bfs::path dirIndexFile = requestPath / m_indexFile;
			if (bfs::exists(dirIndexFile)) {
				// Servimos el índice
				return dirIndexFile;
			} else if (m_allowListing) {
				// Servimos el directorio
				return requestPath;
			} else {
				// No servimos nada
				return bfs::path();
			}
		} else {
			// Es un archivo
			return requestPath;
		}
	} else {
		// O no existe o no es accesible.
		return bfs::path();
	}
}

void DirectorySender::sendDirectoryListing(
	std::string base,
	boost::filesystem::path directory,
	boost::shared_ptr<HttpResponse> response)
{
	// Quitamos cualquier barra al final, para que los enlaces funcionen.
	std::string cleanBase = HttpRequest.unescape(base);
	// TODO: eliminar último caracter si es una "/"
	
	if (m_listing_allowed) {
		response->setStatus(HTTP_STATUS_OK);
		response->headers[HTTP_CONTENT_TYPE] = "text/html";
		response->out << "<html><head><title>Directory listing</title></head><body>";
		
		// TODO: iterar por las entradas del directorio
		
		response->out << "</body></html>"
		response->send();
	} else {
		response->setStatus(HTTP_STATUS_NOT_ALLOWED);
		response->out << "Directory listings not allowed";
		response->send();
	}
}

void DirectorySender::sendFile(
	boost::filesystem::path requestPath,
	boost::shared_ptr<HttpResquest> request,
	boost::shared_ptr<HttpResponse> response)
{
	response->setStatus(200);
	// TODO: establecer el MIME-Type basandose en la extensión
	response->header[HTTP_CONTENT_TYPE] = defaultContentType;
	
	// TODO: enviar ¿status? con content-length
	response->sendHeaders();
	// TODO: enviar el fichero
	// response->sendFile(requestPath, )
}

void