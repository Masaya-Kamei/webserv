#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>
# include <ctime>
# include <map>
# include <fstream>

class HTTPResponse
{
	public:
		HTTPResponse();
		~HTTPResponse();

		std::string CreateResponse();

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
		std::ifstream ifs_;

		void SetRequest();
		void Paser();
		void ParseRqHeader();
		void CreateBody();
		void OpenFile();
		void ReadFile();
		void HandleError(int status_code);
		std::string GenerateHTML(int status_code) const;
		void AppendBody(const char *buffer);

		/* response */
		void SetResponse(bool connection);
		std::string ToString();
		void SetHeader(const std::pair<std::string, std::string> &elem);
		void SetHeaderField();
		std::string FindStatusMassage(int status_code) const;
		static std::map<int, std::string> SetStatusMsg();
		std::string GetDate() const;
		void Clear();
};

#endif  // HTTPRESPONSE_HPP
