#include <parser/ast.hxx>

namespace dim {
	namespace parser {

		std::string Expression::Repr(
			const size_t indent
		) {
			std::string repr = "INVALID EXPRESSION";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType Expression::Type() {
			return NodeType::NONE;
		}
		Datatype Expression::GetDatatype() {
			return Datatype::INFER;
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
			const size_t indent
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
		Datatype ScopeExpression::GetDatatype() {
			return m_expressions.back()->GetDatatype();
		}



		NullExpression::NullExpression() :
			Expression()
		{}

		std::string NullExpression::Repr(
			const size_t indent
		) {
			std::string repr = "null";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType NullExpression::Type() {
			return NodeType::NUL;
		}
		Datatype NullExpression::GetDatatype() {
			return Datatype::INFER;
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
			const size_t indent
		) {
			std::string repr = "NumberExpression(" + m_value + ")";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType NumberExpression::Type() {
			return NodeType::NUMBER;
		}
		Datatype NumberExpression::GetDatatype() {
			return Datatype::INFER;
		}


		__GEN__SUB_NUMBER_CLASS_IMPL(I8Expression, i8, I8)
		__GEN__SUB_NUMBER_CLASS_IMPL(I16Expression, i16, I16)
		__GEN__SUB_NUMBER_CLASS_IMPL(I32Expression, i32, I32)
		__GEN__SUB_NUMBER_CLASS_IMPL(I64Expression, i64, I64)

		__GEN__SUB_NUMBER_CLASS_IMPL(U8Expression, u8, U8)
		__GEN__SUB_NUMBER_CLASS_IMPL(U16Expression, u16, U16)
		__GEN__SUB_NUMBER_CLASS_IMPL(U32Expression, u32, U32)
		__GEN__SUB_NUMBER_CLASS_IMPL(U64Expression, u64, U64)

		__GEN__SUB_NUMBER_CLASS_IMPL(F32Expression, f32, F32)
		__GEN__SUB_NUMBER_CLASS_IMPL(F64Expression, f64, F64)
		
		F128Expression::F128Expression(
			f128 value
		) :
			NumberExpression(utils::f128tos(value)),
			m_value(value)
		{}
		
		std::string F128Expression::Repr(
			size_t indent
		) {
			std::string repr = std::string("F128Expression(") + utils::f128tos(m_value) + ")";
			repr.insert(0, indent, '\t');
			return repr;
		}
		
		NodeType F128Expression::Type() {
			return NodeType::F128;
		}
		Datatype F128Expression::GetDatatype() {
			return Datatype::F128;
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
			const size_t indent
		) {
			std::string repr = "BooleanExpression(" + m_value + ")";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType BooleanExpression::Type() {
			return NodeType::BOOLEAN;
		}
		Datatype BooleanExpression::GetDatatype() {
			return Datatype::BOOLEAN;
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
			const size_t indent
		) {
			std::string repr = "StringExpression(\"" + m_value + "\")";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType StringExpression::Type() {
			return NodeType::STRING;
		}
		Datatype StringExpression::GetDatatype() {
			return Datatype::INFER;
		}



		UnaryExpression::UnaryExpression(
			std::shared_ptr<Expression> term,
			std::string operatorSymbol
		) :
			m_term(term),
			m_operatorSymbol(operatorSymbol)
		{}

		std::shared_ptr<Expression> UnaryExpression::GetTerm() {
			return m_term;
		}
		std::string UnaryExpression::GetOperator() {
			return m_operatorSymbol;
		}

		std::shared_ptr<Expression> UnaryExpression::GetSampleExpression() {
			return std::make_shared<BooleanExpression>("true");
		}

		std::string UnaryExpression::Repr(
			const size_t indent
		) {
			std::string termRepr = m_term->Repr(indent);
			termRepr.erase(
				std::remove_if(
					termRepr.begin(),
					termRepr.end(),
					[](unsigned char c) {
						return c == '\t';
					}
				),
				termRepr.end()
			);
			std::string repr = "UnaryExpression(\n";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - 1, indent + 1, '\t');
			repr += termRepr + "\n)";
			repr.insert(repr.size() - 2, indent, '\t');

			return repr;
		}

		NodeType UnaryExpression::Type() {
			return NodeType::UNARY;
		}
		Datatype UnaryExpression::GetDatatype() {
			return Datatype::BOOLEAN;
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

		std::shared_ptr<Expression> BinaryExpression::GetSampleExpression() {
			if(
				m_operatorSymbol == "+"
				|| m_operatorSymbol == "-"
				|| m_operatorSymbol == "*"
				|| m_operatorSymbol == "/"
				|| m_operatorSymbol == "&"
				|| m_operatorSymbol == "|"
				|| m_operatorSymbol == "^"
			) {
				return m_left;
			}
			else /*if(
				m_operatorSymbol == "<"
				|| m_operatorSymbol == ">"
				|| m_operatorSymbol == "<="
				|| m_operatorSymbol == ">="
				|| m_operatorSymbol == "&&"
				|| m_operatorSymbol == "||"
				|| m_operatorSymbol == "=="
				|| m_operatorSymbol == "!="
			)*/ {
				return std::make_shared<BooleanExpression>("true");
			}
		}

		std::string BinaryExpression::Repr(
			const size_t indent
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
		Datatype BinaryExpression::GetDatatype() {
			if(
				m_operatorSymbol == "+"
				|| m_operatorSymbol == "-"
				|| m_operatorSymbol == "*"
				|| m_operatorSymbol == "/"
				|| m_operatorSymbol == "&"
				|| m_operatorSymbol == "|"
				|| m_operatorSymbol == "^"
			) {
				return m_left->GetDatatype();
			}
			else /*if(
				m_operatorSymbol == "<"
				|| m_operatorSymbol == ">"
				|| m_operatorSymbol == "<="
				|| m_operatorSymbol == ">="
				|| m_operatorSymbol == "&&"
				|| m_operatorSymbol == "||"
				|| m_operatorSymbol == "=="
				|| m_operatorSymbol == "!="
			)*/ {
				return Datatype::BOOLEAN;
			}		
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
			const size_t indent
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
		Datatype IfElseExpression::GetDatatype() {
			return m_scope->GetDatatype();
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
			const size_t indent
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
		Datatype IfElseStructure::GetDatatype() {
			return m_expressions.back()->GetDatatype();
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
			const size_t indent
		) {
			std::string repr = "loop " + m_scope->Repr(indent);
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType LoopExpression::Type() {
			return NodeType::LOOP;
		}
		Datatype LoopExpression::GetDatatype() {
			return m_scope->GetDatatype();
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
			const size_t indent
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
		Datatype WhileLoopExpression::GetDatatype() {
			return m_scope->GetDatatype();
		}


		ForLoopExpression::ForLoopExpression(
			std::shared_ptr<ScopeExpression> scope,
			std::shared_ptr<Expression> initialExpression,
			std::shared_ptr<Expression> condition,
			std::shared_ptr<Expression> updateExpression,
			std::shared_ptr<OrExpression> orExpression
		) :
			WhileLoopExpression(
				scope,
				condition,
				orExpression
			),
			m_initialExpression(initialExpression),
			m_updateExpression(updateExpression)
		{}

		std::shared_ptr<Expression> ForLoopExpression::GetInitialExpression() {
			return m_initialExpression;
		}

		std::shared_ptr<Expression> ForLoopExpression::GetUpdateExpression() {
			return m_updateExpression;
		}

		std::string ForLoopExpression::Repr(
			const size_t indent
		) {
			std::string repr = "loop(\n";
			repr.insert(0, indent, '\t');
			repr += m_initialExpression->Repr(indent + 1) + ";\n";
			repr += m_condition->Repr(indent + 1) + ";\n";
			repr += m_updateExpression->Repr(indent + 1);
			repr += "\n)";
			repr.insert(repr.size() - 1, indent, '\t');
			repr += m_scope->Repr(indent) + " ";
			repr += m_orExpression->Repr(indent);
			return repr;
		}
		NodeType ForLoopExpression::Type() {
			return NodeType::FOR;
		}
		Datatype ForLoopExpression::GetDatatype() {
			return m_scope->GetDatatype();
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
			const size_t indent
		) {
			std::string repr = "(\n" + m_expression->Repr(indent + 1) + "\n)";
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}
		NodeType NestedExpression::Type() {
			return NodeType::NESTED;
		}
		Datatype NestedExpression::GetDatatype() {
			return m_expression->GetDatatype();
		}



		BreakExpression::BreakExpression(
			std::shared_ptr<Expression> expression
		) :
			NestedExpression(expression)
		{}

		std::string BreakExpression::Repr(
			const size_t indent
		) {
			std::string repr = "break" + NestedExpression::Repr(indent);
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType BreakExpression::Type() {
			return NodeType::BREAK;
		}
		Datatype BreakExpression::GetDatatype() {
			return NestedExpression::GetDatatype();
		}



		OrExpression::OrExpression(
			std::shared_ptr<Expression> expression
		) :
			NestedExpression(expression)
		{}

		std::string OrExpression::Repr(
			const size_t indent
		) {
			return "or" + NestedExpression::Repr(indent);
		}
		NodeType OrExpression::Type() {
			return NodeType::OR;
		}
		Datatype OrExpression::GetDatatype() {
			return NestedExpression::GetDatatype();
		}

		IdentifierExpression::IdentifierExpression(
			std::function<
				std::expected<
					IdentifierData,
					std::string
				> (const std::string name)
			> GetIdentifierFn,
			std::string name,
			bool isConst,
			std::shared_ptr<Expression> expression,
			Datatype datatype
		) :
			NestedExpression(expression),
			m_name(name),
			m_isConst(isConst),
			m_datatype(datatype)
		{
			std::expected<
				IdentifierData,
				std::string
			> result = GetIdentifierFn(name);

			if(result) {
				m_isConst = result.value().isConst;
				m_datatype = result.value().datatype;
			}
		}

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
		void IdentifierExpression::SetDatatype(
			Datatype datatype
		) {
			m_datatype = datatype;
		}

		std::string IdentifierExpression::Repr(
			const size_t indent
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
		Datatype IdentifierExpression::GetDatatype() {
			return m_datatype;
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
			const size_t indent
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
		Datatype AssignationExpression::GetDatatype() {
			return m_identifier->GetDatatype();
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
			const size_t indent
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
		Datatype DeclarationExpression::GetDatatype() {
			return m_identifier->GetDatatype();
		}
	}
}
