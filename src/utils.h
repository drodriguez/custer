#ifndef UTILS_H
#define UTILS_H

/**
 * Imprime un mensaje de error fatal y sale de la aplicación.
 */
void fatal(char* fmt, ...);

/**
 * Imprime un mensaje de error "recuperable".
 */
void error(char* fmt, ...);

/**
 * Imprime una advertencia.
 */
void warn(char* fmt, ...);

/**
 * Imprime un mensaje de información.
 */
void info(char* fmt, ...);

/**
 * Imprime un mensaje de depuración.
 */
void debug(char* fmt, ...);

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