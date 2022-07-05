#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>
# include <ctime>
# include <map>
# include <fstream>
# include "ServerSocket.hpp"
# include "HTTPMethod.hpp"

class HTTPResponse
{
	public:
		HTTPResponse();
		explicit HTTPResponse(const HTTPMethod &method);
		~HTTPResponse();

		void SendResponse(ServerSocket *ssocket);
		void CreateResponse(HTTPMethod method);

	private:
		static const std::map<int, std::string> STATUS_MSG;
		int status_code_;
		std::string res_msg_;
		std::map<std::string, std::string> headers_;
		size_t sent_byte_;
		std::string body_;
		bool connection_;
		std::string http_;

		void SetResponse(bool connection);
		std::string ToString();
		void SetHeaderField();
		std::string FindStatusMassage(int status_code) const;
		static std::map<int, std::string> SetStatusMsg();
		std::string GetDate() const;
		void Clear();
		void HandleError(int status_code);
		std::string GenerateHTML(int status_code) const;
};

#endif  // HTTPRESPONSE_HPP
