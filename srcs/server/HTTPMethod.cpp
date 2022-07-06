#include <unistd.h>
#include "HTTPMethod.hpp"
#include "HTTPError.hpp"

const size_t HTTPMethod::BUF_SIZE = 8192;

HTTPMethod::HTTPMethod()
{
}

HTTPMethod::~HTTPMethod()
{
}

int HTTPMethod::ExecHTTPMethod(HTTPRequest req)
{
	ParseReq(req);
	HandleFile(method_);
	// HandleFile(req.GetMethod());
	ReadFile();
	return status_code_;
}

void HTTPMethod::HandleFile(int method)
{
	if (method == HTTPRequest::GET)
	{
		ifs_.open(path_.c_str());
		if (!ifs_)
		{
			throw HTTPError(404);
		}
	}
	else if (method == HTTPRequest::POST)
	{
		ifs_.open(path_.c_str());
	}
	else if (method == HTTPRequest::DELETE)
	{
		int ret = unlink(path_.c_str());
		if (ret != 0)
		{
			throw HTTPError(500);
		}
	}
}

void HTTPMethod::ReadFile()
{
	char buffer[BUF_SIZE];
	ssize_t read_byte;

	while (!ifs_.eof())
	{
		ifs_.read(buffer, BUF_SIZE - 1);
		read_byte = ifs_.gcount();
		buffer[read_byte] = '\0';
		AppendBody(buffer);
	}
	ifs_.close();
}

void HTTPMethod::AppendBody(const char *buffer)
{
	body_.append(buffer);
}

/* set request */
void HTTPMethod::ParseReq(HTTPRequest req)
{
	http_ = req.GetVersion();
	method_ = HTTPRequest::GET;
	http_ = "HTTP/1.1";
	// path_ = "html/index.html";
	path_ = "html/no.html";
	connection_ =  true;
}
/* 
void HTTPMethod::setHeader(const std::pair<std::string, std::string> &elem)
{
    headers_.insert(elem);
} */

int HTTPMethod::GetStatusCode() const
{
	return status_code_;
}

std::string HTTPMethod::GetPath() const
{
	return path_;
}

std::string HTTPMethod::GetHttp() const
{
	return http_;
}

std::string HTTPMethod::GetBody() const
{
	return body_;
}

bool HTTPMethod::GetConnection() const
{
	return connection_;
}
