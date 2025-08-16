#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace dim {
	namespace parser {

		class Expression;
		class ScopeExpression;
		class NullExpression;
		class NumberExpression;
		class BooleanExpression;
		class StringExpression;
		class BinaryExpression;
		class IfElseExpression;
		class IfElseStructure;
		class LoopExpression;
		class WhileLoopExpression;
		class NestedExpression;
		class BreakExpression;
		class OrExpression;
		class IdentifierExpression;
		class AssignationExpression;
		class DeclarationExpression;
		
		enum class NodeType {
			NONE = 0,
			SCOPE,
			NUL,
			NUMBER,
			BOOLEAN,
			STRING,
			BINARY,
			IFELSE_EXPR,
			IFELSE_STRUCT,
			LOOP,
			WHILE,
			NESTED,
			BREAK,
			OR,
			IDENTIFIER,
			ASSIGN,
			DECL,
		};

		const std::array<std::string_view, 17> NodeTypeToStr = {
			"NONE",
			"SCOPE",
			"NUL",
			"NUMBER",
			"BOOLEAN",
			"STRING",
			"BINARY",
			"IFELSE_EXPR",
			"IFELSE_STRUCT",
			"LOOP",
			"WHILE",
			"NESTED",
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
			F32, F64,
			BOOL, CHAR, STR,
		};

		const std::array<std::string_view, 14> DatatypeToStr = {
			"INFER",
			"i8", "i16", "i32", "i64",
			"u8", "u16", "u32", "u64",
			"f32", "f64",
			"bool", "char", "str",
		};

		class Expression {
		public:
			virtual std::string Repr(
				size_t indent = 0
			);
			virtual NodeType Type();

			Expression() = default;
		};

		class ScopeExpression : public Expression {
		public:
			ScopeExpression(
				std::vector<std::shared_ptr<Expression>> expressions = {}
			);

			std::vector<std::shared_ptr<Expression>> GetExpressions();
			void AddExpression(
				std::shared_ptr<Expression> expression
			);

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;

		private:
			std::vector<std::shared_ptr<Expression>> m_expressions;
		};

		class NullExpression : public Expression {
		public:
			NullExpression();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;
		};

		class NumberExpression : public Expression {
		public:
			NumberExpression(
				std::string value
			);

			std::string GetValue();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;

		private:
			std::string m_value;
		};

		class BooleanExpression : public Expression {
		public:
			BooleanExpression(
				std::string value
			);

			std::string GetValue();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;

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
				size_t indent = 0
			) override;
			NodeType Type() override;

		private:
			std::string m_value;
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

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;

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
				size_t indent = 0
			) override;
			NodeType Type() override;

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
				size_t indent = 0
			) override;
			NodeType Type() override;

		private:
			std::vector<std::shared_ptr<IfElseExpression>> m_expressions;
		};

		class LoopExpression : public Expression {
		public:
			LoopExpression(
				std::shared_ptr<ScopeExpression> scope
			);

			std::shared_ptr<ScopeExpression> GetScope();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;

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
				size_t indent = 0
			) override;
			NodeType Type() override;

		protected:
			std::shared_ptr<Expression> m_condition;
			std::shared_ptr<Expression> m_orExpression;
		};

		class NestedExpression : public Expression {
		public:
			NestedExpression(
				std::shared_ptr<Expression> expression
			);

			std::shared_ptr<Expression> GetExpression();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;

		protected:
			std::shared_ptr<Expression> m_expression;
		};

		class BreakExpression : public NestedExpression {
		public:
			BreakExpression(
				std::shared_ptr<Expression> expression
			);

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;
		};

		class OrExpression : public NestedExpression {
		public:
			OrExpression(
				std::shared_ptr<Expression> expression
			);

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;
		};

		class IdentifierExpression : public NestedExpression {
		public:
			IdentifierExpression(
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
			Datatype GetDatatype();
			void SetDatatype(
				Datatype datatype
			);

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;

		private:
			std::string m_name;
			bool m_isConst;
			Datatype m_datatype;
		};

		class AssignationExpression : public Expression {
		public:
			AssignationExpression(
				std::shared_ptr<IdentifierExpression> identifier,
				std::shared_ptr<Expression> expression
			);

			std::shared_ptr<IdentifierExpression> GetIdentifier();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;

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
				size_t indent = 0
			) override;
			NodeType Type() override;

		private:
			std::shared_ptr<IdentifierExpression> m_identifier;
		};
	}
}
