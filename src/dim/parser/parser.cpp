#include <dim/parser/parser.hpp>

[[nodiscard]] struct dim::lexer::Token dim::parser::eat(
	std::vector<struct dim::lexer::Token>& tokens
) {
	struct dim::lexer::Token tk = tokens.front();
	tokens.erase(tokens.begin());
	return tk;
}

std::shared_ptr<dim::parser::Expression> dim::parser::parse_number_expression(
	std::vector<struct dim::lexer::Token>& tokens
) {
	if(tokens.front().type != dim::lexer::TokenType::NUMBER) {
		(void)dim::parser::eat(tokens);
		return std::make_shared<dim::parser::Expression>();
	}

	return std::make_shared<dim::parser::NumberExpression>(
		dim::parser::eat(tokens).value
	);
}

std::shared_ptr<dim::parser::Expression> dim::parser::parse_multiplicative_expression(
	std::vector<struct dim::lexer::Token>& tokens
) {
	std::shared_ptr<dim::parser::Expression> left = dim::parser::parse_number_expression(tokens);

	if(
		!(
			tokens.front().type == dim::lexer::TokenType::OPERATOR &&
			(tokens.front().value == "*" || tokens.front().value == "/")
		)
	) {
		return left;
	}

	std::string operatorSymbol = dim::parser::eat(tokens).value;

	return std::make_shared<dim::parser::BinaryExpression>(
		left,
		operatorSymbol,
		dim::parser::parse_multiplicative_expression(tokens)
	);
}

std::shared_ptr<dim::parser::Expression> dim::parser::parse_additive_expression(
	std::vector<struct dim::lexer::Token>& tokens
) {
	std::shared_ptr<dim::parser::Expression> left = dim::parser::parse_multiplicative_expression(tokens);

	if(
		!(
			tokens.front().type == dim::lexer::TokenType::OPERATOR &&
			(tokens.front().value == "+" || tokens.front().value == "-")
		)
	) {
		return left;
	}

	std::string operatorSymbol = dim::parser::eat(tokens).value;

	return std::make_shared<dim::parser::BinaryExpression>(
		left,
		operatorSymbol,
		dim::parser::parse_additive_expression(tokens)
	);
}

std::shared_ptr<dim::parser::Expression> dim::parser::parse_binary_expression(
	std::vector<struct dim::lexer::Token>& tokens
) {
	return dim::parser::parse_additive_expression(tokens);
}

std::shared_ptr<dim::parser::Expression> dim::parser::parse_expression(
	std::vector<struct dim::lexer::Token>& tokens
) {
	return dim::parser::parse_binary_expression(tokens);
}

std::vector<std::shared_ptr<dim::parser::Expression>> dim::parser::Parse(
	std::vector<struct dim::lexer::Token>& tokens
) {
	std::vector<std::shared_ptr<dim::parser::Expression>> expressions = {};

	while(tokens.size() > 0) {
		expressions.push_back(
			dim::parser::parse_expression(tokens)
		);
	}

	return expressions;
}
