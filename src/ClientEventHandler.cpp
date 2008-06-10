#include "ClientEventHandler.h"
#include "const.h"
#include "CusterServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "DirectorySender.h"
#include "parser_callbacks.h"

#include <cstring>
#include <cerrno>

#include <sys/socket.h>
#include <netinet/in.h>

using namespace custer;

ClientEventHandler::ClientEventHandler(
	boost::shared_ptr<CusterServer> server,
	socket_type connection) :
	m_server(server),
	m_nparsed(0),
	m_dataLength(0)
{
	debug("ClientEventHandler::constructor");
	m_handle = connection;
	m_directorySender = m_server->getDirectorySender();
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

ClientEventHandler::~ClientEventHandler()
{
	debug("ClientEventHandler::~ClientEventHandler");
	if (m_parser) free(m_parser);
	if (m_data) free(m_data);
}

void ClientEventHandler::handleRead(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleRead");
		
	char buffer[CHUNK_SIZE];
	int n;
		
	if ((n = read(m_handle, buffer, CHUNK_SIZE)) == -1) {
		error("Error leyendo de socket: %s", strerror(errno));
		closeConnection(dispatcher);
	} else if (n == 0) {
		warn("Recibidos 0 bytes");
		closeConnection(dispatcher);
	}
	debug("Recibidos %d bytes", n);
	
	// Si ya hemos realizado el parseo de las cabeceras, le damos los
	// contenidos al request.
	if (http_parser_is_finished(m_parser)) {
		m_request->handleRead(buffer, n);
	} else {
		if (m_dataLength + n >= HTTP_MAX_HEADER) {
			error("Longitud de HEADER excesiva. Cliente expulsado");
			closeConnection(dispatcher);
		}
	
		memcpy(m_data+m_dataLength, buffer, n);
		m_dataLength += n;
	
		if (m_nparsed < m_dataLength) {
			debug("Parseando desde %d hasta %d", m_nparsed, m_dataLength);
			m_nparsed = http_parser_execute(m_parser, m_data, m_dataLength, m_nparsed);
			
			int parserFinished = http_parser_finish(m_parser);
			if (parserFinished == 1) {
				debug("El parser ha finalizado");
			
				if (m_params->find(HTTP_REQUEST_PATH) == m_params->end()) {
					error("Sin REQUEST_PATH");
					closeConnection(dispatcher);
				}
							
				m_request = boost::shared_ptr<HttpRequest>(
					new HttpRequest(m_params));
				
				struct sockaddr_in remoteAddress;
				socklen_t remoteAddressSize = sizeof(struct sockaddr_in);
				memset(&remoteAddress, 0, remoteAddressSize);
				
				
				if (getpeername(m_handle,
					(struct sockaddr*) &remoteAddress,
					&remoteAddressSize) == -1) {
					error("REMOTE_ADDR no encontrada");
					m_request->setParam(HTTP_REMOTE_ADDR, "???.???.???.???");
				} else {
					std::stringstream addr;
					uint32_t addrv = remoteAddress.sin_addr.s_addr;
					addr << (addrv       & 0xff) << ".";
					addr << (addrv >> 8  & 0xff) << ".";
					addr << (addrv >> 16 & 0xff) << ".";
					addr << (addrv >> 24       );
					m_request->setParam(HTTP_REMOTE_ADDR, addr.str());
				}
				
				// FIX: SCRIPT_NAME y PATH_INFO
				m_request->setParam(HTTP_SCRIPT_NAME, HTTP_SLASH);
				m_request->setParam(HTTP_PATH_INFO, (*m_params)[HTTP_REQUEST_URI]);		
			} else if (parserFinished == -1) {
				error("error en la peticion HTTP");
				closeConnection(dispatcher);
			}
		}
	}
	
	if (m_request && m_request->isComplete()) {
		boost::shared_ptr<EventHandler> self(shared_from_this());
		dispatcher->removeHandler(self, READ_EVENT);
		dispatcher->registerHandler(self, WRITE_EVENT | CLOSE_EVENT);
	}
	
}

void ClientEventHandler::handleWrite(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleWrite");
	
	// Si el request está completo podemos escribir
	if (m_request->isComplete()) {
		if (!m_response) {
			m_response = boost::shared_ptr<HttpResponse>(
				new HttpResponse(m_handle, m_request->getParams()));
			
			m_directorySender->process(m_request, m_response);
		}
		
		m_response->handleWrite();
		
		if (m_response->done()) {
			closeConnection(dispatcher);
		}
	}
}

void ClientEventHandler::handleClose(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClientEventHandler::handleClose");
	closeConnection(dispatcher);
}

void ClientEventHandler::closeConnection(boost::shared_ptr<IDispatcher> dispatcher)
{
	debug("ClienteEventHandler::closeConnection");
	// Al cerrar un descriptor los kevents asociados se destruyen, si le
	// pedimos eliminarlo y luego lo cerramos se produce un error al invocar
	// a kevent.
	boost::shared_ptr<EventHandler> self(shared_from_this());
	dispatcher->removeHandler(self, ALL_EVENTS);
	close(m_handle);
}
