#include <lexer/lexer.hxx>

namespace dim {
	namespace lexer {

		struct Token MakeToken(
			const TokenType type,
			const std::string value
		) noexcept {
			return Token{
				type, value
			};
		}

		std::string TokenRepr(
			const struct Token& token
		) noexcept {
			switch(token.type) {
				case TokenType::NONE:
					return "NONE";
				
				case TokenType::NUL:
					return "NULL";

				case TokenType::NUMBER:
					return "NUMBER(" + token.value + ")";
				
				case TokenType::BOOLEAN:
					return "BOOLEAN(" + token.value + ")";

				case TokenType::CHAR:
					return "CHAR('" + token.value + "')";

				case TokenType::STRING:
					return "STRING(\"" + token.value + "\")";
				
				case TokenType::EOL:
				case TokenType::UNARY_OPERATOR:
				case TokenType::BINARY_OPERATOR:
				case TokenType::PARENTHESIS:
				case TokenType::BRACE:
				case TokenType::DOT:
				case TokenType::COMMA:
				case TokenType::COLON:
				case TokenType::EQUALS:
				case TokenType::ARROW:
				case TokenType::AT:
				case TokenType::DOUBLE_DOT:
				case TokenType::IFELSE:
				case TokenType::MATCH:
				case TokenType::LOOP:
				case TokenType::BREAK:
				case TokenType::OR:
				case TokenType::DECL:
				case TokenType::TYPE:
				case TokenType::FN:
				case TokenType::RETURN:
				case TokenType::STRUCT:
				case TokenType::IMPL:
				case TokenType::DISCARD:
					return token.value;

				case TokenType::IDENTIFIER:
					return "ID(" + token.value + ")";
				
				default:
					return "UNKNOWN";
			}
		}

		std::expected<std::string, std::string> LexTracker::peek(
			size_t length
		) {
			try {
				const std::string& lineStr = src.at(ctx.line);

				if(ctx.column + length > lineStr.size()) {
					return std::unexpected("Peek exceeds line length.");
				}

				return lineStr.substr(ctx.column, length);
			} catch(...) {
				return std::unexpected("Peek failed: line index out of bounds.");
			}
		}

		std::expected<std::string, std::string> LexTracker::shift(
			size_t length
		) {
			try {
				const std::string& lineStr = src.at(ctx.line);

				if(ctx.column + length > lineStr.size()) {
					return std::unexpected("Shift exceeds end of line.");
				}

				std::string result = lineStr.substr(ctx.column, length);
				ctx.column += length;

				if(ctx.column == lineStr.size()) {
					ctx.line++;
					ctx.column = 0;
				}

				return result;
			} catch(...) {
				return std::unexpected("Shift failed: line index out of bounds.");
			}
		}

		std::expected<char, std::string> to_escaped_char(
			const std::string& chr
		) noexcept {
			if(chr.size() == 1) {
				return chr[0];
			}

			if(chr.size() == 2 && chr[0] == '\\') {
				switch(chr[1]) {
					case 'n': return '\n';
					case 't': return '\t';
					case 'r': return '\r';
					case 'b': return '\b';
					case 'f': return '\f';
					case 'v': return '\v';
					case 'a': return '\a';
					case '\\': return '\\';
					case '\'': return '\'';
					case '\"': return '\"';
					case '0': return '\0';
					default:
							return std::unexpected("Unsupported escape sequence: " + chr);
				}
			}

				return std::unexpected("Invalid input: " + chr);
		}

		Result<struct Token> LexEOL(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == ";") {
				return MakeToken(
					TokenType::EOL,
					tracker.shift()
				);
			}

			return std::unexpected("No EOL token found.");
		}

		Result<struct Token> LexNull(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(4);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "null") {
				return MakeToken(
					TokenType::NUL,
					tracker.shift(4)
				);
			}

