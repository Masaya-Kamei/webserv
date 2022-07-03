#include <sstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include "HTTPResponse.hpp"

const size_t HTTPResponse::BUF_SIZE = 8192;
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
	body_.clear();
	sent_byte_ = 0;
}

void HTTPResponse::SetRequest()
{
	rq_.insert(std::make_pair("method", "GET"));
	rq_.insert(std::make_pair("http", "HTTP/1.1"));
	// rq_.insert(std::make_pair("path", "html/index.html"));
	rq_.insert(std::make_pair("path", "html/no.html"));
	rq_.insert(std::make_pair("connection", "keep_alive"));
}

void HTTPResponse::Paser()
{
	// add header_elem
	ParseRqHeader();
	// body
	CreateBody();
}

void HTTPResponse::ParseRqHeader()
{
	path_ = rq_["path"];
	http_ = rq_["http"];
	if (rq_["connection"] == "keep_alive")
	{
		connection_ = true;
	} else {
		connection_ = false;
	}
	// SetHeader(make_pair());
}

void HTTPResponse::CreateBody()
{
	OpenFile();
	ReadFile();
}

void HTTPResponse::OpenFile()
{
	file_fd_ = open(path_.c_str(), O_RDONLY);
	if (file_fd_ < 0)
    {
        HandleError(CODE_404);
    } else {
        SetStatusCode(CODE_200);
	}
}

void HTTPResponse::CloseFile()
{
	close(file_fd_);
}

void HTTPResponse::ReadFile()
{
	char buffer[BUF_SIZE];
    ssize_t read_byte = read(file_fd_, buffer, BUF_SIZE - 1);

	if (read_byte < 0)
	{
        CloseFile();
        // HandleError(CODE_500);
        return;
    }
    if (read_byte == 0)
    {
        CloseFile();
        return;
    }
    buffer[read_byte] = '\0';
    AppendBody(buffer, read_byte);
	CloseFile();
}

void HTTPResponse::AppendBody(const char *buffer, size_t size)
{
	body_.append(buffer, size);
}

void HTTPResponse::HandleError(int statusCode)
{
    Clear();
    SetStatusCode(statusCode);
    SetBody(GenerateHTML(statusCode));
}

std::string HTTPResponse::GenerateHTML(int statusCode) const
{
    std::stringstream ss;

    std::string status_msg = this->STATUS_MSG.find(statusCode)->second;
    ss << "<html>" << "\r\n"
       << "<head><title>" << statusCode << " " << status_msg << "</title></head>" << "\r\n"
       << "<body>" << "\r\n"
       << "<center><h1>" << statusCode << " " << status_msg << "</h1></center>" << "\r\n"
       << "<hr><center>webserv</center>" << "\r\n"
       << "</body>" << "\r\n"
       << "</html>" << "\r\n";
    return ss.str();
}

std::string HTTPResponse::CreateResponse()
{
	SetRequest();
	Paser();
	SetResponse(connection_);
	return res_msg_;
}

void HTTPResponse::SetResponse(bool connection)
{
	connection_ = connection;
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

const std::string &HTTPResponse::GetResMessage() const
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
	ss << http_ << " " << status_code_ << " " << status_msg << "\r\n";
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
