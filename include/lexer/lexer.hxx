#pragma once

#include <utils/utils.hxx>

#include <array>
#include <expected>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

typedef struct Success {} Success;

namespace dim {
	namespace lexer {

		enum class TokenType {
			NONE = 0,
			EOL,
			NUL,
			NUMBER,
			BINARY_OPERATOR,
			PARENTHESIS,
			BOOLEAN,
			STRING
		};

		const std::array<std::string_view, 8> TokenTypeStr = {
			"NONE",
			"EOL",
			"NULL",
			"NUMBER",
			"BINARY_OPERATOR",
			"PARENTHESIS",
			"BOOLEAN",
			"STRING"
		};

		struct Token {
			TokenType type;
			std::string value;
		};

		typedef std::function<
			std::expected<struct Token, std::string> (std::string& src)
		> LexFunction;
		
		struct Token MakeToken(
			const TokenType type = TokenType::NONE,
			const std::string value = ""
		) noexcept;

		std::string TokenRepr(
			const struct Token& token
		) noexcept;
	
		std::expected<struct Token, std::string> LexEOL(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexNull(
			std::string& src
		) noexcept;
	
		std::expected<struct Token, std::string> LexNumber(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexBinaryOperator(
			std::string& str
		) noexcept;

		std::expected<struct Token, std::string> LexParenthesis(
			std::string& str
		) noexcept;

		std::expected<struct Token, std::string> LexBoolean(
			std::string& str
		) noexcept;

		std::expected<struct Token, std::string> LexString(
			std::string& str
		) noexcept;

		const std::array<const LexFunction, 7> LexFunctionsList = {
			&LexEOL,
			&LexNull,
			&LexNumber,
			&LexBinaryOperator,
			&LexParenthesis,
			&LexBoolean,
			&LexString
		};
		
		std::expected<Success, std::string> Lex(
			std::vector<struct Token>& tokens,
			std::string& src
		) noexcept;
	}
}
