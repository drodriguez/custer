#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include "custer.h"

#include <boost/filesystem.hpp>

#include <map>
#include <string>
#include <sstream>
#include <fstream>

NS_CUSTER_BEGIN

/**
 * Escribe y controla la respuesta HTTP dada al cliente.
 */
class HttpResponse
{
public:
	/**
	 * Constructor para la conexión especificada.
	 *
	 * @param connection El socket que se utilizará para trasmitir datos al
	 *                   cliente.
	 */
	HttpResponse(socket_type connection);
	
	/**
	 * Envia los datos que sean necesarios enviar al cliente.
	 */
	void send();
	
	/**
	 * Envia únicamente el código de estado HTTP al cliente, y opcionalmente
	 * la longitud del contenido especificada o la longitud del cuerpo.
	 *
	 * @param contentLength Longitud del contenido. Si no se especifica se
	 *                      enviará la longitud del cuerpo.
	 */
	void sendStatus(long contentLength = -1);
	
	/**
	 * Envía únicamente las cabeceras HTTP.
	 */
	void sendHeaders();
	
	/**
	 * Envía el cuerpo del mensaje HTTP.
	 */
	void sendBody();
	
	/**
	 * Dispone a cierto fichero para ser enviado.
	 *
	 * @param filePath La ruta al fichero que se quiere enviar.
	 */
	void sendFile(boost::filesystem::path filePath);
	
	/**
	 * Invocado por el EventHandler cuando se pueden enviar datos por la
	 * conexión.
	 */
	void handleWrite();
	
	/**
	 * Devuelve la conexión utilizada para comunicarse con el cliente.
	 *
	 * @return La conexión utilizada para comunicarse con el cliente.
	 */
	socket_type getConnection() { return m_connection; }
	
	/**
	 * Devuelve el código de estado que se enviará al cliente.
	 *
	 * @return El código de estado HTTP.
	 */
	unsigned short getStatus() { return m_status; }
	
	/**
	 * Establece un nuevo código de estado a enviar al cliente.
	 *
	 * @param Código de estado a enviar.
	 */
	void setStatus(unsigned short status) { m_status = status; }
	
	/**
	 * Recupera la cadena de la razón del estado, bien sea una predefinida o
	 * una personalizada.
	 *
	 * @return La razón del estado.
	 */
	std::string getReason();
	
	/**
	 * Establece una cadena personalizada de razón para el estado.
	 *
	 * @param reason Razón del estado.
	 */
	void setReason(std::string reason) { m_reason = reason; }
	
	/**
	 * Establece o modifica una cabecera HTTP.
	 *
	 * @param header Nombre de la cabecera HTTP.
	 * @param value Valor de la cabecera HTTP.
	 */
	void setHeader(std::string header, std::string value);
	
	/**
	 * Determina si el cuerpo ha sido enviado.
	 *
	 * @return Si el cuerpo ha sido enviado.
	 */
	bool isBodySent() { return m_bodySent; }
	
	/**
	 * Determina si las cabeceras han sido enviadas.
	 *
	 * @return Si las cabeceras han sido enviadas.
	 */
	bool isHeadersSent() { return m_headersSent; }
	
	/**
	 * Determina si el código de estado ha sido enviado.
	 *
	 * @return Si el código de estado ha sido enviado.
	 */
	bool isStatusSent() { return m_statusSent; }
	
	/**
	 * Determina si se ha completado la respuesta HTTP.
	 */
	bool done() { return m_done; }
	
	/** Stream donde se mantiene el contenido de la respuesta */
	std::stringstream out;

private:
	/** La conexión utilizada para comunicarse con el cliente. */
	socket_type m_connection;
	
	/** El código de estado que se enviará */
	unsigned short m_status;
	
	/** La razón del estado personalizada */
	std::string m_reason;
	
	typedef std::map<std::string, std::string> HeadersMap;
	/** Las cabeceras que se enviarán */
	HeadersMap m_headers;
	
	/** Indica si el cuerpo ha sido enviado. */
	bool m_bodySent;
	
	/** Indica si las cabeceras han sido enviadas */
	bool m_headersSent;
	
	/** Indica si el código de estado ha sido enviado */
	bool m_statusSent;
	
	/** Indice si se ha terminado de enviar datos */
	bool m_done;
	
	/** La cadena que se envia como mensaje HTTP */
	std::stringstream m_httpMessage;
	
	/** El fichero que se puede enviar */
	std::ifstream m_file;
};

NS_CUSTER_END

#endif