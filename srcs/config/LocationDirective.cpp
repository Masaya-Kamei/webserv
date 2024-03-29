#include <map>
#include "LocationDirective.hpp"
#include "utils.hpp"

LocationDirective::LocationDirective()
	: path_("/")
{
	SetInitValue();
	SetDefaultValue();
}

LocationDirective::LocationDirective(const std::string& path, Tokens::citr begin, Tokens::citr end)
	: path_(path)
{
	const std::pair<std::string, ParseFunc> p[] = {
		std::make_pair("root", &LocationDirective::ParseRoot),
		std::make_pair("index", &LocationDirective::ParseIndex),
		std::make_pair("return", &LocationDirective::ParseReturn),
		std::make_pair("autoindex", &LocationDirective::ParseAutoIndex),
		std::make_pair("allowed_methods", &LocationDirective::ParseAllowedMethods),
		std::make_pair("upload_root", &LocationDirective::ParseUploadRoot),
		std::make_pair("cgi_enable_extension", &LocationDirective::ParseCGIEnableExtension)
	};
	const std::map<std::string, ParseFunc>				parse_funcs(p, &p[7]);
	std::map<std::string, ParseFunc>::const_iterator	found;
	Tokens::citr										itr;
	Tokens::citr										directive_end;

	SetInitValue();
	itr = begin;
	while (itr < end)
	{
		found = parse_funcs.find(*itr);
		if (found == parse_funcs.end())
			throw std::runtime_error("conf syntax error");
		directive_end = GetDirectiveEnd(*itr, itr + 1, end);
		if (directive_end == end || itr + 1 == directive_end)
			throw std::runtime_error("conf syntax error");
		(this->*(found->second))(itr + 1, directive_end);
		itr = directive_end + 1;
	}
	SetDefaultValue();
}

LocationDirective::~LocationDirective()
{
}

const std::string&					LocationDirective::GetPath() const { return (path_); }
const std::string&					LocationDirective::GetRoot() const { return (root_); }
const std::vector<std::string>&		LocationDirective::GetIndex() const { return (index_); }
const std::pair<e_StatusCode, std::string>&	LocationDirective::GetReturn() const { return (return_); }
const int&							LocationDirective::GetAutoIndex() const { return (autoindex_); }
const std::vector<std::string>&		LocationDirective::GetAllowedMethods() const { return (allowed_methods_); }
const std::string&					LocationDirective::GetUploadRoot() const { return (upload_root_); }
const std::vector<std::string>&		LocationDirective::GetCGIEnableExtension() const { return (cgi_enable_extension_); }

Tokens::citr	LocationDirective::GetDirectiveEnd
	(const std::string& name, Tokens::citr begin, Tokens::citr end) const
{
	Tokens::citr	directive_end;

	(void)name;
	directive_end = std::find(begin, end, ";");
	return (directive_end);
}

void	LocationDirective::SetInitValue()
{
	root_ = "";
	return_ = std::make_pair(SC_INVALID, "");
	autoindex_ = -1;
	allowed_methods_.push_back("GET");
	upload_root_ = "";
	cgi_enable_extension_.push_back("cgi");
}

void	LocationDirective::SetDefaultValue()
{
	if (root_ == "")
		root_ = "html";
	if (index_.size() == 0)
		index_.push_back("index.html");
	if (autoindex_ == -1)
		autoindex_ = false;
	if (upload_root_ == "")
		upload_root_ = "html";
}

void	LocationDirective::ParseRoot(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");
	else if (root_ != "")
		throw std::runtime_error("conf syntax error");
	else if (Tokens::isSpecialToken(*begin))
		throw std::runtime_error("conf syntax error");
	root_ = *begin;
}

void	LocationDirective::ParseIndex(Tokens::citr begin, Tokens::citr end)
{
	Tokens::citr	itr = begin;
	while (itr != end)
	{
		if (Tokens::isSpecialToken(*itr))
			throw std::runtime_error("conf syntax error");
		if (Utils::IsNotFound(index_, *itr))
			index_.push_back(*itr);
		itr++;
	}
}

void	LocationDirective::ParseReturn(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end && begin + 2 != end)
		throw std::runtime_error("conf syntax error");

	bool			is_url;
	char			*endptr;
	long			status_code = SC_FOUND;
	std::string		url = "";

	is_url = (*begin).find("http://") != std::string::npos;
	is_url |= (*begin).find("https://") != std::string::npos;
	if (begin + 2 == end || !is_url)
	{
		status_code = std::strtol((*begin).c_str(), &endptr, 10);
		if (*endptr != '\0' || errno == ERANGE || status_code < 1 || 999 < status_code)
			throw std::runtime_error("conf syntax error");
	}
	if (begin + 2 == end || is_url)
	{
		if (begin + 1 == end)
			url = *begin;
		else
			url = *(begin + 1);
		if (Tokens::isSpecialToken(url))
			throw std::runtime_error("conf syntax error");
	}
	if (return_.first == SC_INVALID)
		return_ = std::make_pair(static_cast<e_StatusCode>(status_code), url);
}

void	LocationDirective::ParseAutoIndex(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");
	else if (autoindex_ != -1)
		throw std::runtime_error("conf syntax error");
	if (*begin == "on")
		autoindex_ = true;
	else if (*begin == "off")
		autoindex_ = false;
	else
		throw std::runtime_error("conf syntax error");
}

void	LocationDirective::ParseAllowedMethods(Tokens::citr begin, Tokens::citr end)
{
	const std::string		valid_methods[3] = {"GET", "POST", "DELETE"};
	Tokens::citr			itr;
	const std::string*		found_ptr;

	itr = begin;
	while (itr != end)
	{
		found_ptr = std::find(valid_methods, &valid_methods[3], *itr);
		if (found_ptr == &valid_methods[3])
			throw std::runtime_error("conf syntax error");
		if (Utils::IsNotFound(allowed_methods_, *itr))
			allowed_methods_.push_back(*itr);
		itr++;
	}
}

void	LocationDirective::ParseUploadRoot(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");
	else if (upload_root_ != "")
		throw std::runtime_error("conf syntax error");
	else if (Tokens::isSpecialToken(*begin))
		throw std::runtime_error("conf syntax error");
	upload_root_ = *begin;
}

void	LocationDirective::ParseCGIEnableExtension(Tokens::citr begin, Tokens::citr end)
{
	const std::string	valid_extensions[2] = {"pl", "py"};
	Tokens::citr		itr;
	const std::string*	found_ptr;

	itr = begin;
	while (itr != end)
	{
		found_ptr = std::find(valid_extensions, &valid_extensions[2], *itr);
		if (found_ptr == &valid_extensions[2])
			throw std::runtime_error("conf syntax error");
		if (Utils::IsNotFound(cgi_enable_extension_, *itr))
			cgi_enable_extension_.push_back(*itr);
		itr++;
	}
}
