#include <dim/lexer/lexer.hpp>

#include <dim/utils/utils.hpp>

#include <cctype>
#include <stdexcept>

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
		case dim::lexer::TokenType::OPERATOR:
			return token.value;
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
				throw std::runtime_error("Invalid number litteral \"" + number + ".\"");	
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
	if(number.length() == 0 || (number.length() == 1 && number == ".")) {
		return dim::lexer::MakeToken();
	}
	return dim::lexer::MakeToken(
		dim::lexer::TokenType::NUMBER,
		number
	);
}

dim::lexer::Token dim::lexer::LexOperator(
	std::string& src
) {
	char first = dim::utils::shift(src);
	switch(first) {
		case '+':
		case '-':
		case '*':
		case '/':
			return dim::lexer::MakeToken(
				dim::lexer::TokenType::OPERATOR,
				std::string(1, first)
			);
		default:
			return dim::lexer::MakeToken();
	}
}

bool dim::lexer::TryAddToken(
	std::vector<struct dim::lexer::Token>& tokens,
	std::string& src,
	std::function<struct dim::lexer::Token (std::string& src)> lexFunction
) {
	struct dim::lexer::Token token = lexFunction(src);
	if(token.type != dim::lexer::TokenType::NONE) {
		tokens.push_back(token);
		return true;
	}
	return false;
}

void dim::lexer::Lex(
	std::vector<struct dim::lexer::Token>& tokens,
	std::string& src
) {
	while(src.length() > 0) {
		if(std::isspace(src.at(0))) {
			(void)dim::utils::shift(src);
		}
		struct dim::lexer::Token token = dim::lexer::MakeToken();

		for(const auto& lexFunction : LexFunctionsList) {
			if(TryAddToken(tokens, src, lexFunction)) {
				break;
			}
		}
	}
}
