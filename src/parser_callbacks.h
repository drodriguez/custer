#ifndef PARSER_CALLBACKS_H
#define PARSER_CALLBACKS_H

#include "custer.h"

#define DEF_MAX_LENGTH(N, length) \
	const size_t MAX_##N##_LENGTH = length; \
	const char* MAX_##N##_LENGTH_ERR = "El elemento HTTP " # N " es más grande que la longitud permitida " # length ".";
#define VALIDATE_MAX_LENGTH(len, N) \
	if (len > MAX_##N##_LENGTH) { \
		error(MAX_##N##_LENGTH_ERR); \
	}

NS_CUSTER_BEGIN

/**
 * Método que el parser llama cuando identifica el método de la petición.
 *
 * @param data Estructura de datos opaca del usuario.
 * @param at Donde comienza el nombre del método.
 * @param length Longitud del nombre del método.
 */
void request_method_cb(void* data, const char* at, size_t length);

/**
 * Método que el parser llama cuando identifica un campo de una cabecera HTTP.
 *
 * @param data Estructura de datos opaca del usuario.
 * @param field Donde comienza el nombre del campo.
 * @param flen Longitud del nombre del campo.
 * @param value Donde comienzan los contenidos del campo.
 * @param vlen Longitud de los contenidos del campo.
 */
void http_field_cb(
	void* data,
	const char* field,
	size_t flen,
	const char* value,
	size_t vlen);

/**
 * Método que el parser llama cuando identifica la URI de la petición.
 *
 * @param data Estructura de datos opaca del usuario.
 * @param at Donde comienza la URI.
 * @param lenght Longitud de la URI.
 */
void request_uri_cb(void* data, const char* at, size_t length);

/**
 * Método que el parser llama cuando identifica el fragmento de la petición.
 *
 * @param data Estructura de datos opaca del usuario.
 * @param at Donde comienza el fragmento.
 * @param lenght Longitud del fragmento.
 */
void fragment_cb(void* data, const char* at, size_t length);

/**
 * Método que el parser llama cuando identifica la ruta de la petición.
 *
 * @param data Estructura de datos opaca del usuario.
 * @param at Donde comienza la ruta.
 * @param lenght Longitud de la ruta.
 */
void request_path_cb(void* data, const char* at, size_t length);

/**
 * Método que el parser llama cuando identifica la cadena de consulta de la
 * petición.
 *
 * @param data Estructura de datos opaca del usuario.
 * @param at Donde comienza la cadena de consulta.
 * @param lenght Longitud de la cadena de consulta.
 */
void query_string_cb(void* data, const char* at, size_t length);

/**
 * Método que el parser llama cuando identifica la versión HTTP de la
 * petición.
 *
 * @param data Estructura de datos opaca del usuario.
 * @param at Donde comienza la versión HTTP.
 * @param lenght Longitud de la versión HTTP.
 */
void http_version_cb(void* data, const char* at, size_t length);

/**
 * Método que el parser llama cuando se finaliza la lectura de las cabeceras.
 *
 * @param data Estructura de datos opaca del usuario.
 * @param at Donde comienza el cuerpo.
 * @param lenght Longitud del cuerpo, por ahora.
 */
void header_done_cb(void* data, const char* at, size_t length);

NS_CUSTER_END

#endif