#pragma once

#include <memory>
#include <string>
#include <vector>

namespace dim {
	namespace parser {
		
		enum class NodeType {
			NONE = 0,
			SCOPE,
			NUL,
			NUMBER,
			BOOLEAN,
			STRING,
			BINARY,
			IFELSE_EXPR,
			IFELSE_STRUCT
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
	}
}
