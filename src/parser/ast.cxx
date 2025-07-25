#include <parser/ast.hxx>

namespace dim {
	namespace parser {

		std::string Expression::Repr() {
			return "INVALID EXPRESSION";
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

		std::string ScopeExpression::Repr() {
			std::string repr = "{";
			for(const auto& expression : m_expressions) {
				repr += "\n\t" + expression->Repr();
			}
			return repr + "\n}";
		}
		NodeType ScopeExpression::Type() {
			return NodeType::SCOPE;
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

		std::string NumberExpression::Repr() {
			return "NumberExpression(" + m_value + ")";
		}

		NodeType NumberExpression::Type() {
			return NodeType::NUMBER;
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

		std::string BinaryExpression::Repr() {
			return "BinaryExpression("
				+ m_left->Repr() + " "
				+ m_operatorSymbol + " "
				+ m_right->Repr() + ")";
		}

		NodeType BinaryExpression::Type() {
			return NodeType::BINARY;
		}
	}
}
