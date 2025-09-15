#pragma once

#include <parser/types.hxx>

#include <utils/utils.hxx>

#include <array>
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>

#define __GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(operatorSymbol) \
virtual std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> operator operatorSymbol( \
	std::shared_ptr<Value> other \
);

#define __GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> operator operatorSymbol( \
	std::shared_ptr<Value> other \
) override;

#define __GEN__OPERATOR_VALUE_BODY_VIRTUAL(operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> Value::operator operatorSymbol( \
	std::shared_ptr<Value> other \
) { \
	return std::unexpected("Using Value class on its own"); \
}

#define __GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(_ValueClass, operatorSymbol, errorMessageName) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _ValueClass::operator operatorSymbol( \
	std::shared_ptr<Value> other \
) { \
	return std::unexpected(std::string("Cannot use '") + #operatorSymbol + "' operator on " + #errorMessageName + " value"); \
}

#define __GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(_InputClass, _OutputClass, inputClassDatatype, operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _InputClass::operator operatorSymbol( \
	std::shared_ptr<Value> other \
) { \
	if(other->Type() == inputClassDatatype) { \
		return std::make_shared<_OutputClass>( \
			this->GetValue() \
			operatorSymbol std::dynamic_pointer_cast<_InputClass>(other)->GetValue() \
		); \
	}\
	return std::unexpected( \
		std::string("Cannot use '") + std::string(#operatorSymbol) + "' operator on " \
		+ std::string(#_InputClass) + " value and " + other->Type() \
	); \
}

#define __GEN__AUTO_CAST_NUMBER_SWITCH_CASE(otherType, _Class, classDatatype, operatorSymbol) \
if(otherType == classDatatype) { \
	return AutoCastNumber( \
		m_value operatorSymbol std::dynamic_pointer_cast<_Class>(other)->GetValue() \
	); \
}

#define __GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(otherType, _InputClass, _OutputClass, classDatatype, operatorSymbol) \
if(otherType == classDatatype) { \
	return std::make_shared<_OutputClass>(m_value operatorSymbol std::dynamic_pointer_cast<_InputClass>(other)->GetValue()); \
}

#define __GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED(_InputClass, operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _InputClass::operator operatorSymbol( \
	std::shared_ptr<Value> other \
) { \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), I8Value, "I8", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), I16Value, "I16", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), I32Value, "I32", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), I64Value, "I64", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), U8Value, "U8", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), U16Value, "U16", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), U32Value, "U32", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), U64Value, "U64", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), F32Value, "F32", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), F64Value, "F64", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), F128Value, "F128", operatorSymbol) \
	return std::unexpected( \
		std::string("Cannot use '") + std::string(#operatorSymbol) + "' operator on " \
		+ std::string(#_InputClass) + " and " + other->Type() \
	); \
}

#define __GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_BITWISE(_InputClass, operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _InputClass::operator operatorSymbol( \
	std::shared_ptr<Value> other \
) { \
	if(other->Type() != this->Type()) { \
		return std::unexpected( \
			std::string("Cannot use '") + std::string(#operatorSymbol) + "' operator on " \
			+ std::string(#_InputClass) + " and " + other->Type() \
		); \
	} \
	return std::make_shared<_InputClass>( \
		m_value operatorSymbol std::dynamic_pointer_cast<_InputClass>(other)->GetValue() \
	); \
}

#define __GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_DIV(_InputClass) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _InputClass::operator/( \
	std::shared_ptr<Value> other \
) { \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), I8Value, "I8", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), I16Value, "I16", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), I32Value, "I32", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), I64Value, "I64", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), U8Value, "U8", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), U16Value, "U16", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), U32Value, "U32", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), U64Value, "U64", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), F32Value, "F32", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), F64Value, "F64", /) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE(other->Type(), F128Value, "F128", /) \
	return std::unexpected( \
		std::string("Cannot use '/' operator on ") \
		+ std::string(#_InputClass) + " and " + other->Type() \
	); \
}

