#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include <string>
# include <map>
# include <fstream>

// # include "HTTPRequest.hpp"

class HTTPMethod
{
	public:
		HTTPMethod();
		~HTTPMethod();

		// int ExecHTTPMethod(HTTPRequest req);
		int ExecHTTPMethod();

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
		std::string method_;
		std::ifstream ifs_;

		void ParseReq();

		void HandleFile();
		void ReadFile();
		void AppendBody(const char *buffer);
		// void setHeader(const std::pair<std::string, std::string> &elem);

		/* response */
		int status_code_;
		// std::string res_msg_;
		// std::map<std::string, std::string> headers_;
		// size_t sent_byte_;
		std::string body_;
		bool connection_;
};

#endif  // HTTPMETHOD_HPP
