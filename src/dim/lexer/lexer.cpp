#include <dim/lexer/lexer.hpp>

struct dim::lexer::Token dim::lexer::makeToken(
	const dim::lexer::TokenType type,
	const std::string value
) {
	return dim::lexer::Token{
		type, value
	};
}

void dim::lexer::lex(
	std::vector<struct dim::lexer::Token>& tokens,
	const std::string& src
) {
	
}
