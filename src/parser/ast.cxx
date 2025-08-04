#include <parser/ast.hxx>

namespace dim {
	namespace parser {

		std::string Expression::Repr(
			size_t indent
		) {
			std::string repr = "INVALID EXPRESSION";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType Expression::Type() {
			return NodeType::NONE;
		}

		ScopeExpression::ScopeExpression(
			std::vector<std::shared_ptr<Expression>> expressions
		) :
			Expression(),
			m_expressions(expressions)
		{}

		std::vector<std::shared_ptr<Expression>> ScopeExpression::GetExpressions() {
			return m_expressions;
		}
		void ScopeExpression::AddExpression(
			std::shared_ptr<Expression> expression
		) {
			m_expressions.push_back(expression);
		}

		std::string ScopeExpression::Repr(
			size_t indent
		) {
			std::string repr = "{\n";
			repr.insert(0, indent, '\t');
			for(const auto& expression : m_expressions) {
				repr += expression->Repr(indent + 1) + "\n";
			}
			repr += "}";
			repr.insert(repr.size() - 1, indent, '\t');

			return repr;
		}
		NodeType ScopeExpression::Type() {
			return NodeType::SCOPE;
		}

		NullExpression::NullExpression() :
			Expression()
		{}

		std::string NullExpression::Repr(
			size_t indent
		) {
			std::string repr = "null";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType NullExpression::Type() {
			return NodeType::NUL;
		}

		NumberExpression::NumberExpression(
			std::string value
		) :
			Expression(),
			m_value(value)
		{}

		std::string NumberExpression::GetValue() {
			return m_value;
		}

		std::string NumberExpression::Repr(
			size_t indent
		) {
			std::string repr = "NumberExpression(" + m_value + ")";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType NumberExpression::Type() {
			return NodeType::NUMBER;
		}

		BooleanExpression::BooleanExpression(
			std::string value
		) :
			Expression(),
			m_value(value)
		{}

		std::string BooleanExpression::GetValue() {
			return m_value;
		}

		std::string BooleanExpression::Repr(
			size_t indent
		) {
			std::string repr = "BooleanExpression(" + m_value + ")";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType BooleanExpression::Type() {
			return NodeType::BOOLEAN;
		}

		BinaryExpression::BinaryExpression(
			std::shared_ptr<Expression> left,
			std::string operatorSymbol,
			std::shared_ptr<Expression> right
		) :
			Expression(),
			m_left(left),
			m_operatorSymbol(operatorSymbol),
			m_right(right)
		{}

		std::shared_ptr<Expression> BinaryExpression::GetLeft() {
			return m_left;
		}
		std::string BinaryExpression::GetOperator() {
			return m_operatorSymbol;
		}
		std::shared_ptr<Expression> BinaryExpression::GetRight() {
			return m_right;
		}

		std::string BinaryExpression::Repr(
			size_t indent
		) {
			std::string repr = "BinaryExpression(\n"
				+ m_left->Repr(indent + 1) + "\n"
				+ m_operatorSymbol;

			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - m_operatorSymbol.size(), indent + 1, '\t');

			repr += "\n" + m_right->Repr(indent + 1) + "\n)";
			
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}

		NodeType BinaryExpression::Type() {
			return NodeType::BINARY;
		}
	}
}
