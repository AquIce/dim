#pragma once

#include <parser/ast.hxx>
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

namespace std {
	template<>
	struct hash<
		std::pair<
			dim::parser::Datatype,
			dim::parser::Datatype
		>
	> {
		std::size_t operator()(
			const std::pair<dim::parser::Datatype, dim::parser::Datatype>& p
		) const {
			std::size_t h1 = std::hash<dim::parser::Datatype>{}(p.first);
			std::size_t h2 = std::hash<dim::parser::Datatype>{}(p.second);
			return h1 ^ (h2 << 1);
		}
	};
}


namespace dim {
	namespace parser {

		typedef utils::Iterator<Datatype, Datatype::I8, Datatype::STRING> DatatypeIterator;

		const auto ConversionTable = std::array<uint16_t, 15>({
			0b0111111111111111, // INFER
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
			0b0010000000000001, // CHAR
			0b0100000000000001, // STRING
		});

		const auto UnaryOutputType = std::unordered_map<std::string_view, std::unordered_map<Datatype, Datatype>>({
			{ std::string_view("!"), {
				{ Datatype::I8,			 Datatype::BOOLEAN },
				{ Datatype::I16,			Datatype::BOOLEAN },
				{ Datatype::I32,			Datatype::BOOLEAN },
				{ Datatype::I64,			Datatype::BOOLEAN },
				{ Datatype::U8,			 Datatype::BOOLEAN },
				{ Datatype::U16,			Datatype::BOOLEAN },
				{ Datatype::U32,			Datatype::BOOLEAN },
				{ Datatype::U64,			Datatype::BOOLEAN },
				{ Datatype::F32,			Datatype::BOOLEAN },
				{ Datatype::F64,			Datatype::BOOLEAN },
				{ Datatype::F128,		 Datatype::BOOLEAN },
				{ Datatype::BOOLEAN,	Datatype::BOOLEAN },
				{ Datatype::CHAR,		 Datatype::BOOLEAN },
				{ Datatype::STRING,	 Datatype::BOOLEAN },
			}},
			{ std::string_view("~"), {
				{ Datatype::I8,			 Datatype::I8 },
				{ Datatype::I16,			Datatype::I16 },
				{ Datatype::I32,			Datatype::I32 },
				{ Datatype::I64,			Datatype::I64 },
				{ Datatype::U8,			 Datatype::U8 },
				{ Datatype::U16,			Datatype::U16 },
				{ Datatype::U32,			Datatype::U32 },
				{ Datatype::U64,			Datatype::U64 },
				{ Datatype::BOOLEAN,	Datatype::BOOLEAN },
			}},
			{ std::string_view("++"), {
				{ Datatype::I8,			 Datatype::I8 },
				{ Datatype::I16,			Datatype::I16 },
				{ Datatype::I32,			Datatype::I32 },
				{ Datatype::I64,			Datatype::I64 },
				{ Datatype::U8,			 Datatype::U8 },
				{ Datatype::U16,			Datatype::U16 },
				{ Datatype::U32,			Datatype::U32 },
				{ Datatype::U64,			Datatype::U64 },
				{ Datatype::F32,			Datatype::F32 },
				{ Datatype::F64,			Datatype::F64 },
				{ Datatype::F128,		 Datatype::F128 },
			}},
			{ std::string_view("--"), {
				{ Datatype::I8,			 Datatype::I8 },
				{ Datatype::I16,			Datatype::I16 },
				{ Datatype::I32,			Datatype::I32 },
				{ Datatype::I64,			Datatype::I64 },
				{ Datatype::U8,			 Datatype::U8 },
				{ Datatype::U16,			Datatype::U16 },
				{ Datatype::U32,			Datatype::U32 },
				{ Datatype::U64,			Datatype::U64 },
				{ Datatype::F32,			Datatype::F32 },
				{ Datatype::F64,			Datatype::F64 },
				{ Datatype::F128,		 Datatype::F128 },
			}},
		});

		std::expected<
			Datatype,
			std::string
		> GetUnaryOutputDatatype(
			std::string operatorSymbol,
			Datatype termType
		);

