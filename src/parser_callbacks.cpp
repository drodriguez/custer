#include "parser_callbacks.h"
#include "const.h"
#include "HttpRequest.h"

// TODO: VALIDATE_LENGTH en todos estas funciones

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
	debug("Request URI: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_REQUEST_URI, std::string(at, length)));
}

void custer::fragment_cb(void* data, const char* at, size_t length)
{
	debug("Fragment: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_FRAGMENT, std::string(at, length)));
}

void custer::request_path_cb(void* data, const char* at, size_t length)
{
	debug("Request path: %.*s", length, at);
	ParamsMap* map =
		static_cast<ParamsMap*>(data);
	map->insert(std::make_pair(HTTP_REQUEST_PATH, std::string(at, length)));
}

void custer::query_string_cb(void* data, const char* at, size_t length)
{
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
