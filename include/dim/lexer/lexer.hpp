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

		struct Token makeToken(
			const TokenType type = TokenType::NONE,
			const std::string value = ""
		);

		void lex(
			std::vector<struct Token>& tokens,
			const std::string& src
		);
	
	}
}
