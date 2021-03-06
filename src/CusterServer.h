#ifndef CUSTERSERVER_H
#define CUSTERSERVER_H

#include "custer.h"
#include "Dispatcher.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem/path.hpp>

#include <string>

NS_CUSTER_BEGIN

class DirectorySender;

class CusterServer : public boost::enable_shared_from_this<CusterServer>
{
public:
	/**
	 * Constructor en el que especificamos el puerto a escuchar y el
	 * directorio a servir.
	 *
	 * @param port Puerto en el que escuchar.
	 * @param directory Directorio que servir.
	 */
	CusterServer(uint16_t port, std::string directory);
	
	/**
	 * Devuelve el puerto en el que se escucha.
	 *
	 * @return El puerto en el que se escucha.
	 */
	unsigned int getPort() { return m_port; }
	
	/**
	 * Devuelve el manejador de envio de archivos del directorio base.
	 *
	 * @return El manejador de envio de archivos del directorio base.
	 */
	boost::shared_ptr<DirectorySender> getDirectorySender()
		{ return m_directorySender; }
		
	/**
	 * Ejecuta el servidor.
	 */
	void run();
	
private:
	/** Puerto en el que se escucha */
	unsigned int m_port;
		
	/** Maneja el envio de los archivos del directorio base */
	boost::shared_ptr<DirectorySender> m_directorySender;
	
	/** Dispatcher que gestiona las conexiones de este servidor */
	boost::shared_ptr<Dispatcher> m_dispatcher;
};

NS_CUSTER_END

#endif