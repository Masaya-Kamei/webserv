#ifndef LOCATIONDIRECTIVE_HPP
# define LOCATIONDIRECTIVE_HPP

# include <string>
# include <vector>
# include <utility>
# include "Tokens.hpp"

class LocationDirective
{
	public:
		LocationDirective(const std::string& path, Tokens::citr begin, Tokens::citr end);
		~LocationDirective();

		// Getter
		const std::string&					GetPath() const;
		const std::string&					GetRoot() const;
		const std::vector<std::string>&		GetIndex() const;
		const std::pair<int, std::string>&	GetReturn() const;
		const bool&							GetAutoIndex() const;
		const std::vector<std::string>&		GetAllowedMethods() const;

	private:
		typedef void (LocationDirective::*ParseFunc)(Tokens::citr, Tokens::citr);

		Tokens::citr	GetDirectiveEnd(const std::string& name, Tokens::citr begin, Tokens::citr end) const;
		void			SetDefaultValues();

		// Parse Directive Variable
		void	ParseRoot(Tokens::citr begin, Tokens::citr end);
		void	ParseIndex(Tokens::citr begin, Tokens::citr end);
		void	ParseReturn(Tokens::citr begin, Tokens::citr end);
		void	ParseAutoIndex(Tokens::citr begin, Tokens::citr end);
		void	ParseAllowedMethods(Tokens::citr begin, Tokens::citr end);

		// Location Path
		std::string					path_;

		// Directive Variable
		std::string					root_;
		std::vector<std::string>	index_;
		std::pair<int, std::string>	return_;
		bool						autoindex_;
		std::vector<std::string>	allowed_methods_;
};

#endif  // LOCATIONDIRECTIVE_HPP