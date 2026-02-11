#include <runner/interpreter/interpreter.hxx>

namespace dim {
	namespace interpreter {

		FunctionRegisterManager functionRegisterManager = FunctionRegisterManager();

		Result<std::shared_ptr<Value>> Resultify(
			std::expected<std::shared_ptr<Value>, std::string> value,
			std::shared_ptr<parser::Expression> expression,
			utils::ErrorType type
		) {
			if(value) {
				return value.value();
			}
			return std::unexpected(utils::Error{
				.ctx = expression->ctx,
				.message = value.error(),
				.type = type,
			});
		}

		Result<std::shared_ptr<Value>> EvaluateScopeExpression(
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
				if(scopeValue->GetFlag().flag == ValueFlag::BREAK) {
					if(
						scopeExpression->GetName() != nullptr
						&& scopeExpression->GetName()->GetName() == scopeValue->GetFlag().breakScopeName
					) {
						scopeValue->SetFlag({
							.flag = ValueFlag::NONE,
							.breakScopeName = ""
						});
					}
					return scopeValue;
				}
				if(scopeValue->GetFlag().flag == ValueFlag::RETURN) {
					return scopeValue;
				}
			}

			return scopeValue;
		}

		Result<std::shared_ptr<Value>> EvaluateIdentifierExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto identifierExpression = std::dynamic_pointer_cast<parser::IdentifierExpression>(expression);

			std::expected<
				RegisterValue,
				std::string
			> result = registerManager->Get(identifierExpression->GetName());

			if(!result) {
				return std::unexpected(utils::Error{
					.ctx = identifierExpression->ctx,
					.message = result.error(),
					.type = utils::ErrorType::ERROR,
				});
			}

