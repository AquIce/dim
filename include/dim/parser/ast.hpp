#pragma once

#include <memory>
#include <string>

namespace dim {
	namespace parser {
		
		enum class NodeType {
			NONE,
			NUMBER,
			BINARY
		};

		class Expression {
		public:
			virtual std::string Repr();
			virtual NodeType Type();

			Expression() = default;
		};

		class NumberExpression : public Expression {
		public:
			NumberExpression(
				std::string value
			);

			std::string GetValue();

			std::string Repr() override;
			NodeType Type() override;

		private:
			std::string value;
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

			std::string Repr() override;
			NodeType Type() override;

		private:
			std::shared_ptr<Expression> left;
			std::string operatorSymbol;
			std::shared_ptr<Expression> right;
		};
	}
}
