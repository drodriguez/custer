#include "utils.h"

#include <cstdlib>
#include <cstdarg>
#include <cstdio>

using namespace std;

static void vlog(char* fmt, va_list ap)
{
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
}

#define LOG_FATAL_MSG "FATAL"
#define LOG_ERROR_MSG "ERROR"
#define LOG_WARN_MSG  "WARN "
#define LOG_INFO_MSG  "INFO "
#define LOG_DEBUG_MSG "DEBUG"

unsigned int logLevel = LOG_WARN;

#define LOG_WITH_LEVEL(x) \
	if (logLevel < (x)) return; \
	va_list ap; \
	va_start(ap, fmt); \
	fprintf(stderr, "custer [" x##_MSG "]: "); \
	vlog(fmt, ap); \
	va_end(ap);

void fatal(char* fmt, ...)
{
	LOG_WITH_LEVEL(LOG_FATAL)
	exit(-1);
}

void error(char* fmt, ...)
{
	LOG_WITH_LEVEL(LOG_ERROR)
}

void warn(char* fmt, ...)
{
	LOG_WITH_LEVEL(LOG_WARN)
}

void info(char* fmt, ...)
{
	LOG_WITH_LEVEL(LOG_INFO)
}

void debug(char* fmt, ...)
{
	LOG_WITH_LEVEL(LOG_DEBUG)
}

void* xmalloc(unsigned long size)
{
	void* result = malloc(size);
	
	if (!result)
		fatal("Memoria agotada");
	
	return result;
}

void* xrealloc(void* ptr, unsigned long size)
{
	void *result = realloc(ptr, size);
	
	if (!result)
		fatal("Memoria agotada");
	
	return result;
}
