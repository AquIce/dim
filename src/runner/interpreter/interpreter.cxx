#include <runner/interpreter/interpreter.hxx>

namespace dim {
	namespace interpreter {

		RegisterManager registerManager = RegisterManager();

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
				if(scopeValue->GetFlag() == ValueFlag::BREAK) {
					return scopeValue;
				}
			}

			return scopeValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateIdentifierExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto identifierExpression = std::dynamic_pointer_cast<parser::IdentifierExpression>(expression);

			std::expected<
				RegisterValue,
				std::string
			> result = registerManager.Get(identifierExpression->GetName());

			if(!result) {
				return std::unexpected(result.error());
			}

			return result.value().value;
		}


		std::expected<std::shared_ptr<Value>, std::string> EvaluateNullExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			return std::make_shared<NullValue>();
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateBooleanExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto booleanExpression = std::dynamic_pointer_cast<parser::BooleanExpression>(expression);

			return std::make_shared<BooleanValue>(
				booleanExpression->GetValue() == "true"
			);
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateNumberExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto numberExpression = std::dynamic_pointer_cast<parser::NumberExpression>(expression);

			return std::make_shared<NumberValue>(
				std::stod(numberExpression->GetValue())
			);
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateStringExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto stringExpression = std::dynamic_pointer_cast<parser::StringExpression>(expression);

			return std::make_shared<StringValue>(
				stringExpression->GetValue()
			);
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateBreakExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto breakExpression = std::dynamic_pointer_cast<parser::BreakExpression>(expression);

			std::shared_ptr<Value> breakValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				breakExpression->GetExpression(),
				breakValue
			);

			breakValue->SetFlag(ValueFlag::BREAK);
			return breakValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateOrExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto orExpression = std::dynamic_pointer_cast<parser::OrExpression>(expression);

			std::shared_ptr<Value> orValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				orExpression->GetExpression(),
				orValue
			);

			return orValue;
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

		std::expected<std::shared_ptr<Value>, std::string> EvaluateIfElseStructure(
			std::shared_ptr<parser::Expression> expression
		) {
			auto ifElseStructure = std::dynamic_pointer_cast<parser::IfElseStructure>(expression);

			for(const auto& ifElseExpression : ifElseStructure->GetExpressions()) {
				std::shared_ptr<parser::Expression> condition = ifElseExpression->GetCondition();
				if(condition == nullptr) {
					return EvaluateScopeExpression(ifElseExpression->GetScope());
				}

				std::shared_ptr<Value> condition_value;
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					condition,
					condition_value
				)

				if(condition_value->IsTrue()) {
					std::shared_ptr<Value> result_value;
					__TRY_VALUE_FUNC_WRETERR_WSAVE(
						EvaluateScopeExpression,
						ifElseExpression->GetScope(),
						result_value
					)
					return result_value;
				}
			}

			return std::unexpected("Missing 'else' clause in if-else structure.");
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateLoopExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto loopExpression = std::dynamic_pointer_cast<parser::LoopExpression>(expression);

			std::shared_ptr<Value> scopeValue;

			while(true) {
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateScopeExpression,
					loopExpression->GetScope(),
					scopeValue
				)
				if(scopeValue->GetFlag() == ValueFlag::BREAK) {
					break;
				}
			}

			scopeValue->SetFlag(ValueFlag::NONE);
			return scopeValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateWhileLoopExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto whileLoopExpression = std::dynamic_pointer_cast<parser::WhileLoopExpression>(expression);

			std::shared_ptr<Value> conditionValue;
			std::shared_ptr<Value> scopeValue = nullptr;

			while(true) {
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					whileLoopExpression->GetCondition(),
					conditionValue
				)
				if(!conditionValue->IsTrue()) {
					break;
				}
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateScopeExpression,
					whileLoopExpression->GetScope(),
					scopeValue
				)
				if(scopeValue->GetFlag() == ValueFlag::BREAK) {
					break;
				}
			}

			if(scopeValue == nullptr) {
				return EvaluateOrExpression(whileLoopExpression->GetOrExpression());
			}

			scopeValue->SetFlag(ValueFlag::NONE);

			return scopeValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateForLoopExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto forLoopExpression = std::dynamic_pointer_cast<parser::ForLoopExpression>(expression);

			__TRY_VALUE_FUNC_WRETERR(
				EvaluateExpression,
				forLoopExpression->GetInitialExpression()
			)

			std::shared_ptr<Value> conditionValue;
			std::shared_ptr<Value> scopeValue = nullptr;

			while(true) {
				__TRY_VALUE_FUNC_WRETERR(
					EvaluateExpression,
					forLoopExpression->GetUpdateExpression()
				)
				
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					forLoopExpression->GetCondition(),
					conditionValue
				)
				if(!conditionValue->IsTrue()) {
					break;
				}
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateScopeExpression,
					forLoopExpression->GetScope(),
					scopeValue
				)
				if(scopeValue->GetFlag() == ValueFlag::BREAK) {
					break;
				}
			}

			if(scopeValue == nullptr) {
				return EvaluateOrExpression(forLoopExpression->GetOrExpression());
			}

			scopeValue->SetFlag(ValueFlag::NONE);

			return scopeValue;
		}
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateAssignationExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto assignationExpression = std::dynamic_pointer_cast<parser::AssignationExpression>(expression);

			std::string name = assignationExpression->GetIdentifier()->GetName();

			std::shared_ptr<Value> identifierValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				assignationExpression->GetIdentifier()->GetExpression(),
				identifierValue
			)

			std::expected<
				Success,
				std::string
			> result = registerManager.Set(name, RegisterValue{ identifierValue });

			return identifierValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateDeclarationExpression(
			std::shared_ptr<parser::Expression> expression
		) {
			auto declarationExpression = std::dynamic_pointer_cast<parser::DeclarationExpression>(expression);

			std::string name = declarationExpression->GetIdentifier()->GetName();

			std::shared_ptr<Value> identifierValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				declarationExpression->GetIdentifier()->GetExpression(),
				identifierValue
			)

			std::expected<
				Success,
				std::string
			> result = registerManager.Register(name, RegisterValue{ identifierValue });

			return identifierValue;
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
					+ std::string(parser::NodeTypeToStr.at(int(expression->Type())))
				);
			}

			return value;
		}

	}
}