#define __GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_InputClass, _OutputClass, operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _InputClass::operator operatorSymbol( \
	std::shared_ptr<Value> other \
) { \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), I8Value, _OutputClass, "I8", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), I16Value, _OutputClass, "I16", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), I32Value, _OutputClass, "I32", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), I64Value, _OutputClass, "I64", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), U8Value, _OutputClass, "U8", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), U16Value, _OutputClass, "U16", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), U32Value, _OutputClass, "U32", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), U64Value, _OutputClass, "U64", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), F32Value, _OutputClass, "F32", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), F64Value, _OutputClass, "F64", operatorSymbol) \
	__GEN__AUTO_CAST_NUMBER_SWITCH_CASE_NON_NUMBER(other->Type(), F128Value, _OutputClass, "F128", operatorSymbol) \
	return std::unexpected( \
		std::string("Cannot use '") + std::string(#operatorSymbol) + "' operator on " \
		+ std::string(#_InputClass) + " and " + other->Type() \
	); \
}

#define __GEN__NUMBER_CLASS_DECL(_Class, _Datatype) \
class _Class : public Value { \
public: \
\
	_Class( \
		_Datatype value \
	); \
\
	_Datatype GetValue(); \
	void SetValue( \
		_Datatype value \
	); \
\
	bool IsTrue() override; \
\
	parser::DatatypeStr Type() override; \
	std::string Repr() override; \
\
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(+) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(-) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(*) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(/) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<=) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>=) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&&) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(||) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(==) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(!=) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(|) \
	__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(^) \
	__GEN__UNARY_OPERATOR_VALUE_PROTOTYPE_OVERRIDE(~) \
\
private: \
	_Datatype m_value; \
};

#define __GEN__NUMBER_IU_CLASS_IMPL(_Class, _Datatype, datatypeStr) \
_Class::_Class( \
	_Datatype value \
): \
	Value(), \
	m_value(value) \
{} \
\
_Datatype _Class::GetValue() { \
	return m_value; \
} \
void _Class::SetValue( \
	_Datatype value \
) { \
	m_value = value; \
} \
\
bool _Class::IsTrue() { \
	return m_value != 0; \
} \
\
std::string _Class::Repr() { \
	return std::to_string(m_value); \
} \
parser::DatatypeStr _Class::Type() { \
	return datatypeStr; \
} \
\
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED(_Class, +) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED(_Class, -) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED(_Class, *) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_DIV(_Class) \
\
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, <) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, >) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, <=) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, >=) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, &&) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, ||) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, ==) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, !=) \
\
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_BITWISE(_Class, &) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_BITWISE(_Class, |) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_BITWISE(_Class, ^) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _Class::operator~() { \
	return std::make_shared<_Class>(~m_value); \
}

#define __GEN__NUMBER_F_CLASS_IMPL(_Class, _Datatype, datatypeStr, to_string_fn) \
_Class::_Class( \
	_Datatype value \
): \
	Value(), \
	m_value(value) \
{} \
\
_Datatype _Class::GetValue() { \
	return m_value; \
} \
void _Class::SetValue( \
	_Datatype value \
) { \
	m_value = value; \
} \
\
bool _Class::IsTrue() { \
	return m_value != 0; \
} \
 \
std::string _Class::Repr() { \
	return to_string_fn(m_value); \
} \
parser::DatatypeStr _Class::Type() { \
	return datatypeStr; \
} \
\
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED(_Class, +) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED(_Class, -) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED(_Class, *) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_DIV(_Class) \
\
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, <) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, >) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, <=) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, >=) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, &&) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, ||) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, ==) \
__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_NON_NUMBER(_Class, BooleanValue, !=) \
\
__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(_Class, &, _Datatype) \
__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(_Class, |, _Datatype) \
__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(_Class, ^, _Datatype) \
__GEN__UNARY_OPERATOR_VALUE_BODY_OVERRRIDE_ERR(_Class, ~, _Datatype)

#define __GEN__UNARY_OPERATOR_VALUE_PROTOTYPE_VIRTUAL(operatorSymbol) \
virtual std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> operator operatorSymbol();

#define __GEN__UNARY_OPERATOR_VALUE_PROTOTYPE_OVERRIDE(operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> operator operatorSymbol() override;

#define __GEN__UNARY_OPERATOR_VALUE_BODY_VIRTUAL(operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> Value::operator operatorSymbol() { \
	return std::unexpected("Using Value class on its own"); \
}

#define __GEN__UNARY_OPERATOR_VALUE_BODY_OVERRRIDE_ERR(_ValueClass, operatorSymbol, errorMessageName) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _ValueClass::operator operatorSymbol() { \
	return std::unexpected(std::string("Cannot use '") + #operatorSymbol + "' operator on " + #errorMessageName + " value"); \
}

#define __GEN__UNARY_OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(_InputClass, _OutputClass, operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _InputClass::operator operatorSymbol() { \
	return std::make_shared<_OutputClass>( \
		operatorSymbol this->GetValue() \
	); \
}

namespace dim {
	namespace interpreter {

		enum class ValueFlag {
			NONE = 0,
			BREAK,
			RETURN
		};

		struct ValueFlagWVal {
			ValueFlag flag;
			std::string breakScopeName;
		};

		class Value {
		public:

			virtual bool IsTrue();

			virtual std::string Repr();
			virtual parser::DatatypeStr Type();

			Value() = default;

