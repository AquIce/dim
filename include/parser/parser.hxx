#include <parser/ast.hxx>

#include <lexer/lexer.hxx>

#include <memory>
#include <vector>

namespace dim {
	namespace parser {

		[[nodiscard]] struct lexer::Token eat(
			std::vector<struct lexer::Token>& tokens
		);

		[[nodiscard]] struct lexer::Token expect(
			std::vector<struct lexer::Token>& tokens,
			struct lexer::Token expected
		);

		std::shared_ptr<Expression> parse_number_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_parenthesis_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_multiplicative_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_additive_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_binary_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::shared_ptr<ScopeExpression> Parse(
			std::vector<struct lexer::Token>& tokens
		);
	}
}
