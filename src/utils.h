#ifndef UTILS_H
#define UTILS_H

void fatal(char* fmt, ...);
void error(char* fmt, ...);
void warn(char* fmt, ...);
void info(char* fmt, ...);
void debug(char* fmt, ...);

void* xmalloc(unsigned long size);
void* xrealloc(void* ptr, unsigned long size);

#endif