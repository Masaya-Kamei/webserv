#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <vector>
# include "Tokens.hpp"
# include "ServerDirective.hpp"

class Config
{
	public:
		explicit Config(const std::string& file_path);
		~Config();

		// Getter
		const std::vector<ServerDirective>&	GetServers() const;

	private:
		// Parse Directive Variable
		void	ParseServer(Tokens::citr start, Tokens::citr end);

		// Tokens
		Tokens							tokens_;
		// Directive Variable
		std::vector<ServerDirective>	servers_;
};

#endif  // CONFIG_HPP
