#include <runner/interpreter/interpreter.hxx>

namespace dim {
	namespace interpreter {

		std::expected<std::shared_ptr<Value>, std::string> EvaluateScopeExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto scopeExpression = std::dynamic_pointer_cast<parser::ScopeExpression>(expression);

			std::shared_ptr<Value> scopeValue = std::make_shared<NullValue>();

			for(const auto& expression : scopeExpression->GetExpressions()) {
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					expression,
					scopeValue
				)
			}

			return scopeValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateNullExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			return std::make_shared<NullValue>();
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateNumberExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto numberExpression = std::dynamic_pointer_cast<parser::NumberExpression>(expression);

			return std::make_shared<NumberValue>(
				std::stod(numberExpression->GetValue())
			);
		}
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateBinaryExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto binaryExpression = std::dynamic_pointer_cast<parser::BinaryExpression>(expression);

			std::string binaryOperator = binaryExpression->GetOperator();

			std::shared_ptr<Value> lhs;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				binaryExpression->GetLeft(),
				lhs
			)
			std::shared_ptr<Value> rhs;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				binaryExpression->GetRight(),
				rhs
			)

			switch(lhs->Type()) {
			case ValueType::NUL: {
				auto lhsCast = std::dynamic_pointer_cast<NullValue>(lhs);

				if(binaryOperator == "+") {
					return *lhsCast.get() + rhs;
				} if(binaryOperator == "-") {
					return *lhsCast.get() - rhs;
				} if(binaryOperator == "*") {
					return *lhsCast.get() * rhs;
				} if(binaryOperator == "/") {
					return *lhsCast.get() / rhs;
				} else {
					return std::unexpected(
						std::string("Invalid operator ")
						+ binaryOperator
					);
				}
			}
			case ValueType::NUMBER: {
				auto lhsCast = std::dynamic_pointer_cast<NumberValue>(lhs);
				
				if(binaryOperator == "+") {
					return *lhsCast.get() + rhs;
				} if(binaryOperator == "-") {
					return *lhsCast.get() - rhs;
				} if(binaryOperator == "*") {
					return *lhsCast.get() * rhs;
				} if(binaryOperator == "/") {
					return *lhsCast.get() / rhs;
				} else {
					return std::unexpected(
						std::string("Invalid operator ")
						+ binaryOperator
					);
				}
			}
			default:
				return std::unexpected(std::string("Invalid lhs type"));
			}
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			std::shared_ptr<Value> value;

			try {
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateFunctionsMap.at(expression->Type()),
					expression,
					value
				)
			} catch(...) {
				return std::unexpected(
					std::string("Invalid expression type :")
					+ std::to_string(int(expression->Type()))
				);
			}

			return value;
		}

	}
}