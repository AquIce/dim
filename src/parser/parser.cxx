#include <parser/parser.hxx>

namespace dim {
	namespace parser {

		[[nodiscard]] struct lexer::Token eat(
			std::vector<struct lexer::Token>& tokens
		) {
			struct lexer::Token tk = tokens.front();
			tokens.erase(tokens.begin());
			return tk;
		}

		std::shared_ptr<Expression> parse_number_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			lexer::Token token = eat(tokens);

			if(token.type != lexer::TokenType::NUMBER) {
				return std::make_shared<Expression>();
			}

			return std::make_shared<NumberExpression>(
				token.value
			);
		}

		std::shared_ptr<Expression> parse_multiplicative_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			std::shared_ptr<Expression> left = parse_number_expression(tokens);

			while(
				tokens.size() != 0 &&
				tokens.front().type == lexer::TokenType::BINARY_OPERATOR &&
				(tokens.front().value == "*" || tokens.front().value == "/")
			) {
				std::string operatorSymbol = eat(tokens).value;

				left = std::make_shared<BinaryExpression>(
					left,
					operatorSymbol,
					parse_multiplicative_expression(tokens)
				);
			}

			return left;
		}

		std::shared_ptr<Expression> parse_additive_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			std::shared_ptr<Expression> left = parse_multiplicative_expression(tokens);

			while(
				tokens.size() != 0 &&
				tokens.front().type == lexer::TokenType::BINARY_OPERATOR &&
				(tokens.front().value == "+" || tokens.front().value == "-")
			) {
				std::string operatorSymbol = eat(tokens).value;

				left = std::make_shared<BinaryExpression>(
					left,
					operatorSymbol,
					parse_multiplicative_expression(tokens)
				);
			}

			return left;
		}

		std::shared_ptr<Expression> parse_binary_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			return parse_additive_expression(tokens);
		}

		std::shared_ptr<Expression> parse_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			return parse_binary_expression(tokens);
		}

		std::shared_ptr<ScopeExpression> Parse(
			std::vector<struct lexer::Token>& tokens
		) {
			auto program = std::make_shared<ScopeExpression>();

			while(tokens.size() > 0) {
				program->AddExpression(
					parse_expression(tokens)
				);
			}

			return program;
		}
	}
}