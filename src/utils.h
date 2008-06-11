#ifndef UTILS_H
#define UTILS_H

#define FATAL 0
#define ERROR 1
#define WARN  2
#define INFO  3
#define DEBUG 4

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
 * fallando en caso contrario. Ver llamada librería estándar C malloc.
 */
void* xmalloc(unsigned long size);

/**
 * Intenta aumentar la zona de memoria de un puntero del heap, recolocandola
 * si es necesario y fallando en caso de que no se pueda. Ver llamada
 * librería estándar C realloc.
 */
void* xrealloc(void* ptr, unsigned long size);

#endif