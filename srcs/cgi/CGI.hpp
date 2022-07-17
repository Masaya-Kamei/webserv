#ifndef CGI_HPP
# define CGI_HPP

# include <unistd.h> // close
# include <sys/types.h> // wait
# include <sys/wait.h> // wait

# include <iostream>
# include <string>
# include <cstdlib> // exit
# include <map>
# include <utility>

# include "CGIEnv.hpp"
# include "HTTPError.hpp"
# include "utils.hpp"

class CGI
{
	public:
		CGI(const std::string& file_path, const HTTPRequest& req);
		~CGI(void);

		std::string		GetData(void) const;
		std::string		GetContentType(void) const;
		std::string		GetBody(void) const;

	private:
		typedef	void (CGI::*ParseFunc)(const std::string& content);

		void	ExecuteCGI(void);
		void	ParseCGI(void);
		void	DoChild(const int pipe_fd[2]);
		void	DoParent(const int pipe_fd[2], const pid_t pid);
		void	ParseHeader(const std::string& line);
		void	ParseContentType(const std::string& content);

		const std::string		file_path_;
		const HTTPRequest&		req_;
		std::string				data_;
		std::string				content_type_;
		std::string				body_;
};

#endif