#include "utils.hpp"

std::vector<std::string>	Utils::MySplit
	(std::string const & str, std::string const & separator)
{
	std::vector<std::string>	list;
	std::string::size_type		sep_len;
	std::string::size_type		offset;
	std::string::size_type		pos;
	std::string					tmp;

	sep_len = separator.length();

	if (str == "")
		return (list);

	if (sep_len == 0)
		list.push_back(str);
	else
	{
		offset = 0;
		while (1)
		{
			pos = str.find(separator, offset);
			if (pos == std::string::npos)
			{
				tmp = str.substr(offset);
				if (tmp != "")
					list.push_back(str.substr(offset));
				break;
			}
			else
			{
				tmp = str.substr(offset, pos - offset);
				if (tmp != "")
					list.push_back(tmp);
				offset = pos + sep_len;
			}
		}
	}

	return (list);
}