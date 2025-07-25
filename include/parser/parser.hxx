#include <parser/ast.hxx>

#include <lexer/lexer.hxx>

#include <memory>
#include <vector>

namespace dim {
	namespace parser {

		[[nodiscard]] struct dim::lexer::Token eat(
			std::vector<struct dim::lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_number_expression(
			std::vector<struct dim::lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_multiplicative_expression(
			std::vector<struct dim::lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_additive_expression(
			std::vector<struct dim::lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_binary_expression(
			std::vector<struct dim::lexer::Token>& tokens
		);

		std::shared_ptr<Expression> parse_expression(
			std::vector<struct dim::lexer::Token>& tokens
		);

		std::shared_ptr<ScopeExpression> Parse(
			std::vector<struct dim::lexer::Token>& tokens
		);
	}
}
