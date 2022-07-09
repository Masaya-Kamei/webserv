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
	CreatePath();
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

void URI::CreatePath()
{
	std::string path = raw_path_;
	const std::vector<LocationDirective> locations = server_.GetLocations();
	std::vector<LocationDirective>::const_reverse_iterator ite = locations.rbegin();
	for (; ite != locations.rend(); ite++)
	{
		const std::string root_path = ite->GetRoot();
		std::string location_path;
		if (root_path.size() != 0)
		{
			location_path = ite->GetPath() + "/" + ite->GetRoot();
		}
		else
		{
			location_path = ite->GetPath();
		}
		// longest_match(location + root)
		if (!location_match(location_path, path))
		{
			continue;
		}
		// indexの中からFILEを見つける
		FindPathLocation(&path, *ite);
		break;
	}
}

bool URI::location_match(const std::string& location_path, std::string path)
{
	return path.size() >= location_path.size()
		&& !path.compare(0, location_path.size(), location_path);
}

void URI::FindPathLocation(std::string *path, const LocationDirective &location)
{
	const std::vector<std::string> allowed_methods = location.GetAllowedMethods();
	const std::vector<std::string> index = location.GetIndex();
	std::vector<std::string>::iterator ite;

	if (path->at(path->size() - 1) == '/' && method_ == HTTPRequest::GET)
	{
		FindFileIndex(location, index, path);
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
            return;
        }
    }
    uri_type_ = AUTOINDEX;
}

void URI::FindFileIndex(const LocationDirective &location,
							const std::vector<std::string> index, std::string *path)
{
	struct stat path_stat;
	for (size_t i = 0; i < index.size(); i++)
	{
		std::string join_path = *path + index.at(i);
		if (stat(join_path.c_str(), &path_stat) < 0)
		{
			continue;
		}
		if (IsRegularFile(path_stat))
		{
			uri_type_ = FILE;
			*path = join_path;
			return;
		}
	}
	if (!AllowAutoIndex(location, path))
	{
		throw HTTPError(404);
	}
	uri_type_ = AUTOINDEX;
}

// bool URI::HasExtension(std::string path)
// {
// 	if (path.rfind(".") == std::string::npos)
// 	{
// 		return false;
// 	}
// 	return true;
// }

bool URI::AllowAutoIndex(const LocationDirective &location, std::string *path) const
{
	if (!location.GetAutoIndex())
	{
		return false;
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
