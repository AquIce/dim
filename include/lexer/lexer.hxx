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
			CHAR,
			STRING,
			
			BINARY_OPERATOR,
			UNARY_OPERATOR,
			
			PARENTHESIS,
			BRACE,
			
			DOT,
			COMMA,
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
			STRUCT,
			IMPL,

			DISCARD,
			IDENTIFIER,
		};

		const std::array<std::string_view, 31> TokenTypeStr = {
			"NONE",
			"EOL",
			"NULL",
			"NUMBER",
			"BOOLEAN",
			"CHAR",
			"STRING",
			"BINARY_OPERATOR",
			"UNARY_OPERATOR",
			"PARENTHESIS",
			"BRACE",
			"DOT",
			"COMMA",
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
			"STRUCT",
			"IMPL",
			"DISCARD",
			"IDENTIFIER",
		};

		struct Token {
			TokenType type;
			std::string value;
		};

		class LexTracker {
		public:
			const std::vector<std::string> src;
			struct utils::Context ctx;
			std::vector<struct Token> tokens;

		public:
			std::expected<std::string, std::string> shift(
				size_t length = 1
			);
			std::expected<std::string, std::string> peek(
				size_t length = 1
			);
		};

		typedef std::function<
			Result<struct Token> (LexTracker&)
		> LexFunction;
		
		struct Token MakeToken(
			const TokenType type = TokenType::NONE,
			const std::string value = ""
		) noexcept;

		std::string TokenRepr(
			const struct Token& token
		) noexcept;

		std::expected<char, std::string> to_escaped_char(
			const std::string& chr
		) noexcept;
	
		Result<struct Token> LexEOL(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexNull(
			LexTracker& tracker
		) noexcept;
	
		Result<struct Token> LexNumber(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexBoolean(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexChar(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexString(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexBinaryOperator(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexUnaryOperator(
			LexTracker& tracker
		) noexcept;
		
		Result<struct Token> LexParenthesis(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexBrace(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexDot(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexComma(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexColon(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexEquals(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexArrow(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexAt(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexDoubleDot(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexIfElse(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexMatch(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexLoop(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexBreak(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexOr(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexDecl(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexType(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexFn(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexReturn(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexStruct(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexImpl(
			LexTracker& tracker
		) noexcept;

		Result<struct Token> LexIdentifier(
			LexTracker& tracker
		) noexcept;

		const std::array<const LexFunction, 29> LexFunctionsList = {
			&LexEOL,
			&LexNull,
			&LexNumber,
			&LexBoolean,
			&LexChar,
			&LexString,
			&LexArrow,
			&LexUnaryOperator,
			&LexBinaryOperator,
			&LexParenthesis,
			&LexBrace,
			&LexDoubleDot,
			&LexDot,
			&LexComma,
			&LexColon,
			&LexEquals,
			&LexAt,
			&LexIfElse,
			&LexMatch,
			&LexLoop,
			&LexBreak,
			&LexOr,
			&LexDecl,
			&LexStruct,
			&LexImpl,
			&LexType,
			&LexFn,
			&LexReturn,
			&LexIdentifier,
		};

		void StripWhitespaces(
			LexTracker& tracker
		) noexcept;

		std::expected<Success, std::string> StripComments(
			LexTracker& tracker
		) noexcept;
		
		Result<> Lex(
			std::vector<struct Token>& tokens,
			std::string& src
		) noexcept;
	}
}
