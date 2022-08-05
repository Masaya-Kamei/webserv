#include "CGI.hpp"

CGI::CGI(const URI& uri, const HTTPRequest& req)
	: uri_(uri)
	, req_(req)
	, server_conf_(req.GetServerConf())
{
	ExecuteCGI();
	ParseCGI();
}

CGI::~CGI(void)
{
}

static int	pipe_set(int src, int dst)
{
	if (close(dst) < 0
		|| dup2(src, dst) < 0
		|| close(src) < 0)
	{
		return (0);
	}
	return (1);
}

void	CGI::SendData(const int write_pipe_fd[2], const int read_pipe_fd[2])
{
	CGIEnv	env(uri_, req_);
	char*	argv[2];

	if (close(write_pipe_fd[1]) < 0 || !pipe_set(write_pipe_fd[0], STDIN_FILENO))
		std::exit(EXIT_FAILURE);

	if (close(read_pipe_fd[0]) < 0 || !pipe_set(read_pipe_fd[1], STDOUT_FILENO))
		std::exit(EXIT_FAILURE);

	argv[0] = const_cast<char *>(file_path_.c_str());
	argv[1] = NULL;

	if (execve(argv[0], argv, env.GetEnv()) < 0)
		std::exit(EXIT_FAILURE);
}

void	CGI::ReceiveData(const int write_pipe_fd[2], const int read_pipe_fd[2], const pid_t pid)
{
	const size_t	buf_size = 4;
	char			buf[buf_size + 1];
	int				read_byte;
	pid_t			ret_pid;
	int				status;

	if (close(write_pipe_fd[0]) < 0 || !pipe_set(write_pipe_fd[1], STDOUT_FILENO))
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);

	if (close(read_pipe_fd[1]) < 0 || !pipe_set(read_pipe_fd[0], STDIN_FILENO))
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);

	write(STDOUT_FILENO, req_.GetBody().c_str(), req_.GetBody().size());

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
		|| !WIFEXITED(status)
		|| WEXITSTATUS(status) == EXIT_FAILURE)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
}

void	CGI::ExecuteCGI(void)
{
	int		write_pipe_fd[2];
	int		read_pipe_fd[2];
	pid_t	pid;
	int		stdin_save;
	int		stdout_save;

	stdin_save = dup(STDIN_FILENO);
	stdout_save = dup(STDOUT_FILENO);

	if (pipe(write_pipe_fd) < 0
		|| pipe(read_pipe_fd) < 0
		|| (pid = fork()) < 0)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);

	if (pid == 0)
		SendData(write_pipe_fd, read_pipe_fd);
	else
		ReceiveData(write_pipe_fd, read_pipe_fd, pid);

	dup2(stdin_save, STDIN_FILENO);
	dup2(stdout_save, STDOUT_FILENO);
	close(stdin_save);
	close(stdout_save);

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
		if (pos == std::string::npos)
			throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);

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
