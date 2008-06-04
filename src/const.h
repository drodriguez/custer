#ifndef CONST_H
#define CONST_H

#define HTTP_PATH_INFO "PATH_INFO"
#define HTTP_SCRIPT_NAME "SCRIPT_NAME"
#define HTTP_REQUEST_URI "REQUEST_URI"
#define HTTP_REQUEST_PATH "REQUEST_PATH"
#define HTTP_CONTENT_LENGTH "Content-Length"

#define CHUNK_SIZE (16 * 1024)

#define HTTP_MAX_HEADER (1024 * (80 + 32))
#define HTTP_MAX_BODY "HTTP_MAX_HEADER"

#define HTTP_STATUS_FORMAT "HTTP/1.1 %d %s\r\nConnection: close\r\n"
#define HTTP_CONTENT_TYPE "Content-Type"
#define HTTP_LAST_MODIFIED "Last-Modified"
#define HTTP_REQUEST_METHOD "REQUEST_METHOD"
#define HTTP_HEADER_FORMAT "%s: %s\r\n"
#define HTTP_LINE_END "\r\n"
#define HTTP_REMOTE_ADDR "REMOTE_ADDR"
#define HTTP_HOST "HOST"
#define HTTP_FRAGMENT "Fragment"
#define HTTP_QUERY_STRING "Query-String"
#define HTTP_VERSION "Version"
#define HTTP_BODY "Body"

#endif