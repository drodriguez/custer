#include "http/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEF_MAX_LENGTH(N, length) const size_t MAX_##N##_LENGTH = length; const char *MAX_##N##_LENGTH_ERR = "HTTP element " # N " is longer that the " # length " allowed length."
#define VALIDATE_MAX_LENGTH(len, N) if (len > MAX_##N##_LENGTH) { fprintf(stderr, MAX_##N##_LENGTH_ERR); fputc('\n', stderr); }

DEF_MAX_LENGTH(FIELD_NAME, 256);
DEF_MAX_LENGTH(FIELD_VALUE, 80 * 1024);
DEF_MAX_LENGTH(REQUEST_URI, 1024 * 12);
DEF_MAX_LENGTH(FRAGMENT, 1024);
DEF_MAX_LENGTH(REQUEST_PATH, 1024);
DEF_MAX_LENGTH(QUERY_STRING, (1024 * 10));
DEF_MAX_LENGTH(HEADER, (1024 * (80 + 32)));

static const char *test_request =
	"GET / HTTP/1.1\r\n"
	"Host: www.google.com\r\n"
	"User-Agent: Mozilla/5.0 (Macintosh; U; Intel Mac OS X; es-ES; rv:1.8.1.11) Gecko/20071127 Firefox/2.0.0.11\r\n"
	"Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n"
	"Accept-Language: es-es,es;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
	"Accept-Encoding: gzip,deflate\r\n"
	"Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
	"Keep-Alive: 300\r\n"
	"Connection: keep-alive\r\n"
	"Referer: http://www.google.com/firefox?client=firefox-a&rls=org.mozilla:es-ES:official\r\n"
	"Cookie: SS=Q0=cmFnZWw; rememberme=false; PREF=ID=fd8958d0abb2fd9f:LD=en:CR=2:TM=1150072332:LM=1207516639:GM=1:S=fnWk0aAAMVkUa-fT\r\n"
	"Cache-Control: max-age=0\r\n\r\n";

void http_field_cb(void *data, const char *field, size_t flen, const char *value, size_t vlen)
{
	VALIDATE_MAX_LENGTH(flen, FIELD_NAME);
	VALIDATE_MAX_LENGTH(vlen, FIELD_VALUE);
	
	printf("Http field: %.*s : %.*s\n", flen, field, vlen, value);
}

void request_method_cb(void *data, const char *at, size_t length)
{
	printf("Request method: %.*s\n", length, at);
}

void request_uri_cb(void *data, const char *at, size_t length)
{
	VALIDATE_MAX_LENGTH(length, REQUEST_URI);
	
	printf("Request URI: %.*s\n", length, at);
}

void fragment_cb(void *data, const char *at, size_t length)
{
	VALIDATE_MAX_LENGTH(length, FRAGMENT);
	
	printf("Fragment: %.*s\n", length, at);
}

void request_path_cb(void *data, const char *at, size_t length)
{
	VALIDATE_MAX_LENGTH(length, REQUEST_PATH);
	
	printf("Request path: %.*s\n", length, at);
}

void query_string_cb(void *data, const char *at, size_t length)
{
	VALIDATE_MAX_LENGTH(length, QUERY_STRING);
	
	printf("Query string: %.*s\n", length, at);
}

void http_version_cb(void *data, const char *at, size_t length)
{
	printf("HTTP Version: %.*s\n", length, at);
}

void header_done_cb(void *data, const char *at, size_t length)
{
	if (length > 0) {
		printf("Body: %.*s\n", length, at);
	}
}

int main(int argc, char *argv[]) {
	http_parser *hp = (http_parser *) malloc(sizeof(http_parser));
	if (!hp) {
		fprintf(stderr, "Out of mem?");
		exit(-1);
	}
	hp->http_field = http_field_cb;
	hp->request_method = request_method_cb;
	hp->request_uri = request_uri_cb;
	hp->fragment = fragment_cb;
	hp->request_path = request_path_cb;
	hp->query_string = query_string_cb;
	hp->http_version = http_version_cb;
	hp->header_done = header_done_cb;
	http_parser_init(hp);
	
	http_parser_execute(hp, test_request, strlen(test_request), 0);
	
	VALIDATE_MAX_LENGTH(http_parser_nread(hp), HEADER);
	
	if (http_parser_has_error(hp)) {
		fprintf(stderr, "Invalid HTTP format.\n");
	} else {
		printf("Header size %d\n", http_parser_nread(hp));
	}
	
	http_parser_finish(hp);
	
	return 0;
}