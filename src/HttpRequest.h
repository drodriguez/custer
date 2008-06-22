#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "custer.h"

#include <boost/shared_ptr.hpp>

#include <map>
#include <string>
#include <sstream>

NS_CUSTER_BEGIN

typedef std::map<std::string, std::string> ParamsMap;

class HttpRequest
{
public:
	
	/** Métodos HTTP (1.0) */
	enum Method {
		GET,
		POST,
		PUT,
		DELETE,
		HEAD
	};
		
	/**
	 * "Escapa" una URI para poder construir más sencillamente.
	 *
	 * @param s La URI a "escapar"
	 * @return La URI "escapada"
	 */
	static std::string escape(std::string s);
	
	/**
	 * "Desescapa" una URI "escapada".
	 *
	 * @param s La URI "escapada"
	 * @return La URI sin "escapar"
	 */
	static std::string unescape(std::string s);
	
	/**
	 * Constructor.
	 *
	 * @param params Parámetros de esta petición.
	 */
	HttpRequest(boost::shared_ptr<ParamsMap> params);
	
	/**
	 * Devuelve los parámetros de esta petición.
	 *
	 * @return Los parámetros de esta petición.
	 */
	boost::shared_ptr<ParamsMap> getParams() { return m_params; }
	
	/**
	 * Establece un nuevo párametro de la petición.
	 *
	 * @param key Nombre de la clave para el parámetro.
	 * @param value Valor del parámetro.
	 */
	void setParam(std::string key, std::string value);
	
	/**
	 * Devuelve el cuerpo de esta petición.
	 *
	 * @return El cuerpo de esta petición.
	 */
	const std::stringstream& getBody() { return m_body; }
	
	/**
	 * Devuelve el método de la petición.
	 *
	 * @return El método de la petición.
	 */
	Method getRequestMethod();
	
	/**
	 * Devuelve la URI de la petición.
	 *
	 * @return la URI de la petición.
	 */
	std::string getRequestURI();
	
	/**
	 * Devuelve el path de la petición.
	 *
	 * @return el path de la petición.
	 */
	std::string getRequestPath();
	
	/**
	 * Devuelve el script de la petición.
	 *
	 * @return el script de la petición.
	 */
	std::string getScriptName();
	
	/**
	 * Devuelve la información del path de la petición.
	 *
	 * @return la información del path de la petición.
	 */
	std::string getPathInfo();
	
	/**
	 * Invocado desde los EventHandler para poder recibir más datos, como un
	 * cuerpo muy largo.
	 *
	 * @param buffer El buffer con los datos recibidos.
	 * @param length La longitud de los datos recibidos.
	 */
	void handleRead(const char* buffer, size_t length);
	
	/**
	 * Indica si la petición está completa.
	 *
	 * @return Si la petición está completa.
	 */
	bool isComplete() { return m_remain <= 0; }
	
private:
	/** Parámetros de la petición */
	boost::shared_ptr<ParamsMap> m_params;
	
	/** Socket con la conexión a utilizar */
	socket_type m_connection;
	
	/** Cuerpo de la petición */
	// FIXME: ¿Qué pasa si esto es realmente enorme?
	std::stringstream m_body;
	
	/** Longitud del cuerpo */
	int m_contentLength;
	
	/** Cantidad de cuerpo que queda por leer */
	int m_remain;
};

NS_CUSTER_END

#endif