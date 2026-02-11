#pragma once

#include <runner/interpreter/register.hxx>
#include <runner/interpreter/value.hxx>
#include <parser/ast.hxx>
#include <utils/utils.hxx>

#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#define __TRY_VALUE_FUNC_WRETERR(func, expression, registerManager) \
__TRY_EXPECTED_FUNC_WRETERR__NEW( \
	func, \
	std::shared_ptr<Value>, \
	expression, \
	registerManager \
)
#define __TRY_VALUE_FUNC_WRETERR_WSAVE(func, expression, registerManager, value) \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE__NEW( \
	func, \
	std::shared_ptr<Value>, \
	value, \
	expression, \
	registerManager \
)

#define __GEN__BINARY_OPERATOR_TYPE_CASE(lookupType, valueType, currentType) \
if(lookupType == currentType) { \
	auto lhsCast = std::dynamic_pointer_cast<valueType>(lhs); \
	if(binaryOperator == "+") { \
		return Resultify(*lhsCast + rhs, expression); \
	} if(binaryOperator == "-") { \
		return Resultify(*lhsCast - rhs, expression); \
	} if(binaryOperator == "*") { \
		return Resultify(*lhsCast * rhs, expression); \
	} if(binaryOperator == "/") { \
		return Resultify(*lhsCast / rhs, expression); \
	} if(binaryOperator == "<") { \
		return Resultify(*lhsCast < rhs, expression); \
	} if(binaryOperator == ">") { \
		return Resultify(*lhsCast > rhs, expression); \
	} if(binaryOperator == "<=") { \
		return Resultify(*lhsCast <= rhs, expression); \
	} if(binaryOperator == ">=") { \
		return Resultify(*lhsCast >= rhs, expression); \
	} if(binaryOperator == "&&") { \
		return Resultify(*lhsCast && rhs, expression); \
	} if(binaryOperator == "||") { \
		return Resultify(*lhsCast || rhs, expression); \
	} if(binaryOperator == "==") { \
		return Resultify(*lhsCast == rhs, expression); \
	} if(binaryOperator == "!=") { \
		return Resultify(*lhsCast != rhs, expression); \
	} if(binaryOperator == "&") { \
		return Resultify(*lhsCast & rhs, expression); \
	} if(binaryOperator == "|") { \
		return Resultify(*lhsCast | rhs, expression); \
	} if(binaryOperator == "^") { \
		return Resultify(*lhsCast ^ rhs, expression); \
	} else { \
		return std::unexpected(utils::Error{ \
			.ctx = expression->ctx, \
			.message = std::string("Invalid operator ") \
				+ binaryOperator, \
			.type = utils::ErrorType::ERROR, \
    }); \
	} \
}

#define __GEN__EVALUTE_NUMBER_EXPRESSION(name, _ExpressionDatatype, _ValueDatatype, convertfn) \
Result<std::shared_ptr<Value>> name( \
	std::shared_ptr<parser::Expression> expression, \
	std::shared_ptr<RegisterManager> registerManager \
) { \
	auto numberExpression = std::dynamic_pointer_cast<_ExpressionDatatype>(expression); \
\
	return std::make_shared<_ValueDatatype>( \
		convertfn(numberExpression->GetValue()) \
	); \
}

namespace dim {
	namespace interpreter {

		typedef std::function<
			Result<std::shared_ptr<Value>> (
				std::shared_ptr<parser::Expression>,
				std::shared_ptr<RegisterManager>
			)
		> EvaluateFunction;

		extern FunctionRegisterManager functionRegisterManager;

		Result<std::shared_ptr<Value>> Resultify(
			std::expected<std::shared_ptr<Value>, std::string> value,
			std::shared_ptr<parser::Expression> expression,
			utils::ErrorType type = utils::ErrorType::ERROR
		);

