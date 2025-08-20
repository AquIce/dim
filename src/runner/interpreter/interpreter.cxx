#include <runner/interpreter/interpreter.hxx>

namespace dim {
	namespace interpreter {

		std::expected<std::shared_ptr<Value>, std::string> EvaluateScopeExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto scopeExpression = std::dynamic_pointer_cast<parser::ScopeExpression>(expression);
	
			std::shared_ptr<Value> scopeValue = std::make_shared<NullValue>();

			for(const auto& expression : scopeExpression->GetExpressions()) {
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					expression,
					registerManager,
					scopeValue
				)
				if(scopeValue->GetFlag() == ValueFlag::BREAK) {
					return scopeValue;
				}
			}

			return scopeValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateIdentifierExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto identifierExpression = std::dynamic_pointer_cast<parser::IdentifierExpression>(expression);

			std::expected<
				RegisterValue,
				std::string
			> result = registerManager->Get(identifierExpression->GetName());

			if(!result) {
				return std::unexpected(result.error());
			}

			return result.value().value;
		}


		std::expected<std::shared_ptr<Value>, std::string> EvaluateNullExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			return std::make_shared<NullValue>();
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateBooleanExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto booleanExpression = std::dynamic_pointer_cast<parser::BooleanExpression>(expression);

			return std::make_shared<BooleanValue>(
				booleanExpression->GetValue() == "true"
			);
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateNumberExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto numberExpression = std::dynamic_pointer_cast<parser::NumberExpression>(expression);

			return std::make_shared<NumberValue>(
				std::stod(numberExpression->GetValue())
			);
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateStringExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto stringExpression = std::dynamic_pointer_cast<parser::StringExpression>(expression);

			return std::make_shared<StringValue>(
				stringExpression->GetValue()
			);
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateBreakExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto breakExpression = std::dynamic_pointer_cast<parser::BreakExpression>(expression);

			std::shared_ptr<Value> breakValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				breakExpression->GetExpression(),
				registerManager,
				breakValue
			);

			breakValue->SetFlag(ValueFlag::BREAK);
			return breakValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateOrExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto orExpression = std::dynamic_pointer_cast<parser::OrExpression>(expression);

			std::shared_ptr<Value> orValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				orExpression->GetExpression(),
				registerManager,
				orValue
			);

			return orValue;
		}
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateBinaryExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto binaryExpression = std::dynamic_pointer_cast<parser::BinaryExpression>(expression);

			std::string binaryOperator = binaryExpression->GetOperator();

			std::shared_ptr<Value> lhs;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				binaryExpression->GetLeft(),
				registerManager,
				lhs
			)
			std::shared_ptr<Value> rhs;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				binaryExpression->GetRight(),
				registerManager,
				rhs
			)

			switch(lhs->Type()) {
				__GEN__BINARY_OPERATOR_TYPE_CASE(NullValue, ValueType::NUL)
				__GEN__BINARY_OPERATOR_TYPE_CASE(NumberValue, ValueType::NUMBER)
				__GEN__BINARY_OPERATOR_TYPE_CASE(BooleanValue, ValueType::BOOLEAN)
				__GEN__BINARY_OPERATOR_TYPE_CASE(StringValue, ValueType::STRING)
			default:
				return std::unexpected(std::string("Invalid lhs type"));
			}
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateIfElseStructure(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto ifElseStructure = std::dynamic_pointer_cast<parser::IfElseStructure>(expression);

			auto ifElseRegisterManager = std::make_shared<RegisterManager>(registerManager);

			for(const auto& ifElseExpression : ifElseStructure->GetExpressions()) {
				std::shared_ptr<parser::Expression> condition = ifElseExpression->GetCondition();
				if(condition == nullptr) {
					return EvaluateScopeExpression(
						ifElseExpression->GetScope(),
						ifElseRegisterManager
					);
				}

				std::shared_ptr<Value> condition_value;
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					condition,
					ifElseRegisterManager,
					condition_value
				)

				if(condition_value->IsTrue()) {
					std::shared_ptr<Value> result_value;
					__TRY_VALUE_FUNC_WRETERR_WSAVE(
						EvaluateScopeExpression,
						ifElseExpression->GetScope(),
						ifElseRegisterManager,
						result_value
					)
					return result_value;
				}
			}

			return std::unexpected("Missing 'else' clause in if-else structure.");
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateLoopExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto loopExpression = std::dynamic_pointer_cast<parser::LoopExpression>(expression);

			auto loopRegisterManager = std::make_shared<RegisterManager>(registerManager);

			std::shared_ptr<Value> scopeValue;

			while(true) {
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateScopeExpression,
					loopExpression->GetScope(),
					loopRegisterManager,
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
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto whileLoopExpression = std::dynamic_pointer_cast<parser::WhileLoopExpression>(expression);

			auto loopRegisterManager = std::make_shared<RegisterManager>(registerManager);

			std::shared_ptr<Value> conditionValue;
			std::shared_ptr<Value> scopeValue = nullptr;

			while(true) {
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					whileLoopExpression->GetCondition(),
					loopRegisterManager,
					conditionValue
				)
				if(!conditionValue->IsTrue()) {
					break;
				}
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateScopeExpression,
					whileLoopExpression->GetScope(),
					loopRegisterManager,
					scopeValue
				)
				if(scopeValue->GetFlag() == ValueFlag::BREAK) {
					break;
				}
			}

			if(scopeValue == nullptr) {
				return EvaluateOrExpression(
					whileLoopExpression->GetOrExpression(),
					loopRegisterManager
				);
			}

			scopeValue->SetFlag(ValueFlag::NONE);

			return scopeValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateForLoopExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto forLoopExpression = std::dynamic_pointer_cast<parser::ForLoopExpression>(expression);

			auto loopRegisterManager = std::make_shared<RegisterManager>(registerManager);

			__TRY_VALUE_FUNC_WRETERR(
				EvaluateExpression,
				forLoopExpression->GetInitialExpression(),
				loopRegisterManager
			)

			std::shared_ptr<Value> conditionValue;
			std::shared_ptr<Value> scopeValue = nullptr;

			while(true) {
				__TRY_VALUE_FUNC_WRETERR(
					EvaluateExpression,
					forLoopExpression->GetUpdateExpression(),
					loopRegisterManager
				)
				
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					forLoopExpression->GetCondition(),
					loopRegisterManager,
					conditionValue
				)
				if(!conditionValue->IsTrue()) {
					break;
				}
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateScopeExpression,
					forLoopExpression->GetScope(),
					loopRegisterManager,
					scopeValue
				)
				if(scopeValue->GetFlag() == ValueFlag::BREAK) {
					break;
				}
			}

			if(scopeValue == nullptr) {
				return EvaluateOrExpression(
					forLoopExpression->GetOrExpression(),
					loopRegisterManager
				);
			}

			scopeValue->SetFlag(ValueFlag::NONE);

			return scopeValue;
		}
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateAssignationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto assignationExpression = std::dynamic_pointer_cast<parser::AssignationExpression>(expression);

			std::string name = assignationExpression->GetIdentifier()->GetName();

			std::shared_ptr<Value> identifierValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				assignationExpression->GetIdentifier()->GetExpression(),
				registerManager,
				identifierValue
			)

			std::expected<
				Success,
				std::string
			> result = registerManager->Set(name, RegisterValue{ identifierValue });

			return identifierValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateDeclarationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto declarationExpression = std::dynamic_pointer_cast<parser::DeclarationExpression>(expression);

			std::string name = declarationExpression->GetIdentifier()->GetName();

			std::shared_ptr<Value> identifierValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				declarationExpression->GetIdentifier()->GetExpression(),
				registerManager,
				identifierValue
			)

			std::expected<
				Success,
				std::string
			> result = registerManager->Register(name, RegisterValue{ identifierValue });

			if(!result) {
				return std::unexpected(result.error());
			}

			return identifierValue;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			std::shared_ptr<Value> value;

			try {
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateFunctionsMap.at(expression->Type()),
					expression,
					registerManager,
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

		std::expected<std::shared_ptr<Value>, std::string> EvaluateProgram(
			std::shared_ptr<parser::ScopeExpression> program
		) {
			std::expected<
				std::shared_ptr<Value>,
				std::string
			> result = EvaluateScopeExpression(
				program,
				std::make_shared<RegisterManager>(nullptr)	
			);

			if(!result) {
				return std::unexpected(
					std::string("[ERR::RUNNER::INTERPRETER] Got error :\n\t\"") + result.error()
					+ "\"\nwhile interpreting program."
				);
			}

			return result.value();
		}
	}
}
