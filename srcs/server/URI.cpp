#include <algorithm>
#include "URI.hpp"
#include "HTTPError.hpp"

URI::URI(const std::string &uri, const ServerDirective &server, const int &method)
	: server_(server),
	  raw_uri_(uri),
	  method_(method),
 	  uri_type_(INVALID)
{
	SeparateRawUri();
	FindPath();
}

URI::~URI()
{
}

void URI::SeparateRawUri()
{
	size_t query_pos = raw_uri_.find('?');
	if (query_pos == std::string::npos)
	{
		raw_path_ = raw_uri_;
		query_ = "";
		return;
	}
	raw_path_ = raw_uri_.substr(0, query_pos);
	query_ = raw_uri_.substr(query_pos + 1);
}

void URI::FindPath()
{
	std::string path = raw_path_;
	const std::vector<LocationDirective> locations = server_.GetLocations();
	std::vector<LocationDirective>::const_iterator ite = locations.begin();
	std::vector<LocationDirective>::const_iterator longest_ite = ite;

	for (; ite != locations.end(); ite++)
	{
		if (location_match(ite->GetPath(), path) && ite->GetPath().size() >= longest_ite->GetPath().size())
		{
			longest_ite = ite;
		}
	}
	FindFile(*longest_ite, &path);
	path_ = path;
}

bool URI::location_match(const std::string& location_path, std::string path)
{
	return path.size() >= location_path.size()
		&& !path.compare(0, location_path.size(), location_path);
}

void URI::FindFile(const LocationDirective &location, std::string *path)
{
	if (method_ == HTTPRequest::GET && *path == location.GetPath() && location.GetAutoIndex())
	{
		uri_type_ = AUTOINDEX;
		return;
	}
	if (location.GetReturn().second.size() != 0)
	{
		uri_type_ = REDIRECT;
		return;
	}
	FindFileIndex(location, path);
}

void URI::FindFileIndex(const LocationDirective &location, std::string *path)
{
	struct stat path_stat;
	const std::vector<std::string> index = location.GetIndex();

	for (size_t i = 0; i < index.size(); i++)
	{
		std::string current_path = location.GetPath() + "/" + location.GetRoot();
		current_path = current_path.substr(1, current_path.size()) + "/" + index.at(i);
		if (stat(current_path.c_str(), &path_stat) == 0 && IsRegularFile(path_stat))
		{
			uri_type_ = FILE;
			*path = current_path;
			stat_ = path_stat;
			return;
		}
	}

	*path = path->substr(1, path->size());
	if (stat(path->c_str(), &path_stat) == 0)
	{
		if (IsRegularFile(path_stat))
		{
			uri_type_ = FILE;
			stat_ = path_stat;
			return;
		}
	}
	if (!AllowAutoIndex(location, path))
	{
		throw HTTPError(404);
	}
	uri_type_ = AUTOINDEX;
}

bool URI::AllowAutoIndex(const LocationDirective &location, std::string *path) const
{
	struct stat path_stat;
	if (!location.GetAutoIndex() || stat(path->c_str(), &path_stat) < 0)
	{
		return false;
	}
	return IsDirectory(path_stat);
}

bool URI::IsRegularFile(const struct stat &path_stat) const
{
    return S_ISREG(path_stat.st_mode);
}

bool URI::IsDirectory(const struct stat &path_stat) const
{
    return S_ISDIR(path_stat.st_mode);
}

URI::Type URI::GetType() const
{
	return uri_type_;
}

std::string URI::GetRawPath() const
{
	return raw_path_;
}

std::string URI::GetPath() const
{
	return path_;
}

std::string URI::GetQuery() const
{
	return query_;
}

struct stat URI::GetStat() const
{
	return stat_;
}
