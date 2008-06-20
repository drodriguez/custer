#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(__APPLE__)
#  include <sys/event.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <cstdio>
#  include <cerrno>
#  include <cstring>
	typedef int socket_type;
#  define socketClose(s) ::close((s))
#  define SOCKET_ERROR -1
#  define socketSetOpt(s,l,n,v,sz) ::setsockopt((s),(l),(n),(v),(sz))
#  define ERROR_NUM errno
#  include "SelectDispatcher.h"

#elif defined(__BSD__)
#  include <sys/event.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <cstdio>
#  include <cerrno>
#  include <cstring>
	typedef int socket_type;
#  define socketClose(s) ::close((s))
#  define SOCKET_ERROR -1
#  define socketSetOpt(s,l,n,v,sz) ::setsockopt((s),(l),(n),(v),(sz))
#  define ERROR_NUM errno
#  include "KQueueDispatcher.h"

#elif defined(WIN32)
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  undef PUT
#  undef DELETE
	typedef unsigned short uint16_t;
	typedef unsigned int uint32_t;
	typedef SOCKET socket_type;
#  define socketClose(s) ::closesocket((s))
#  define socketSetOpt(s,l,n,v,sz) ::setsockopt((s),(l),(n),(char*)(v),(sz))
#  define ERROR_NUM WSAGetLastError()
#  ifndef FD_COPY
#    define FD_COPY(f, t) memcpy((t), (f), sizeof(*(f)))
#  endif
#  include "SelectDispatcher.h"

#elif defined(__linux__)
#  include <sys/poll.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <cstdio>
#  include <cerrno>
#  include <cstring>
	typedef int socket_type;
#  define socketClose(s) ::close((s))
#  define SOCKET_ERROR -1
#  define socketSetOpt(s,l,n,v,sz) ::setsockopt((s),(l),(n),(v),(sz))
#  define ERROR_NUM errno
#  include "EpollDispatcher.h"
#else
#  error "No hay dispatcher asociado a esta plataforma"
#endif

#endif
