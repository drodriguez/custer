#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(__APPLE__)
#  include <sys/event.h>
	typedef int socket_type;
#  include "KQueueDispatcher.h"
#elif defined(__BSD__)
#  include <sys/event.h>
	typedef int socket_type;
#  include "KQueueDispatcher.h"
#elif defined(WIN32)
#  include <winsock2.h>
	typedef SOCKET socket_type;
#  include "SelectDispatcher.h"
#elif defined(__linux__)
#  include <sys/poll.h>
	typedef int socket_type;
#  include "EpollDispatcher.h"
#else
#  error "No hay dispatcher asociado a esta plataforma"
#endif

#endif
