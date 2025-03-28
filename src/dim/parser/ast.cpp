#include <dim/parser/ast.hpp>

std::string dim::parser::Expression::Repr() {
	return "INVALID EXPRESSION";
}

dim::parser::NodeType dim::parser::Expression::Type() {
	return dim::parser::NodeType::NONE;
}

dim::parser::NumberExpression::NumberExpression(
	std::string value
) :
	Expression(),
	value(value)
{}

std::string dim::parser::NumberExpression::GetValue() {
	return this->value;
}

std::string dim::parser::NumberExpression::Repr() {
	return "NumberExpression(" + this->value + ")";
}

dim::parser::NodeType dim::parser::NumberExpression::Type() {
	return dim::parser::NodeType::NUMBER;
}

dim::parser::BinaryExpression::BinaryExpression(
	std::shared_ptr<dim::parser::Expression> left,
	std::string operatorSymbol,
	std::shared_ptr<dim::parser::Expression> right
) :
	Expression(),
	left(left),
	operatorSymbol(operatorSymbol),
	right(right)
{}

std::shared_ptr<dim::parser::Expression> dim::parser::BinaryExpression::GetLeft() {
	return this->left;
}
std::string dim::parser::BinaryExpression::GetOperator() {
	return this->operatorSymbol;
}
std::shared_ptr<dim::parser::Expression> dim::parser::BinaryExpression::GetRight() {
	return this->right;
}

std::string dim::parser::BinaryExpression::Repr() {
	return "BinaryExpression("
		+ this->left->Repr() + " "
		+ this->operatorSymbol + " "
		+ this->right->Repr() + ")";
}

dim::parser::NodeType dim::parser::BinaryExpression::Type() {
	return dim::parser::NodeType::BINARY;
}
