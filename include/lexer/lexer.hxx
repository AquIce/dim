#pragma once

#include <utils/utils.hxx>

#include <array>
#include <cctype>
#include <expected>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace dim {
	namespace lexer {

		enum class TokenType {
			NONE = 0,
			EOL,

			NUL,
			NUMBER,
			BOOLEAN,
			STRING,
			
			BINARY_OPERATOR,
			UNARY_OPERATOR,
			
			PARENTHESIS,
			BRACE,
			
			DOT,
			COLON,
			EQUALS,
			ARROW,
			AT,
			DOUBLE_DOT,

			IFELSE,
			MATCH,
			LOOP,
			BREAK,
			OR,

			DECL,
			TYPE,
			FN,
			RETURN,

			DISCARD,
			IDENTIFIER,
		};

		const std::array<std::string_view, 27> TokenTypeStr = {
			"NONE",
			"EOL",
			"NULL",
			"NUMBER",
			"BOOLEAN",
			"STRING",
			"BINARY_OPERATOR",
			"UNARY_OPERATOR",
			"PARENTHESIS",
			"BRACE",
			"DOT",
			"COLON",
			"EQUALS",
			"ARROW",
			"AT",
			"DOUBLE_DOT",
			"IFELSE",
			"MATCH",
			"LOOP",
			"BREAK",
			"OR",
			"DECL",
			"TYPE",
			"FN",
			"RETURN",
			"DISCARD",
			"IDENTIFIER",
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

		std::expected<struct Token, std::string> LexBoolean(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexString(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexBinaryOperator(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexUnaryOperator(
			std::string& src
		) noexcept;
		
		std::expected<struct Token, std::string> LexParenthesis(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexBrace(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexDot(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexColon(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexEquals(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexArrow(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexAt(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexDoubleDot(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexIfElse(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexMatch(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexLoop(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexBreak(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexOr(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexDecl(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexType(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexFn(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexReturn(
			std::string& src
		) noexcept;

		std::expected<struct Token, std::string> LexIdentifier(
			std::string& src
		) noexcept;

		const std::array<const LexFunction, 25> LexFunctionsList = {
			&LexEOL,
			&LexNull,
			&LexNumber,
			&LexBoolean,
			&LexString,
			&LexArrow,
			&LexUnaryOperator,
			&LexBinaryOperator,
			&LexParenthesis,
			&LexBrace,
			&LexDoubleDot,
			&LexDot,
			&LexColon,
			&LexEquals,
			&LexAt,
			&LexIfElse,
			&LexMatch,
			&LexLoop,
			&LexBreak,
			&LexOr,
			&LexDecl,
			&LexType,
			&LexFn,
			&LexReturn,
			&LexIdentifier,
		};

		void StripWhitespaces(
			std::string& src
		) noexcept;

		std::expected<Success, std::string> StripComments(
			std::string& src
		) noexcept;
		
		std::expected<Success, std::string> Lex(
			std::vector<struct Token>& tokens,
			std::string& src
		) noexcept;
	}
}
