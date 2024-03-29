#ifndef PIPE_HPP
# define PIPE_HPP

# include <unistd.h>  // write(), read(), close(), pipe(), dup2()
# include <fcntl.h>  // fcntl()
# include <string>

class Pipe
{
	public:
		enum	e_PipeIo
		{
			READ = 0,
			WRITE = 1
		};

		Pipe(void);
		~Pipe(void);

		bool		Fail() const;
		int			CloseFd(e_PipeIo type);
		void		ChangeNonBlocking(e_PipeIo type) const;
		int			WriteToPipe(const std::string& str) const;
		ssize_t		ReadFromPipe(std::string* str) const;
		int			RedirectToPipe(e_PipeIo type, int fd);
		int			GetPipeFd(e_PipeIo type) const;

	private:
		int			pipe_[2];
		bool		failed_;
};

#endif
