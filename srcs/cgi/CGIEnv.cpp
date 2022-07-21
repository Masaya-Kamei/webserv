#include "CGIEnv.hpp"

CGIEnv::CGIEnv(const HTTPRequest& req)
	: req_(req)
{
	SetEnv();
}

CGIEnv::~CGIEnv(void)
{
}

void	CGIEnv::AddEnv(const std::string& key, const std::string& value)
{
	env_.push_back(key + "=" + value);
}

void	CGIEnv::SetEnv(void)
{
	AddEnv("AUTH_TYPE", "TEST");
	AddEnv("CONTENT_LENGTH", "TEST");
	AddEnv("CONTENT_TYPE", "TEST");
	AddEnv("GATEWAY_INTERFACE", "CGI/1.1");
	AddEnv("HTTP_ACCEPT", req_.GetContentType());
	//AddEnv("HTTP_REFERER", "TEST");
	AddEnv("HTTP_USER_AGENT", ret_.GetUserAgent());
	AddEnv("PATH_INFO", "TEST");
	AddEnv("PATH_TRANSLATED", "TEST");
	AddEnv("QUERY_STRING", "TEST");
	AddEnv("REMOTE_ADDR", "TEST");
	AddEnv("REMOTE_HOST", "TEST");
	AddEnv("REMOTE_IDENT", "TEST");
	AddEnv("REMOTE_USER", "TEST");
	AddEnv("REQUEST_METHOD", "TEST");
	AddEnv("SCRIPT_NAME", "TEST");
	AddEnv("SERVER_NAME", "TEST");
	AddEnv("SERVER_PORT", "TEST");
	AddEnv("SERVER_PROTOCOL", "HTTP/1.1");
	AddEnv("SERVER_SOFTWARE", "42Webserv");
}

char**	CGIEnv::GetEnv(void) const
{
	char**		exec_env;
	size_t		size;

	size = env_.size();
	exec_env = new char*[size + 1];
	for (size_t i = 0; i < size; i++)
		exec_env[i] = const_cast<char *>(env_[i].c_str());
	exec_env[size] = NULL;

	return (exec_env);
}
