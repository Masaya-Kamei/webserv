#include "HTTPMethod.hpp"
#include "HTTPError.hpp"

const size_t HTTPMethod::BUF_SIZE = 8192;

HTTPMethod::HTTPMethod()
{
}

HTTPMethod::~HTTPMethod()
{
}

int HTTPMethod::ExecHTTPMethod()
{
	status_code_ = 200;

	ParseReq();
	HandleFile();
	ReadFile();
	return status_code_;
}

void HTTPMethod::HandleFile()
{
	if (method_ == "GET")
	{
		ifs_.open(path_.c_str());
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

/* set request */
void HTTPMethod::ParseReq()
{
	method_ = "GET";
	http_ = "HTTP/1.1";
	path_ = "html/index.html";
	// path_ = "html/no.html";
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
