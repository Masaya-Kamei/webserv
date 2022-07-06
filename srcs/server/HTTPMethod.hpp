#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include <string>
# include <map>
# include <fstream>
# include "HTTPRequest.hpp"

class HTTPMethod
{
	public:
		HTTPMethod();
		~HTTPMethod();

		int ExecHTTPMethod(HTTPRequest req);
		// int ExecHTTPMethod();

		int GetStatusCode() const;
		std::string GetPath() const;
		std::string GetHttp() const;
		std::string GetBody() const;
		bool GetConnection() const;

	private:
		static const size_t BUF_SIZE;
		std::map<std::string, std::string> rq_;
		std::string path_;
		std::string http_;
		std::ifstream ifs_;
		int status_code_;
		// std::map<std::string, std::string> headers_;
		// size_t sent_byte_;
		std::string body_;
		bool connection_;
		int method_;
		
		void ParseReq(HTTPRequest req);
		void HandleFile(int method);
		void ReadFile();
		void AppendBody(const char *buffer);
		// void setHeader(const std::pair<std::string, std::string> &elem);
};

#endif  // HTTPMETHOD_HPP
