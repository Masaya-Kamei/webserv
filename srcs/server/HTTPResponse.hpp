#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>
# include <ctime>
# include <map>
# include "HTTPStatusCode.hpp"

class HTTPResponse
{
	public:
		HTTPResponse();
		~HTTPResponse();

		void Clear();
		std::string CreateResponse();
		void SetResponse(bool connection);

		void SetStatusCode(int status_code);
		void SetHeader(const std::pair<std::string, std::string> &elem);
		void SetBody(const std::string &body);
		void SetSentByte(size_t sent_byte);
		void AppendBody(const char *body, size_t size);

		int GetStatusCode() const;
		const std::map<std::string, std::string> GetHeaders() const;
		const std::string &GetResMessage() const;
		size_t GetSentByte() const;

	private:
		static const size_t BUF_SIZE;
		static const std::map<int, std::string> STATUS_MSG;
		int status_code_;
		std::string res_msg_;
		std::map<std::string, std::string> headers_;
		size_t sent_byte_;
		std::string body_;
		bool connection_;

		/* Paser */
		std::map<std::string, std::string> rq_;
		std::string path_;
		std::string http_;
		int file_fd_;

		void SetRequest();
		void Paser();
		void ParseRqHeader();
		void CreateBody();
		void OpenFile();
		void ReadFile();
		void CloseFile();
		void HandleError(int status_code);
		std::string GenerateHTML(int status_code) const;

		/* response */
		std::string ToString();
		void SetHeaderField();
		std::string FindStatusMassage(int status_code) const;
		static std::map<int, std::string> SetStatusMsg();
		std::string GetDate() const;
};

#endif  // HTTPRESPONSE_HPP
