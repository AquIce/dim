#include <parser/ast.hxx>

namespace dim {
	namespace parser {

		ScopeIdentifierRegister::ScopeIdentifierRegister(
			std::shared_ptr<ScopeIdentifierRegister> parent
		) :
			m_parent(parent),
			m_identifiers()
		{}

		std::expected<
			IdentifierData,
			std::string
		> ScopeIdentifierRegister::Get(
			const std::string name
		) {
			std::vector<IdentifierData>::iterator identifier = std::find_if(
				m_identifiers.begin(), m_identifiers.end(),
				[&name](const IdentifierData& ident) {
					return name == ident.name;
				}
			);
			if(identifier == m_identifiers.end()) {
				if(m_parent) {
					return m_parent->Get(name);
				}
				return std::unexpected("Trying to get non existing identifier.");
			}
			return *identifier;
		}
		void ScopeIdentifierRegister::Register(
			IdentifierData identifier
		) {
			m_identifiers.push_back(identifier);
		}

		Expression::Expression(
			struct utils::Context ctx
		) :
			ctx(ctx)
		{}

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
		DatatypeStr Expression::GetDatatype() {
			return "INFER";
		}



		AssignableExpression::AssignableExpression(
			struct utils::Context ctx
		) :
			Expression(ctx)
		{}

