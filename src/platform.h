#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(__APPLE__)
#  define NATIVE_DISPATCHER KQueueDispatcher
#  include "KQueueDispatcher.h"
#elif defined(__BSD__)
#  define NATIVE_DISPATCHER KQueueDispatcher
#  include "KQueueDispatcher.h"
#elif defined(WIN32)
#  define NATIVE_DISPATCHER SelectDispatcher
#  include "SelectDispatcher.h"
#elif defined(__linux__)
#  define NATIVE_DISPATCHER PollDispatcher
#  include "PollDispatcher.h"
#else
#  error "No hay dispatcher asociado a esta plataforma"
#endif

#define NativeDispatcher NATIVE_DISPATCHER

#endif