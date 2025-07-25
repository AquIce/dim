#include <lexer/lexer.hxx>
#include <iostream>
namespace dim {
	namespace lexer {

		struct Token MakeToken(
			const TokenType type,
			const std::string value
		) {
			return Token{
				type, value
			};
		}

		std::string TokenRepr(
			const struct Token& token
		) {
			switch(token.type) {
				case TokenType::NONE:
					return "NONE";

				case TokenType::NUMBER:
					return "NUMBER(" + token.value + ")";
				
				case TokenType::BINARY_OPERATOR:
					return token.value;
				
				default:
					return "UNKNOWN";
			}
		}

		struct Token LexNumber(
			std::string& src
		) {
			std::string number = "";
			bool isDecimal = false;

			while(src.length() > 0) {
				char first = src.at(0);

				if(first == '.') {
					if(isDecimal) {
						throw std::runtime_error("Invalid number literal \"" + number + ".\"");	
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
				return MakeToken();
			}
			return MakeToken(
				TokenType::NUMBER,
				number
			);
		}

		Token LexBinaryOperator(
			std::string& src
		) {
			char first_char = utils::shift(src);

			switch(first_char) {
				case '+':
				case '-':
				case '*':
				case '/':
					return MakeToken(
						TokenType::BINARY_OPERATOR,
						std::string(1, first_char)
					);
				default:
					return MakeToken();
			}
		}

		bool TryAddToken(
			std::vector<struct Token>& tokens,
			std::string& src,
			std::function<struct Token (std::string& src)> lexFunction
		) {
			struct Token token = lexFunction(src);

			if(token.type != TokenType::NONE) {
				tokens.push_back(token);
				return true;
			}

			return false;
		}

		void Lex(
			std::vector<struct Token>& tokens,
			std::string& src
		) {
			while(src.size() > 0) {
				while(src.size() > 0 && std::isspace(src.front())) {
					(void)utils::shift(src);
				}
				if(src.size() == 0) {
					break;
				}

				struct Token token = MakeToken();

				for(const auto& lexFunction : LexFunctionsList) {
					if(TryAddToken(tokens, src, lexFunction)) {
						break;
					}
				}
			}
		}
	}
}