		std::string AssignableExpression::Repr(
			size_t indent
		) {
			std::string repr = "ASSIGNABLE EXPRESSION";
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType AssignableExpression::Type() {
			return NodeType::ASSIGNABLE;
		}
		DatatypeStr AssignableExpression::GetDatatype() {
			return "INFER";
		}
		std::expected<
			Success,
			std::string
		> AssignableExpression::TryAssign(
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			std::shared_ptr<Expression> expression
		) {
			return std::unexpected("Using AssignableExpression on its own.");
		}



		IdentifierExpression::IdentifierExpression(
			struct utils::Context ctx,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			std::string name,
			bool isConst,
			std::shared_ptr<Expression> expression,
			DatatypeStr datatype
		) :
			AssignableExpression(ctx),
			m_expression(expression),
			m_name(name),
			m_isConst(isConst),
			m_datatype(datatype)
		{
			std::expected<
				IdentifierData,
				std::string
			> result = identifierRegister->Get(name);

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
		std::shared_ptr<Expression> IdentifierExpression::GetExpression() {
			return m_expression;
		}
		void IdentifierExpression::SetExpression(
			std::shared_ptr<Expression> expression
		) {
			m_expression = expression;
		}
		void IdentifierExpression::SetDatatype(
			DatatypeStr datatype
		) {
			m_datatype = datatype;
		}

		std::string IdentifierExpression::Repr(
			const size_t indent
		) {
			std::string repr =
				std::string("(") + m_name + ": "
				+ m_datatype + ")";
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType IdentifierExpression::Type() {
			return NodeType::IDENTIFIER;
		}
		DatatypeStr IdentifierExpression::GetDatatype() {
			return m_datatype;
		}
		std::expected<
			Success,
			std::string
		> IdentifierExpression::TryAssign(
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			std::shared_ptr<Expression> expression
		) {
			std::shared_ptr<IdentifierExpression> identifier;
			{
				std::expected<
					IdentifierData,
					std::string
				> result = identifierRegister->Get(m_name);
				if(!result) {
					return std::unexpected(result.error());
				}
				identifier = std::make_shared<IdentifierExpression>(
					ctx,
					identifierRegister,
					result.value().name,
					result.value().isConst,
					nullptr,
					result.value().datatype
				);
			}

			if(identifier->GetIsConst()) {
				return std::unexpected("Trying to assign to constant expression '" + m_name + "'");
			}

			DatatypeStr expectedDatatype = identifier->GetDatatype();
			DatatypeStr gotDatatype = expression->GetDatatype();
			if(expectedDatatype != gotDatatype) {
				std::expected<
					std::shared_ptr<Expression>,
					std::string
				> castResult = try_cast(expression, expectedDatatype);

				if(!castResult) {
					return std::unexpected(
						std::string("Expected type ")
						+ expectedDatatype + ", got " + gotDatatype
					);
				}
				expression = castResult.value();
			}

			identifier->SetExpression(expression);

			return Success{};
		}



		ScopeExpression::ScopeExpression(
			struct utils::Context ctx,
			std::vector<std::shared_ptr<Expression>> expressions,
			std::shared_ptr<IdentifierExpression> name
		) :
			Expression(ctx),
			m_expressions(expressions),
			m_name(name)
		{}

		std::vector<std::shared_ptr<Expression>> ScopeExpression::GetExpressions() {
			return m_expressions;
		}
		void ScopeExpression::AddExpression(
			std::shared_ptr<Expression> expression
		) {
			m_expressions.push_back(expression);
		}

		std::shared_ptr<IdentifierExpression> ScopeExpression::GetName() {
			return m_name;
		}
		void ScopeExpression::SetName(
			std::shared_ptr<IdentifierExpression> name
		) {
			m_name = name;
		}

		std::string ScopeExpression::Repr(
			const size_t indent
		) {
			std::string repr = m_name ? m_name->Repr() + " " : "";
			repr += "{\n";
			for(const auto& expression : m_expressions) {
				repr += expression->Repr(indent + 1) + "\n";
			}
			repr += "}";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - 1, indent, '\t');

			return repr;
		}
		NodeType ScopeExpression::Type() {
			return NodeType::SCOPE;
		}
		DatatypeStr ScopeExpression::GetDatatype() {
			return m_expressions.back()->GetDatatype();
		}



		NullExpression::NullExpression(
			struct utils::Context ctx
		) :
			Expression(ctx)
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
		DatatypeStr NullExpression::GetDatatype() {
			return "INFER";
		}



		NumberExpression::NumberExpression(
			struct utils::Context ctx,
			std::string value
		) :
			Expression(ctx),
			m_value(value)
		{}

		std::string NumberExpression::GetValue() {
			return m_value;
		}

		void NumberExpression::SetValue(
			std::string value
		) {
			m_value = value;
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
		DatatypeStr NumberExpression::GetDatatype() {
			return "INFER";
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
			struct utils::Context ctx,
			f128 value
		) :
			NumberExpression(ctx, utils::f128tos(value)),
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
		DatatypeStr F128Expression::GetDatatype() {
			return "F128";
		}



		BooleanExpression::BooleanExpression(
			struct utils::Context ctx,
			std::string value
		) :
			Expression(ctx),
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
		DatatypeStr BooleanExpression::GetDatatype() {
			return "BOOLEAN";
		}



		CharExpression::CharExpression(
			struct utils::Context ctx,
			std::string value
		) :
			Expression(ctx),
			m_value(value)
		{}

		std::string CharExpression::GetValue() {
			return m_value;
		}

		std::string CharExpression::Repr(
			const size_t indent
		) {
			std::string repr = "CharExpression('" + m_value + "')";
			repr.insert(0, indent, '\t');
			return repr;
		}

		NodeType CharExpression::Type() {
			return NodeType::CHAR;
		}
		DatatypeStr CharExpression::GetDatatype() {
			return "CHAR";
		}



		StringExpression::StringExpression(
			struct utils::Context ctx,
			std::string value
		) :
			Expression(ctx),
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
		DatatypeStr StringExpression::GetDatatype() {
			return "STRING";
		}



		UnaryExpression::UnaryExpression(
			struct utils::Context ctx,
			std::shared_ptr<Expression> term,
			std::string operatorSymbol
		) :
			Expression(ctx),
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
			return std::make_shared<BooleanExpression>(ctx, "true");
		}

		std::string UnaryExpression::Repr(
			const size_t indent
		) {
			std::string repr = "UnaryExpression(\n"
				+ m_operatorSymbol + "\n";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - m_operatorSymbol.size() - 1, indent + 1, '\t');
			repr += m_term->Repr(indent + 1) + "\n)";
			repr.insert(repr.size() - 1, indent, '\t');

			return repr;
		}

		NodeType UnaryExpression::Type() {
			return NodeType::UNARY;
		}
		DatatypeStr UnaryExpression::GetDatatype() {
			return GetUnaryOutputDatatype(
				m_operatorSymbol,
				m_term->GetDatatype()
			).value();
		}



		BinaryExpression::BinaryExpression(
			struct utils::Context ctx,
			std::shared_ptr<Expression> left,
			std::string operatorSymbol,
			std::shared_ptr<Expression> right
		) :
			Expression(ctx),
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
			/*
				m_operatorSymbol == "<"
				|| m_operatorSymbol == ">"
				|| m_operatorSymbol == "<="
				|| m_operatorSymbol == ">="
				|| m_operatorSymbol == "&&"
				|| m_operatorSymbol == "||"
				|| m_operatorSymbol == "=="
				|| m_operatorSymbol == "!="
			*/
			return std::make_shared<BooleanExpression>(ctx, "true");
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
		DatatypeStr BinaryExpression::GetDatatype() {
			return GetBinaryOutputDatatype(
				m_left->GetDatatype(),
				m_operatorSymbol,
				m_right->GetDatatype()
			).value();
		}



		IfElseExpression::IfElseExpression(
			struct utils::Context ctx,
			std::shared_ptr<ScopeExpression> scope,
			std::shared_ptr<Expression> condition
		) :
			Expression(ctx),
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
		DatatypeStr IfElseExpression::GetDatatype() {
			return m_scope->GetDatatype();
		}



		IfElseStructure::IfElseStructure(
			struct utils::Context ctx,
			std::vector<std::shared_ptr<IfElseExpression>> expressions
		) :
			Expression(ctx),
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
		DatatypeStr IfElseStructure::GetDatatype() {
			return m_expressions.back()->GetDatatype();
		}



		MatchExpression::MatchExpression(
			struct utils::Context ctx,
			std::shared_ptr<ScopeExpression> scope,
			std::shared_ptr<Expression> condition
		) :
			Expression(ctx),
			m_scope(scope),
			m_condition(condition)
		{}

		std::shared_ptr<ScopeExpression> MatchExpression::GetScope() {
			return m_scope;
		}
		std::shared_ptr<Expression> MatchExpression::GetCondition() {
			return m_condition;
		}

		std::string MatchExpression::Repr(
			const size_t indent
		) {
			std::string repr = "";

			if(m_condition) {
				repr += "(\n" + m_condition->Repr(indent + 1) + "\n) -> ";
				repr.insert(repr.size() - 5, indent, '\t');
			} else {
				repr += "_ -> ";
			}
			repr.insert(0, indent, '\t');
			repr += m_scope->Repr(indent);

			return repr;
		}

		NodeType MatchExpression::Type() {
			return NodeType::MATCH_EXPR;
		}
		DatatypeStr MatchExpression::GetDatatype() {
			return m_scope->GetDatatype();
		}



		MatchStructure::MatchStructure(
			struct utils::Context ctx,
			std::shared_ptr<Expression> expression,
			std::vector<std::shared_ptr<MatchExpression>> expressions
		) :
			NestedExpression(ctx, expression),
			m_expressions(expressions)
		{}

		std::vector<std::shared_ptr<MatchExpression>> MatchStructure::GetExpressions() {
			return m_expressions;
		}

		std::string MatchStructure::Repr(
			const size_t indent
		) {
			std::string repr = "match(\n";
			repr.insert(0, indent, '\t');
			repr += m_expression->Repr(indent + 1);
			repr += "\n) {\n";
			repr.insert(repr.size() - 4, indent, '\t');

			std::for_each(
				m_expressions.begin(),
				m_expressions.end(),
				[&repr, &indent](const std::shared_ptr<Expression>& expression) -> void {
					repr += expression->Repr(indent + 1) + "\n";
				}
			);

			return repr;
		}

		NodeType MatchStructure::Type() {
			return NodeType::MATCH_STRUCT;
		}
		DatatypeStr MatchStructure::GetDatatype() {
			return m_expressions.back()->GetDatatype();
		}



		LoopExpression::LoopExpression(
			struct utils::Context ctx,
			std::shared_ptr<ScopeExpression> scope
		) :
			Expression(ctx),
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
		DatatypeStr LoopExpression::GetDatatype() {
			return m_scope->GetDatatype();
		}


		WhileLoopExpression::WhileLoopExpression(
			struct utils::Context ctx,
			std::shared_ptr<ScopeExpression> scope,
			std::shared_ptr<Expression> condition,
			std::shared_ptr<OrExpression> orExpression
		) :
			LoopExpression(ctx, scope),
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
			repr += "\n) ";
			repr.insert(repr.size() - 1, indent, '\t');
			repr += m_scope->Repr(indent) + " ";
			repr += m_orExpression->Repr(indent);
			return repr;
		}
		NodeType WhileLoopExpression::Type() {
			return NodeType::WHILE;
		}
		DatatypeStr WhileLoopExpression::GetDatatype() {
			return m_scope->GetDatatype();
		}


