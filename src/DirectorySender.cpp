#include "DirectorySender.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace custer;
namespace bfs = boost::filesystem;
namespace ba = boost::algorithm;

static std::string defaultContentType("application/octet-stream");
static std::string onlyHeadOrGet("Only HEAD and GET allowed.");

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
	std::string cleanBase = HttpRequest::unescape(base);
	if (ba::ends_with(cleanBase, "/")) {
		ba::erase_tail(cleanBase, 1);
	}
	
	if (m_listing_allowed) {
		response->setStatus(HTTP_STATUS_OK);
		response->headers[HTTP_CONTENT_TYPE] = "text/html";
		response->out << "<html><head><title>Directory listing</title></head><body>";
		
		bfs::directory_iterator endIter;
		for (bfs::directory_iterator iter(directory);
			iter != endIter;
			++iter) {
				response->out << "<a href=\"" << cleanBase << "/"
					<< HttpRequest::escape(iter->leaf()) << "\">";
				response->out << "</a></br />";
		}
		
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
	boost::shared_ptr<HttpResponse> response,
	bool headerOnly)
{
	response->setStatus(HTTP_STATUS_OK);
	// TODO: establecer el MIME-Type basandose en la extensión
	response->header[HTTP_CONTENT_TYPE] = defaultContentType;
	
	// TODO: enviar ¿status? con content-length
	response->sendHeaders();
	if (!headerOnly) {
		// TODO: enviar el fichero
		// response->sendFile(requestPath, )
	}
	response->send();
}

void DirectorySender::process(
	boost::shared_ptr<HttpRequest> request,
	boost::shared_ptr<HttpResponse> response)
{
	HttpRequest::Method requestMethod = request->getRequestMethod();
	bfs::path requestPath = canServe(request->getPathInfo());
	
	if (requestPath.empty()) {
		response->setStatus(HTTP_STATUS_NOT_FOUND);
		response->out << "Not found";
		response->send();
	} else {
		if (bfs::is_directory(requestPath)) {
			sendDirectoryListing(
				request->getRequestURI(),
				requestPath,
				response);
		} else if (requestMethod == HTTP_REQUEST_METHOD_HEAD) {
			sendFile(requestPath, request, response, true);
		} else if (requestMethod == HTTP_REQUEST_METHOD_GET) {
			sendFile(requestPath, request, response, false);
		} else {
			response->setStatus(HTTP_STATUS_METHOD_NOT_ALLOWED);
			response->out << onlyHeadOrGet;
			response->send();
		}
	}
}