			return result.value().value;
		}

		Result<std::shared_ptr<Value>> EvaluateDiscardExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			std::shared_ptr<Value> discardValue = registerManager->GetDiscard();
			if(!discardValue) {
				return std::unexpected(utils::Error{
					.ctx = expression->ctx,
					.message = "No discard value defined.",
					.type = utils::ErrorType::ERROR,
				});
			}
			return discardValue;
		}

		Result<std::shared_ptr<Value>> EvaluateNullExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			return std::make_shared<NullValue>();
		}

		Result<std::shared_ptr<Value>> EvaluateBooleanExpression(
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

		Result<std::shared_ptr<Value>> EvaluateCharExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto stringExpression = std::dynamic_pointer_cast<parser::CharExpression>(expression);

			return std::make_shared<CharValue>(
				stringExpression->GetValue().at(0)
			);
		}

		Result<std::shared_ptr<Value>> EvaluateStringExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto stringExpression = std::dynamic_pointer_cast<parser::StringExpression>(expression);

			return std::make_shared<StringValue>(
				stringExpression->GetValue()
			);
		}

		Result<std::shared_ptr<Value>> EvaluateBreakExpression(
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

			std::shared_ptr<parser::IdentifierExpression> scopeName = breakExpression->GetScopeName();
			breakValue->SetFlag({
				.flag = ValueFlag::BREAK,
				.breakScopeName = scopeName ? scopeName->GetName() : ""
			});
			return breakValue;
		}

		Result<std::shared_ptr<Value>> EvaluateReturnExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto returnExpression = std::dynamic_pointer_cast<parser::ReturnExpression>(expression);

			std::shared_ptr<Value> returnValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				returnExpression->GetExpression(),
				registerManager,
				returnValue
			);

			returnValue->SetFlag({
				.flag = ValueFlag::RETURN
			});
			return returnValue;
		}

		Result<std::shared_ptr<Value>> EvaluateOrExpression(
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
		
		Result<std::shared_ptr<Value>> EvaluateUnaryExpression(
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
				return Resultify(!(*term), expression);
			} else if(unaryOperator == "~") {
				return Resultify(~(*term), expression);
			}
			
			return std::unexpected(utils::Error{
				.ctx = expression->ctx,
				.message = "Invalid unary operator.",
				.type = utils::ErrorType::ERROR,
			});
		}
		
		Result<std::shared_ptr<Value>> EvaluateBinaryExpression(
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

			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), NullValue, "VOID")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), I8Value, "I8")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), I16Value, "I16")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), I32Value, "I32")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), I64Value, "I64")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), U8Value, "U8")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), U16Value, "U16")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), U32Value, "U32")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), U64Value, "U64")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), F32Value, "F32")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), F64Value, "F64")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), F128Value, "F128")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), BooleanValue, "BOOLEAN")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), CharValue, "CHAR")
			__GEN__BINARY_OPERATOR_TYPE_CASE(lhs->Type(), StringValue, "STRING")

			return std::unexpected(utils::Error{
				.ctx = expression->ctx,
				.message = "Invalid lhs type",
				.type = utils::ErrorType::ERROR,
			});
		}

		Result<std::shared_ptr<Value>> EvaluateIfElseStructure(
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

			return std::unexpected(utils::Error{
				.ctx = expression->ctx,
				.message = "Missing 'else' clause in if-else structure.",
				.type = utils::ErrorType::ERROR,
			});
		}

		Result<std::shared_ptr<Value>> EvaluateMatchStructure(
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
						condition_value->Type() == "BOOLEAN"
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

			return std::unexpected(utils::Error{
				.ctx = expression->ctx,
				.message = "Missing default clause in match structure.",
				.type = utils::ErrorType::ERROR,
			});
		}

		Result<std::shared_ptr<Value>> EvaluateLoopExpression(
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
				if(scopeValue->GetFlag().flag == ValueFlag::BREAK) {
					break;
				}
			}

			std::shared_ptr<parser::IdentifierExpression> scopeName = loopExpression->GetScope()->GetName();
			struct ValueFlagWVal flagWValue = scopeValue->GetFlag();
			
			if(
				flagWValue.flag == ValueFlag::BREAK
				&& (
					flagWValue.breakScopeName == ""
					|| (
						scopeName
						&& flagWValue.breakScopeName == scopeName->GetName()
					)
				)
			) {
				scopeValue->SetFlag({
					.flag = ValueFlag::NONE
				});
			}

			return scopeValue;
		}

		Result<std::shared_ptr<Value>> EvaluateWhileLoopExpression(
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
				if(scopeValue->GetFlag().flag == ValueFlag::BREAK) {
					break;
				}
			}

			if(scopeValue == nullptr) {
				return EvaluateOrExpression(
					whileLoopExpression->GetOrExpression(),
					loopRegisterManager
				);
			}

			std::shared_ptr<parser::IdentifierExpression> scopeName = whileLoopExpression->GetScope()->GetName();
			struct ValueFlagWVal flagWValue = scopeValue->GetFlag();
			
			if(
				flagWValue.flag == ValueFlag::BREAK
				&& (
					flagWValue.breakScopeName == ""
					|| (
						scopeName
						&& flagWValue.breakScopeName == scopeName->GetName()
					)
				)
			) {
				scopeValue->SetFlag({
					.flag = ValueFlag::NONE
				});
			}

			return scopeValue;
		}

		Result<std::shared_ptr<Value>> EvaluateForLoopExpression(
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
				if(scopeValue->GetFlag().flag == ValueFlag::BREAK) {
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

			std::shared_ptr<parser::IdentifierExpression> scopeName = forLoopExpression->GetScope()->GetName();
			struct ValueFlagWVal flagWValue = scopeValue->GetFlag();
			
			if(
				flagWValue.flag == ValueFlag::BREAK
				&& (
					flagWValue.breakScopeName == ""
					|| (
						scopeName
						&& flagWValue.breakScopeName == scopeName->GetName()
					)
				)
			) {
				scopeValue->SetFlag({
					.flag = ValueFlag::NONE
				});
			}

			return scopeValue;
		}
		
		Result<std::shared_ptr<Value>> EvaluateAssignationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto assignationExpression = std::dynamic_pointer_cast<parser::AssignationExpression>(expression);

			switch(assignationExpression->GetDestination()->Type()) {
				case parser::NodeType::IDENTIFIER: {
					std::string name = std::dynamic_pointer_cast<parser::IdentifierExpression>(
						assignationExpression->GetDestination()
					)->GetName();

					std::shared_ptr<Value> identifierValue;
					__TRY_VALUE_FUNC_WRETERR_WSAVE(
						EvaluateExpression,
						assignationExpression->GetExpression(),
						registerManager,
						identifierValue
					)

					std::expected<
						Success,
						std::string
					> result = registerManager->Set(name, RegisterValue{ identifierValue });

					if(!result) {
						return std::unexpected(utils::Error{
							.ctx = expression->ctx,
							.message = result.error(),
							.type = utils::ErrorType::ERROR,
						});
					}

					return identifierValue;
				}
				case parser::NodeType::STRUCT_ACCESS: {
					auto structMemberAccessExpression = std::dynamic_pointer_cast<parser::StructMemberAccessExpression>(
						assignationExpression->GetDestination()
					);

					std::shared_ptr<StructValue> structValue;
					{
						std::expected<
							RegisterValue,
							std::string
						> result = registerManager->Get(structMemberAccessExpression->GetStruct()->GetName());

						if(!result) {
							return std::unexpected(utils::Error{
								.ctx = expression->ctx,
								.message = result.error(),
								.type = utils::ErrorType::ERROR,
							});
						}
						structValue = std::dynamic_pointer_cast<StructValue>(result.value().value);
					}

					std::shared_ptr<Value> structMemberValue;
					__TRY_VALUE_FUNC_WRETERR_WSAVE(
						EvaluateExpression,
						assignationExpression->GetExpression(),
						registerManager,
						structMemberValue
					)

					{
						std::expected<
							Success,
							std::string
						> result = structValue->SetValue(
							structMemberAccessExpression->GetMember()->GetName(),
							structMemberValue
						);

						if(!result) {
							return std::unexpected(utils::Error{
								.ctx = expression->ctx,
								.message = result.error(),
								.type = utils::ErrorType::ERROR,
							});
						}
					}

					return structMemberValue;
				}
				default:
					return std::unexpected(utils::Error{
						.ctx = expression->ctx,
						.message = "Invalid expression type as assignable.",
						.type = utils::ErrorType::ERROR,
					});
			}
		}

		Result<std::shared_ptr<Value>> EvaluateDeclarationExpression(
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
				return std::unexpected(utils::Error{
					.ctx = expression->ctx,
					.message = result.error(),
					.type = utils::ErrorType::ERROR,
				});
			}

			return identifierValue;
		}

		Result<std::shared_ptr<Value>> EvaluateFunctionDeclarationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			functionRegisterManager.Register(
				FunctionRegisterValue{
					.function = std::dynamic_pointer_cast<parser::FunctionDeclarationExpression>(expression)
				}
			);

			return std::make_shared<NullValue>();
		}

		Result<std::shared_ptr<Value>> EvaluateFunctionCallExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto functionCallExpression = std::dynamic_pointer_cast<parser::FunctionCallExpression>(expression);
			auto innerRegisterManager = std::make_shared<RegisterManager>();

			std::string functionName = functionCallExpression->GetIdentifier()->GetName();

			std::vector<std::shared_ptr<parser::Expression>> argumentsExpressions = functionCallExpression->GetArguments();

			std::shared_ptr<parser::FunctionDeclarationExpression> functionDeclarationExpression;
			{
				std::expected<
					FunctionRegisterValue,
					std::string
				> result = functionRegisterManager.Get(functionName);
				if(!result) {
					return std::unexpected(utils::Error{
						.ctx = expression->ctx,
						.message = result.error(),
						.type = utils::ErrorType::ERROR,
					});
				}
				functionDeclarationExpression = result.value().function;
			}

			std::vector<std::shared_ptr<parser::DeclarationExpression>> argumentsDeclarationExpressions = functionDeclarationExpression->GetArguments();

			for(size_t i = 0; i < argumentsExpressions.size(); i++) {

				Result<std::shared_ptr<Value>> result = EvaluateExpression(
					argumentsExpressions.at(i),
					registerManager
				);
				if(!result) {
				  return std::unexpected(result.error());
				}
				
				innerRegisterManager->Register(
					argumentsDeclarationExpressions.at(i)->GetIdentifier()->GetName(),
					RegisterValue{
						.value = result.value()
					}
				);
			}

			std::shared_ptr<Value> scopeValue;
			{
				Result<std::shared_ptr<Value>> result = EvaluateScopeExpression(
					functionDeclarationExpression->GetScope(),
					innerRegisterManager
				);
				if(!result) {
				  return std::unexpected(result.error());
				}
				scopeValue = result.value();
			}
			
			struct ValueFlagWVal flagWValue = scopeValue->GetFlag();
			
			if(
				flagWValue.flag == ValueFlag::RETURN
			) {
				scopeValue->SetFlag({
					.flag = ValueFlag::NONE
				});
			}

			return scopeValue;
		}

		Result<std::shared_ptr<Value>> EvaluateInterfaceDeclarationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto interfaceDeclarationExpression = std::dynamic_pointer_cast<parser::InterfaceDeclarationExpression>(expression);
			return std::make_shared<NullValue>();
		}

		Result<std::shared_ptr<Value>> EvaluateStructDeclarationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto structDeclarationExpression = std::dynamic_pointer_cast<parser::StructDeclarationExpression>(expression);
			return std::make_shared<NullValue>();
		}

		Result<std::shared_ptr<Value>> EvaluateStructExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto structExpression = std::dynamic_pointer_cast<parser::StructExpression>(expression);
			std::unordered_map<std::string, std::shared_ptr<Value>> members = {};

			for(const auto& member : structExpression->GetMembers()) {
				std::shared_ptr<Value> memberValue;
				__TRY_VALUE_FUNC_WRETERR_WSAVE(
					EvaluateExpression,
					member->GetExpression(),
					registerManager,
					memberValue
				)
				members.insert({ member->GetName(), memberValue });
			}

			return std::make_shared<StructValue>(
				structExpression->GetName()->GetName(),
				members
			);
		}

		Result<std::shared_ptr<Value>> EvaluateStructMemberAccessExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto structMemberAccessExpression = std::dynamic_pointer_cast<parser::StructMemberAccessExpression>(expression);
			
			RegisterValue structValue;
			{
				std::expected<
					RegisterValue,
					std::string
				> result = registerManager->Get(structMemberAccessExpression->GetStruct()->GetName());

				if(!result) {
					return std::unexpected(utils::Error{
						.ctx = expression->ctx,
						.message = result.error(),
						.type = utils::ErrorType::ERROR,
					});
				}
				structValue = result.value();
			}

			if(std::dynamic_pointer_cast<StructValue>(structValue.value) == nullptr) {
				return std::unexpected(utils::Error{
					.ctx = expression->ctx,
					.message = "Invalid non struct value for "
						+ structMemberAccessExpression->GetStruct()->GetName(),
					.type = utils::ErrorType::ERROR,
				});
			}
			
			std::shared_ptr<Value> value;

			{
				std::expected<
					std::shared_ptr<Value>,
					std::string
				> result = std::dynamic_pointer_cast<StructValue>(
					structValue.value
				)->GetValue(structMemberAccessExpression->GetMember()->GetName());

				if(!result) {
					return std::unexpected(utils::Error{
						.ctx = expression->ctx,
						.message = result.error(),
						.type = utils::ErrorType::ERROR,
					});
				}

				value = result.value();
			}

			return value;
		}


		Result<std::shared_ptr<Value>> EvaluateStructImplementationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto structImplementationExpression = std::dynamic_pointer_cast<parser::StructImplementationExpression>(expression);
			for(const auto& memberFunction : structImplementationExpression->GetMemberFunctions()) {
				functionRegisterManager.CustomRegister(
					structImplementationExpression->GetStruct()->GetName(),
					FunctionRegisterValue{
						.function = memberFunction
					}
				);
			}
			return std::make_shared<NullValue>();
		}


		Result<std::shared_ptr<Value>> EvaluateStructMemberFunctionAccessExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		) {
			auto structMemberFunctionAccessExpression = std::dynamic_pointer_cast<parser::StructMemberFunctionAccessExpression>(expression);

			auto thisRegisterManager = std::make_shared<RegisterManager>(registerManager);

			std::shared_ptr<Value> structValue;
			__TRY_VALUE_FUNC_WRETERR_WSAVE(
				EvaluateExpression,
				structMemberFunctionAccessExpression->GetStruct(),
				thisRegisterManager,
				structValue
			)

			registerManager->Register(
				"this",
				RegisterValue{
					.value = structValue
				}
			);

			std::vector<std::shared_ptr<parser::Expression>> argumentsExpressions = structMemberFunctionAccessExpression->GetArguments();

			std::shared_ptr<parser::FunctionDeclarationExpression> functionDeclarationExpression;
			{
				std::expected<
					FunctionRegisterValue,
					std::string
				> result = functionRegisterManager.CustomGet(
					structMemberFunctionAccessExpression->GetStruct()->GetDatatype(),
					structMemberFunctionAccessExpression->GetMemberFunction()->GetName()
				);
				if(!result) {
					return std::unexpected(utils::Error{
						.ctx = expression->ctx,
						.message = result.error(),
						.type = utils::ErrorType::ERROR,
					});
				}
				functionDeclarationExpression = result.value().function;
			}

			std::vector<std::shared_ptr<parser::DeclarationExpression>> argumentsDeclarationExpressions = functionDeclarationExpression->GetArguments();

			for(size_t i = 0; i < argumentsExpressions.size(); i++) {

				Result<std::shared_ptr<Value>> result = EvaluateExpression(
					argumentsExpressions.at(i),
					thisRegisterManager
				);
				if(!result) {
				  return std::unexpected(result.error());
				}
				
				thisRegisterManager->Register(
					argumentsDeclarationExpressions.at(i)->GetIdentifier()->GetName(),
					RegisterValue{
						.value = result.value()
					}
				);
			}

			std::shared_ptr<Value> scopeValue;
			{
				Result<std::shared_ptr<Value>> result = EvaluateScopeExpression(
					functionDeclarationExpression->GetScope(),
					thisRegisterManager
				);
				if(!result) {
				  return std::unexpected(result.error());
				}
				scopeValue = result.value();
			}
			
			struct ValueFlagWVal flagWValue = scopeValue->GetFlag();
			
			if(
				flagWValue.flag == ValueFlag::RETURN
			) {
				scopeValue->SetFlag({
					.flag = ValueFlag::NONE
				});
			}

			return scopeValue;
		}


		Result<std::shared_ptr<Value>> EvaluateExpression(
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
				return std::unexpected(utils::Error{
					.ctx = expression->ctx,
					.message = std::string("Invalid expression type: ")
						+ std::string(parser::NodeTypeToStr.at(int(expression->Type()))),
					.type = utils::ErrorType::ERROR,
				});
			}

			return value;
		}

		std::expected<std::shared_ptr<Value>, std::string> EvaluateProgram(
			std::shared_ptr<parser::ScopeExpression> program
		) {
			Result<std::shared_ptr<Value>> result = EvaluateScopeExpression(
				program,
				std::make_shared<RegisterManager>(nullptr)
			);

			if(!result) {
				return std::unexpected(
					std::string("[ERR::RUNNER::INTERPRETER] Got error :\n\t\"") + utils::ErrorRepr(result.error())
					+ "\"\nwhile interpreting program."
				);
			}

			return result.value();
		}
	}
}
