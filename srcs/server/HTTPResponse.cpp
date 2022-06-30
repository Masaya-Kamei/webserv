#include <sstream>
#include "HTTPResponse.hpp"

const std::map<int, std::string> HTTPResponse::STATUS_MSG = SetStatusMsg();

HTTPResponse::HTTPResponse(): sent_byte_(0)
{
}

HTTPResponse::~HTTPResponse()
{
}

void HTTPResponse::Clear()
{
	headers_.clear();
	res_msg_.clear();
	sent_byte_ = 0;
	body_.clear();
}

std::string HTTPResponse::CreateResponse(std::string path)
{
	bool keep_alive = true;

	if (path == "/index.html")
	{
		SetStatusCode(CODE_200);
	} else {
		SetStatusCode(CODE_409);
	}
	SetResponse(keep_alive);
	return res_msg_;
}

void HTTPResponse::SetResponse(bool keep_alive)
{
	connection_ = keep_alive;
	res_msg_ = ToString();
}

void HTTPResponse::SetStatusCode(int status_code)
{
	status_code_ = status_code;
}

void HTTPResponse::SetHeader(const std::pair<std::string, std::string> &elem)
{
	headers_.insert(elem);
}

void HTTPResponse::SetBody(const std::string &body)
{
	body_ = body;
}

void HTTPResponse::SetSentByte(size_t sent_byte)
{
	sent_byte_ = sent_byte;
}

int HTTPResponse::GetStatusCode() const
{
	return status_code_;
}

const std::map<std::string, std::string> HTTPResponse::GetHeaders() const
{
	return headers_;
}

const std::string &HTTPResponse::GetMessage() const
{
	return res_msg_;
}

size_t HTTPResponse::GetSentByte() const
{
	return sent_byte_;
}

/* private */
std::string HTTPResponse::ToString()
{
	std::string status_msg = FindStatusMassage(status_code_);
	std::stringstream ss;

	SetHeaderField();
	ss << "HTTP/1.1 " << status_code_ << " " << status_msg << "\r\n";
	for (std::map<std::string, std::string>::iterator ite = headers_.begin();
		 ite != headers_.end(); ite++)
	{
		ss << ite->first << ": " << ite->second << "\r\n";
	}
	ss << "\r\n";
	ss << body_;
	return ss.str();
}

std::map<int, std::string> HTTPResponse::SetStatusMsg()
{
	std::map<int, std::string> status_msg;

	status_msg[CODE_100] = "Continue";
    status_msg[CODE_101] = "Switching Protocols";
    status_msg[CODE_102] = "Processing";
    status_msg[CODE_103] = "Early Hints";
    status_msg[CODE_200] = "OK";
    status_msg[CODE_201] = "Created";
    status_msg[CODE_202] = "Accepted";
    status_msg[CODE_203] = "Non-Authoritative Information";
    status_msg[CODE_204] = "No Content";
    status_msg[CODE_205] = "Reset Content";
    status_msg[CODE_206] = "Partial Content";
    status_msg[CODE_207] = "Multi-Status";
    status_msg[CODE_208] = "Already Reported";
    status_msg[CODE_226] = "IM Used";
    status_msg[CODE_300] = "Multiple Choice";
    status_msg[CODE_301] = "Moved Permanently";
    status_msg[CODE_302] = "Found";
    status_msg[CODE_303] = "See Other";
    status_msg[CODE_304] = "Not Modified";
    status_msg[CODE_307] = "Temporary Redirect";
    status_msg[CODE_308] = "Permanent Redirect";
    status_msg[CODE_400] = "Bad Request";
    status_msg[CODE_401] = "Unauthorized";
    status_msg[CODE_402] = "Payment Required";
    status_msg[CODE_403] = "Forbidden";
    status_msg[CODE_404] = "Not Found";
    status_msg[CODE_405] = "Method Not Allowed";
    status_msg[CODE_406] = "Not Acceptable";
    status_msg[CODE_407] = "Proxy Authentication Required";
    status_msg[CODE_408] = "Request Timeout";
    status_msg[CODE_409] = "Conflict";
    status_msg[CODE_410] = "Gone";
    status_msg[CODE_411] = "Length Required";
    status_msg[CODE_412] = "Precondition Failed";
    status_msg[CODE_413] = "Payload Too Large";
    status_msg[CODE_414] = "URI Too Long";
    status_msg[CODE_415] = "Unsupported Media Type";
    status_msg[CODE_416] = "Range Not Satisfiable";
    status_msg[CODE_417] = "Expectation Failed";
    status_msg[CODE_418] = "I'm a teapot";
    status_msg[CODE_421] = "Misdirected Request";
    status_msg[CODE_422] = "Unprocessable Entity";
    status_msg[CODE_423] = "Locked";
    status_msg[CODE_424] = "Failed Dependency";
    status_msg[CODE_425] = "Too Early";
    status_msg[CODE_426] = "Upgrade Required";
    status_msg[CODE_428] = "Precondition Required";
    status_msg[CODE_429] = "Too Many Requests";
    status_msg[CODE_431] = "Request Header Fields Too Large";
    status_msg[CODE_451] = "Unavailable For Legal Reasons";
    status_msg[CODE_500] = "Internal Server Error";
    status_msg[CODE_501] = "Not Implemented";
    status_msg[CODE_502] = "Bad Gateway";
    status_msg[CODE_503] = "Service Unavailable";
    status_msg[CODE_504] = "Gateway Timeout";
    status_msg[CODE_505] = "HTTP Version Not Supported";
    status_msg[CODE_506] = "Variant Also Negotiates";
    status_msg[CODE_507] = "Insufficient Storage";
    status_msg[CODE_508] = "Loop Detected";
    status_msg[CODE_510] = "Not Extended";
    status_msg[CODE_511] = "Network Authentication Required";
    return status_msg;
}

std::string HTTPResponse::FindStatusMassage(int status_code) const
{
	std::map<int, std::string>::const_iterator ite = this->STATUS_MSG.find(status_code);
	if (ite != STATUS_MSG.end())
	{
		return ite->second;
	}
	return "";
}

/* 
	// HTTP/1.1 200 OK
	// Server: nginx/1.11.8
	// Date: Tue, 17 Jan 2017 17:42:27 GMT
	Content-Type: application/json
	// Content-Length: 612
	Last-Modified: Tue, 17 Jan 2017 14:48:14 GMT
	// Connection: keep-alive
	ETag: "587e2eae-264"
	Accept-Ranges: bytes
*/

void HTTPResponse::SetHeaderField()
{
	std::stringstream ss;
	ss << body_.size();
	std::string content_length = ss.str();

	headers_.insert(std::make_pair("Server", "webserv"));
	headers_.insert(std::make_pair("Date", GetDate()));
	// headers_.insert(std::make_pair("Content-Type", ""));
	headers_.insert(std::make_pair("Content-Length", content_length));
	if (connection_)
	{
		headers_.insert(std::make_pair("Connection", "keep-alive"));
	} else {
		headers_.insert(std::make_pair("Connection", "close"));
	}
}

std::string HTTPResponse::GetDate() const
{
	time_t now = time(NULL);
	struct tm current_time;
	char str[50];

	asctime_r(localtime_r(&now, &current_time), str);
	strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S GMT", &current_time);
    return str;
}
