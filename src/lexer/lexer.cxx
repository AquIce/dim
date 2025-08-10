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
				
				case TokenType::EOL:
					return token.value;

				case TokenType::NUL:
					return "NULL";

				case TokenType::NUMBER:
					return "NUMBER(" + token.value + ")";
				
				case TokenType::BINARY_OPERATOR:
					return token.value;
				
				case TokenType::PARENTHESIS:
					return token.value;
				
				case TokenType::BOOLEAN:
					return "BOOLEAN(" + token.value + ")";

				case TokenType::STRING:
					return "STRING(\"" + token.value + "\")";
				
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
						return std::unexpected("Invalid number literal \"" + number + ".\"");
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

		std::expected<struct Token, std::string> LexBinaryOperator(
			std::string& src
		) noexcept {

			switch(src.front()) {
				case '+':
				case '-':
				case '*':
				case '/':
					return MakeToken(
						TokenType::BINARY_OPERATOR,
						std::string(1, utils::shift(src))
					);
				default:
					return std::unexpected("Invalid operator " + src.front());
			}
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

		std::expected<Success, std::string> Lex(
			std::vector<struct Token>& tokens,
			std::string& src
		) noexcept {
			while(src.size() > 0) {
				while(src.size() > 0 && std::isspace(src.front())) {
					(void)utils::shift(src);
				}
				if(src.size() == 0) {
					break;
				}

				bool token_added = false;
				for(const auto& lexFunction : LexFunctionsList) {
					
					std::expected<struct Token, std::string> result = lexFunction(src);
					if(!result) {
						continue;
					}

					tokens.push_back(result.value());
					token_added = true;
					break;
				}

				if(!token_added) {
					return std::unexpected(
						"[ERR] Got error (No valid token found) while lexing source. \""
						+ src
					);
				}
			}
			return Success{};
		}
	}
}