		ForLoopExpression::ForLoopExpression(
			struct utils::Context ctx,
			std::shared_ptr<ScopeExpression> scope,
			std::shared_ptr<Expression> initialExpression,
			std::shared_ptr<Expression> condition,
			std::shared_ptr<Expression> updateExpression,
			std::shared_ptr<OrExpression> orExpression
		) :
			WhileLoopExpression(
				ctx,
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
		DatatypeStr ForLoopExpression::GetDatatype() {
			return m_scope->GetDatatype();
		}


		NestedExpression::NestedExpression(
			struct utils::Context ctx,
			std::shared_ptr<Expression> expression
		) :
			Expression(ctx),
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
		DatatypeStr NestedExpression::GetDatatype() {
			return m_expression->GetDatatype();
		}



		BreakExpression::BreakExpression(
			struct utils::Context ctx,
			std::shared_ptr<Expression> expression,
			std::shared_ptr<IdentifierExpression> name
		) :
			NestedExpression(ctx, expression),
			m_name(name)
		{}

		std::shared_ptr<IdentifierExpression> BreakExpression::GetScopeName() {
			return m_name;
		}
		void BreakExpression::SetScopeName(
			std::shared_ptr<IdentifierExpression> name
		) {
			m_name = name;
		}

		std::string BreakExpression::Repr(
			const size_t indent
		) {
			std::string repr = "break ";
			if(m_name) {
				repr += m_name->Repr() + " ";
			}
			repr += NestedExpression::Repr(indent);
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType BreakExpression::Type() {
			return NodeType::BREAK;
		}
		DatatypeStr BreakExpression::GetDatatype() {
			return NestedExpression::GetDatatype();
		}



		ReturnExpression::ReturnExpression(
			struct utils::Context ctx,
			std::shared_ptr<Expression> expression
		) :
			NestedExpression(ctx, expression)
		{}

		std::string ReturnExpression::Repr(
			const size_t indent
		) {
			std::string repr = "return";
			if(m_expression) {
				repr += std::string(" ") + NestedExpression::Repr(indent);
			}
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType ReturnExpression::Type() {
			return NodeType::RETURN;
		}
		DatatypeStr ReturnExpression::GetDatatype() {
			if(m_expression == nullptr) {
				return "VOID";
			}
			return NestedExpression::GetDatatype();
		}



		OrExpression::OrExpression(
			struct utils::Context ctx,
			std::shared_ptr<Expression> expression
		) :
			NestedExpression(ctx, expression)
		{}

		std::string OrExpression::Repr(
			const size_t indent
		) {
			return "or" + NestedExpression::Repr(indent);
		}
		NodeType OrExpression::Type() {
			return NodeType::OR;
		}
		DatatypeStr OrExpression::GetDatatype() {
			return NestedExpression::GetDatatype();
		}



		DiscardExpression::DiscardExpression(
			struct utils::Context ctx
		) :
			AssignableExpression(ctx)
		{}

		std::string DiscardExpression::Repr(
			const size_t indent
		) {
			std::string repr = "_";
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType DiscardExpression::Type() {
			return NodeType::DISCARD;
		}
		DatatypeStr DiscardExpression::GetDatatype() {
			return "INFER";
		}
		std::expected<
			Success,
			std::string
		> DiscardExpression::TryAssign(
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			std::shared_ptr<Expression> expression
		) {
			// TODO: Add discard logic to parser
			return Success{};
		}



		AssignationExpression::AssignationExpression(
			struct utils::Context ctx,
			std::shared_ptr<AssignableExpression> destination,
			std::shared_ptr<Expression> expression
		) :
		NestedExpression(ctx, expression),
			m_destination(destination)
		{}

		std::shared_ptr<AssignableExpression> AssignationExpression::GetDestination() {
			return m_destination;
		}

		std::string AssignationExpression::Repr(
			const size_t indent
		) {
			std::string repr =
				m_destination->Repr(indent) + " = (\n"
				+ m_expression->Repr(indent + 1)
				+ "\n)";
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}
		NodeType AssignationExpression::Type() {
			return NodeType::ASSIGN;
		}
		DatatypeStr AssignationExpression::GetDatatype() {
			return m_destination->GetDatatype();
		}

		

		DeclarationExpression::DeclarationExpression(
			struct utils::Context ctx,
			std::shared_ptr<IdentifierExpression> identifier,
			std::shared_ptr<Expression> expression,
			DatatypeStr datatype,
			bool isConst
		) :
			Expression(ctx),
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
			identifierRepr.erase(0, identifierRepr.find_first_not_of('\t'));
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
		DatatypeStr DeclarationExpression::GetDatatype() {
			return m_identifier->GetDatatype();
		}



		FunctionPrototypeExpression::FunctionPrototypeExpression(
			struct utils::Context ctx,
			std::shared_ptr<IdentifierExpression> identifier,
			std::vector<std::shared_ptr<DeclarationExpression>> arguments,
			DatatypeStr returnDatatype
		) :
			Expression(ctx),
			m_identifier(identifier),
			m_arguments(arguments),
			m_returnDatatype(returnDatatype)
		{}

		std::shared_ptr<IdentifierExpression> FunctionPrototypeExpression::GetIdentifier() {
			return m_identifier;
		}
		std::vector<std::shared_ptr<DeclarationExpression>> FunctionPrototypeExpression::GetArguments() {
			return m_arguments;
		}

		std::string FunctionPrototypeExpression::Repr(
			const size_t indent
		) {

			std::string repr = "fn ";
			repr.insert(0, indent, '\t');
			repr += m_identifier->GetName() + "(\n";
			for(size_t i = 0; i < m_arguments.size(); i++) {
				repr += m_arguments.at(i)->GetIdentifier()->Repr(indent + 1) + (i < m_arguments.size() - 1 ? "," : "") + "\n";
			}
			repr.insert(repr.size(), indent, '\t');
			repr += ") -> " + m_returnDatatype;

			return repr;
		}
		NodeType FunctionPrototypeExpression::Type() {
			return NodeType::FN_PROTO;
		}
		DatatypeStr FunctionPrototypeExpression::GetDatatype() {
			return m_returnDatatype;
		}



		FunctionDeclarationExpression::FunctionDeclarationExpression(
			struct utils::Context ctx,
			std::shared_ptr<IdentifierExpression> identifier,
			std::vector<std::shared_ptr<DeclarationExpression>> arguments,
			std::shared_ptr<ScopeExpression> scope,
			DatatypeStr returnDatatype
		) :
			FunctionPrototypeExpression(
				ctx,
				identifier,
				arguments,
				returnDatatype
			),
			m_scope(scope)
		{}

		std::shared_ptr<ScopeExpression> FunctionDeclarationExpression::GetScope() {
			return m_scope;
		}
		void FunctionDeclarationExpression::SetScope(
			std::shared_ptr<ScopeExpression> scope
		) {
			m_scope = scope;
		}

		std::string FunctionDeclarationExpression::Repr(
			const size_t indent
		) {
			std::string scopeRepr = m_scope->Repr(indent);
			scopeRepr.erase(0, scopeRepr.find_first_not_of('\t'));

			std::string repr = "fn ";
			repr.insert(0, indent, '\t');
			repr += m_identifier->GetName() + "(\n";
			for(size_t i = 0; i < m_arguments.size(); i++) {
				repr += m_arguments.at(i)->GetIdentifier()->Repr(indent + 1) + (i < m_arguments.size() - 1 ? "," : "") + "\n";
			}
			repr.insert(repr.size(), indent, '\t');
			repr += ") -> " + m_returnDatatype + " " + scopeRepr;

			return repr;
		}
		NodeType FunctionDeclarationExpression::Type() {
			return NodeType::FN;
		}
		DatatypeStr FunctionDeclarationExpression::GetDatatype() {
			return m_returnDatatype;
		}



		FunctionCallExpression::FunctionCallExpression(
			struct utils::Context ctx,
			std::shared_ptr<IdentifierExpression> identifier,
			std::vector<std::shared_ptr<Expression>> arguments,
			DatatypeStr returnDatatype
		) :
			Expression(ctx),
			m_identifier(identifier),
			m_arguments(arguments),
			m_returnDatatype(returnDatatype)
		{}

		std::shared_ptr<IdentifierExpression> FunctionCallExpression::GetIdentifier() {
			return m_identifier;
		}
		std::vector<std::shared_ptr<Expression>> FunctionCallExpression::GetArguments() {
			return m_arguments;
		}

		std::string FunctionCallExpression::Repr(
			size_t indent
		) {
			std::string repr = m_identifier->GetName();
			repr += "(\n";
			for(size_t i = 0; i < m_arguments.size(); i++) {
				repr += m_arguments.at(i)->Repr(indent + 1) + (i < m_arguments.size() - 1 ? "," : "") + "\n";
			}
			repr += "\n)";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}
		NodeType FunctionCallExpression::Type() {
			return NodeType::FN_CALL;
		}
		DatatypeStr FunctionCallExpression::GetDatatype() {
			return m_returnDatatype;
		}


		
		InterfaceDeclarationExpression::InterfaceDeclarationExpression(
			struct utils::Context ctx,
			std::vector<std::shared_ptr<IdentifierExpression>> members,
			std::unordered_set<std::shared_ptr<FunctionPrototypeExpression>> memberFunctionsPrototypes,
			std::shared_ptr<IdentifierExpression> name
		) :
			Expression(ctx),
			m_members(members),
			m_memberFunctionsPrototypes(memberFunctionsPrototypes),
			m_name(name)
		{}

		std::vector<std::shared_ptr<IdentifierExpression>> InterfaceDeclarationExpression::GetMembers() {
			return m_members;
		}
		std::unordered_set<std::shared_ptr<FunctionPrototypeExpression>> InterfaceDeclarationExpression::GetMemberFunctionsPrototypes() {
			return m_memberFunctionsPrototypes;
		}
		std::shared_ptr<IdentifierExpression> InterfaceDeclarationExpression::GetName() {
			return m_name;
		}
		std::expected<
			std::shared_ptr<FunctionPrototypeExpression>,
			std::string
		> InterfaceDeclarationExpression::GetMemberFunctionPrototype(
			const std::string& name
		) {
			std::unordered_set<std::shared_ptr<FunctionPrototypeExpression>>::iterator itResult = std::find_if(
				m_memberFunctionsPrototypes.begin(),
				m_memberFunctionsPrototypes.end(),
				[&name](const std::shared_ptr<FunctionPrototypeExpression>& element) {
					return element->GetIdentifier()->GetName() == name;
				}
			);
			if(itResult == m_memberFunctionsPrototypes.end()) {
				return std::unexpected(
					"Invalid member function prototype '" + name + "' requested on interface '" + m_name->GetName() + "'"
				);
			}
			return *itResult;
		}

		std::string InterfaceDeclarationExpression::Repr(
			size_t indent
		) {
			std::string repr = "interface {\n";
			std::for_each(
				m_memberFunctionsPrototypes.begin(),
				m_memberFunctionsPrototypes.end(),
				[&repr, &indent](const std::shared_ptr<FunctionPrototypeExpression>& expression) -> void {
					repr += expression->Repr(indent + 1) + "\n";
				}
			);
			for(const auto& member : m_members) {
				repr.insert(repr.size(), indent + 1, '\t');
				repr += member->GetName() + ": "
					+ member->GetDatatype() + "\n";
			}
			repr += "}";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}

		NodeType InterfaceDeclarationExpression::Type() {
			return NodeType::INTERFACE;
		}
		DatatypeStr InterfaceDeclarationExpression::GetDatatype() {
			return m_name->GetName();
		}



		StructDeclarationExpression::StructDeclarationExpression(
			struct utils::Context ctx,
			std::vector<std::shared_ptr<IdentifierExpression>> members,
			std::shared_ptr<IdentifierExpression> name
		) :
			Expression(ctx),
			m_members(members),
			m_name(name)
		{}

		std::vector<std::shared_ptr<IdentifierExpression>> StructDeclarationExpression::GetMembers() {
			return m_members;
		}
		std::shared_ptr<IdentifierExpression> StructDeclarationExpression::GetName() {
			return m_name;
		}

		std::string StructDeclarationExpression::Repr(
			size_t indent
		) {
			std::string repr = "struct {\n";
			for(const auto& member : m_members) {
				repr.insert(repr.size(), indent + 1, '\t');
				repr += member->GetName() + ": "
			  		+ member->GetDatatype() + "\n";
			}
			repr += "} ";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - 2, indent, '\t');
			repr += m_name->GetName() + ";";
			return repr;
		}
		NodeType StructDeclarationExpression::Type() {
			return NodeType::STRUCT_DECL;
		}
		DatatypeStr StructDeclarationExpression::GetDatatype() {
			return m_name->GetName();
		}



		StructExpression::StructExpression(
			struct utils::Context ctx,
			std::vector<std::shared_ptr<IdentifierExpression>> members,
			std::shared_ptr<IdentifierExpression> name
		) :
			Expression(ctx),
			m_members(members),
			m_name(name)
		{}

		std::vector<std::shared_ptr<IdentifierExpression>> StructExpression::GetMembers() {
			return m_members;
		}

		std::shared_ptr<IdentifierExpression> StructExpression::GetName() {
			return m_name;
		}

		std::string StructExpression::Repr(
		  size_t indent
		) {
			std::string repr = m_name->GetName() + " {\n";
			for(const auto& member : m_members) {
				repr.insert(repr.size(), indent + 1, '\t');
				repr += member->GetName() + ": "
			  		+ member->GetDatatype() + "\n";
			}
			repr += "}";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}
		NodeType StructExpression::Type() {
		  	return NodeType::STRUCT;
		}
		DatatypeStr StructExpression::GetDatatype() {
		  	return m_name->GetName();
		}

		
		
		StructMemberAccessExpression::StructMemberAccessExpression(
			struct utils::Context ctx,
			std::shared_ptr<IdentifierExpression> structIdentfier,
			std::shared_ptr<IdentifierExpression> memberIdentifier,
			DatatypeStr datatype
		) :
			AssignableExpression(ctx),
			m_structIdentifier(structIdentfier),
			m_memberIdentifier(memberIdentifier)
		{
			m_memberIdentifier->SetDatatype(datatype);
		}

		std::shared_ptr<IdentifierExpression> StructMemberAccessExpression::GetStruct() {
			return m_structIdentifier;
		}
		std::shared_ptr<IdentifierExpression> StructMemberAccessExpression::GetMember() {
			return m_memberIdentifier;
		}

		std::string StructMemberAccessExpression::Repr(
			size_t indent
		) {
			std::string repr = m_structIdentifier->GetName() + "." + m_memberIdentifier->GetName();
			repr.insert(0, indent, '\t');
			return repr;
		}
		NodeType StructMemberAccessExpression::Type() {
			return NodeType::STRUCT_ACCESS;
		}
		DatatypeStr StructMemberAccessExpression::GetDatatype() {
			return m_memberIdentifier->GetDatatype();
		}
		std::expected<
			Success,
			std::string
		> StructMemberAccessExpression::TryAssign(
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			std::shared_ptr<Expression> expression
		) {
			std::shared_ptr<IdentifierExpression> structIdentifier;
			{
				std::expected<
					IdentifierData,
					std::string
				> result = identifierRegister->Get(m_structIdentifier->GetName());
				if(!result) {
					return std::unexpected(result.error());
				}
				structIdentifier = std::make_shared<IdentifierExpression>(
					ctx,
					identifierRegister,
					result.value().name,
					result.value().isConst,
					nullptr,
					result.value().datatype
				);
			}

      		if(structIdentifier->GetIsConst()) {
				return std::unexpected("Trying to assign to constant struct expression '" + m_structIdentifier->GetName() + "'");
			}

			std::shared_ptr<CustomDatatypeClass> structClass;
			{
				std::expected<
					std::shared_ptr<DatatypeClass>,
					std::string
				> result = GetDatatypeClass(structIdentifier->GetDatatype());
				if(!result) {
					return std::unexpected(result.error());
				}
				structClass = std::dynamic_pointer_cast<CustomDatatypeClass>(
					result.value()
				);
			}

      		DatatypeStr expectedDatatype = structClass->GetMember(
				m_memberIdentifier->GetName()
			).value().type->GetName();

			DatatypeStr gotDatatype = expression->GetDatatype();
			if(expectedDatatype != gotDatatype) {
				std::expected<
					std::shared_ptr<Expression>,
					std::string
				> castResult = try_cast(expression, expectedDatatype);

				if(!castResult) {
					return std::unexpected(
						std::string("Expected type ")
						+ expectedDatatype + ", got " + gotDatatype
					);
				}
				expression = castResult.value();
			}

			m_memberIdentifier->SetExpression(expression);

			return Success{};
		}



		StructImplementationExpression::StructImplementationExpression(
			struct utils::Context ctx,
			std::shared_ptr<IdentifierExpression> structIdentifier,
			std::unordered_set<std::shared_ptr<FunctionDeclarationExpression>> memberFunctions
		) :
			Expression(ctx),
			m_structIdentifier(structIdentifier),
			m_memberFunctions(memberFunctions)
		{}

		std::shared_ptr<IdentifierExpression> StructImplementationExpression::GetStruct() {
			return m_structIdentifier;
		}
		std::unordered_set<std::shared_ptr<FunctionDeclarationExpression>> StructImplementationExpression::GetMemberFunctions() {
			return m_memberFunctions;
		}
		std::expected<
			std::shared_ptr<FunctionDeclarationExpression>,
			std::string
		> StructImplementationExpression::GetMemberFunction(
			const std::string& name
		) {
			std::unordered_set<std::shared_ptr<FunctionDeclarationExpression>>::iterator itResult = std::find_if(
				m_memberFunctions.begin(),
				m_memberFunctions.end(),
				[&name](const std::shared_ptr<FunctionDeclarationExpression>& element) {
					return element->GetIdentifier()->GetName() == name;
				}
			);
			if(itResult == m_memberFunctions.end()) {
				return std::unexpected("Invalid member function '" + name + "' requested on struct '" + m_structIdentifier->GetName() + "'");
			}
			return *itResult;
		}

		std::string StructImplementationExpression::Repr(
			size_t indent
		) {
			std::string repr = "impl ";
			repr += m_structIdentifier->GetName() + " {\n";
			std::for_each(
				m_memberFunctions.begin(),
				m_memberFunctions.end(),
				[&repr, &indent](const std::shared_ptr<FunctionDeclarationExpression>& expression) -> void {
					repr += expression->Repr(indent + 1) + "\n";
				}
			);
			repr += "}";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}
		NodeType StructImplementationExpression::Type() {
			return NodeType::STRUCT_IMPL;
		}
		DatatypeStr StructImplementationExpression::GetDatatype() {
			return "VOID";
		}



		StructMemberFunctionAccessExpression::StructMemberFunctionAccessExpression(
			struct utils::Context ctx,
			std::shared_ptr<IdentifierExpression> structIdentifier,
			std::shared_ptr<IdentifierExpression> memberFunctionIdentifier,
			std::vector<std::shared_ptr<Expression>> arguments,
			DatatypeStr returnDatatype
		) :
			Expression(ctx),
			m_structIdentifier(structIdentifier),
			m_memberFunctionIdentifier(memberFunctionIdentifier),
			m_arguments(arguments),
			m_returnDatatype(returnDatatype)
		{}

		std::shared_ptr<IdentifierExpression> StructMemberFunctionAccessExpression::GetStruct() {
			return m_structIdentifier;
		}
		std::shared_ptr<IdentifierExpression> StructMemberFunctionAccessExpression::GetMemberFunction() {
			return m_memberFunctionIdentifier;
		}
		std::vector<std::shared_ptr<Expression>> StructMemberFunctionAccessExpression::GetArguments() {
			return m_arguments;
		}
		
		std::string StructMemberFunctionAccessExpression::Repr(
			size_t indent
		) {
			std::string repr = m_structIdentifier->GetName() + "." + m_memberFunctionIdentifier->GetName();
			repr += "(\n";
			for(size_t i = 0; i < m_arguments.size(); i++) {
				repr += m_arguments.at(i)->Repr(indent + 1) + (i < m_arguments.size() - 1 ? "," : "") + "\n";
			}
			repr += "\n)";
			repr.insert(0, indent, '\t');
			repr.insert(repr.size() - 1, indent, '\t');
			return repr;
		}
		NodeType StructMemberFunctionAccessExpression::Type() {
			return NodeType::STRUCT_ACCESS_FN;
		}
		DatatypeStr StructMemberFunctionAccessExpression::GetDatatype() {
			return m_returnDatatype;
		}
  	}
}
