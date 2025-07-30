#include <parser/parser.hxx>

#include <stdexcept>

namespace dim {
	namespace parser {

		[[nodiscard]] struct lexer::Token eat(
			std::vector<struct lexer::Token>& tokens
		) {
			struct lexer::Token tk = tokens.front();
			tokens.erase(tokens.begin());
			return tk;
		}

		[[nodiscard]] struct lexer::Token expect(
			std::vector<struct lexer::Token>& tokens,
			struct lexer::Token expected
		) {
			struct lexer::Token tk = eat(tokens);

			if(tk.type != expected.type && expected.value == "") {
				throw std::runtime_error(
					std::string("Invalid token type: got ")
					+ std::to_string(int(tk.type)) + " expected "
					+ std::to_string(int(expected.type))
				);
			}

			if(tk.type != expected.type && tk.value != expected.value) {
				throw std::runtime_error(
					std::string("Invalid token value: got ")
					+ tk.value + " expected " + expected.value
				);
			}

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

		std::shared_ptr<Expression> parse_parenthesis_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			if(
				tokens.front().type != lexer::TokenType::PARENTHESIS ||
				tokens.front().value != "("
			) {
				return parse_number_expression(tokens);
			}

			(void)eat(tokens);

			std::shared_ptr<Expression> inner = parse_expression(tokens);

			(void)expect(tokens, lexer::MakeToken(
				lexer::TokenType::PARENTHESIS,
				")"
			));

			return inner;
		}

		std::shared_ptr<Expression> parse_multiplicative_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			std::shared_ptr<Expression> left = parse_parenthesis_expression(tokens);

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