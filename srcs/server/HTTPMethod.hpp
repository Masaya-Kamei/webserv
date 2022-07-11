#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include <string>
# include <map>
# include <fstream>
# include "HTTPRequest.hpp"
# include "URI.hpp"
# include "ServerDirective.hpp"

class HTTPMethod
{
	public:
		HTTPMethod();
		~HTTPMethod();

		int ExecHTTPMethod(HTTPRequest req, const ServerDirective &server);

		int GetStatusCode() const;
		std::string GetHttp() const;
		std::string GetBody() const;
		bool GetConnection() const;
		HTTPRequest::e_method GetMethod() const;
		// std::string GetUri() const;

		void ParseReq(HTTPRequest req);
	private:
		static const size_t BUF_SIZE;
		// ServerDirective server_;
		// std::string uri_;
		std::string http_;
		std::ifstream ifs_;
		int status_code_;
		// std::map<std::string, std::string> headers_;
		// size_t sent_byte_;
		std::string body_;
		bool connection_;
		HTTPRequest::e_method method_;

		void HandleFile(int method, const URI &uri);
		void ReadFile();
		void AppendBody(const char *buffer);
		// void setHeader(const std::pair<std::string, std::string> &elem);
};

#endif  // HTTPMETHOD_HPP
