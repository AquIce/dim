#include <dim/lexer/lexer.hpp>

#include <dim/utils/utils.hpp>

#include <cctype>

struct dim::lexer::Token dim::lexer::MakeToken(
	const dim::lexer::TokenType type,
	const std::string value
) {
	return dim::lexer::Token{
		type, value
	};
}

std::string dim::lexer::TokenRepr(
	const struct Token& token
) {
	switch(token.type) {
		case dim::lexer::TokenType::NONE:
			return "NONE";
		case dim::lexer::TokenType::NUMBER:
			return "NUMBER(" + token.value + ")";
		default:
			return "UNKNOWN";
	}
}

struct dim::lexer::Token dim::lexer::LexNumber(
	std::string& src
) {
	std::string number = "";
	bool isDecimal = false;
	while(src.length() > 0) {
		char first = dim::utils::peek(src);
		if(first == '.') {
			if(isDecimal) {
				return dim::lexer::MakeToken();
			}
			isDecimal = true;
			number += dim::utils::shift(src);
			continue;
		}
		if(std::isdigit(first)) {
			number += dim::utils::shift(src);
		} else {
			break;
		}
	}
	return dim::lexer::MakeToken(
		dim::lexer::TokenType::NUMBER,
		number
	);
}

void dim::lexer::Lex(
	std::vector<struct dim::lexer::Token>& tokens,
	std::string& src
) {
	while(src.length() > 0) {
		if(std::isspace(src.at(0))) {
			(void)dim::utils::shift(src);
		}
		tokens.push_back(
			dim::lexer::LexNumber(src)
		);
	}
}
