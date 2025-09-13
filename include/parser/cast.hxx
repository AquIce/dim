#pragma once

#include <parser/ast.hxx>
#include <parser/types.hxx>
#include <utils/utils.hxx>

#include <expected>
#include <functional>
#include <limits>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#define __GEN__TRY_CAST_ITYPE(_IType, _RetType) \
std::shared_ptr<NumberExpression> numberExpression; \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE( \
	try_cast_number, \
	std::shared_ptr<NumberExpression>, \
	std::string, \
	numberExpression, \
	expression \
) \
__TRY_EXPECTED_FUNC_WRETERR( \
	try_cast_integer, \
	Success, \
	std::string, \
	numberExpression \
) \
\
try { \
	f128 value = utils::stof128(numberExpression->GetValue()); \
	if( \
		value > std::numeric_limits<_IType>::max() || \
		value < std::numeric_limits<_IType>::min() \
	) { \
		return std::unexpected( \
			"Value '" + numberExpression->GetValue() \
			+ "' out of bounds for type " + #_IType \
		); \
	} \
\
	if(expressions.size() != 2) { \
		return expressionRef; \
	} \
	return std::make_shared<_RetType>(value); \
} catch(...) { \
	return std::unexpected( \
		"Value '" + numberExpression->GetValue() \
		+ "' out of global bounds" \
	); \
}

#define __GEN__TRY_CAST_UTYPE(_UType, _RetType) \
std::shared_ptr<NumberExpression> numberExpression; \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE( \
	try_cast_number, \
	std::shared_ptr<NumberExpression>, \
	std::string, \
	numberExpression, \
	expression \
) \
__TRY_EXPECTED_FUNC_WRETERR( \
	try_cast_integer, \
	Success, \
	std::string, \
	numberExpression \
) \
\
try { \
	f128 value = utils::stof128(numberExpression->GetValue()); \
	if( \
		value > std::numeric_limits<_UType>::max() || \
		value < 0 \
	) { \
		return std::unexpected( \
			"Value '" + numberExpression->GetValue() \
			+ "' out of bounds for type " + #_UType \
		); \
	} \
\
	if(expressions.size() != 2) { \
		return expressionRef; \
	} \
	return std::make_shared<_RetType>(value); \
} catch(...) { \
	return std::unexpected( \
		"Value '" + numberExpression->GetValue() \
		+ "' out of global bounds" \
	); \
}

#define __GEN__TRY_CAST_FTYPE(_FType, _RetType) \
std::shared_ptr<NumberExpression> numberExpression; \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE( \
	try_cast_number, \
	std::shared_ptr<NumberExpression>, \
	std::string, \
	numberExpression, \
	expression \
) \
\
try { \
	f128 value = utils::stof128(numberExpression->GetValue()); \
	if( \
		value > std::numeric_limits<_FType>::max() || \
		value < std::numeric_limits<_FType>::min() \
	) { \
		return std::unexpected( \
			"Value '" + numberExpression->GetValue() \
			+ "' out of bounds for type " + #_FType \
		); \
	} \
\
	if(expressions.size() != 2) { \
		return expressionRef; \
	} \
	return std::make_shared<_RetType>(static_cast<_FType>(value)); \
} catch(...) { \
	return std::unexpected( \
		"Value '" + numberExpression->GetValue() \
		+ "' out of global bounds" \
	); \
}

namespace dim {
	namespace parser {

		std::expected<
			std::shared_ptr<NumberExpression>,
			std::string
		> try_cast_number(
			std::shared_ptr<Expression> expression
		) noexcept;

		std::expected<
			Success,
			std::string
		> try_cast_integer(
			std::shared_ptr<NumberExpression> numberExpression
		) noexcept;

		std::expected<
			std::shared_ptr<BooleanExpression>,
			std::string
		> try_cast_bool(
			std::shared_ptr<Expression> expression
		) noexcept;

		std::expected<
			std::shared_ptr<CharExpression>,
			std::string
		> try_cast_char(
			std::shared_ptr<Expression> expression
		) noexcept;

		std::expected<
			std::shared_ptr<StringExpression>,
			std::string
		> try_cast_str(
			std::shared_ptr<Expression> expression
		) noexcept;

		std::shared_ptr<Expression> get_root_expression(
			std::shared_ptr<Expression> expression
		) noexcept;

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> try_cast(
			std::shared_ptr<Expression> expression,
			DatatypeStr datatype
		) noexcept;

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> auto_cast(
			std::shared_ptr<Expression> expression
		) noexcept;

		bool try_n_cast(
			std::vector<std::shared_ptr<Expression>> expressions
		) noexcept;
	}
}