			return std::unexpected("No null token found");
		}

		Result<struct Token> LexNumber(
		LexTracker& tracker
) noexcept {
		std::string number = "";
		bool isDecimal = false;

		std::expected<std::string, std::string> peekRes = tracker.peek();
		if(!peekRes) {
			return std::unexpected(peekRes.error());
		}
		if(peekRes.value().at(0) == '.') {
			return std::unexpected(". at the start of number token.");
		}

		while(true) {
			std::expected<std::string, std::string> peekRes = tracker.peek();
			if(!peekRes) {
					break;
			}
			char first = peekRes.value().at(0);

			if(first == '.') {
				if(isDecimal) {
					number.pop_back();
					break;
				}
				isDecimal = true;
				std::expected<Success, std::string> shiftRes = tracker.shift();
				if(!shiftRes) {
					return std::unexpected(shiftRes.error());
				}
				number += shiftRes.value();
				continue;
			}

			if(std::isdigit(static_cast<unsigned char>(first))) {
				std::expected<Success, std::string> shiftRes = tracker.shift();
				if(!shiftRes) {
					return std::unexpected(shiftRes.error());
				}
				number += shiftRes.value();
			} else {
				break;
			}
		}

		if(number.length() == 0 || (number.length() == 1 && number == ".")) {
				return std::unexpected("Invalid number literal \"" + number + "\"");
		}
		return MakeToken(
				TokenType::NUMBER,
				number
		);
}


		Result<struct Token> LexBoolean(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(4);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "true") {
				return MakeToken(
					TokenType::BOOLEAN,
					tracker.shift(4)
				);
			}

			result = tracker.peek(5);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "true") {
				return MakeToken(
					TokenType::BOOLEAN,
					tracker.shift(5)
				);
			}

			return std::unexpected("No boolean token found");
		}

		Result<struct Token> LexChar(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> peekRes = tracker.peek();
			if(!peekRes) {
				return std::unexpected(peekRes.error());
			}
			if(peekRes.value().at(0) != '\'') {
				return std::unexpected("No char token found");
			}
			std::expected<Success, std::string> shiftRes = tracker.shift();
			if(!shiftRes) {
				return std::unexpected(shiftRes.error());
			}

			std::string chr = "";
			peekRes = tracker.peek();
			if(!peekRes) {
				return std::unexpected(peekRes.error());
			}
			if(peekRes.value().at(0) != '\\') {
				shiftRes = tracker.shift();
				if(!shiftRes) {
					return std::unexpected(shiftRes.error());
				}
				chr = shiftRes.value();
			} else {
				shiftRes = tracker.shift(2);
				if(!shiftRes) {
					return std::unexpected(shiftRes.error());
				}
				chr = shiftRes.value();
			}

			peekRes = tracker.peek();
			if(!peekRes) {
				return std::unexpected(peekRes.error());
			}
			if(peekRes.value().at(0) != '\'') {
				return std::unexpected("Invalid character found.");
			}
			shiftRes = tracker.shift();
			if(!shiftRes) {
				return std::unexpected(shiftRes.error());
			}

			std::expected<char, std::string> result = to_escaped_char(chr);
			if(!result) {
				return std::unexpected(result.error());
			}

			return MakeToken(
				TokenType::CHAR,
				std::string(1, result.value())
			);
		}


		Result<struct Token> LexString(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> peekRes = tracker.peek();
			if(!peekRes) {
				return std::unexpected(peekRes.error());
			}
			if(peekRes.value().at(0) != '"') {
				return std::unexpected("No string token found");
			}
			std::expected<Success, std::string> shiftRes = tracker.shift();
			if(!shiftRes) {
				return std::unexpected(shiftRes.error());
			}

			std::string str = "";

			while(true) {
				peekRes = tracker.peek();
				if(!peekRes) {
					return std::unexpected("Unexpected end of file.");
				}
				char first = peekRes.value().at(0);

				if(first == '"') {
					shiftRes = tracker.shift();
					if(!shiftRes) {
						return std::unexpected(shiftRes.error());
					}
					break;
				}

				if(first == '\\') {
					// peek 2 chars for escape sequence
					peekRes = tracker.peek(2);
					if(!peekRes) {
						return std::unexpected(peekRes.error());
					}

					std::expected<char, std::string> result = to_escaped_char(peekRes.value());
					if(!result) {
						return std::unexpected(result.error());
					}
					str += std::string(1, result.value());

					shiftRes = tracker.shift(2);
					if(!shiftRes) {
						return std::unexpected(shiftRes.error());
					}
					continue;
				}

				shiftRes = tracker.shift();
				if(!shiftRes) {
					return std::unexpected(shiftRes.error());
				}
				str += shiftRes.value();
			}

			return MakeToken(
				TokenType::STRING,
				str
			);
		}


		Result<struct Token> LexBinaryOperator(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(
				result.value() == "+"
				|| result.value() == "-"
				|| result.value() == "*"
				|| result.value() == "/"
				|| result.value() == ">"
				|| result.value() == "<"
				|| result.value() == "&"
				|| result.value() == "|"
				|| result.value() == "^"
			) {
				return MakeToken(
					TokenType::BINARY_OPERATOR,
					tracker.shift()
				);
			}
			
			result = tracker.peek(2);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(
				result.value() == ">="
				|| result.value() == "<="
				|| result.value() == "&&"
				|| result.value() == "||"
				|| result.value() == "=="
				|| result.value() == "!="
			) {
				return MakeToken(
					TokenType::BINARY_OPERATOR,
					tracker.shift(2)
				);
			}
			
			return std::unexpected("No binary operator token found.");
		}

		Result<struct Token> LexUnaryOperator(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "!" || result.value() == "~") {
				return MakeToken(
					TokenType::UNARY_OPERATOR,
					tracker.shift()
				);
			}

			result = tracker.peek(2);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "++" || result.value() == "--") {
				return MakeToken(
					TokenType::UNARY_OPERATOR,
					tracker.shift(2)
				);
			}

			return std::unexpected("No unary operator token found.");
		}

		Result<struct Token> LexParenthesis(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "(" || result.value() == ")") {
				return MakeToken(
					TokenType::PARENTHESIS,
					tracker.shift()
				);
			}
			return std::unexpected("No parenthesis token found.");
		}

		Result<struct Token> LexBrace(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "{" || result.value() == "}") {
				return MakeToken(
					TokenType::BRACE,
					tracker.shift()
				);
			}
			return std::unexpected("No brace token found.");
		}

		Result<struct Token> LexDot(
			LexTracker& tracker
		) noexcept {
			
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == ".") {
				return MakeToken(
					TokenType::DOT,
					tracker.shift()
				);
			}
			return std::unexpected("No dot token found.");
		}

		Result<struct Token> LexComma(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == ",") {
				return MakeToken(
					TokenType::COMMA,
					tracker.shift()
				);
			}
			return std::unexpected("No comma token found.");
		}

		Result<struct Token> LexColon(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == ":") {
				return MakeToken(
					TokenType::COLON,
					tracker.shift()
				);
			}
			return std::unexpected("No colon token found.");
		}

		Result<struct Token> LexEquals(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "=") {
				return MakeToken(
					TokenType::EQUALS,
					tracker.shift()
				);
			}
			return std::unexpected("No equals token found.");
		}

		Result<struct Token> LexArrow(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "->") {
				return MakeToken(
					TokenType::ARROW,
					tracker.shift(2)
				);
			}
			return std::unexpected("No arrow token found.");
		}

		Result<struct Token> LexAt(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "@") {
				return MakeToken(
					TokenType::AT,
					tracker.shift()
				);
			}
			return std::unexpected("No at token found.");
		}

		Result<struct Token> LexDoubleDot(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "..") {
				return MakeToken(
					TokenType::DOUBLE_DOT,
					tracker.shift(2)
				);
			}
			return std::unexpected("No double dot token found.");
		}

		Result<struct Token> LexIfElse(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "if") {
				return MakeToken(
					TokenType::IFELSE,
					tracker.shift(2)
				);
			}
			result = tracker.peek(4);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "else") {
				return MakeToken(
					TokenType::IFELSE,
					tracker.shift(4)
				);
			}
			result = tracker.peek(6);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "elseif") {
				return MakeToken(
					TokenType::IFELSE,
					tracker.shift(6)
				);
			}

			return std::unexpected("No if-else token found.");
		}

		Result<struct Token> LexMatch(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek(5);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "match") {
				return MakeToken(
					TokenType::MATCH,
					tracker.shift(5)
				);
			}
			return std::unexpected("No match token found.");
		}

		Result<struct Token> LexLoop(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(4);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "loop") {
				return MakeToken(
					TokenType::LOOP,
					tracker.shift(4)
				);
			}

			return std::unexpected("No loop token found.");
		}

		Result<struct Token> LexBreak(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(5);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "break") {
				return MakeToken(
					TokenType::BREAK,
					tracker.shift(5)
				);
			}

			return std::unexpected("No break token found.");
		}

		Result<struct Token> LexOr(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "or") {
				return MakeToken(
					TokenType::OR,
					tracker.shift(2)
				);
			}

			return std::unexpected("No or token found.");
		}

		Result<struct Token> LexDecl(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(3);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "var") {
				return MakeToken(
					TokenType::DECL,
					tracker.shift(3)
				);
			}
			result = tracker.peek(5);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "const") {
				return MakeToken(
					TokenType::DECL,
					tracker.shift(5)
				);
			}

			return std::unexpected("No var/const token found.");
		}

		Result<struct Token> LexType(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "i8") {
				(void)tracker.shift(2);
				return MakeToken(
					TokenType::DOT,
					"I8"
				);
			}
			if(result.value() == "u8") {
				(void)tracker.shift(2);
				return MakeToken(
					TokenType::DOT,
					"U8"
				);
			}

			result = tracker.peek(3);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "i16") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"I16"
				);
			}
			if(result.value() == "i32") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"I32"
				);
			}
			if(result.value() == "i64") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"I64"
				);
			}
			if(result.value() == "u16") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"U16"
				);
			}
			if(result.value() == "u32") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"U32"
				);
			}
			if(result.value() == "u64") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"U64"
				);
			}
			if(result.value() == "f32") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"F32"
				);
			}
			if(result.value() == "f64") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"F64"
				);
			}
			if(result.value() == "str") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"STRING"
				);
			}

			result = tracker.peek(4);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "void") {
				(void)tracker.shift(4);
				return MakeToken(
					TokenType::TYPE,
					"VOID"
				);
			}
			if(result.value() == "char") {
				(void)tracker.shift(4);
				return MakeToken(
					TokenType::TYPE,
					"CHAR"
				);
			}
			if(result.value() == "bool") {
				(void)tracker.shift(4);
				return MakeToken(
					TokenType::TYPE,
					"BOOL"
				);
			}

			return std::unexpected("No type token found.");
		}

		Result<struct Token> LexFn(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "fn") {
				return MakeToken(
					TokenType::FN,
					tracker.shift(2)
				);
			}

			return std::unexpected("No fn token found.");
		}

		Result<struct Token> LexReturn(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(6);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "return") {
				return MakeToken(
					TokenType::RETURN,
					tracker.shift(6)
				);
			}

			return std::unexpected("No return token found.");
		}

		Result<struct Token> LexStruct(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(6);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "struct") {
				return MakeToken(
					TokenType::STRUCT,
					tracker.shift(6)
				);
			}

			return std::unexpected("No struct token found.");
		}

		Result<struct Token> LexImpl(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(4);
			if(!result) {
				return std::unexpected(result.error());
			}
			if(result.value() == "impl") {
				return MakeToken(
					TokenType::IMPL,
					tracker.shift(4)
				);
			}

			return std::unexpected("No impl token found.");
		}

		Result<struct Token> LexIdentifier(
			LexTracker& tracker
		) noexcept {
			std::string identifier = "";

			while(
				src.size() > 0 && (
					std::isalpha(src.front()) ||
					std::isdigit(src.front()) ||
					src.front() == '_'
				)
			) {
				identifier += utils::shift(src);
			}
			
			if(
				identifier.size() == 0
			) {
				return std::unexpected("Invalid identifier found.");
			}

			if(identifier == "_") {
				return MakeToken(
					TokenType::DISCARD,
					identifier
				);
			}

			return MakeToken(
				TokenType::IDENTIFIER,
				identifier
			);
		}

		void StripWhitespaces(
			LexTracker& tracker
		) noexcept {
			while(src.size() > 0 && std::isspace(src.front())) {
				(void)utils::shift(src);
			}
		}

		std::expected<Success, std::string> StripComments(
			LexTracker& tracker
		) noexcept {
			if(src.rfind("/*", 0) == 0) {
				// To avoid /*/
				(void)utils::shift(src, 2);
				
				while(src.size() > 0) {
					if(src.rfind("*/", 0) == 0) {
						(void)utils::shift(src, 2);
						return Success{};
					} 
					(void)utils::shift(src);
				}
				return std::unexpected("Unclosed multiline comment.");
			}
			if(src.rfind("//", 0) == 0) {
				while(src.size() > 0 && src.front() != '\n') {
					(void)utils::shift(src);
				}
			}
			return Success{};
		}

		std::expected<Success, std::string> Lex(
			std::vector<struct Token>& tokens,
			LexTracker& tracker
		) noexcept {
			while(src.size() > 0) {
				
				StripWhitespaces(src);
				if(src.size() == 0) {
					break;
				}

				std::expected<Success, std::string> stripResult = StripComments(
					src
				);
				if(!stripResult) {
					return std::unexpected(stripResult.error());
				}
				if(src.size() == 0) {
					break;
				}

				StripWhitespaces(src);
				if(src.size() == 0) {
					break;
				}

				std::string error = "";
				for(const auto& lexFunction : LexFunctionsList) {
					
					Result<struct Token> result = lexFunction(src);
					if(!result) {
						error = result.error();
						continue;
					}

					tokens.push_back(result.value());
					error = "";
					break;
				}

				if(error.size() > 0) {
					return std::unexpected(
						"[ERR::LEXER] Got error :\n\t\"" + error + "\"\nwhile lexing source. \""
						+ src
					);
				}
			}
			return Success{};
		}
	}
}
