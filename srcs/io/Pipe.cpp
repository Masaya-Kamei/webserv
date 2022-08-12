#include "Pipe.hpp"

Pipe::Pipe(void)
{
	pipe_[READ] = -1;
	pipe_[WRITE] = -1;
}

Pipe::~Pipe(void)
{
	if (pipe_[READ] != -1)
		close(pipe_[READ]);
	if (pipe_[WRITE] != -1)
		close(pipe_[WRITE]);
}

int		Pipe::OpenPipe(void)
{
	return (pipe(pipe_));
}

int		Pipe::ClosePipe(e_PipeIo type)
{
	int		result = 0;

	if (pipe_[type] != -1)
	{
		result = close(pipe_[type]);
		pipe_[type] = -1;
	}

	return (result);
}

void	Pipe::NonBlockingPipe(e_PipeIo type) const
{
	int		val;

	val = fcntl(pipe_[type], F_GETFL, 0);
	fcntl(pipe_[type], F_SETFL, val | O_NONBLOCK);
}

int		Pipe::WriteToPipe(void* buf, unsigned int byte) const
{
	return (write(pipe_[WRITE], buf, byte));
}

ssize_t	Pipe::ReadFromPipe(void* buf, size_t byte) const
{
	return (read(pipe_[READ], buf, byte));
}

int		Pipe::RedirectToPipe(e_PipeIo type, int fd)
{
	if (close(fd) < 0
		|| dup2(pipe_[type], fd) < 0
		|| ClosePipe(type) < 0)
	{
		return (0);
	}
	return (1);
}

int		Pipe::GetPipeFd(e_PipeIo type) const
{
	return (pipe_[type]);
}
