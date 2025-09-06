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

				case TokenType::STRING:
					return "STRING(\"" + token.value + "\")";
				
				case TokenType::EOL:
				case TokenType::UNARY_OPERATOR:
				case TokenType::BINARY_OPERATOR:
				case TokenType::PARENTHESIS:
				case TokenType::BRACE:
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
					return token.value;

				case TokenType::IDENTIFIER:
					return "ID(" + token.value + ")";
				
				default:
					return "UNKNOWN";
			}
		}

		std::expected<struct Token, std::string> LexEOL(
			std::string& src
		) noexcept {

			if(src.front() == ';') {
				return MakeToken(
					TokenType::EOL,
					std::string(1, utils::shift(src))
				);
			}

			return std::unexpected("Invalid EOL " + src.front());
		}

		std::expected<struct Token, std::string> LexNull(
			std::string& src
		) noexcept {

			if(src.rfind("null", 0) == 0) {
				return MakeToken(
					TokenType::NUL,
					utils::shift(src, 4)
				);
			}

			return std::unexpected("No null token found");
		}

		std::expected<struct Token, std::string> LexNumber(
			std::string& src
		) noexcept {
			std::string number = "";
			bool isDecimal = false;

			while(src.length() > 0) {
				char first = src.front();

				if(first == '.') {
					if(isDecimal) {
						src = std::string(".") + src;
						number.pop_back();
						break;
					}
					isDecimal = true;
					number += utils::shift(src);
					continue;
				}

				if(std::isdigit(first)) {
					number += utils::shift(src);
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

		std::expected<struct Token, std::string> LexBoolean(
			std::string& src
		) noexcept {

			if(src.rfind("true", 0) == 0) {
				return MakeToken(
					TokenType::BOOLEAN,
					utils::shift(src, 4)
				);
			}
			if(src.rfind("false", 0) == 0) {
				return MakeToken(
					TokenType::BOOLEAN,
					utils::shift(src, 5)
				);
			}

			return std::unexpected("No boolean token found");
		}

		std::expected<struct Token, std::string> LexString(
			std::string& src
		) noexcept {

			if(src.at(0) != '"') {
				return std::unexpected("No string token found");
			}
			(void)utils::shift(src);

			std::string str = "";
			bool isBackslashed = false;

			while(src.length() > 0) {
				char first = utils::shift(src);

				if(isBackslashed) {
					isBackslashed = false;
					if(first == '"') {
						str += first;
						continue;
					}
				}

				if(first == '\\') {
					str += first;
					isBackslashed = true;
					continue;
				}

				if(first == '"') {
					break;
				}

				str += first;
			}
			
			return MakeToken(
				TokenType::STRING,
				str
			);
		}

		std::expected<struct Token, std::string> LexBinaryOperator(
			std::string& src
		) noexcept {
			
			if(
				src.rfind(">=", 0) == 0
				|| src.rfind("<=", 0) == 0
				|| src.rfind("&&", 0) == 0
				|| src.rfind("||", 0) == 0
				|| src.rfind("==", 0) == 0
				|| src.rfind("!=", 0) == 0
			) {
				return MakeToken(
					TokenType::BINARY_OPERATOR,
					utils::shift(src, 2)
				);
			}

			switch(src.front()) {
				case '+':
				case '-':
				case '*':
				case '/':
				case '>':
				case '<':
				case '&':
				case '|':
				case '^':
					return MakeToken(
						TokenType::BINARY_OPERATOR,
						std::string(1, utils::shift(src))
					);
				default:
					return std::unexpected("Invalid operator " + src.front());
			}
		}

		std::expected<struct Token, std::string> LexUnaryOperator(
			std::string& src
		) noexcept {
			if(src.front() == '!') {
				return MakeToken(
					TokenType::UNARY_OPERATOR,
					std::string(1, utils::shift(src))
				);
			}
			if(src.front() == '~') {
				return MakeToken(
					TokenType::UNARY_OPERATOR,
					std::string(1, utils::shift(src))
				);
			}
			if(
				src.rfind("++", 0) == 0
				|| src.rfind("--", 0) == 0
			) {
				return MakeToken(
					TokenType::UNARY_OPERATOR,
					utils::shift(src, 2)
				);
			}

			return std::unexpected("No unary operator token found.");
		}

		std::expected<struct Token, std::string> LexParenthesis(
			std::string& src
		) noexcept {

			switch(src.front()) {
				case '(':
				case ')':
					return MakeToken(
						TokenType::PARENTHESIS,
						std::string(1, utils::shift(src))
					);
				default:
					return std::unexpected("Invalid parenthesis " + src.front());
			}
		}

		std::expected<struct Token, std::string> LexBrace(
			std::string& src
		) noexcept {

			switch(src.front()) {
				case '{':
				case '}':
					return MakeToken(
						TokenType::BRACE,
						std::string(1, utils::shift(src))
					);
				default:
					return std::unexpected("Invalid brace " + src.front());
			}
		}

		std::expected<struct Token, std::string> LexDot(
			std::string& src
		) noexcept {
			if(src.front() == '.') {
				return MakeToken(
					TokenType::DOT,
					std::string(1, utils::shift(src))
				);
			}
			return std::unexpected("No dot token found.");
		}

		std::expected<struct Token, std::string> LexColon(
			std::string& src
		) noexcept {
			if(src.front() == ':') {
				return MakeToken(
					TokenType::COLON,
					std::string(1, utils::shift(src))
				);
			}
			return std::unexpected("No colon token found.");
		}

		std::expected<struct Token, std::string> LexEquals(
			std::string& src
		) noexcept {
			if(src.front() == '=') {
				return MakeToken(
					TokenType::EQUALS,
					std::string(1, utils::shift(src))
				);
			}
			return std::unexpected("No equals token found.");
		}

		std::expected<struct Token, std::string> LexArrow(
			std::string& src
		) noexcept {
			if(src.rfind("->", 0) == 0) {
				return MakeToken(
					TokenType::ARROW,
					utils::shift(src, 2)
				);
			}
			return std::unexpected("No arrow token found.");
		}

		std::expected<struct Token, std::string> LexAt(
			std::string& src
		) noexcept {
			if(src.front() == '@') {
				return MakeToken(
					TokenType::AT,
					utils::shift(src, 1)
				);
			}
			return std::unexpected("No at token found.");
		}

		std::expected<struct Token, std::string> LexDoubleDot(
			std::string& src
		) noexcept {
			if(src.rfind("..", 0) == 0) {
				return MakeToken(
					TokenType::DOUBLE_DOT,
					utils::shift(src, 2)
				);
			}
			return std::unexpected("No double dot token found.");
		}

		std::expected<struct Token, std::string> LexIfElse(
			std::string& src
		) noexcept {

			if(src.rfind("elseif", 0) == 0) {
				return MakeToken(
					TokenType::IFELSE,
					utils::shift(src, 6)
				);
			}
			if(src.rfind("if", 0) == 0) {
				return MakeToken(
					TokenType::IFELSE,
					utils::shift(src, 2)
				);
			}
			if(src.rfind("else", 0) == 0) {
				return MakeToken(
					TokenType::IFELSE,
					utils::shift(src, 4)
				);
			}

			return std::unexpected("No if-else token found.");
		}

		std::expected<struct Token, std::string> LexMatch(
			std::string& src
		) noexcept {
			if(src.rfind("match", 0) == 0) {
				return MakeToken(
					TokenType::MATCH,
					utils::shift(src, 5)
				);
			}
			return std::unexpected("No match token found.");
		}

		std::expected<struct Token, std::string> LexLoop(
			std::string& src
		) noexcept {

			if(src.rfind("loop", 0) == 0) {
				return MakeToken(
					TokenType::LOOP,
					utils::shift(src, 4)
				);
			}

			return std::unexpected("No loop token found.");
		}

		std::expected<struct Token, std::string> LexBreak(
			std::string& src
		) noexcept {

			if(src.rfind("break", 0) == 0) {
				return MakeToken(
					TokenType::BREAK,
					utils::shift(src, 5)
				);
			}

			return std::unexpected("No break token found.");
		}

		std::expected<struct Token, std::string> LexOr(
			std::string& src
		) noexcept {

			if(src.rfind("or", 0) == 0) {
				return MakeToken(
					TokenType::OR,
					utils::shift(src, 2)
				);
			}

			return std::unexpected("No or token found.");
		}

		std::expected<struct Token, std::string> LexDecl(
			std::string& src
		) noexcept {

			if(src.rfind("var", 0) == 0) {
				return MakeToken(
					TokenType::DECL,
					utils::shift(src, 3)
				);
			}
			if(src.rfind("const", 0) == 0) {
				return MakeToken(
					TokenType::DECL,
					utils::shift(src, 5)
				);
			}

			return std::unexpected("No var/const token found.");
		}

		std::expected<struct Token, std::string> LexType(
			std::string& src
		) noexcept {

			if(
				src.rfind("i8", 0) == 0
				|| src.rfind("u8", 0) == 0
			) {
				return MakeToken(
					TokenType::TYPE,
					utils::shift(src, 2)
				);
			}
			if(
				src.rfind("i16", 0) == 0
				|| src.rfind("i32", 0) == 0
				|| src.rfind("i64", 0) == 0
				|| src.rfind("u16", 0) == 0
				|| src.rfind("u32", 0) == 0
				|| src.rfind("u64", 0) == 0
				|| src.rfind("f32", 0) == 0
				|| src.rfind("f64", 0) == 0
				|| src.rfind("str", 0) == 0
			) {
				return MakeToken(
					TokenType::TYPE,
					utils::shift(src, 3)
				);
			}
			if(
				src.rfind("void", 0) == 0
				|| src.rfind("char", 0) == 0
				|| src.rfind("bool", 0) == 0
			) {
				return MakeToken(
					TokenType::TYPE,
					utils::shift(src, 4)
				);
			}

			return std::unexpected("No type token found.");
		}

		std::expected<struct Token, std::string> LexFn(
			std::string& src
		) noexcept {

			if(src.rfind("fn", 0) == 0) {
				return MakeToken(
					TokenType::FN,
					utils::shift(src, 2)
				);
			}

			return std::unexpected("No fn token found.");
		}

		std::expected<struct Token, std::string> LexReturn(
			std::string& src
		) noexcept {

			if(src.rfind("return", 0) == 0) {
				return MakeToken(
					TokenType::RETURN,
					utils::shift(src, 6)
				);
			}

			return std::unexpected("No return token found.");
		}

		std::expected<struct Token, std::string> LexIdentifier(
			std::string& src
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
			std::string& src
		) noexcept {
			while(src.size() > 0 && std::isspace(src.front())) {
				(void)utils::shift(src);
			}
		}

		std::expected<Success, std::string> StripComments(
			std::string& src
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
			std::string& src
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
					
					std::expected<struct Token, std::string> result = lexFunction(src);
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
