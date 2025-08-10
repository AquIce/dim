#include <runner/interpreter/value.hxx>

namespace dim {
	namespace interpreter {

		std::string Value::Repr() {
			return "INVALID";
		}
		ValueType Value::Type() {
			return ValueType::NONE;
		}

 		std::expected<
			std::shared_ptr<Value>,
			std::string
		> Value::operator+(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Using Value class on its own");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> Value::operator-(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Using Value class on its own");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> Value::operator*(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Using Value class on its own");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> Value::operator/(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Using Value class on its own");
		}


		NullValue::NullValue() :
			Value()
		{}

		std::string NullValue::Repr() {
			return "null";
		}
		ValueType NullValue::Type() {
			return ValueType::NUL;
		}

		std::expected<
			std::shared_ptr<Value>,
			std::string
		> NullValue::operator+(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '+' operator on null value");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> NullValue::operator-(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '-' operator on null value");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> NullValue::operator*(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '+' operator on null value");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> NullValue::operator/(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '+' operator on null value");
		} 



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

		std::string NumberValue::Repr() {
			return std::to_string(m_value);
		}
		ValueType NumberValue::Type() {
			return ValueType::NUMBER;
		}

		std::expected<
			std::shared_ptr<Value>,
			std::string
		> NumberValue::operator+(
			std::shared_ptr<Value> other
		) {
			switch(other->Type()) {
			case ValueType::NUMBER:
				return std::make_shared<NumberValue>(
					this->GetValue()
					+ std::dynamic_pointer_cast<NumberValue>(other)->GetValue()
				);
			default:
				return std::unexpected(
					std::string("Cannot use '+' operator on number value and ")
					+ std::string(ValueTypeStr.at(int(other->Type())))
				);
			}
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> NumberValue::operator-(
			std::shared_ptr<Value> other
		) {
			switch(other->Type()) {
			case ValueType::NUMBER:
				return std::make_shared<NumberValue>(
					this->GetValue()
					- std::dynamic_pointer_cast<NumberValue>(other)->GetValue()
				);
			default:
				return std::unexpected(
					std::string("Cannot use '-' operator on number value and ")
					+ std::string(ValueTypeStr.at(int(other->Type())))
				);
			}
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> NumberValue::operator*(
			std::shared_ptr<Value> other
		) {
			switch(other->Type()) {
			case ValueType::NUMBER:
				return std::make_shared<NumberValue>(
					this->GetValue()
					* std::dynamic_pointer_cast<NumberValue>(other)->GetValue()
				);
			default:
				return std::unexpected(
					std::string("Cannot use '*' operator on number value and ")
					+ std::string(ValueTypeStr.at(int(other->Type())))
				);
			}
		}
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

		std::string BooleanValue::Repr() {
			return m_value ? "true" : "false";
		}
		ValueType BooleanValue::Type() {
			return ValueType::BOOLEAN;
		}

		std::expected<
			std::shared_ptr<Value>,
			std::string
		> BooleanValue::operator+(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '+' operator on boolean value");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> BooleanValue::operator-(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '-' operator on boolean value");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> BooleanValue::operator*(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '*' operator on boolean value");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> BooleanValue::operator/(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '/' operator on boolean value");
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

		std::string StringValue::Repr() {
			return "\"" + m_value + "\"";
		}
		ValueType StringValue::Type() {
			return ValueType::STRING;
		}

		std::expected<
			std::shared_ptr<Value>,
			std::string
		> StringValue::operator+(
			std::shared_ptr<Value> other
		) {
			switch(other->Type()) {
			case ValueType::STRING: {
				std::string value = std::dynamic_pointer_cast<StringValue>(other)->GetValue();
				return std::make_shared<StringValue>(
					this->GetValue() + value
				);
			}
			default:
				return std::unexpected(
					std::string("Cannot use '+' operator on string value and ")
					+ std::string(ValueTypeStr.at(int(other->Type())))
				);
			}
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> StringValue::operator-(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '-' operator on string value");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> StringValue::operator*(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '*' operator on string value");
		}
		std::expected<
			std::shared_ptr<Value>,
			std::string
		> StringValue::operator/(
			std::shared_ptr<Value> other
		) {
			return std::unexpected("Cannot use '/' operator on string value");
		}
	}
}