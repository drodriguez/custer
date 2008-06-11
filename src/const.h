#ifndef CONST_H
#define CONST_H

#define HTTP_PATH_INFO "PATH_INFO"
#define HTTP_SCRIPT_NAME "SCRIPT_NAME"
#define HTTP_REQUEST_URI "REQUEST_URI"
#define HTTP_REQUEST_PATH "REQUEST_PATH"
#define HTTP_CONTENT_LENGTH "Content-Length"
#define HTTP_REQUEST_METHOD "REQUEST_METHOD"
#define HTTP_REMOTE_ADDR "REMOTE_ADDR"
#define HTTP_HOST "HOST"
#define HTTP_SLASH "/"


#define CHUNK_SIZE (16 * 1024)

#define HTTP_CONTENT_TYPE "Content-Type"
#define HTTP_LAST_MODIFIED "Last-Modified"
#define HTTP_USER_AGENT "User-Agent"
#define HTTP_FRAGMENT "Fragment"
#define HTTP_QUERY_STRING "Query-String"
#define HTTP_VERSION "Version"
#define HTTP_BODY "Body"
#define HTTP_DATE "Date"

#define HTTP_STATUS_FORMAT(s, r) \
	"HTTP/1.1 " << s << " " << r << "\r\nConnection: close\r\n"
#define HTTP_HEADER_FORMAT(k, v) k << ": " << v << "\r\n"
#define HTTP_LINE_END "\r\n"

#define HTTP_STATUS_CONTINUE                      100
#define HTTP_STATUS_SWITCHING_PROTOCOLS           101
#define HTTP_STATUS_OK                            200
#define HTTP_STATUS_CREATED                       201
#define HTTP_STATUS_ACCEPTED                      202
#define HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION 203
#define HTTP_STATUS_NO_CONTENT                    204
#define HTTP_STATUS_RESET_CONTENT                 205
#define HTTP_STATUS_PARTIAL_CONTENT               206
#define HTTP_STATUS_MULTIPLE_CHOICES              300
#define HTTP_STATUS_MOVED_PERMANENTLY             301
#define HTTP_STATUS_MOVED_TEMPORARILY             302
#define HTTP_STATUS_SEE_OTHER                     303
#define HTTP_STATUS_NOT_MODIFIED                  304
#define HTTP_STATUS_USE_PROXY                     305
#define HTTP_STATUS_BAD_REQUEST                   400
#define HTTP_STATUS_UNAUTHORIZED                  401
#define HTTP_STATUS_PAYMENT_REQUIRED              402
#define HTTP_STATUS_FORBIDDEN                     403
#define HTTP_STATUS_NOT_FOUND                     404
#define HTTP_STATUS_METHOD_NOT_ALLOWED            405
#define HTTP_STATUS_NOT_ACCEPTABLE                406
#define HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED 407
#define HTTP_STATUS_REQUEST_TIME_OUT              408
#define HTTP_STATUS_CONFLICT                      409
#define HTTP_STATUS_GONE                          410
#define HTTP_STATUS_LENGTH_REQUIRED               411
#define HTTP_STATUS_PRECONDITION_FAILED           412
#define HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE      413
#define HTTP_STATUS_REQUEST_URI_TOO_LARGE         414
#define HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE        415
#define HTTP_STATUS_INTERNAL_SERVER_ERROR         500
#define HTTP_STATUS_NOT_IMPLEMENTED               501
#define HTTP_STATUS_BAD_GATEWAY                   502
#define HTTP_STATUS_SERVICE_UNAVAILABLE           503
#define HTTP_STATUS_GATEWAY_TIMEOUT               504
#define HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED    505

#define HTTP_STATUS_REASON_100 "Continue"
#define HTTP_STATUS_REASON_101 "Switching Protocols"
#define HTTP_STATUS_REASON_200 "OK"
#define HTTP_STATUS_REASON_201 "Created"
#define HTTP_STATUS_REASON_202 "Accepted"
#define HTTP_STATUS_REASON_203 "Non-Authoritative Information"
#define HTTP_STATUS_REASON_204 "No Content"
#define HTTP_STATUS_REASON_205 "Reset Content"
#define HTTP_STATUS_REASON_206 "Partial Content"
#define HTTP_STATUS_REASON_300 "Multiple Choices"
#define HTTP_STATUS_REASON_301 "Moved Permanently"
#define HTTP_STATUS_REASON_302 "Moved Temporarily"
#define HTTP_STATUS_REASON_303 "See Other"
#define HTTP_STATUS_REASON_304 "Not Modified"
#define HTTP_STATUS_REASON_305 "Use Proxy"
#define HTTP_STATUS_REASON_400 "Bad Request"
#define HTTP_STATUS_REASON_401 "Unauthorized"
#define HTTP_STATUS_REASON_402 "Payment Required"
#define HTTP_STATUS_REASON_403 "Forbidden"
#define HTTP_STATUS_REASON_404 "Not Found"
#define HTTP_STATUS_REASON_405 "Method Not Allowed"
#define HTTP_STATUS_REASON_406 "Not Acceptable"
#define HTTP_STATUS_REASON_407 "Proxy Authentication Required"
#define HTTP_STATUS_REASON_408 "Request Time-out"
#define HTTP_STATUS_REASON_409 "Conflict"
#define HTTP_STATUS_REASON_410 "Gone"
#define HTTP_STATUS_REASON_411 "Length Required"
#define HTTP_STATUS_REASON_412 "Precondition Failed"
#define HTTP_STATUS_REASON_413 "Request Entity Too Large"
#define HTTP_STATUS_REASON_414 "Request-URI Too Large"
#define HTTP_STATUS_REASON_415 "Unsupported Media Type"
#define HTTP_STATUS_REASON_500 "Internal Server Error"
#define HTTP_STATUS_REASON_501 "Not Implemented"
#define HTTP_STATUS_REASON_502 "Bad Gateway"
#define HTTP_STATUS_REASON_503 "Service Unavailable"
#define HTTP_STATUS_REASON_504 "Gateway Time-out"
#define HTTP_STATUS_REASON_505 "HTTP Version not supported"

#endif