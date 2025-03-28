#pragma once

#include <array>
#include <functional>
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
	
		struct Token LexNumber(
			std::string& src
		);

		struct Token LexOperator(
			std::string& str
		);

		bool TryAddToken(
			std::vector<struct Token>& tokens,
			std::string& src,
			std::function<struct Token (std::string& src)> lexFunction
		);

		const std::array<const std::function<struct Token (std::string& src)>, 2> LexFunctionsList = { &LexNumber, &LexOperator };
		
		void Lex(
			std::vector<struct Token>& tokens,
			std::string& src
		);
	}
}
