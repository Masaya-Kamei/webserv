#include "CGIEnv.hpp"

CGIEnv::CGIEnv(const HTTPRequest& req, const ServerDirective& server_conf)
	: req_(req)
	: server_conf_(server_conf)
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
	AddEnv("REMOTE_HOST", "");
	AddEnv("REMOTE_IDENT", "");
	AddEnv("REMOTE_USER", "");
	AddEnv("REQUEST_METHOD", req_.GetMethod());
	AddEnv("SCRIPT_NAME", "TEST");
	AddEnv("SERVER_NAME", server_conf_.GetServerNames.at(0));
	AddEnv("SERVER_PORT", Utils::ToString(server_conf_.GetListen().second));
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