		const auto BinaryOutputType = std::unordered_map<std::string_view, std::unordered_map<std::pair<Datatype, Datatype>, Datatype>>({
			{ std::string_view("+"), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::I8 },
				{ { Datatype::I8, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I8, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I8, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::I8, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::I8, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I8, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I8, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I8, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I8, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::I16, Datatype::I8 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I16, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I16, Datatype::U8 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::I16, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I16, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I16, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I16, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I16, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I16 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I32, Datatype::U8 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::U16 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I32, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I16 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I32 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U8 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U16 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U32 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I64, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I64, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::U8, Datatype::I8 }, Datatype::U8 },
				{ { Datatype::U8, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::U8, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::U8, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::U8, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U8, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U8, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U8, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U8, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U8, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::U16, Datatype::I8 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::I16 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::U16, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U16, Datatype::U8 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U16, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U16, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U16, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U16, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I16 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I32 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U32, Datatype::U8 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U16 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I16 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I32 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I64 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U8 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U16 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U32 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U64, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I16 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I64 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U8 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U16 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U64 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::F32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I16 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U8 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U16 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I16 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U8 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U16 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F128 }, Datatype::F128 },

				{ {Datatype::CHAR, Datatype::CHAR }, Datatype::STRING },
				{ {Datatype::CHAR, Datatype::STRING }, Datatype::STRING },

				{ {Datatype::STRING, Datatype::CHAR }, Datatype::STRING },
				{ {Datatype::STRING, Datatype::STRING }, Datatype::STRING },
			}},
			{ std::string_view("-"), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::I8 },
				{ { Datatype::I8, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I8, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I8, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::I8, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::I8, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I8, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I8, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I8, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I8, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::I16, Datatype::I8 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I16, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I16, Datatype::U8 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::I16, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I16, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I16, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I16, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I16, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I16 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I32, Datatype::U8 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::U16 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I32, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I16 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I32 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U8 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U16 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U32 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I64, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I64, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::U8, Datatype::I8 }, Datatype::U8 },
				{ { Datatype::U8, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::U8, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::U8, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::U8, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U8, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U8, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U8, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U8, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U8, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::U16, Datatype::I8 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::I16 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::U16, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U16, Datatype::U8 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U16, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U16, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U16, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U16, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I16 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I32 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U32, Datatype::U8 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U16 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I16 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I32 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I64 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U8 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U16 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U32 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U64, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I16 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I64 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U8 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U16 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U64 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::F32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I16 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U8 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U16 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I16 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U8 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U16 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F128 }, Datatype::F128 },
			}},
			{ std::string_view("*"), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::I8 },
				{ { Datatype::I8, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I8, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I8, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::I8, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::I8, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I8, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I8, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I8, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I8, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::I16, Datatype::I8 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I16, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I16, Datatype::U8 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::I16, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I16, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I16, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I16, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I16, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I16 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I32, Datatype::U8 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::U16 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I32, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I16 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I32 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U8 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U16 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U32 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I64, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I64, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::U8, Datatype::I8 }, Datatype::U8 },
				{ { Datatype::U8, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::U8, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::U8, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::U8, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U8, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U8, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U8, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U8, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U8, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::U16, Datatype::I8 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::I16 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::U16, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U16, Datatype::U8 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U16, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U16, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U16, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U16, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I16 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I32 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U32, Datatype::U8 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U16 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I16 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I32 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I64 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U8 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U16 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U32 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U64, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I16 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I64 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U8 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U16 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U64 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::F32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I16 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U8 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U16 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I16 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U8 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U16 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F128 }, Datatype::F128 },
			}},
			{ std::string_view("/"), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::I8 },
				{ { Datatype::I8, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I8, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I8, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::I8, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::I8, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I8, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I8, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I8, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I8, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::I16, Datatype::I8 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I16, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I16, Datatype::U8 }, Datatype::I16 },
				{ { Datatype::I16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::I16, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I16, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I16, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I16, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I16, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I16 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I32, Datatype::U8 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::U16 }, Datatype::I32 },
				{ { Datatype::I32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::I32, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I16 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I32 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U8 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U16 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U32 }, Datatype::I64 },
				{ { Datatype::I64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::I64, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::I64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::I64, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::U8, Datatype::I8 }, Datatype::U8 },
				{ { Datatype::U8, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::U8, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::U8, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::U8, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U8, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U8, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U8, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U8, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U8, Datatype::F128 }, Datatype::F128 },

				{ { Datatype::U16, Datatype::I8 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::I16 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::U16, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U16, Datatype::U8 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U16, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U16, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U16, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U16, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U16, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I16 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I32 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U32, Datatype::U8 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U16 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U32, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I16 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I32 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::I64 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U8 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U16 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U32 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::U64, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::U64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::U64, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I16 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::I64 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U8 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U16 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::U64 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::F32 }, Datatype::F32 },
				{ { Datatype::F32, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::F32, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I16 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::I64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U8 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U16 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::U64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F32 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F64 }, Datatype::F64 },
				{ { Datatype::F64, Datatype::F128 }, Datatype::F128 },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I16 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::I64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U8 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U16 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::U64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F32 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F64 }, Datatype::F128 },
				{ { Datatype::F128, Datatype::F128 }, Datatype::F128 },
			}},
			{ std::string_view("&&"), {
				{ { Datatype::BOOLEAN, Datatype::BOOLEAN }, Datatype::BOOLEAN },
			}},
			{ std::string_view("||"), {
				{ { Datatype::BOOLEAN, Datatype::BOOLEAN }, Datatype::BOOLEAN },
			}},
			{ std::string_view("&&"), {
				{ { Datatype::BOOLEAN, Datatype::BOOLEAN }, Datatype::BOOLEAN },
			}},
			{ std::string_view("<"), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::I16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F128 }, Datatype::BOOLEAN },
			}},
			{ std::string_view(">"), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::I16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F128 }, Datatype::BOOLEAN },
			}},
			{ std::string_view("<="), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::I16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F128 }, Datatype::BOOLEAN },
			}},
			{ std::string_view(">="), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::I16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F128 }, Datatype::BOOLEAN },
			}},
			{ std::string_view("=="), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::I16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::BOOLEAN, Datatype::BOOLEAN }, Datatype::BOOLEAN },

				{ { Datatype::CHAR, Datatype::CHAR }, Datatype::CHAR },

				{ { Datatype::STRING, Datatype::STRING }, Datatype::STRING },
			}},
			{ std::string_view("!="), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::I16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::I64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::I64, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U8, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U8, Datatype::F128 }, Datatype::BOOLEAN },

				{ { Datatype::U16, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U16, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::U64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::U64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F32, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F32, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F64, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F64, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::F128, Datatype::I8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::I64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U8 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U16 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::U64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F32 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F64 }, Datatype::BOOLEAN },
				{ { Datatype::F128, Datatype::F128 }, Datatype::BOOLEAN },
				
				{ { Datatype::BOOLEAN, Datatype::BOOLEAN }, Datatype::BOOLEAN },

				{ { Datatype::CHAR, Datatype::CHAR }, Datatype::CHAR },

				{ { Datatype::STRING, Datatype::STRING }, Datatype::STRING },
			}},
			{ std::string_view("&"), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::I8 },
				{ { Datatype::I16, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I32, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I64, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::U16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::BOOLEAN, Datatype::BOOLEAN }, Datatype::BOOLEAN },
			}},
			{ std::string_view("|"), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::I8 },
				{ { Datatype::I16, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I32, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I64, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::U16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::BOOLEAN, Datatype::BOOLEAN }, Datatype::BOOLEAN },
			}},
			{ std::string_view("^"), {
				{ { Datatype::I8, Datatype::I8 }, Datatype::I8 },
				{ { Datatype::I16, Datatype::I16 }, Datatype::I16 },
				{ { Datatype::I32, Datatype::I32 }, Datatype::I32 },
				{ { Datatype::I64, Datatype::I64 }, Datatype::I64 },
				{ { Datatype::U8, Datatype::U8 }, Datatype::U8 },
				{ { Datatype::U16, Datatype::U16 }, Datatype::U16 },
				{ { Datatype::U32, Datatype::U32 }, Datatype::U32 },
				{ { Datatype::U64, Datatype::U64 }, Datatype::U64 },
				{ { Datatype::BOOLEAN, Datatype::BOOLEAN }, Datatype::BOOLEAN },
			}},
		});

		std::expected<
			Datatype,
			std::string
		> GetBinaryOutputDatatype(
			Datatype leftType,
			std::string operatorSymbol,
			Datatype rightType
		);

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
