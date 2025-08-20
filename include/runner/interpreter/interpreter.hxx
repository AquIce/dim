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

#define __TRY_VALUE_FUNC_WRETERR(func, expression) \
__TRY_EXPECTED_FUNC_WRETERR( \
	func, \
	std::shared_ptr<Value>, \
	std::string, \
	expression \
)
#define __TRY_VALUE_FUNC_WRETERR_WSAVE(func, expression, value) \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE( \
	func, \
	std::shared_ptr<Value>, \
	std::string, \
	value, \
	expression \
)

namespace dim {
	namespace interpreter {

		extern RegisterManager registerManager;

		typedef std::function<
			std::expected<std::shared_ptr<Value>, std::string> (
				std::shared_ptr<parser::Expression>
			)
		> EvaluateFunction;

		std::expected<std::shared_ptr<Value>, std::string> EvaluateScopeExpression(
			std::shared_ptr<parser::Expression> expression
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateIdentifierExpression(
			std::shared_ptr<parser::Expression> expression
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateNullExpression(
			std::shared_ptr<parser::Expression> expression
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateBooleanExpression(
			std::shared_ptr<parser::Expression> expression
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateNumberExpression(
			std::shared_ptr<parser::Expression> expression
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateStringExpression(
			std::shared_ptr<parser::Expression> expression
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateBreakExpression(
			std::shared_ptr<parser::Expression> expression
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateOrExpression(
			std::shared_ptr<parser::Expression> expression
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateBinaryExpression(
			std::shared_ptr<parser::Expression> expression
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateIfElseStructure(
			std::shared_ptr<parser::Expression> expression
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateLoopExpression(
			std::shared_ptr<parser::Expression> expression
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateWhileLoopExpression(
			std::shared_ptr<parser::Expression> expression
		);
		
		std::expected<std::shared_ptr<Value>, std::string> EvaluateForLoopExpression(
			std::shared_ptr<parser::Expression> expression
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateAssignationExpression(
			std::shared_ptr<parser::Expression> expression
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateDeclarationExpression(
			std::shared_ptr<parser::Expression> expression
		);

		std::expected<std::shared_ptr<Value>, std::string> EvaluateExpression(
			std::shared_ptr<parser::Expression> expression
		);

		const std::unordered_map<parser::NodeType, const EvaluateFunction> EvaluateFunctionsMap = {
			{ parser::NodeType::SCOPE, &EvaluateScopeExpression },
			{ parser::NodeType::IDENTIFIER, &EvaluateIdentifierExpression },
			{ parser::NodeType::NUL, &EvaluateNullExpression },
			{ parser::NodeType::BOOLEAN, &EvaluateBooleanExpression },
			{ parser::NodeType::NUMBER, &EvaluateNumberExpression },
			{ parser::NodeType::STRING, &EvaluateStringExpression },
			{ parser::NodeType::BREAK, &EvaluateBreakExpression },
			{ parser::NodeType::OR, &EvaluateOrExpression },
			{ parser::NodeType::BINARY, &EvaluateBinaryExpression },
			{ parser::NodeType::IFELSE_STRUCT, &EvaluateIfElseStructure },
			{ parser::NodeType::LOOP, &EvaluateLoopExpression },
			{ parser::NodeType::WHILE, &EvaluateWhileLoopExpression },
			{ parser::NodeType::FOR, &EvaluateForLoopExpression },
			{ parser::NodeType::ASSIGN, &EvaluateAssignationExpression },
			{ parser::NodeType::DECL, &EvaluateDeclarationExpression },
		};

	}
}
