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

void HttpRequest::handleRead(const char* buffer, size_t length)
{
	if (m_remain > 0) {
		m_body << std::string(buffer, length);
		m_remain -= length;
		debug("Aun queda %d bytes por recibir", m_remain);
	}
}

// A partir de aquí los métodos estáticos
// Cosas complicadas de comprender con el API de Regex de Boost.

class RegexFunction
{
public:
	RegexFunction(std::string * s);
	bool operator()(const boost::match_results<std::string::const_iterator>& what);
	virtual std::string gsub(const boost::match_results<std::string::const_iterator>& what) = 0;
	std::string getResult();

protected:
	std::string m_result;
	
private:
	std::string* m_s;
	std::string::iterator m_pos;
};

RegexFunction::RegexFunction(std::string* s) :
	m_s(s), m_result()
{
	m_pos = m_s->begin();
}

bool RegexFunction::operator()(const boost::match_results<std::string::const_iterator>& what)
{
	m_result.insert(m_result.end(), m_pos, m_pos+what.prefix().length());
	m_result += gsub(what);
	m_pos += what.prefix().length() + what.length();
}

std::string RegexFunction::getResult()
{
	m_result.insert(m_result.end(), m_pos, m_s->end());
	return m_result;
}

class EscapeRegexFunction : public RegexFunction
{
public:
	EscapeRegexFunction(std::string* s) : RegexFunction(s) {};
	std::string gsub(const boost::match_results<std::string::const_iterator>& what)
	{
		std::stringstream result;
		result << std::hex;
		std::string characters = what[1].str();
		std::string::const_iterator iter;
		for (iter = characters.begin(); iter != characters.end(); ++iter) {
			result << "%" << static_cast<short>(*iter);
		}
		return result.str();
	}
};

class UnescapeRegexFunction : public RegexFunction
{
public:
	UnescapeRegexFunction(std::string* s) : RegexFunction(s) {};
	std::string gsub(const boost::match_results<std::string::const_iterator>& what)
	{
		char * end;
		std::string digits = what[1].str();
		long int r1 = strtol(digits.c_str(), &end, 16);
		char r2 = static_cast<char>(r1);
		return std::string(&r2, 1);
	}
};

std::string HttpRequest::escape(std::string s)
{
	EscapeRegexFunction erf(&s);
	boost::regex re("([^ a-zA-Z0-9_.-]+)");
	boost::sregex_iterator iter(s.begin(), s.end(), re);
	boost::sregex_iterator endIter;
	std::string result = std::for_each(iter, endIter, erf).getResult();
	return ba::replace_all_copy(result, " ", "+");
}

std::string HttpRequest::unescape(std::string s)
{
	std::string st = ba::replace_all_copy(s, "+", " ");
	boost::regex re("%([0-9a-fA-F]{2})");
	UnescapeRegexFunction urf(&st);
	boost::sregex_iterator iter(st.begin(), st.end(), re);
	boost::sregex_iterator endIter;
	return std::for_each(iter, endIter, urf).getResult();
}
