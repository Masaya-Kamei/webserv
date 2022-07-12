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

void HTTPMethod::ParseReq(HTTPRequest req)
{
	http_ = req.GetVersion();
	method_ = req.GetMethod();
	target_ = req.GetTarget();
	connection_ = true;
	status_code_ = 200;
}

int HTTPMethod::ExecHTTPMethod(HTTPRequest req, const ServerDirective &server)
{
	ParseReq(req);
	const URI uri(target_, server, method_);
	void (HTTPMethod::*funcs[])(HTTPRequest::e_method, const URI&) = {
		&HTTPMethod::HandleFile
		// &HTTPMethod::HandleAutoIndex,
		// &HTTPMethod::HandleRedirect,
		// &HTTPMethod::HandleCGI
	};
	(this->*funcs[uri.GetType()])(method_, uri);
	return status_code_;
}

void HTTPMethod::HandleFile(HTTPRequest::e_method method_, const URI &uri)
{
	if (method_ == HTTPRequest::DELETE)
	{
		int ret = unlink(uri.GetPath().c_str());
		if (ret != 0)
		{
			throw HTTPError(500);
		}
	}
	else
	{
		ifs_.open(uri.GetPath().c_str());
		if (!ifs_)
		{
			throw HTTPError(404);
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

HTTPRequest::e_method HTTPMethod::GetMethod() const
{
	return method_;
}
