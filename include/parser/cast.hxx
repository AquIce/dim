#pragma once

#include <parser/ast.hxx>
#include <utils/utils.hxx>

#include <expected>
#include <limits>
#include <memory>
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

		typedef utils::Iterator<Datatype, Datatype::I8, Datatype::STRING> DatatypeIterator;

		const auto ConversionTable = std::array<uint16_t, 15>({
			0b0011111111111111, // INFER
			0b0000111000011111, // I8
			0b0000111000011101, // I16
			0b0000111000011001, // I32
			0b0000111000010001, // I64
			0b0000111111111101, // U8
			0b0000111111011001, // U16
			0b0000111110010001, // U32
			0b0000111100000001, // U64
			0b0000111000000001, // F32
			0b0000110000000001, // F64
			0b0000100000000001, // F128
			0b0001000000000001, // BOOLEAN
			0b0010000000000001, // STRING
		});

		bool isConvertible(
			Datatype first,
			Datatype second
		) noexcept;

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
			Datatype datatype
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