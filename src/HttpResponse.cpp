#include "HttpResponse.h"
#include "const.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include <cerrno>
#include <cstring>

using namespace custer;

namespace bfs = boost::filesystem;
namespace bpt = boost::posix_time;
namespace bgr = boost::gregorian;

static char* RFC2822_DAY_NAME[] =
	{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static char* RFC2822_MONTH_NAME[] =
	{ "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

static std::string toHTTPDate(boost::posix_time::ptime time)
{
	bgr::date dt = time.date();
	bpt::time_duration td = time.time_of_day();
	
	std::stringstream s;
	s << RFC2822_DAY_NAME[dt.day_of_week()] << ", "
		<< std::setw(2) << std::setfill('0') << dt.day() << " "
		<< RFC2822_MONTH_NAME[dt.month()] << " "
		<< dt.year() << " "
		<< std::setw(2) << std::setfill('0') << td.hours() << ":"
		<< std::setw(2) << std::setfill('0') << td.minutes() << ":"
		<< std::setw(2) << std::setfill('0') << td.seconds() << " GMT";
	return s.str();
}

HttpResponse::HttpResponse(socket_type connection) :
	m_connection(connection),
	m_status(HTTP_STATUS_NOT_FOUND),
	m_bodySent(false),
	m_headersSent(false),
	m_statusSent(false),
	m_httpMessagePos(0)
{
	bpt::ptime now = bpt::second_clock::universal_time();
	m_headers[HTTP_DATE] = toHTTPDate(now);
}

void HttpResponse::send()
{
	sendStatus();
	sendHeaders();
	sendBody();
}

void HttpResponse::sendStatus(long contentLength)
{
	if (m_statusSent) return;
	debug("HttpResponse::sendStatus");
	
	
	// TODO: calcular la longitud del body
	if (contentLength < 0) contentLength = 0;
	
	if (contentLength > 0 && m_status != HTTP_STATUS_NOT_MODIFIED) {
		setHeader(
			HTTP_CONTENT_LENGTH,
			boost::lexical_cast<std::string>(contentLength));
	}
	
	debug("Status: %d %s", m_status, getReason().c_str());
	m_httpMessage << HTTP_STATUS_FORMAT(m_status, getReason());
	
	m_statusSent = true;
}

void HttpResponse::sendHeaders()
{
	if (m_headersSent) return;
	debug("HttpResponse::sendHeaders");
	
	HeadersMap::iterator iter;
	for (iter = m_headers.begin(); iter != m_headers.end(); ++iter) {
		debug("HEADER: %s: %s", iter->first.c_str(), iter->second.c_str());
		m_httpMessage << HTTP_HEADER_FORMAT(iter->first, iter->second);
	}
	m_httpMessage << HTTP_LINE_END;
	
	m_headersSent = true;
}

void HttpResponse::sendBody()
{
	if (m_bodySent) return;
	debug("HttpResponse::sendBody");
	
	
	// TODO: enviar el body o el file.
	
	m_bodySent = true;
}

void HttpResponse::sendFile(boost::filesystem::path filePath)
{
	// TODO
}

void HttpResponse::handleWrite()
{
	char buffer[CHUNK_SIZE];
	int nw, nr;
	
	if (!m_httpMessage.eof()) {
		debug("Leyendo parte de las cabeceras");
		m_httpMessage.read(buffer, CHUNK_SIZE);
		nr = m_httpMessage.gcount();
	} else {
		debug("Leyendo out");
		out.read(buffer, CHUNK_SIZE);
		nr = out.gcount();
	}
	debug("leidos %d bytes", nr);
	
	if ((nw = write(m_connection, buffer, nr)) == -1) {
		fatal("escribiendo en el socket: %s", strerror(errno));
	}
	
	debug("escritos %d bytes", nw);
	
	if (m_httpMessage.eof()) debug("al final de m_httpMessage");

	if (nw != nr) {
		error("nw y nr son diferentes: nw = %d, nr = %d", nw, nr);
	}
	
	if (out.eof()) {
		debug("cerrando el socket");
		close(m_connection);
	}
}

#define HTTP_STATUS_REASON(x) case x: return HTTP_STATUS_REASON_##x;

std::string HttpResponse::getReason()
{
	if (m_reason.empty()) {
		switch(m_status) {
		HTTP_STATUS_REASON(100)
		HTTP_STATUS_REASON(101)
		HTTP_STATUS_REASON(200)
		HTTP_STATUS_REASON(201)
		HTTP_STATUS_REASON(202)
		HTTP_STATUS_REASON(203)
		HTTP_STATUS_REASON(204)
		HTTP_STATUS_REASON(205)
		HTTP_STATUS_REASON(206)
		HTTP_STATUS_REASON(300)
		HTTP_STATUS_REASON(301)
		HTTP_STATUS_REASON(302)
		HTTP_STATUS_REASON(303)
		HTTP_STATUS_REASON(304)
		HTTP_STATUS_REASON(305)
		HTTP_STATUS_REASON(400)
		HTTP_STATUS_REASON(401)
		HTTP_STATUS_REASON(402)
		HTTP_STATUS_REASON(403)
		HTTP_STATUS_REASON(404)
		HTTP_STATUS_REASON(405)
		HTTP_STATUS_REASON(406)
		HTTP_STATUS_REASON(407)
		HTTP_STATUS_REASON(408)
		HTTP_STATUS_REASON(409)
		HTTP_STATUS_REASON(410)
		HTTP_STATUS_REASON(411)
		HTTP_STATUS_REASON(412)
		HTTP_STATUS_REASON(413)
		HTTP_STATUS_REASON(414)
		HTTP_STATUS_REASON(415)
		HTTP_STATUS_REASON(500)
		HTTP_STATUS_REASON(501)
		HTTP_STATUS_REASON(502)
		HTTP_STATUS_REASON(503)
		HTTP_STATUS_REASON(504)
		HTTP_STATUS_REASON(505)
		}
	}
	return m_reason;
}

#undef HTTP_STATUS_REASON

void HttpResponse::setHeader(std::string header, std::string value)
{
	m_headers[header] = value;
}