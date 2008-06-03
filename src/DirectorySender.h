#ifndef DIRECTORYSENDER_H
#define DIRECTORYSENDER_H

#include "custer.h"

#include <boost/filesystem/path.hpp>

NS_CUSTER_BEGIN

class DirectorySender
{
public:
	/**
	 * Constructor para que se envien los archivos del directorio indicado.
	 *
	 * @param directory El directorio base que se servirá.
	 * @param allowListing En el caso de pedir un directorio sin archivo
	 *                     índice, determina si se mostrarán los contenidos
	 *                     del directorio.
	 * @param indexFile Nombre del archivo que servirá como archivo índice.
	 */
	DirectorySender(
		std::string directory,
		bool allowListing = true,
		std::string indexFile);
	
	/**
	 * Procesa una petición HTTP y almacena su respuesta para ser enviada.
	 *
	 * @param request Petición HTTP.
	 * @param response Respuesta HTTP.
	 */
	void process(
		boost::shared_ptr<HttpRequest> request,
		boost::shared_ptr<HttpResponse> response);
	
private:
	/**
	 * Indica si determinada ruta puede ser servida por el directorio.
	 *
	 * @param pathInfo Ruta que se desea comprobar.
	 * @return El archivo a servir o una cadena en blanco.
	 */
	boost::filesystem::path canServe(std::string pathInfo);
	
	/**
	 * Devuelve un listado del directorio muy simple si se permite.
	 *
	 * @param base Directorio de la URI, para que los archivos se puedan
	 *             enlazar correctamente.
	 * @param directory Directorio en el sistema de ficheros.
	 * @param response Respuesta HTTP.
	 */
	void sendDirectoryListing(
		std::string base,
		boost::filesystem::path directory,
		boost::shared_ptr<HttpResponse> response);
	
	/**
	 * Envia los contenidos de un archivo al usuario.
	 *
	 * @param requestPath Ruta al archivo en el sistema de ficheros.
	 * @param request Petición HTTP.
	 * @param response Respuesta HTTP.
	 */
	void sendFile(
		boost::filesystem::path requestPath,
		boost::shared_ptr<HttpResquest> request,
		boost::shared_ptr<HttpResponse> response);
		
	
	/** Indica si se permiten los listados de los directorios */
	bool m_allowListing;
	
	/** Directorio base que se servirá */
	boost::filesystem::path m_directory;
	
	/** Nombre del archivo índice */
	boost::filesystem::path m_indexFile;
};

NS_CUSTER_END

#endif