#include <parser/parser.hxx>

namespace dim {
	namespace parser {

		[[nodiscard]]
		std::expected<struct lexer::Token, std::string> eat(
			std::vector<struct lexer::Token>& tokens
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Trying to eat a token that in empty token list.");
			}
			struct lexer::Token tk = tokens.front();
			tokens.erase(tokens.begin());
			return tk;
		}

		[[nodiscard]]
		std::expected<struct lexer::Token, std::string> expect(
			std::vector<struct lexer::Token>& tokens,
			struct lexer::Token expected
		) {

			std::expected<struct lexer::Token, std::string> result = eat(tokens);

			if(!result) {
				return std::unexpected(
					result.error() + " Expected "
					+ std::string(lexer::TokenTypeStr.at(int(expected.type)))
					+ " (" + expected.value + ")"
				);
			}

			struct lexer::Token tk = result.value();

			if(tk.type != expected.type && expected.value == "") {
				tokens.insert(tokens.begin(), tk);
				return std::unexpected(
					std::string("Invalid token type: got ")
					+ std::string(lexer::TokenTypeStr.at(int(tk.type))) + " expected "
					+ std::string(lexer::TokenTypeStr.at(int(expected.type)))
					+ " (" + tk.value + ")"
				);
			}

			if(tk.type != expected.type && tk.value != expected.value) {
				tokens.insert(tokens.begin(), tk);
				return std::unexpected(
					std::string("Invalid token value: got ")
					+ tk.value + " expected " + expected.value
				);
			}

			return tk;
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_null_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::NUL)
			)

			return std::make_shared<NullExpression>();
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_boolean_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			if(tokens.front().type != lexer::TokenType::BOOLEAN) {
				return parse_null_expression(tokens);
			}

			return std::make_shared<BooleanExpression>(
				eat(tokens).value().value
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_string_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			if(tokens.front().type != lexer::TokenType::STRING) {
				return parse_boolean_expression(tokens);
			}

			return std::make_shared<StringExpression>(
				eat(tokens).value().value
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_number_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			if(tokens.front().type != lexer::TokenType::NUMBER) {
				return parse_string_expression(tokens);
			}

			return std::make_shared<NumberExpression>(
				eat(tokens).value().value
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_parenthesis_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			if(
				tokens.front().type != lexer::TokenType::PARENTHESIS ||
				tokens.front().value != "("
			) {
				return parse_number_expression(tokens);
			}

			__TRY_TOKEN_FUNC_WRETERR(
				eat,
				tokens
			)

			std::shared_ptr<Expression> inner;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_expression,
				tokens,
				inner
			)

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(
					lexer::TokenType::PARENTHESIS,
					")"
				)
			)

			return inner;
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_multiplicative_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			std::shared_ptr<Expression> left;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_parenthesis_expression,
				tokens,
				left
			)

			while(
				tokens.size() != 0 &&
				tokens.front().type == lexer::TokenType::BINARY_OPERATOR &&
				(tokens.front().value == "*" || tokens.front().value == "/")
			) {
				std::string operatorSymbol = eat(tokens).value().value;

				std::shared_ptr<Expression> right;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_multiplicative_expression,
					tokens,
					right
				)

				left = std::make_shared<BinaryExpression>(
					left,
					operatorSymbol,
					right
				);
			}

			return left;
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_additive_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			std::shared_ptr<Expression> left;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_multiplicative_expression,
				tokens,
				left
			)

			while(
				tokens.size() != 0 &&
				tokens.front().type == lexer::TokenType::BINARY_OPERATOR &&
				(tokens.front().value == "+" || tokens.front().value == "-")
			) {
				std::string operatorSymbol = eat(tokens).value().value;

				std::shared_ptr<Expression> right;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_multiplicative_expression,
					tokens,
					right
				)

				left = std::make_shared<BinaryExpression>(
					left,
					operatorSymbol,
					right
				);
			}

			return left;
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_binary_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			return parse_additive_expression(tokens);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_ifelse_expression(
			std::vector<struct lexer::Token>& tokens,
			const bool allow_if
		) {
			struct lexer::Token keyword;

			if(
				tokens.size() > 0 && tokens.front().type == lexer::TokenType::IFELSE
				&& !allow_if && tokens.front().value == "if"
			) {
				return std::unexpected("Start of a new structure");
			}

			__TRY_TOKEN_FUNC_WRETERR_WSAVE(
				expect,
				keyword,
				tokens,
				lexer::MakeToken(lexer::TokenType::IFELSE)
			)

			std::shared_ptr<Expression> condition = nullptr;
			if(keyword.value != "else") {
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_parenthesis_expression,
					tokens,
					condition
				)
			}

			std::shared_ptr<Expression> scope;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_scope_expression,
				tokens,
				scope
			)

			return std::make_shared<IfElseExpression>(
				std::dynamic_pointer_cast<ScopeExpression>(scope),
				condition
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_ifelse_structure(
			std::vector<struct lexer::Token>& tokens
		) {
			std::expected<
				std::shared_ptr<Expression>,
				std::string
			> result = parse_ifelse_expression(tokens);

			if(!result) {
				return parse_binary_expression(tokens);
			}

			std::vector<std::shared_ptr<IfElseExpression>> expressions = {};
			while(result) {
				expressions.push_back(
					std::dynamic_pointer_cast<IfElseExpression>(result.value())
				);
				result = parse_ifelse_expression(tokens, false);
			}

			return std::make_shared<IfElseStructure>(
				expressions
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			return parse_ifelse_structure(tokens);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_scope_expression(
			std::vector<struct lexer::Token>& tokens
		) {
			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::BRACE, "{")
			)

			auto scope = std::make_shared<ScopeExpression>();

			bool closingBraceFound = false;

			while(tokens.size() > 0) {

				if(
					tokens.front().type == lexer::TokenType::BRACE
					&& tokens.front().value == "}"
				) {
					(void)eat(tokens);
					closingBraceFound = true;
					break;
				}

				std::shared_ptr<Expression> expr;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					expr
				)

				scope->AddExpression(
					expr
				);
				
				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::EOL)
				)
			}

			if(!closingBraceFound) {
				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::BRACE, "}")
				)
			}

			return scope;
		}

		std::expected<
			std::shared_ptr<ScopeExpression>,
			std::string
		> Parse(
			std::vector<struct lexer::Token>& tokens
		) {
			std::shared_ptr<Expression> program_expr;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_scope_expression,
				tokens,
				program_expr
			);

			return std::dynamic_pointer_cast<ScopeExpression>(program_expr);
		}
	}
}