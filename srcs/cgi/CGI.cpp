#include "CGI.hpp"

CGI::CGI(const std::string& file_path, const HTTPRequest& req)
	: file_path_(file_path)
	, req_(req)
{
	ExecuteCGI();
	ParseCGI();
}

CGI::~CGI(void)
{
}

static void	pipe_set(int src, int dst, int not_use, bool child)
{
	if (close(not_use) < 0
		|| close(dst) < 0
		|| dup2(src, dst) < 0
		|| close(src) < 0)
	{
		if (child)
			std::exit(EXIT_FAILURE);
		else
			throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
	}
}

void	CGI::DoChild(const int pipe_fd[2])
{
	CGIEnv	env(req_);
	char*	argv[2];

	pipe_set(pipe_fd[1], 1, pipe_fd[0], true);

	argv[0] = const_cast<char *>(file_path_.c_str());
	argv[1] = NULL;

	if (execve(argv[0], argv, env.GetEnv()) < 0)
		std::exit(EXIT_FAILURE);
}

void	CGI::DoParent(const int pipe_fd[2], const pid_t pid)
{
	const size_t	buf_size = 4;
	char			buf[buf_size + 1];
	int				read_byte;
	pid_t			ret_pid;
	int				status;

	pipe_set(pipe_fd[0], 0, pipe_fd[1], false);

	while (1)
	{
		read_byte = read(0, buf, buf_size);
		if (read_byte < 0)
			throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
		if (read_byte == 0)
			break;
		buf[read_byte] = '\0';
		data_ += std::string(buf);
	}

	ret_pid = waitpid(pid, &status, 0);
	if (ret_pid < 0
		|| ret_pid != pid
		|| !WIFEXITED(status)
		|| WEXITSTATUS(status) == EXIT_FAILURE)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
}

void	CGI::ExecuteCGI(void)
{
	int		pipe_fd[2];
	pid_t	pid;

	if (pipe(pipe_fd) < 0 || (pid = fork()) < 0)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);

	if (pid == 0)
		DoChild(pipe_fd);
	else
		DoParent(pipe_fd, pid);

	return;
}

void	CGI::ParseHeader(const std::string& line)
{
	const std::pair<std::string, ParseFunc> p[] = {
		std::make_pair("Content-type", &CGI::ParseContentType)
	};
	const std::map<std::string, ParseFunc>				parse_funcs(p, &p[1]);
	std::map<std::string, ParseFunc>::const_iterator	found;
	std::string											field;
	std::string											content;
	std::string::size_type								pos;

	pos = line.find(":");
	if (pos == std::string::npos)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);

	field = line.substr(0, pos);
	content = line.substr(pos + 1);
	found = parse_funcs.find(field);
	if (found != parse_funcs.end())
		(this->*(found->second))(content);

	return;
}

void	CGI::ParseContentType(const std::string& content)
{
	content_type_ = Utils::MyTrim(content, " ");
}

void	CGI::ParseCGI(void)
{
	std::string::size_type	offset;
	std::string::size_type	pos;
	std::string				ret;
	std::string				line;

	offset = 0;
	while (1)
	{
		pos = data_.find("\n", offset);
		line = data_.substr(offset, pos - offset);
		offset = pos + 1;
		if (line == "")
			break;
		ParseHeader(line);
	}

	body_ = data_.substr(offset);
}

std::string		CGI::GetData(void) const { return (data_); }
std::string		CGI::GetContentType(void) const { return (content_type_); }
std::string		CGI::GetBody(void) const { return (body_); }