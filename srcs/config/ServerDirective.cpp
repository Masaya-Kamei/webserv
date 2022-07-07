#include <arpa/inet.h>
#include "ServerDirective.hpp"

ServerDirective::ServerDirective(Tokens::citr begin, Tokens::citr end)
{
	const std::pair<std::string, ParseFunc> p[] = {
		std::make_pair("listen", &ServerDirective::ParseListen),
		std::make_pair("server_name", &ServerDirective::ParseServerNames),
		std::make_pair("error_page", &ServerDirective::ParseErrorPages),
		std::make_pair("client_max_body_size", &ServerDirective::ParseClientMaxBodySize),
		std::make_pair("location", &ServerDirective::ParseLocation)
	};
	const std::map<std::string, ParseFunc>				parse_funcs(p, &p[5]);
	std::map<std::string, ParseFunc>::const_iterator	found;
	Tokens::citr										itr;
	Tokens::citr										directive_end;

	SetDefaultValues();

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
}

ServerDirective::~ServerDirective()
{
}

const std::pair<unsigned int, int>&		ServerDirective::GetListen() const { return (listen_); }
const std::vector<std::string>&			ServerDirective::GetServerNames() const { return(server_names_); }
const std::map<int, std::string>&		ServerDirective::GetErrorPages() const { return (error_pages_); }
const long&								ServerDirective::GetClientMaxBodySize() const { return (client_max_body_size_); }
const std::vector<LocationDirective>&	ServerDirective::GetLocations() const { return (locations_); }

Tokens::citr	ServerDirective::GetDirectiveEnd
	(const std::string& name, Tokens::citr begin, Tokens::citr end) const
{
	Tokens::citr	directive_end;

	if (name == "location")
		directive_end = Tokens::GetEndBracesItr(begin + 1, end);
	else
		directive_end = std::find(begin, end, ";");
	return (directive_end);
}

void	ServerDirective::SetDefaultValues()
{
	listen_ = std::make_pair(INADDR_ANY, 80);
	server_names_.push_back("");
	client_max_body_size_ = 1048576;
}

void	ServerDirective::ParseListen(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");

	in_addr_t						ip;
	long							port;
	char							*endptr;
	const std::string&				s = *begin;
	const std::string::size_type	colon = s.find(':');
	const std::string::size_type	period = s.find('.');

	if (colon != std::string::npos || period != std::string::npos)
	{
		if (colon != std::string::npos)
			ip = inet_addr(s.substr(0, colon).c_str());
		else
			ip = inet_addr(s.c_str());
		if (ip == INADDR_NONE)
			throw std::runtime_error("conf syntax error");
		listen_.first = ip;
	}
	if (colon != std::string::npos || period == std::string::npos)
	{
		if (colon != std::string::npos)
			port = std::strtol(s.substr(colon + 1).c_str(), &endptr, 10);
		else
			port = std::strtol(s.c_str(), &endptr, 10);
		if (*endptr != '\0' || errno == ERANGE || port < 1 || 65535 < port)
			throw std::runtime_error("conf syntax error");
		listen_.second = port;
	}
}

void	ServerDirective::ParseServerNames(Tokens::citr begin, Tokens::citr end)
{
	server_names_.clear();

	Tokens::citr	itr = begin;
	while (itr != end)
	{
		if (Tokens::isSpecialToken(*itr))
			throw std::runtime_error("conf syntax error");
		server_names_.push_back(*itr);
		itr++;
	}
}

void	ServerDirective::ParseErrorPages(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 2 > end)
		throw std::runtime_error("conf syntax error");

	const std::string		error_file = *(end - 1);
	long					status_code;
	char					*endptr;
	Tokens::citr			itr = begin;

	while (itr != end - 1)
	{
		status_code = std::strtol((*itr).c_str(), &endptr, 10);
		if (*endptr != '\0' || errno == ERANGE || status_code < 300 || 599 < status_code)
			throw std::runtime_error("conf syntax error");
		error_pages_[status_code] = error_file;
		++itr;
	}
}

void	ServerDirective::ParseClientMaxBodySize(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");

	char		*endptr;
	client_max_body_size_ = std::strtol((*begin).c_str(), &endptr, 10);
	if (errno == ERANGE)
		throw std::runtime_error("conf syntax error");
	if (*endptr == '\0')
		return;

	const char	unit_chars[6] = {'k', 'K', 'm', 'M', 'g', 'G'};
	const char *unit_found = std::find(unit_chars, &unit_chars[6], *endptr);
	if (unit_found == &unit_chars[6] || *(endptr + 1) != '\0')
		throw std::runtime_error("conf syntax error");

	const int	unit_nums[6] = {1024, 1024, 1048576, 1048576, 1073741824, 1073741824};
	const int	unit_num = unit_nums[unit_found - unit_chars];
	const long	old = client_max_body_size_;
	client_max_body_size_ *= unit_num;
	if (client_max_body_size_ / unit_num != old)
		throw std::runtime_error("conf syntax error");
}

void	ServerDirective::ParseLocation(Tokens::citr begin, Tokens::citr end)
{
	if (begin >= end || Tokens::isSpecialToken(*begin))
		throw std::runtime_error("conf syntax error");

	locations_.push_back(LocationDirective(*begin, begin + 2, end));
}