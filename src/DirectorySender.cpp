#include "DirectorySender.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "const.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace custer;
namespace bfs = boost::filesystem;
namespace ba = boost::algorithm;

static std::string defaultContentType("application/octet-stream");
static std::string onlyHeadOrGet("Only HEAD and GET allowed.");

std::map<std::string, std::string> DirectorySender::s_mimeTypes;

void DirectorySender::initializeMimeTypes()
{
	s_mimeTypes[".atom"]  = "application/atom+xml";
	s_mimeTypes[".avi"]   = "video/avi";
	s_mimeTypes[".bmp"]   = "image/bmp";
	s_mimeTypes[".bz2"]   = "application/x-bzip2";
	s_mimeTypes[".c"]     = "text/plain";
	s_mimeTypes[".class"] = "application/java";
	s_mimeTypes[".cpp"]   = "text/x-c";
	s_mimeTypes[".css"]   = "text/css";
	s_mimeTypes[".doc"]   = "application/msword";
	s_mimeTypes[".exe"]   = "application/octet-stream";
	s_mimeTypes[".flv"]   = "video/x-flv";
	s_mimeTypes[".gif"]   = "image/gif";
	s_mimeTypes[".gz"]    = "application/x-compressed";
	s_mimeTypes[".gzip"]  = "application/x-gzip";
	s_mimeTypes[".h"]     = "text/plain";
	s_mimeTypes[".htm"]   = "text/html";
	s_mimeTypes[".html"]  = "text/html";
	s_mimeTypes[".ico"]   = "image/x-icon";
	s_mimeTypes[".jar"]   = "application/java-archive";
	s_mimeTypes[".java"]  = "text/plain";
	s_mimeTypes[".jpeg"]  = "image/jpeg";
	s_mimeTypes[".jpg"]   = "image/jpeg";
	s_mimeTypes[".js"]    = "application/x-javascript";
	s_mimeTypes[".mid"]   = "audio/midi";
	s_mimeTypes[".midi"]  = "audio/midi";
	s_mimeTypes[".mov"]   = "video/quicktime";
	s_mimeTypes[".mp3"]   = "audio/mpeg";
	s_mimeTypes[".mpg"]   = "video/mpeg";
	s_mimeTypes[".mpeg"]  = "video/mpeg";
	s_mimeTypes[".pdf"]   = "application/pdf";
	s_mimeTypes[".png"]   = "image/png";
	s_mimeTypes[".ppt"]   = "application/mspowerpoint";
	s_mimeTypes[".rar"]   = "application/x-rar-compressed";
	s_mimeTypes[".rss"]   = "text/xml";
	s_mimeTypes[".txt"]   = "text/plain";
	s_mimeTypes[".tgz"]   = "application/x-compressed";
	s_mimeTypes[".vcf"]   = "text/x-vcard";
	s_mimeTypes[".vcs"]   = "text/x-vcalendar";
	s_mimeTypes[".wav"]   = "audio/wav";
	s_mimeTypes[".xhtml"] = "application/xhtml+xml";
	s_mimeTypes[".xls"]   = "application/excel";
	s_mimeTypes[".xml"]   = "text/xml";
	s_mimeTypes[".zip"]   = "application/zip";
}

DirectorySender::DirectorySender(
	std::string directory,
	bool allowListing,
	std::string indexFile) :
	m_allowListing(allowListing),
	m_directory(directory),
	m_indexFile(indexFile)
{
	m_directory = bfs::system_complete(m_directory);
	m_directory.normalize();
}

boost::filesystem::path DirectorySender::canServe(std::string pathInfo)
{
	std::string requestPathStr = HttpRequest::unescape(pathInfo);
	// Lo completamos con el directorio base
	bfs::path requestPath = m_directory / requestPathStr;
	requestPath = bfs::system_complete(requestPath);
	requestPath.normalize();
	
	// Comprobamos que esté dentro del directorio base
	debug("requestPath: %s", requestPath.string().c_str());
	debug("m_directory: %s", m_directory.string().c_str());
	if (ba::starts_with(requestPath.string(), m_directory.string()) &&
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
	
	if (m_allowListing) {
		response->setStatus(HTTP_STATUS_OK);
		response->setHeader(HTTP_CONTENT_TYPE, "text/html");
		response->out << "<html><head><title>Listado del directorio</title></head><body>";
		
		response->out << "<a href=\"" << cleanBase << "/..\">Ir al padre...</a><br />";
		
		bfs::directory_iterator endIter;
		for (bfs::directory_iterator iter(directory);
			iter != endIter;
			++iter) {
				response->out << "<a href=\"" << cleanBase << "/"
					<< HttpRequest::escape(iter->leaf()) << "\">";
				response->out << iter->leaf() << "</a></br />";
		}
		
		response->out << "</body></html>";
		response->send();
	} else {
		response->setStatus(HTTP_STATUS_FORBIDDEN);
		response->out << "Directory listings not allowed";
		response->send();
	}
}

void DirectorySender::sendFile(
	boost::filesystem::path requestPath,
	boost::shared_ptr<HttpRequest> request,
	boost::shared_ptr<HttpResponse> response,
	bool headerOnly)
{
	debug("DirectorySender::sendFile");
	response->setStatus(HTTP_STATUS_OK);
	
	std::string extension = bfs::extension(requestPath);
	if (extension.empty()) {
		response->setHeader(HTTP_CONTENT_TYPE, defaultContentType);
	} else {
		std::map<std::string, std::string>::iterator iter;
		if ((iter = s_mimeTypes.find(extension)) != s_mimeTypes.end()) {
			response->setHeader(HTTP_CONTENT_TYPE, s_mimeTypes[extension]);
		} else {
			response->setHeader(HTTP_CONTENT_TYPE, defaultContentType);
		}
	}
	
	response->sendStatus(bfs::file_size(requestPath));
	response->sendHeaders();
	if (!headerOnly) {
		response->sendFile(requestPath);
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
		debug("Request Path vacio");
		response->setStatus(HTTP_STATUS_NOT_FOUND);
		response->out << "Not found";
		response->send();
	} else {
		if (bfs::is_directory(requestPath)) {
			sendDirectoryListing(
				request->getRequestURI(),
				requestPath,
				response);
		} else if (requestMethod == HttpRequest::GET) {
			sendFile(requestPath, request, response, false);
		} else if (requestMethod == HttpRequest::HEAD) {
			sendFile(requestPath, request, response, true);
		} else {
			response->setStatus(HTTP_STATUS_METHOD_NOT_ALLOWED);
			response->out << onlyHeadOrGet;
			response->send();
		}
	}
}