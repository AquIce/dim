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
				LOG(scopeValue->Repr());
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

		std::expected<std::shared_ptr<Value>, std::string> EvaluateDiscardExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			std::shared_ptr<Value> discardValue = registerManager->GetDiscard();
			if(!discardValue) {
				return std::unexpected("No discard value defined.");
			}
			return discardValue;
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

		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateI8Expression, parser::I8Expression, I8Value, utils::stoi8)
		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateI16Expression, parser::I16Expression, I16Value, utils::stoi16)
		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateI32Expression, parser::I32Expression, I32Value, utils::stoi32)
		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateI64Expression, parser::I64Expression, I64Value, utils::stoi64)

		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateU8Expression, parser::U8Expression, U8Value, utils::stou8)
		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateU16Expression, parser::U16Expression, U16Value, utils::stou16)
		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateU32Expression, parser::U32Expression, U32Value, utils::stou32)
		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateU64Expression, parser::U64Expression, U64Value, utils::stou64)

		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateF32Expression, parser::F32Expression, F32Value, utils::stof32)
		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateF64Expression, parser::F64Expression, F64Value, utils::stof64)
		__GEN__EVALUTE_NUMBER_EXPRESSION(EvaluateF128Expression, parser::F128Expression, F128Value, utils::stof128)

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
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateUnaryExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto unaryExpression = std::dynamic_pointer_cast<parser::UnaryExpression>(expression);

			std::string unaryOperator = unaryExpression->GetOperator();

			std::shared_ptr<Value> term;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				unaryExpression->GetTerm(),
				registerManager,
				term
			)

			if(unaryOperator == "!") {
				return !(*term);
			} else if(unaryOperator == "~") {
				return ~(*term);
			}
			
			return std::unexpected("Invalid unary operator.");
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
				__GEN__BINARY_OPERATOR_TYPE_CASE(I8Value, ValueType::I8)
				__GEN__BINARY_OPERATOR_TYPE_CASE(I16Value, ValueType::I16)
				__GEN__BINARY_OPERATOR_TYPE_CASE(I32Value, ValueType::I32)
				__GEN__BINARY_OPERATOR_TYPE_CASE(I64Value, ValueType::I64)
				__GEN__BINARY_OPERATOR_TYPE_CASE(U8Value, ValueType::U8)
				__GEN__BINARY_OPERATOR_TYPE_CASE(U16Value, ValueType::U16)
				__GEN__BINARY_OPERATOR_TYPE_CASE(U32Value, ValueType::U32)
				__GEN__BINARY_OPERATOR_TYPE_CASE(U64Value, ValueType::U64)
				__GEN__BINARY_OPERATOR_TYPE_CASE(F32Value, ValueType::F32)
				__GEN__BINARY_OPERATOR_TYPE_CASE(F64Value, ValueType::F64)
				__GEN__BINARY_OPERATOR_TYPE_CASE(F128Value, ValueType::F128)
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

		std::expected<std::shared_ptr<Value>, std::string> EvaluateMatchStructure(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto matchStructure = std::dynamic_pointer_cast<parser::MatchStructure>(expression);

			auto matchRegisterManager = std::make_shared<RegisterManager>(
				registerManager
			);

			std::shared_ptr<Value> selectorValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				matchStructure->GetExpression(),
				matchRegisterManager,
				selectorValue
			)

			matchRegisterManager->SetDiscard(selectorValue);

			for(const auto& matchExpression : matchStructure->GetExpressions()) {
				std::shared_ptr<parser::Expression> condition = matchExpression->GetCondition();
				if(condition == nullptr) {
					return EvaluateScopeExpression(
						matchExpression->GetScope(),
						matchRegisterManager
					);
				}

				std::shared_ptr<Value> condition_value;
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					condition,
					matchRegisterManager,
					condition_value
				)

				std::expected<
					std::shared_ptr<Value>,
					std::string
				> result = *condition_value == selectorValue;

				if(
					(
						condition_value->Type() == ValueType::BOOLEAN
						&& condition_value->IsTrue()
					) ||
					(result && result.value()->IsTrue())
				) {
					std::shared_ptr<Value> result_value;
					__TRY_VALUE_FUNC_WRETERR_WSAVE(
						EvaluateScopeExpression,
						matchExpression->GetScope(),
						matchRegisterManager,
						result_value
					)
					return result_value;
				}
			}

			return std::unexpected("Missing default clause in match structure.");
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

				__TRY_VALUE_FUNC_WRETERR(
					EvaluateExpression,
					forLoopExpression->GetUpdateExpression(),
					loopRegisterManager
				)
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
