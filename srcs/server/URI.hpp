#ifndef URI_HPP
# define URI_HPP

#include <string>
#include <sys/stat.h>
#include "HTTPRequest.hpp"
#include "ServerDirective.hpp"

class URI
{
	public:
	    enum Type
		{
			FILE,
			AUTOINDEX,
			REDIRECT,
			CGI,
			INVALID
		};
		URI(const std::string &uri, const ServerDirective &server, const int &method);
		~URI();

		Type GetType() const;
		std::string GetRawPath() const;
		std::string GetPath() const;
		std::string GetQuery() const;

	private:
		const ServerDirective server_;
		const std::string &raw_uri_;
		const int &method_;
		// LocationDirective location_;
		std::string raw_path_;
		std::string path_;
		std::string query_;
		Type uri_type_;
		// struct stat stat_;

		void SeparateRawUri();
		void CreatePath();
		bool location_match(const std::string &location_path, std::string path);
		void FindPathLocation(std::string *path, const LocationDirective &location);
		void FindFileIndex(const LocationDirective &location,
							const std::vector<std::string> index, std::string *path);
		bool AllowAutoIndex(const LocationDirective &location, std::string *path) const;
		bool IsRegularFile(const struct stat &path_stat) const;
		bool IsDirectory(const struct stat &path_stat) const;
		// bool HasExtension(std::string path);
};

#endif  // URI_HPP
