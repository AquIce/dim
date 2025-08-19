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



		StringExpression::StringExpression(
			std::string value
		) :
			Expression(),
			m_value(value)
		{}

		std::string StringExpression::GetValue() {
			return m_value;
		}

		std::string StringExpression::Repr(
			size_t indent
		) {
			std::string repr = "StringExpression(\"" + m_value + "\")";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType StringExpression::Type() {
			return NodeType::STRING;
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



		IfElseExpression::IfElseExpression(
			std::shared_ptr<ScopeExpression> scope,
			std::shared_ptr<Expression> condition
		) :
			Expression(),
			m_scope(scope),
			m_condition(condition)
		{}

		std::shared_ptr<ScopeExpression> IfElseExpression::GetScope() {
			return m_scope;
		}
		std::shared_ptr<Expression> IfElseExpression::GetCondition() {
			return m_condition;
		}

		std::string IfElseExpression::Repr(
			size_t indent
		) {
			std::string repr = "";

			if(m_condition) {
				repr += "(\n" + m_condition->Repr(indent + 1) + "\n)";
				repr.insert(repr.size() - 1, indent, '\t');
			}

			repr += " " + m_scope->Repr(indent);

			return repr;
		}

		NodeType IfElseExpression::Type() {
			return NodeType::IFELSE_EXPR;
		}



		IfElseStructure::IfElseStructure(
			std::vector<std::shared_ptr<IfElseExpression>> expressions
		) :
			Expression(),
			m_expressions(expressions)
		{}

		std::vector<std::shared_ptr<IfElseExpression>> IfElseStructure::GetExpressions() {
			return m_expressions;
		}

		std::string IfElseStructure::Repr(
			size_t indent
		) {
			std::string repr = "";

			for(size_t i = 0; i < m_expressions.size(); i++) {
				if(i == 0) {
					repr += "if";
				} else if(i == m_expressions.size() - 1) {
					repr += " else";
				} else {
					repr += " elseif";
				}
				repr += m_expressions.at(i)->Repr(indent);
			}

			repr.insert(0, indent, '\t');

			return repr;
		}

		NodeType IfElseStructure::Type() {
			return NodeType::IFELSE_STRUCT;
		}



		LoopExpression::LoopExpression(
			std::shared_ptr<ScopeExpression> scope
		) :
			Expression(),
			m_scope(scope)
		{}

		std::shared_ptr<ScopeExpression> LoopExpression::GetScope() {
			return m_scope;
		}

		std::string LoopExpression::Repr(
			size_t indent
		) {
			std::string repr = "loop " + m_scope->Repr(indent);
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType LoopExpression::Type() {
			return NodeType::LOOP;
		}


		WhileLoopExpression::WhileLoopExpression(
			std::shared_ptr<ScopeExpression> scope,
			std::shared_ptr<Expression> condition,
			std::shared_ptr<OrExpression> orExpression
		) :
			LoopExpression(scope),
			m_condition(condition),
			m_orExpression(orExpression)
		{}

		std::shared_ptr<Expression> WhileLoopExpression::GetCondition() {
			return m_condition;
		}

		std::shared_ptr<Expression> WhileLoopExpression::GetOrExpression() {
			return m_orExpression;
		}

		std::string WhileLoopExpression::Repr(
			size_t indent
		) {
			std::string repr = "loop(\n";
			repr.insert(0, indent, '\t');
			repr += m_condition->Repr(indent + 1);
			repr += "\n)";
			repr.insert(repr.size() - 1, indent, '\t');
			repr += m_scope->Repr(indent) + " ";
			repr += m_orExpression->Repr(indent);
			return repr;
		}
		NodeType WhileLoopExpression::Type() {
			return NodeType::WHILE;
		}


		NestedExpression::NestedExpression(
			std::shared_ptr<Expression> expression
		) :
			Expression(),
			m_expression(expression)
		{}

		std::shared_ptr<Expression> NestedExpression::GetExpression() {
			return m_expression;
		}

		std::string NestedExpression::Repr(
			size_t indent
		) {
			std::string repr = "(\n" + m_expression->Repr(indent + 1) + "\n)";
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}
		NodeType NestedExpression::Type() {
			return NodeType::NESTED;
		}



		BreakExpression::BreakExpression(
			std::shared_ptr<Expression> expression
		) :
			NestedExpression(expression)
		{}

		std::string BreakExpression::Repr(
			size_t indent
		) {
			std::string repr = "break" + NestedExpression::Repr(indent);
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType BreakExpression::Type() {
			return NodeType::BREAK;
		}



		OrExpression::OrExpression(
			std::shared_ptr<Expression> expression
		) :
			NestedExpression(expression)
		{}

		std::string OrExpression::Repr(
			size_t indent
		) {
			return "or" + NestedExpression::Repr(indent);
		}
		NodeType OrExpression::Type() {
			return NodeType::OR;
		}

		IdentifierExpression::IdentifierExpression(
			std::string name,
			bool isConst,
			std::shared_ptr<Expression> expression,
			Datatype datatype
		) :
			NestedExpression(expression),
			m_name(name),
			m_isConst(isConst),
			m_datatype(datatype)
		{}

		std::string IdentifierExpression::GetName() {
			return m_name;
		}
		bool IdentifierExpression::GetIsConst() {
			return m_isConst;
		}
		void IdentifierExpression::SetIsConst(
			bool isConst
		) {
			m_isConst = isConst;
		}
		void IdentifierExpression::SetExpression(
			std::shared_ptr<Expression> expression
		) {
			m_expression = expression;
		}
		Datatype IdentifierExpression::GetDatatype() {
			return m_datatype;
		}
		void IdentifierExpression::SetDatatype(
			Datatype datatype
		) {
			m_datatype = datatype;
		}

		std::string IdentifierExpression::Repr(
			size_t indent
		) {
			std::string repr =
				std::string("(") + m_name + ": "
				+ std::string(DatatypeToStr.at(int(m_datatype)))
				+ ")";
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType IdentifierExpression::Type() {
			return NodeType::IDENTIFIER;
		}



		AssignationExpression::AssignationExpression(
			std::shared_ptr<IdentifierExpression> identifier,
			std::shared_ptr<Expression> expression
		) :
			m_identifier(identifier)
		{
			m_identifier->SetExpression(expression);
		}

		std::shared_ptr<IdentifierExpression> AssignationExpression::GetIdentifier() {
			return m_identifier;
		}

		std::string AssignationExpression::Repr(
			size_t indent
		) {			
			std::string repr =
				m_identifier->Repr(indent) + " = (\n"
				+ m_identifier->GetExpression()->Repr(indent + 1)
				+ "\n)";
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}
		NodeType AssignationExpression::Type() {
			return NodeType::ASSIGN;
		}

		DeclarationExpression::DeclarationExpression(
			std::shared_ptr<IdentifierExpression> identifier,
			std::shared_ptr<Expression> expression,
			Datatype datatype,
			bool isConst
		) :
			m_identifier(identifier)
		{
			m_identifier->SetExpression(expression);
			m_identifier->SetDatatype(datatype);
			m_identifier->SetIsConst(isConst);
		}

		std::shared_ptr<IdentifierExpression> DeclarationExpression::GetIdentifier() {
			return m_identifier;
		}

		std::string DeclarationExpression::Repr(
			size_t indent
		) {
			std::string identifierRepr = m_identifier->Repr(indent);
			identifierRepr.erase(
				std::remove_if(
					identifierRepr.begin(),
					identifierRepr.end(),
					[](unsigned char c) {
						return c == '\t';
					}
				),
				identifierRepr.end()
			);
			std::string repr =
				std::string(m_identifier->GetIsConst() ? "const " : "var ")
				+ identifierRepr + " := (\n"
				+ m_identifier->GetExpression()->Repr(indent + 1)
				+ "\n)";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}
		NodeType DeclarationExpression::Type() {
			return NodeType::DECL;
		}
	}
}
