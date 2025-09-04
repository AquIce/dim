#include <runner/interpreter/value.hxx>

namespace dim {
	namespace interpreter {

		bool Value::IsTrue() {
			return false;
		}

		std::string Value::Repr() {
			return "INVALID";
		}
		ValueType Value::Type() {
			return ValueType::NONE;
		}

		ValueFlagWVal Value::GetFlag() {
			return m_flag;
		}
		void Value::SetFlag(
			ValueFlagWVal flag
		) {
			m_flag = flag;
		}

 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(+)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(-)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(*)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(/)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(<)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(>)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(<=)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(>=)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(&&)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(||)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(==)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(!=)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(&)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(|)
 		__GEN__OPERATOR_VALUE_BODY_VIRTUAL(^)
 		__GEN__UNARY_OPERATOR_VALUE_BODY_VIRTUAL(~)

		std::expected<
			std::shared_ptr<Value>,
			std::string
		> Value::operator!() {
			return std::make_shared<BooleanValue>(
				!this->IsTrue()
			);
		}


		std::shared_ptr<Value> AutoCastINumber(
			f128 number
		) {
			if(number < std::numeric_limits<i8>::max()) {
				return std::make_shared<I8Value>(
					(i8)number
				);
			}
			if(number < std::numeric_limits<i16>::max()) {
				return std::make_shared<I16Value>(
					(i16)number
				);
			}
			if(number < std::numeric_limits<i32>::max()) {
				return std::make_shared<I32Value>(
					(i32)number
				);
			}
			if(number < std::numeric_limits<i64>::max()) {
				return std::make_shared<I64Value>(
					(i64)number
				);
			}
			return AutoCastFNumber(number);
		}

		std::shared_ptr<Value> AutoCastUNumber(
			f128 number
		) {
			if(number < std::numeric_limits<u8>::max()) {
				return std::make_shared<U8Value>(
					(u8)number
				);
			}
			if(number < std::numeric_limits<u16>::max()) {
				return std::make_shared<U16Value>(
					(u16)number
				);
			}
			if(number < std::numeric_limits<u32>::max()) {
				return std::make_shared<U32Value>(
					(u32)number
				);
			}
			if(number < std::numeric_limits<u64>::max()) {
				return std::make_shared<U64Value>(
					(u64)number
				);
			}
			return AutoCastFNumber(number);
		}

		std::shared_ptr<Value> AutoCastFNumber(
			f128 number
		) {
			if(number < std::numeric_limits<f32>::max()) {
				return std::make_shared<F32Value>(
					(f32)number
				);
			}
			if(number < std::numeric_limits<f64>::max()) {
				return std::make_shared<F64Value>(
					(f64)number
				);
			}
			return std::make_shared<F128Value>(number);
		}

		std::shared_ptr<Value> AutoCastNumber(
			f128 number
		) {
			if(!utils::f128isint(number)) {
				return AutoCastFNumber(number);
			}
			if(number > 0) {
				return AutoCastUNumber(number);
			}
			return AutoCastINumber(number);
		}


		NullValue::NullValue() :
			Value()
		{}

		bool NullValue::IsTrue() {
			return false;
		}

		std::string NullValue::Repr() {
			return "null";
		}
		ValueType NullValue::Type() {
			return ValueType::NUL;
		}

		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, +, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, -, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, *, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, /, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, <, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, >, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, <=, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, >=, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, &&, null) 
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, ||, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, ==, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, !=, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, &, null) 
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, |, null)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, ^, null)
		__GEN__UNARY_OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NullValue, ~, null)


		__GEN__NUMBER_IU_CLASS_IMPL(I8Value, i8, I8)
		__GEN__NUMBER_IU_CLASS_IMPL(I16Value, i16, I16)
		__GEN__NUMBER_IU_CLASS_IMPL(I32Value, i32, I32)
		__GEN__NUMBER_IU_CLASS_IMPL(I64Value, i64, I64)

		__GEN__NUMBER_IU_CLASS_IMPL(U8Value, u8, U8)
		__GEN__NUMBER_IU_CLASS_IMPL(U16Value, u16, U16)
		__GEN__NUMBER_IU_CLASS_IMPL(U32Value, u32, U32)
		__GEN__NUMBER_IU_CLASS_IMPL(U64Value, u64, U64)

		__GEN__NUMBER_F_CLASS_IMPL(F32Value, f32, F32, std::to_string)
		__GEN__NUMBER_F_CLASS_IMPL(F64Value, f64, F64, std::to_string)
		__GEN__NUMBER_F_CLASS_IMPL(F128Value, f128, F128, utils::f128tos)


		BooleanValue::BooleanValue(
			bool value
		):
			Value(),
			m_value(value)
		{}

		bool BooleanValue::GetValue() {
			return m_value;
		}
		void BooleanValue::SetValue(
			bool value
		) {
			m_value = value;
		}

		bool BooleanValue::IsTrue() {
			return m_value;
		}

		std::string BooleanValue::Repr() {
			return m_value ? "true" : "false";
		}
		ValueType BooleanValue::Type() {
			return ValueType::BOOLEAN;
		}

		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, +, boolean)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, -, boolean)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, *, boolean)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, /, boolean)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, <, boolean)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, >, boolean)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, <=, boolean)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, >=, boolean)

		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(BooleanValue, BooleanValue, ValueType::BOOLEAN, &&)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(BooleanValue, BooleanValue, ValueType::BOOLEAN, ||)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(BooleanValue, BooleanValue, ValueType::BOOLEAN, ==)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(BooleanValue, BooleanValue, ValueType::BOOLEAN, !=)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_BITWISE(BooleanValue, &)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_BITWISE(BooleanValue, |)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_ADVANCED_BITWISE(BooleanValue, ^)
		
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> BooleanValue::operator~() {
			return !(*this);
		}
		

		StringValue::StringValue(
			std::string value
		):
			Value(),
			m_value(value)
		{}

		std::string StringValue::GetValue() {
			return m_value;
		}
		void StringValue::SetValue(
			std::string value
		) {
			m_value = value;
		}

		bool StringValue::IsTrue() {
			return m_value.size() > 0;
		}

		std::string StringValue::Repr() {
			return "\"" + m_value + "\"";
		}
		ValueType StringValue::Type() {
			return ValueType::STRING;
		}

		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(StringValue, StringValue, ValueType::STRING, +)
		
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, -, string)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, *, string)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, /, string)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, <, string)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, >, string)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, <=, string)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, >=, string)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, &&, string) 
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, ||, string)

		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(StringValue, BooleanValue, ValueType::STRING, ==)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(StringValue, BooleanValue, ValueType::STRING, !=)

		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, &, string)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, |, string) 
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, ^, string)
		__GEN__UNARY_OPERATOR_VALUE_BODY_OVERRRIDE_ERR(StringValue, ~, string)
	}
}
