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
__TRY_EXPECTED_FUNC_WRETERR( \
	func, \
	std::shared_ptr<Value>, \
	std::string, \
	expression, \
	registerManager \
)
#define __TRY_VALUE_FUNC_WRETERR_WSAVE(func, expression, registerManager, value) \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE( \
	func, \
	std::shared_ptr<Value>, \
	std::string, \
	value, \
	expression, \
	registerManager \
)

#define __GEN__BINARY_OPERATOR_TYPE_CASE(valueType, valueTypeEnum) \
case valueTypeEnum: { \
	auto lhsCast = std::dynamic_pointer_cast<valueType>(lhs); \
	if(binaryOperator == "+") { \
		return *lhsCast + rhs; \
	} if(binaryOperator == "-") { \
		return *lhsCast - rhs; \
	} if(binaryOperator == "*") { \
		return *lhsCast * rhs; \
	} if(binaryOperator == "/") { \
		return *lhsCast / rhs; \
	} if(binaryOperator == "<") { \
		return *lhsCast < rhs; \
	} if(binaryOperator == ">") { \
		return *lhsCast > rhs; \
	} if(binaryOperator == "<=") { \
		return *lhsCast <= rhs; \
	} if(binaryOperator == ">=") { \
		return *lhsCast >= rhs; \
	} if(binaryOperator == "&&") { \
		return *lhsCast && rhs; \
	} if(binaryOperator == "||") { \
		return *lhsCast || rhs; \
	} if(binaryOperator == "==") { \
		return *lhsCast == rhs; \
	} if(binaryOperator == "!=") { \
		return *lhsCast != rhs; \
	} if(binaryOperator == "&") { \
		return *lhsCast & rhs; \
	} if(binaryOperator == "|") { \
		return *lhsCast | rhs; \
	} if(binaryOperator == "^") { \
		return *lhsCast ^ rhs; \
	} else { \
		return std::unexpected( \
			std::string("Invalid operator ") \
			+ binaryOperator \
		); \
	} \
}

#define __GEN__EVALUTE_NUMBER_EXPRESSION(name, _ExpressionDatatype, _ValueDatatype, convertfn) \
std::expected<std::shared_ptr<Value>, std::string> name( \
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
			std::expected<std::shared_ptr<Value>, std::string> (
				std::shared_ptr<parser::Expression>,
				std::shared_ptr<RegisterManager>
			)
		> EvaluateFunction;

		std::expected<std::shared_ptr<Value>, std::string> EvaluateScopeExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateIdentifierExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateDiscardExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateNullExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateBooleanExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateI8Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateI16Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateI32Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateI64Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateU8Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateU16Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateU32Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateU64Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateF32Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateF64Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateF128Expression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateStringExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateBreakExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateOrExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateUnaryExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateBinaryExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateIfElseStructure(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateMatchStructure(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateLoopExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateWhileLoopExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateForLoopExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateAssignationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateDeclarationExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateExpression(
			std::shared_ptr<parser::Expression> expression,
			std::shared_ptr<RegisterManager> registerManager
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateProgram(
			std::shared_ptr<parser::ScopeExpression> program
		);

		const std::unordered_map<parser::NodeType, const EvaluateFunction> EvaluateFunctionsMap = {
			{ parser::NodeType::SCOPE, 			&EvaluateScopeExpression },
			{ parser::NodeType::IDENTIFIER, 	&EvaluateIdentifierExpression },
			{ parser::NodeType::DISCARD,	 	&EvaluateDiscardExpression },
			{ parser::NodeType::NUL, 			&EvaluateNullExpression },
			{ parser::NodeType::BOOLEAN, 		&EvaluateBooleanExpression },
			{ parser::NodeType::I8, 			&EvaluateI8Expression },
			{ parser::NodeType::I16, 			&EvaluateI16Expression },
			{ parser::NodeType::I32, 			&EvaluateI32Expression },
			{ parser::NodeType::I64, 			&EvaluateI64Expression },
			{ parser::NodeType::U8, 			&EvaluateU8Expression },
			{ parser::NodeType::U16, 			&EvaluateU16Expression },
			{ parser::NodeType::U32, 			&EvaluateU32Expression },
			{ parser::NodeType::U64, 			&EvaluateU64Expression },
			{ parser::NodeType::F32, 			&EvaluateF32Expression },
			{ parser::NodeType::F64, 			&EvaluateF64Expression },
			{ parser::NodeType::F128, 			&EvaluateF128Expression },
			{ parser::NodeType::STRING, 		&EvaluateStringExpression },
			{ parser::NodeType::BREAK, 			&EvaluateBreakExpression },
			{ parser::NodeType::OR, 			&EvaluateOrExpression },
			{ parser::NodeType::UNARY, 			&EvaluateUnaryExpression },
			{ parser::NodeType::BINARY, 		&EvaluateBinaryExpression },
			{ parser::NodeType::IFELSE_STRUCT,	&EvaluateIfElseStructure },
			{ parser::NodeType::MATCH_STRUCT,	&EvaluateMatchStructure },
			{ parser::NodeType::LOOP, 			&EvaluateLoopExpression },
			{ parser::NodeType::WHILE, 			&EvaluateWhileLoopExpression },
			{ parser::NodeType::FOR, 			&EvaluateForLoopExpression },
			{ parser::NodeType::ASSIGN, 		&EvaluateAssignationExpression },
			{ parser::NodeType::DECL, 			&EvaluateDeclarationExpression },
		};
	}
}
