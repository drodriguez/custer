#include "utils.h"

#include <cstdlib>
#include <cstdio>
#include <cstdarg>

using namespace std;

static void vlog(char* fmt, va_list ap)
{
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
}

#define FATAL_MSG "FATAL"
#define ERROR_MSG "ERROR"
#define WARN_MSG  "WARN "
#define INFO_MSG  "INFO "
#define DEBUG_MSG "DEBUG"

unsigned int logLevel = WARN;

#define LOG_WITH_LEVEL(x) \
	if (logLevel < (x)) return; \
	va_list ap; \
	va_start(ap, fmt); \
	fprintf(stderr, "custer [" x##_MSG "]: "); \
	vlog(fmt, ap); \
	va_end(ap);

void fatal(char* fmt, ...)
{
	LOG_WITH_LEVEL(FATAL)
	exit(-1);
}

void error(char* fmt, ...)
{
	LOG_WITH_LEVEL(ERROR)
}

void warn(char* fmt, ...)
{
	LOG_WITH_LEVEL(WARN)
}

void info(char* fmt, ...)
{
	LOG_WITH_LEVEL(INFO)
}

void debug(char* fmt, ...)
{
	LOG_WITH_LEVEL(DEBUG)
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
