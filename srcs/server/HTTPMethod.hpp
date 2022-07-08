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

		int ExecHTTPMethod(HTTPRequest req, std::vector<ServerDirective> servers);
		// int ExecHTTPMethod();

		int GetStatusCode() const;
		std::string GetUri() const;
		std::string GetHttp() const;
		std::string GetBody() const;
		bool GetConnection() const;
		HTTPRequest::e_method GetMethod();

		void ParseReq(HTTPRequest req, std::vector<ServerDirective>servers, ServerDirective *server);
		void FindServer(HTTPRequest req, std::vector<ServerDirective> servers, ServerDirective *server);
		bool ServerMatch(const std::pair<unsigned int, int> &listen, const std::string host);
		void SeparateHost(const std::string &host, std::string *host_ip, std::string *port);

	private:
		static const size_t BUF_SIZE;
		// ServerDirective server_;
		std::string uri_;
		std::string http_;
		std::ifstream ifs_;
		int status_code_;
		// std::map<std::string, std::string> headers_;
		// size_t sent_byte_;
		std::string body_;
		bool connection_;
		HTTPRequest::e_method method_;

		void HandleFile(int method);
		void ReadFile();
		void AppendBody(const char *buffer);
		// void setHeader(const std::pair<std::string, std::string> &elem);
};

#endif  // HTTPMETHOD_HPP
