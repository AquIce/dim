#pragma once

#include <string>
#include <vector>

namespace dim {
	namespace lexer {

		enum class TokenType {
			NONE = 0,
			NUMBER,
			OPERATOR
		};

		struct Token {
			TokenType type;
			std::string value;
		};

		struct Token MakeToken(
			const TokenType type = TokenType::NONE,
			const std::string value = ""
		);

		std::string TokenRepr(
			const struct Token& token
		);

		Token LexNumber(
			std::string& src
		);

		void Lex(
			std::vector<struct Token>& tokens,
			std::string& src
		);
	
	}
}