		Result<std::shared_ptr<Value>> EvaluateScopeExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateIdentifierExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateDiscardExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateNullExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateBooleanExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateI8Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateI16Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateI32Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateI64Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateU8Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateU16Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateU32Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateU64Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateF32Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateF64Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateF128Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateCharExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateStringExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		Result<std::shared_ptr<Value>> EvaluateBreakExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		Result<std::shared_ptr<Value>> EvaluateReturnExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		Result<std::shared_ptr<Value>> EvaluateOrExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		Result<std::shared_ptr<Value>> EvaluateUnaryExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		Result<std::shared_ptr<Value>> EvaluateBinaryExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		Result<std::shared_ptr<Value>> EvaluateIfElseStructure(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		Result<std::shared_ptr<Value>> EvaluateMatchStructure(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		Result<std::shared_ptr<Value>> EvaluateLoopExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateWhileLoopExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		Result<std::shared_ptr<Value>> EvaluateForLoopExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateAssignationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateDeclarationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateFunctionDeclarationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateFunctionCallExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateStructDeclarationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateInterfaceDeclarationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateStructExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateStructMemberAccessExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateStructImplementationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateStructMemberFunctionAccessExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		Result<std::shared_ptr<Value>> EvaluateExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<
			std::shared_ptr<Value>,
			std::string
		> EvaluateProgram(
			std::shared_ptr<parser::ScopeExpression> program
		);

		const std::unordered_map<parser::NodeType, const EvaluateFunction> EvaluateFunctionsMap = {
			{ parser::NodeType::SCOPE,				&EvaluateScopeExpression						},
			{ parser::NodeType::IDENTIFIER,			&EvaluateIdentifierExpression					},
			{ parser::NodeType::DISCARD,			&EvaluateDiscardExpression						},
			{ parser::NodeType::NUL,				&EvaluateNullExpression							},
			{ parser::NodeType::BOOLEAN,			&EvaluateBooleanExpression						},
			{ parser::NodeType::I8,					&EvaluateI8Expression							},
			{ parser::NodeType::I16,				&EvaluateI16Expression							},
			{ parser::NodeType::I32,				&EvaluateI32Expression							},
			{ parser::NodeType::I64,				&EvaluateI64Expression							},
			{ parser::NodeType::U8,					&EvaluateU8Expression							},
			{ parser::NodeType::U16,				&EvaluateU16Expression							},
			{ parser::NodeType::U32,				&EvaluateU32Expression							},
			{ parser::NodeType::U64,				&EvaluateU64Expression							},
			{ parser::NodeType::F32,				&EvaluateF32Expression							},
			{ parser::NodeType::F64,				&EvaluateF64Expression							},
			{ parser::NodeType::F128,				&EvaluateF128Expression							},
			{ parser::NodeType::CHAR,			    &EvaluateCharExpression							},
			{ parser::NodeType::STRING,			    &EvaluateStringExpression						},
			{ parser::NodeType::BREAK,			    &EvaluateBreakExpression						},
			{ parser::NodeType::RETURN,			    &EvaluateReturnExpression						},
			{ parser::NodeType::OR,				    &EvaluateOrExpression							},
			{ parser::NodeType::UNARY,			    &EvaluateUnaryExpression						},
			{ parser::NodeType::BINARY,			    &EvaluateBinaryExpression						},
			{ parser::NodeType::IFELSE_STRUCT,		&EvaluateIfElseStructure						},
			{ parser::NodeType::MATCH_STRUCT,	    &EvaluateMatchStructure							},
			{ parser::NodeType::LOOP,			    &EvaluateLoopExpression							},
			{ parser::NodeType::WHILE,			    &EvaluateWhileLoopExpression					},
			{ parser::NodeType::FOR,			    &EvaluateForLoopExpression						},
			{ parser::NodeType::ASSIGN,			    &EvaluateAssignationExpression					},
			{ parser::NodeType::DECL,			    &EvaluateDeclarationExpression					},
			{ parser::NodeType::FN,				    &EvaluateFunctionDeclarationExpression			},
			{ parser::NodeType::FN_CALL,		    &EvaluateFunctionCallExpression					},
			{ parser::NodeType::INTERFACE,			&EvaluateInterfaceDeclarationExpression			},
			{ parser::NodeType::STRUCT_DECL,	    &EvaluateStructDeclarationExpression			},
			{ parser::NodeType::STRUCT,			    &EvaluateStructExpression						},
			{ parser::NodeType::STRUCT_ACCESS,		&EvaluateStructMemberAccessExpression			},
			{ parser::NodeType::STRUCT_IMPL,	    &EvaluateStructImplementationExpression			},
			{ parser::NodeType::STRUCT_ACCESS_FN,	&EvaluateStructMemberFunctionAccessExpression	},
		};
	}
}
