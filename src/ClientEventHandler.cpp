#include "ClientEventHandler.h"
#include "const.h"
#include "HttpRequest.h"

#include <cstring>
#include <cerrno>

using namespace custer;

static char* response =
"HTTP/1.0 200 OK\r\n"
"Connection: close\r\n\r\n"
"Esto es una prueba.\r\n";

// TODO: VALIDATE_LENGTH en todos estas funciones
void request_method_cb(void* data, const char* at, size_t length)
{
	debug("Request method: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_REQUEST_METHOD, std::string(at, length)));
}

void http_field_cb(
	void* data,
	const char* field,
	size_t flen,
	const char* value,
	size_t vlen)
{
	debug("HTTP field: %.*s: %.*s",
		flen,
		field,
		(vlen > 40 ? 40 : vlen),
		value);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(
		std::string(field, flen),
		std::string(value, vlen)
	));
}

void request_uri_cb(void* data, const char* at, size_t length)
{
	debug("Request URI: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_REQUEST_URI, std::string(at, length)));
}

void fragment_cb(void* data, const char* at, size_t length)
{
	debug("Fragment: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_FRAGMENT, std::string(at, length)));
}

void request_path_cb(void* data, const char* at, size_t length)
{
	debug("Request path: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_REQUEST_PATH, std::string(at, length)));
}

void query_string_cb(void* data, const char* at, size_t length)
{
	debug("Query string: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_QUERY_STRING, std::string(at, length)));
}

void http_version_cb(void* data, const char* at, size_t length)
{
	debug("HTTP version: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_VERSION, std::string(at, length)));
}

void header_done_cb(void* data, const char* at, size_t length)
{
	if (length <= 0) return;
	
	debug("Body: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_BODY, std::string(at, length)));
}

ClientEventHandler::ClientEventHandler(
	boost::shared_ptr<CusterServer> server,
	socket_type connection) :
	m_server(server),
	m_nparsed(0),
	m_dataLength(0)
{
	debug("ClientEventHandler::constructor");
	m_handle = connection;
	m_data = (char *) xmalloc(HTTP_MAX_HEADER * sizeof(char));
	m_parser = (http_parser*) xmalloc(sizeof(http_parser));
	m_params = boost::shared_ptr<ParamsMap>(new ParamsMap());
	
	m_parser->data = m_params.get();
	m_parser->http_field = http_field_cb;
	m_parser->request_method = request_method_cb;
	m_parser->request_uri = request_uri_cb;
	m_parser->fragment = fragment_cb;
	m_parser->request_path = request_path_cb;
	m_parser->query_string = query_string_cb;
	m_parser->http_version = http_version_cb;
	m_parser->header_done = header_done_cb;
	http_parser_init(m_parser);
}

static unsigned int bufferSize = CHUNK_SIZE;

void ClientEventHandler::handleRead(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleRead");
		
	char buffer[bufferSize];
	int n;
		
	if ((n = read(m_handle, buffer, bufferSize)) == -1) {
		error("Error leyendo de socket: %s", strerror(errno));
		unregister(dispatcher);
	} else if (n == 0) {
		warn("Recibidos 0 bytes");
		unregister(dispatcher);
	}
	debug("Recibidos %d bytes", n);
	
	// Si ya hemos realizado el parseo de las cabeceras, le damos los
	// contenidos al request.
	if (http_parser_is_finished(m_parser)) {
		m_request->handleRead(buffer, n);
		return;
	}
	
	if (m_dataLength + n >= HTTP_MAX_HEADER) {
		error("Longitud de HEADER excesiva. Cliente expulsado");
		unregister(dispatcher);
	}
	
	memcpy(m_data+m_dataLength, buffer, n);
	m_dataLength += n;
	
	if (m_nparsed < m_dataLength) {
		debug("Parseando desde %d hasta %d", m_nparsed, m_dataLength);
		m_nparsed = http_parser_execute(m_parser, m_data, m_dataLength, m_nparsed);
		
		if (http_parser_is_finished(m_parser)) {
			debug("El parser ha finalizado");
			
			if (m_params->find(HTTP_REQUEST_PATH) == m_params->end()) {
				error("Sin REQUEST_PATH");
				unregister(dispatcher);
			}
			
			// FIX: SCRIPT_NAME y PATH_INFO?
			
			// TODO: REMOTE_ADDR
			
			m_request = boost::shared_ptr<HttpRequest>(
				new HttpRequest(m_params));
			
			
			// TODO break if body == NULL
			
			// unregister(dispatcher);
		}
	}
}

void ClientEventHandler::handleWrite(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleWrite");
	
	if (m_request->isComplete()) {
		write(m_handle, response, strlen(response));
		unregister(dispatcher);
	}
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
