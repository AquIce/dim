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

		ValueFlag Value::GetFlag() {
			return m_flag;
		}
		void Value::SetFlag(
			ValueFlag flag
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

		std::expected<
			std::shared_ptr<Value>,
			std::string
		> Value::operator!() {
			return std::make_shared<BooleanValue>(
				!this->IsTrue()
			);
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


		NumberValue::NumberValue(
			double value
		):
			Value(),
			m_value(value)
		{}

		double NumberValue::GetValue() {
			return m_value;
		}
		void NumberValue::SetValue(
			double value
		) {
			m_value = value;
		}

		bool NumberValue::IsTrue() {
			return m_value != 0;
		}

		std::string NumberValue::Repr() {
			return std::to_string(m_value);
		}
		ValueType NumberValue::Type() {
			return ValueType::NUMBER;
		}
		
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, NumberValue, ValueType::NUMBER, +)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, NumberValue, ValueType::NUMBER, -)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, NumberValue, ValueType::NUMBER, *)
		
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> NumberValue::operator/(
			std::shared_ptr<Value> other
		) {
			switch(other->Type()) {
			case ValueType::NUMBER: {
				double value = std::dynamic_pointer_cast<NumberValue>(other)->GetValue();
				if(value == 0) {
					return std::unexpected("Cannot divide by 0");
				}
				return std::make_shared<NumberValue>(
					this->GetValue() / value
				);
			}
			default:
				return std::unexpected(
					std::string("Cannot use '/' operator on number value and ")
					+ std::string(ValueTypeStr.at(int(other->Type())))
				);
			}
		}

		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, BooleanValue, ValueType::NUMBER, <)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, BooleanValue, ValueType::NUMBER, >)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, BooleanValue, ValueType::NUMBER, <=)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, BooleanValue, ValueType::NUMBER, >=)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, BooleanValue, ValueType::NUMBER, &&)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, BooleanValue, ValueType::NUMBER, ||)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, BooleanValue, ValueType::NUMBER, ==)
		__GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(NumberValue, BooleanValue, ValueType::NUMBER, !=)

		// Can be re-enabled when distinction between double and int is made
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NumberValue, &, number) 
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NumberValue, |, number)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(NumberValue, ^, number)



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
		
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, &, boolean)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, |, boolean)
		__GEN__OPERATOR_VALUE_BODY_OVERRRIDE_ERR(BooleanValue, ^, boolean)
		

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
	}
}
