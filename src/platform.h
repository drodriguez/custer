#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(__APPLE__)
#  include <sys/event.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
	typedef int socket_type;
#  define socketRead(s,b,l) ::read((s), (b), (l))
#  define socketWrite(s,b,l) ::write((s), (b), (l))
#  define socketClose(s) ::close((s))
#  define SOCKET_ERROR -1
#  define socketSetOpt(s,l,n,v,sz) ::setsockopt((s),(l),(n),(v),(sz))
#  include "KQueueDispatcher.h"

#elif defined(__BSD__)
#  include <sys/event.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
	typedef int socket_type;
#  define socketRead(s,b,l) ::read((s), (b), (l))
#  define socketWrite(s,b,l) ::write((s), (b), (l))
#  define socketClose(s) ::close((s))
#  define SOCKET_ERROR -1
#  define socketSetOpt(s,l,n,v,sz) ::setsockopt((s),(l),(n),(v),(sz))
#  include "KQueueDispatcher.h"

#elif defined(WIN32)
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  undef PUT
#  undef DELETE
	typedef unsigned short uint16_t;
	typedef unsigned int uint32_t;
	typedef SOCKET socket_type;
#  define socketRead(s,b,l) ::recv((s), (b), (l), 0)
#  define socketWrite(s,b,l) ::send((s), (b), (l), 0)
#  define socketClose(s) ::closesocket((s))
#  define socketSetOpt(s,l,n,v,sz) ::setsockopt((s),(l),(n),(char*)(v),(sz))
#  include "SelectDispatcher.h"

#elif defined(__linux__)
#  include <sys/poll.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
	typedef int socket_type;
#  define socketRead(s,b,l) ::read((s), (b), (l))
#  define socketWrite(s,b,l) ::write((s), (b), (l))
#  define socketClose(s) ::close((s))
#  define SOCKET_ERROR -1
#  define socketSetOpt(s,l,n,v,sz) ::setsockopt((s),(l),(n),(v),(sz))
// Linux debería funcionar también con SelectDispatcher.h
#  include "EpollDispatcher.h"
#else
#  error "No hay dispatcher asociado a esta plataforma"
#endif

#endif
