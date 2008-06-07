#include "HttpRequest.h"
#include "const.h"

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>

using namespace custer;
namespace ba = boost::algorithm;


HttpRequest::HttpRequest(
	boost::shared_ptr<ParamsMap> params) :
	m_params(params),
	m_contentLength(0),
	m_remain(0)
{
	ParamsMap::iterator iter;
	if ((iter = m_params->find(HTTP_CONTENT_LENGTH)) != m_params->end()) {
		m_contentLength = boost::lexical_cast<int>(iter->second);
	}
	
	
	std::string& body = (*m_params)[HTTP_BODY];
	m_remain = m_contentLength - body.length();
	debug("Content-Length: %d (quedan %d)", m_contentLength, m_remain);
	
	m_body << body;
}

void HttpRequest::setParam(std::string key, std::string value)
{
	(*m_params)[key] = value;
}

HttpRequest::Method HttpRequest::getRequestMethod()
{
	std::string& rm = (*m_params)[HTTP_REQUEST_METHOD];
	
	if      (rm == "GET")    return GET;
	else if (rm == "POST")   return POST;
	else if (rm == "PUT")    return PUT;
	else if (rm == "DELETE") return DELETE;
	else if (rm == "HEAD")   return HEAD;
	else                     return GET; // FIX: Mejor peor elección
}

std::string HttpRequest::getRequestURI()
{
	return (*m_params)[HTTP_REQUEST_URI];
}

std::string HttpRequest::getRequestPath()
{
	return (*m_params)[HTTP_REQUEST_PATH];
}

std::string HttpRequest::getScriptName()
{
	return (*m_params)[HTTP_SCRIPT_NAME];
}

std::string HttpRequest::getPathInfo()
{
	return (*m_params)[HTTP_PATH_INFO];
}

void HttpRequest::handleRead(const char* buffer, size_t length)
{
	if (m_remain > 0) {
		m_body << std::string(buffer, length);
		m_remain -= length;
		debug("Aun quedan %d bytes por recibir", m_remain);
	}
}

static char* dec2hex = "0123456789ABCDEF";

std::string HttpRequest::escape(std::string s)
{
	std::string result;
	std::string::iterator lastPos = s.begin();
	std::string::iterator current = s.begin();
	
	while (current != s.end()) {
		unsigned char c = *current;
		if ((c >= 'A' && c <= 'Z')
			|| (c >= 'a' && c <= 'z')
			|| (c >= '0' && c <= '9')
			|| c == '_' || c == '.' || c == '-') {
			++current;
		} else if (c == ' ') {
			result.append(lastPos, current);
			result.append(1, '+');
			lastPos = ++current;
		} else {
			result.append(lastPos, current);
			result.append(1, '%');
			result.append(1, dec2hex[c >> 4]);
			result.append(1, dec2hex[c & 0x0f]);
			lastPos = ++current;
		}
	}
	
	result.append(lastPos, s.end());
	return result;
}

std::string HttpRequest::unescape(std::string s)
{
	std::string result;
	std::string::iterator lastPos = s.begin();
	std::string::iterator current = s.begin();
	
	while (current != s.end()) {
		unsigned char c = *current;
		if (c == '+') {
			result.append(lastPos, current);
			result.append(1, ' ');
			lastPos = ++current;
		} else if (c == '%') {
			unsigned char c1 = *(current+1);
			unsigned char c2 = *(current+2);
			
			if (isxdigit(c1) && isxdigit(c2)) {
				char nc = 0;
				if (c1 >> 6) nc += 0x09;
				nc += c1 & 0x0F;
				nc <<= 4;
				if (c2 >> 6) nc += 0x09;
				nc += c2 & 0x0F;
				result.append(lastPos, current);
				result.append(1, nc);
				lastPos = ++ ++ ++current;
			} else {
				++current;
			}
		} else {
			++current;
		}
	}
	
	result.append(lastPos, s.end());
	return result;
}
