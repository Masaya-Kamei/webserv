#include <unistd.h>
#include "HTTPMethod.hpp"
#include "HTTPError.hpp"
#include "URI.hpp"

const size_t HTTPMethod::BUF_SIZE = 8192;

HTTPMethod::HTTPMethod()
{
}

HTTPMethod::~HTTPMethod()
{
}

/* parse request */
void HTTPMethod::ParseReq(HTTPRequest req)
{
	// http_ = "HTTP/1.1";
	// uri_ = "html/index.html";
	// uri_ = "html/no.html";
	http_ = req.GetVersion();
	method_ = req.GetMethod();
	uri_ = req.GetTarget();
	connection_ =  true;
}

/* 
void HTTPMethod::setHeader(const std::pair<std::string, std::string> &elem)
{
    headers_.insert(elem);
} */

int HTTPMethod::ExecHTTPMethod(HTTPRequest req, const ServerDirective &server)
{
	ParseReq(req);
	URI uri(uri_, server, method_);

	switch (uri.GetType())
	{
		case URI::FILE:
			// HandleFile(req.GetMethod());
			HandleFile(method_);
			ReadFile();
		case URI::AUTOINDEX:
		case URI::REDIRECT:
		case URI::CGI:
		default:
			break;
	}
	return status_code_;
}

void HTTPMethod::HandleFile(int method)
{
	if (method == HTTPRequest::GET)
	{
		ifs_.open(uri_.c_str());
		if (!ifs_)
		{
			throw HTTPError(404);
		}
	}
	else if (method == HTTPRequest::POST)
	{
		ifs_.open(uri_.c_str());
	}
	else if (method == HTTPRequest::DELETE)
	{
		int ret = unlink(uri_.c_str());
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


int HTTPMethod::GetStatusCode() const
{
	return status_code_;
}

std::string HTTPMethod::GetUri() const
{
	return uri_;
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

HTTPRequest::e_method HTTPMethod::GetMethod()
{
	return method_;
}