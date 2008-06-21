#ifndef UTILS_H
#define UTILS_H

#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_WARN  2
#define LOG_INFO  3
#define LOG_DEBUG 4

extern unsigned int logLevel;

/**
 * Imprime un mensaje de error fatal y sale de la aplicación.
 */
void fatal(const char* fmt, ...);

/**
 * Imprime un mensaje de error "recuperable".
 */
void error(const char* fmt, ...);

/**
 * Imprime una advertencia.
 */
void warn(const char* fmt, ...);

/**
 * Imprime un mensaje de información.
 */
void info(const char* fmt, ...);

/**
 * Imprime un mensaje de depuración.
 */
void debug(const char* fmt, ...);

/**
 * Intenta crear una zona de memoria en el heap del tamaño especificado,
 * fallando en caso contrario. Ver llamada biblioteca estándar C malloc.
 */
void* xmalloc(unsigned long size);

/**
 * Intenta aumentar la zona de memoria de un puntero del heap, recolocandola
 * si es necesario y fallando en caso de que no se pueda. Ver llamada
 * biblioteca estándar C realloc.
 */
void* xrealloc(void* ptr, unsigned long size);

#endif