			ValueFlagWVal GetFlag();
			void SetFlag(
				ValueFlagWVal flag
			);

			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(+)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(-)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(*)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(/)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(<)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(>)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(<=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(>=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(&&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(||)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(==)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(!=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(|)
			__GEN__OPERATOR_VALUE_PROTOTYPE_VIRTUAL(^)
			__GEN__UNARY_OPERATOR_VALUE_PROTOTYPE_VIRTUAL(~)

			std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator!();

		private:
			ValueFlagWVal m_flag;
		};

		std::shared_ptr<Value> AutoCastINumber(
			f128 number
		);

		std::shared_ptr<Value> AutoCastUNumber(
			f128 number
		);

		std::shared_ptr<Value> AutoCastFNumber(
			f128 number
		);

		std::shared_ptr<Value> AutoCastNumber(
			f128 number
		);

		// TODO: Determine if it should be removed
		class NullValue : public Value {
		public:

			NullValue();

			bool IsTrue() override;

			parser::DatatypeStr Type() override;
			std::string Repr() override;

			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(+)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(-)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(*)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(/)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(||)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(==)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(!=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(|)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(^)
			__GEN__UNARY_OPERATOR_VALUE_PROTOTYPE_OVERRIDE(~)
		};

		__GEN__NUMBER_CLASS_DECL(I8Value, i8)
		__GEN__NUMBER_CLASS_DECL(I16Value, i16)
		__GEN__NUMBER_CLASS_DECL(I32Value, i32)
		__GEN__NUMBER_CLASS_DECL(I64Value, i64)

		__GEN__NUMBER_CLASS_DECL(U8Value, u8)
		__GEN__NUMBER_CLASS_DECL(U16Value, u16)
		__GEN__NUMBER_CLASS_DECL(U32Value, u32)
		__GEN__NUMBER_CLASS_DECL(U64Value, u64)

		__GEN__NUMBER_CLASS_DECL(F32Value, f32)
		__GEN__NUMBER_CLASS_DECL(F64Value, f64)
		__GEN__NUMBER_CLASS_DECL(F128Value, f128)

		class BooleanValue : public Value {
		public:

			BooleanValue(
				bool value
			);

			bool GetValue();
			void SetValue(
				bool value
			);

			bool IsTrue() override;

			parser::DatatypeStr Type() override;
			std::string Repr() override;

			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(+)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(-)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(*)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(/)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(||)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(==)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(!=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(|)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(^)
			__GEN__UNARY_OPERATOR_VALUE_PROTOTYPE_OVERRIDE(~)

		private:
			bool m_value;
		};

		class CharValue : public Value {
		public:

			CharValue(
				char value
			);

			char GetValue();
			void SetValue(
				char value
			);

			bool IsTrue() override;

			parser::DatatypeStr Type() override;
			std::string Repr() override;

			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(+)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(-)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(*)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(/)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(||)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(==)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(!=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(|)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(^)
			__GEN__UNARY_OPERATOR_VALUE_PROTOTYPE_OVERRIDE(~)

		private:
			char m_value;
		};

		class StringValue : public Value {
		public:

			StringValue(
				std::string value
			);

			std::string GetValue();
			void SetValue(
				std::string value
			);

			bool IsTrue() override;

			parser::DatatypeStr Type() override;
			std::string Repr() override;

			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(+)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(-)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(*)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(/)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(||)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(==)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(!=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(|)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(^)
			__GEN__UNARY_OPERATOR_VALUE_PROTOTYPE_OVERRIDE(~)

		private:
			std::string m_value;
		};

		class StructValue : public Value {
		public:
			StructValue(
				parser::DatatypeStr name,
				std::unordered_map<std::string, std::shared_ptr<Value>> members
			);

			parser::DatatypeStr GetName();
			std::unordered_map<std::string, std::shared_ptr<Value>> GetValue();
			std::expected<
				std::shared_ptr<Value>,
				std::string
			> GetValue(
				std::string key
			);
			void SetName(
				parser::DatatypeStr name
			);
			void SetValue(
				std::unordered_map<std::string, std::shared_ptr<Value>> members
			);
			std::expected<
				Success,
				std::string
			> SetValue(
				std::string key,
				std::shared_ptr<Value> value
			);

			bool IsTrue() override;
			
			parser::DatatypeStr Type() override;
			std::string Repr() override;

			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(+)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(-)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(*)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(/)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(<=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(>=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(||)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(==)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(!=)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(&)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(|)
			__GEN__OPERATOR_VALUE_PROTOTYPE_OVERRIDE(^)
			__GEN__UNARY_OPERATOR_VALUE_PROTOTYPE_OVERRIDE(~)

		private:
			parser::DatatypeStr m_name;
			std::unordered_map<std::string, std::shared_ptr<Value>> m_members;
		};
	}
}
