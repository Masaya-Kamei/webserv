#include <fstream>
#include "Tokens.hpp"

Tokens::Tokens(const std::string& file_path)
{
	LexConfigFile(file_path);
}

Tokens::~Tokens()
{
}

Tokens::citr	Tokens::begin() const
{
	return (tokens_.begin());
}

Tokens::citr	Tokens::end() const
{
	return (tokens_.end());
}

Tokens::citr	Tokens::GetEndBracesItr(citr braces_begin, citr end)
{
	if (braces_begin >= end || *braces_begin != "{")
		return (end);

	citr	itr			 = braces_begin + 1;
	int		braces_count = 1;

	while (itr < end)
	{
		if (*itr == "}")
		{
			if (--braces_count == 0)
				return (itr);
		}
		else if (*itr == "{")
			++braces_count;
		++itr;
	}
	return (end);
}

bool	Tokens::isSpecialToken(const std::string& s)
{
	const std::string 	symbols[] = {";", "{", "}"};
	return (std::find(symbols, &symbols[3], s) != &symbols[3]);
}

std::string	Tokens::GetFileContent(const std::string& file_path) const
{
	std::ifstream ifs(file_path);
	if (ifs.fail())
		throw std::runtime_error("open error");
	std::istreambuf_iterator<char> it(ifs);
	std::istreambuf_iterator<char> last;
	std::string content(it, last);
	return (content);
}

bool	Tokens::isSpecialSymbol(const char c) const
{
	const char 	symbols[] = {';', '{', '}'};
	return (std::find(symbols, &symbols[3], c) != &symbols[3]);
}

std::string	Tokens::GetToken(const std::string& content, const std::string::size_type& start) const
{
	std::string::size_type	i = start;
	char					quote;

	if (isSpecialSymbol(content[start]))
		return (content.substr(start, 1));

	while (i < content.length()
		&& !std::isspace(content[i]) && !isSpecialSymbol(content[i]))
	{
		if (content[i] == '\'' || content[i] == '\"')
		{
			quote = content[i++];
			while (i < content.length() && content[i] != quote)
				i++;
			if (i == content.length())
				throw std::runtime_error("conf syntex error");
		}
		i++;
	}
	return (content.substr(start, i - start));
}

void	Tokens::LexConfigFile(const std::string& file_path)
{
	std::string::size_type		i;
	const std::string&			content = GetFileContent(file_path);

	i = 0;
	while (i < content.length())
	{
		while (i < content.length() && std::isspace(content[i]))
			i++;
		if (i == content.length())
			break;
		const std::string&	token = GetToken(content, i);
		tokens_.push_back(token);
		i += token.length();
	}
}
