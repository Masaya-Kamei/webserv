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
	// ParseQuery();
}

void URI::FindPath()
{
	std::string path = raw_path_;
	const std::vector<LocationDirective> locations = server_.GetLocations();
	std::vector<LocationDirective>::const_iterator ite = locations.begin();
	std::vector<LocationDirective>::const_iterator longest_ite = ite;

	for (; ite != locations.end(); ite++)
	{
		if (!location_match(ite->GetPath(), path))
		{
			continue;
		}
		if (ite->GetPath().size() >= longest_ite->GetPath().size())
		{
			longest_ite = ite;
		}
	}
	FindFile(&path, *longest_ite);
	path_ = path;
}

bool URI::location_match(const std::string& location_path, std::string path)
{
	return path.size() >= location_path.size()
		&& !path.compare(0, location_path.size(), location_path);
}

void URI::FindFile(std::string *path, const LocationDirective &location)
{
	if (location.GetReturn().second.size())
	{
		uri_type_ = REDIRECT;
		return;
	}
	if (path->at(path->size() - 1) == '/' && method_ == HTTPRequest::GET)
		{
			FindFileIndex(location, path);
			return;
		}
	*path = path->substr(1, path->size());
	struct stat path_stat;

	if (stat((*path).c_str(), &path_stat) < 0)
    {
		throw HTTPError(404);
	}
    if (IsRegularFile(path_stat))
    {
		uri_type_ = FILE;
		stat_ = path_stat;
		return;
	}
    if (!AllowAutoIndex(location, path))
    {
        if (method_ == HTTPRequest::GET)
        {
            throw HTTPError(404);
        }
        else
        {
            uri_type_ = FILE;
			stat_ = path_stat;
			return;
		}
    }
	uri_type_ = AUTOINDEX;
	stat_ = path_stat;
}

void URI::FindFileIndex(const LocationDirective &location, std::string *path)
{
	struct stat path_stat;
	const std::vector<std::string> index = location.GetIndex();

	for (size_t i = 0; i < index.size(); i++)
	{
		std::string join_path = location.GetPath() + "/" + location.GetRoot();
		join_path = join_path.substr(1, join_path.size());
		join_path += "/" + index.at(i);
		if (stat(join_path.c_str(), &path_stat) < 0)
		{
			continue;
		}
		if (IsRegularFile(path_stat))
		{
			uri_type_ = FILE;
			*path = join_path;
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
	if (!location.GetAutoIndex())
	{
		return false;
	}

	if (path->at(0) == '/' && path->size() >= 2)
	{
		*path = path->substr(1, path->size());
	}
	struct stat path_stat;
	if (stat(path->c_str(), &path_stat) < 0)
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
