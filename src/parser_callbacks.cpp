#include "parser_callbacks.h"
#include "const.h"
#include "HttpRequest.h"

DEF_MAX_LENGTH(FIELD_NAME, 256);
DEF_MAX_LENGTH(FIELD_VALUE, 80 * 1024);
DEF_MAX_LENGTH(REQUEST_URI, 1024 * 12);
DEF_MAX_LENGTH(FRAGMENT, 1024);
DEF_MAX_LENGTH(REQUEST_PATH, 1024);
DEF_MAX_LENGTH(QUERY_STRING, 1024 * 10);
	
void custer::request_method_cb(void* data, const char* at, size_t length)
{
	debug("Request method: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_REQUEST_METHOD, std::string(at, length)));
}

void custer::http_field_cb(
	void* data,
	const char* field,
	size_t flen,
	const char* value,
	size_t vlen)
{
	VALIDATE_MAX_LENGTH(flen, FIELD_NAME);
	VALIDATE_MAX_LENGTH(vlen, FIELD_VALUE);
	
	debug("HTTP field: %.*s: %.*s",
		flen,
		field,
		(vlen > 40 ? 40 : vlen),
		value);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(
		std::string(field, flen),
		std::string(value, vlen)
	));
}

void custer::request_uri_cb(void* data, const char* at, size_t length)
{
	VALIDATE_MAX_LENGTH(length, REQUEST_URI);
	
	debug("Request URI: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_REQUEST_URI, std::string(at, length)));
}

void custer::fragment_cb(void* data, const char* at, size_t length)
{
	VALIDATE_MAX_LENGTH(length, FRAGMENT);
	
	debug("Fragment: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_FRAGMENT, std::string(at, length)));
}

void custer::request_path_cb(void* data, const char* at, size_t length)
{
	VALIDATE_MAX_LENGTH(length, REQUEST_PATH);
	
	debug("Request path: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_REQUEST_PATH, std::string(at, length)));
}

void custer::query_string_cb(void* data, const char* at, size_t length)
{
	VALIDATE_MAX_LENGTH(length, QUERY_STRING);
	
	debug("Query string: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_QUERY_STRING, std::string(at, length)));
}

void custer::http_version_cb(void* data, const char* at, size_t length)
{
	debug("HTTP version: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_VERSION, std::string(at, length)));
}

void custer::header_done_cb(void* data, const char* at, size_t length)
{
	if (length <= 0) return;
	
	debug("Body: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_BODY, std::string(at, length)));
}
