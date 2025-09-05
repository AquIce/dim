#pragma once

#include <utils/utils.hxx>

#include <algorithm>
#include <array>
#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#define __GEN__SUB_NUMBER_CLASS(_ClassName, _ValueType) \
class _ClassName : public NumberExpression { \
public: \
	_ClassName( \
		_ValueType value \
	); \
	\
	std::string Repr( \
		size_t indent = 0 \
	) override; \
	NodeType Type() override; \
	Datatype GetDatatype() override; \
\
private: \
	_ValueType m_value; \
};

#define __GEN__SUB_NUMBER_CLASS_IMPL(_ClassName, _ValueType, ENUM_MEMBER) \
_ClassName::_ClassName( \
	_ValueType value \
) : \
	NumberExpression(std::to_string(value)), \
	m_value(value) \
{} \
\
std::string _ClassName::Repr( \
	size_t indent \
) { \
	std::string repr = std::string(#_ClassName) + "(" + std::to_string(m_value) + ")"; \
	repr.insert(0, indent, '\t'); \
	return repr; \
} \
\
NodeType _ClassName::Type() { \
	return NodeType::ENUM_MEMBER; \
} \
Datatype _ClassName::GetDatatype() { \
	return Datatype::ENUM_MEMBER; \
}

namespace dim {
	namespace parser {

		class Expression;
		class NestedExpression;
		class ScopeExpression;
		class NullExpression;
		class NumberExpression;
		class I8Expression;
		class I16Expression;
		class I32Expression;
		class I64Expression;
		class U8Expression;
		class U16Expression;
		class U32Expression;
		class U64Expression;
		class F32Expression;
		class F64Expression;
		class F128Expression;
		class BooleanExpression;
		class StringExpression;
		class UnaryExpression;
		class BinaryExpression;
		class IfElseExpression;
		class IfElseStructure;
		class MatchExpression;
		class MatchStructure;
		class LoopExpression;
		class WhileLoopExpression;
		class ForLoopExpression;
		class BreakExpression;
		class OrExpression;
		class IdentifierExpression;
		class DiscardExpression;
		class AssignationExpression;
		class DeclarationExpression;
		
		enum class NodeType {
			NONE = 0,
			NESTED,
			SCOPE,
			NUL,
			NUMBER,
			I8, I16, I32, I64,
			U8, U16, U32, U64,
			F32, F64, F128,
			BOOLEAN,
			STRING,
			UNARY,
			BINARY,
			IFELSE_EXPR,
			IFELSE_STRUCT,
			MATCH_EXPR,
			MATCH_STRUCT,
			LOOP,
			WHILE,
			FOR,
			BREAK,
			OR,
			IDENTIFIER,
			DISCARD,
			ASSIGN,
			DECL,
		};

		const std::array<std::string_view, 33> NodeTypeToStr = {
			"NONE",
			"NESTED",
			"SCOPE",
			"NUL",
			"NUMBER",
			"i8", "i16", "i32", "i64",
			"u8", "u16", "u32", "u64",
			"f32", "f64", "f128",
			"BOOLEAN",
			"STRING",
			"UNARY",
			"BINARY",
			"IFELSE_EXPR",
			"IFELSE_STRUCT",
			"MATCH_EXPR",
			"MATCH_STRUCT",
			"LOOP",
			"WHILE",
			"FOR",
			"BREAK",
			"OR",
			"IDENTIFIER",
			"ASSIGN",
			"DECL",
		};

		enum class Datatype {
			INFER = 0,
			I8, I16, I32, I64,
			U8, U16, U32, U64,
			F32, F64, F128,
			BOOLEAN, /*CHAR,*/ STRING,
		};

		const std::array<std::string_view, 14> DatatypeToStr = {
			"INFER",
			"i8", "i16", "i32", "i64",
			"u8", "u16", "u32", "u64",
			"f32", "f64", "f128",
			"bool", /*"char",*/ "str",
		};

		typedef struct {
			const std::string name;
			const bool isConst;
			const Datatype datatype;
		} IdentifierData;

		class ScopeIdentifierRegister {
		public:
			ScopeIdentifierRegister(
				std::shared_ptr<ScopeIdentifierRegister> parent = nullptr
			);

			std::expected<
				IdentifierData,
				std::string
			> Get(
				const std::string name
			);
			void Register(
				IdentifierData identifier
			);

		private:
			std::shared_ptr<ScopeIdentifierRegister> m_parent;
			std::vector<IdentifierData> m_identifiers;
		};

		class Expression {
		public:
			virtual std::string Repr(
				const size_t indent = 0
			);
			virtual NodeType Type();
			virtual Datatype GetDatatype();

			Expression() = default;
		};

		class NestedExpression : public Expression {
		public:
			NestedExpression(
				std::shared_ptr<Expression> expression
			);

			std::shared_ptr<Expression> GetExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		protected:
			std::shared_ptr<Expression> m_expression;
		};

		class IdentifierExpression : public NestedExpression {
		public:
			IdentifierExpression(
				std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
				std::string name,
				bool isConst = true,
				std::shared_ptr<Expression> expression = nullptr,
				Datatype datatype = Datatype::INFER
			);

			std::string GetName();
			bool GetIsConst();
			void SetIsConst(
				bool isConst
			);
			void SetExpression(
				std::shared_ptr<Expression> expression
			);
			void SetDatatype(
				Datatype datatype
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::string m_name;
			bool m_isConst;
			Datatype m_datatype;
		};

		class ScopeExpression : public Expression {
		public:
			ScopeExpression(
				std::vector<std::shared_ptr<Expression>> expressions = {},
				std::shared_ptr<IdentifierExpression> name = nullptr
			);

			std::vector<std::shared_ptr<Expression>> GetExpressions();
			void AddExpression(
				std::shared_ptr<Expression> expression
			);

			std::shared_ptr<IdentifierExpression> GetName();
			void SetName(
				std::shared_ptr<IdentifierExpression> name
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::vector<std::shared_ptr<Expression>> m_expressions;
			std::shared_ptr<IdentifierExpression> m_name;
		};

		class NullExpression : public Expression {
		public:
			NullExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;
		};

		class NumberExpression : public Expression {
		public:
			NumberExpression(
				std::string value
			);

			std::string GetValue();
			void SetValue(
				std::string value
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		protected:
			std::string m_value;
		};

		__GEN__SUB_NUMBER_CLASS(I8Expression, i8)
		__GEN__SUB_NUMBER_CLASS(I16Expression, i16)
		__GEN__SUB_NUMBER_CLASS(I32Expression, i32)
		__GEN__SUB_NUMBER_CLASS(I64Expression, i64)

		__GEN__SUB_NUMBER_CLASS(U8Expression, u8)
		__GEN__SUB_NUMBER_CLASS(U16Expression, u16)
		__GEN__SUB_NUMBER_CLASS(U32Expression, u32)
		__GEN__SUB_NUMBER_CLASS(U64Expression, u64)

		__GEN__SUB_NUMBER_CLASS(F32Expression, f32)
		__GEN__SUB_NUMBER_CLASS(F64Expression, f64)
		__GEN__SUB_NUMBER_CLASS(F128Expression, f128)

		class BooleanExpression : public Expression {
		public:
			BooleanExpression(
				std::string value
			);

			std::string GetValue();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::string m_value;
		};

		class StringExpression : public Expression {
		public:
			StringExpression(
				std::string value
			);

			std::string GetValue();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::string m_value;
		};

		class UnaryExpression : public Expression {
		public:
			UnaryExpression(
				std::shared_ptr<Expression> term,
				std::string operatorSymbol
			);

			std::shared_ptr<Expression> GetTerm();
			std::string GetOperator();

			std::shared_ptr<Expression> GetSampleExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::shared_ptr<Expression> m_term;
			std::string m_operatorSymbol;
		};

		class BinaryExpression : public Expression {
		public:
			BinaryExpression(
				std::shared_ptr<Expression> left,
				std::string operatorSymbol,
				std::shared_ptr<Expression> right
			);

			std::shared_ptr<Expression> GetLeft();
			std::string GetOperator();
			std::shared_ptr<Expression> GetRight();

			std::shared_ptr<Expression> GetSampleExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::shared_ptr<Expression> m_left;
			std::string m_operatorSymbol;
			std::shared_ptr<Expression> m_right;
		};

		class IfElseExpression : public Expression {
		public:
			IfElseExpression(
				std::shared_ptr<ScopeExpression> scope,
				std::shared_ptr<Expression> condition
			);

			std::shared_ptr<ScopeExpression> GetScope();
			std::shared_ptr<Expression> GetCondition();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::shared_ptr<ScopeExpression> m_scope;
			std::shared_ptr<Expression> m_condition;
		};

		class IfElseStructure : public Expression {
		public:
			IfElseStructure(
				std::vector<std::shared_ptr<IfElseExpression>> expressions
			);

			std::vector<std::shared_ptr<IfElseExpression>> GetExpressions();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::vector<std::shared_ptr<IfElseExpression>> m_expressions;
		};

		class MatchExpression : public Expression {
		public:
			MatchExpression(
				std::shared_ptr<ScopeExpression> scope,
				std::shared_ptr<Expression> condition
			);

			std::shared_ptr<ScopeExpression> GetScope();
			std::shared_ptr<Expression> GetCondition();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::shared_ptr<ScopeExpression> m_scope;
			std::shared_ptr<Expression> m_condition;
		};

		class MatchStructure : public NestedExpression {
		public:
			MatchStructure(
				std::shared_ptr<Expression> expression,
				std::vector<std::shared_ptr<MatchExpression>> expressions
			);

			std::vector<std::shared_ptr<MatchExpression>> GetExpressions();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::vector<std::shared_ptr<MatchExpression>> m_expressions;
		};

		class LoopExpression : public Expression {
		public:
			LoopExpression(
				std::shared_ptr<ScopeExpression> scope
			);

			std::shared_ptr<ScopeExpression> GetScope();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		protected:
			std::shared_ptr<ScopeExpression> m_scope;
		};

		class WhileLoopExpression : public LoopExpression {
		public:
			WhileLoopExpression(
				std::shared_ptr<ScopeExpression> scope,
				std::shared_ptr<Expression> condition,
				std::shared_ptr<OrExpression> orExpression
			);

			std::shared_ptr<Expression> GetCondition();
			std::shared_ptr<Expression> GetOrExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		protected:
			std::shared_ptr<Expression> m_condition;
			std::shared_ptr<Expression> m_orExpression;
		};

		class ForLoopExpression : public WhileLoopExpression {
		public:
			ForLoopExpression(
				std::shared_ptr<ScopeExpression> scope,
				std::shared_ptr<Expression> initialExpression,
				std::shared_ptr<Expression> condition,
				std::shared_ptr<Expression> updateExpression,
				std::shared_ptr<OrExpression> orExpression
			);

			std::shared_ptr<Expression> GetInitialExpression();
			std::shared_ptr<Expression> GetUpdateExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::shared_ptr<Expression> m_initialExpression;
			std::shared_ptr<Expression> m_updateExpression;
		};

		class BreakExpression : public NestedExpression {
		public:
			BreakExpression(
				std::shared_ptr<Expression> expression,
				std::shared_ptr<IdentifierExpression> scopeName = nullptr
			);

			std::shared_ptr<IdentifierExpression> GetScopeName();
			void SetScopeName(
				std::shared_ptr<IdentifierExpression> scopeName
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;
		
		private:
			std::shared_ptr<IdentifierExpression> m_name;
		};

		class OrExpression : public NestedExpression {
		public:
			OrExpression(
				std::shared_ptr<Expression> expression
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;
		};

		class DiscardExpression : public Expression {
		public:
			DiscardExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;
		};

		class AssignationExpression : public Expression {
		public:
			AssignationExpression(
				std::shared_ptr<IdentifierExpression> identifier,
				std::shared_ptr<Expression> expression
			);

			std::shared_ptr<IdentifierExpression> GetIdentifier();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::shared_ptr<IdentifierExpression> m_identifier;
		};

		class DeclarationExpression : public Expression {
		public:
			DeclarationExpression(
				std::shared_ptr<IdentifierExpression> identifier,
				std::shared_ptr<Expression> expression,
				Datatype datatype,
				bool isConst
			);

			std::shared_ptr<IdentifierExpression> GetIdentifier();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			Datatype GetDatatype() override;

		private:
			std::shared_ptr<IdentifierExpression> m_identifier;
		};
	}
}
