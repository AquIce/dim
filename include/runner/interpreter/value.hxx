#pragma once

#include <array>
#include <expected>
#include <memory>
#include <string>

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
	return std::unexpected("Cannot use 'operatorSymbol' operator on errorMessageName value"); \
}

#define __GEN__OPERATOR_VALUE_BODY_OVERRIDE_WDISCARD(_InputClass, _OutputClass, inputClassEnum, operatorSymbol) \
std::expected< \
	std::shared_ptr<Value>, \
	std::string \
> _InputClass::operator operatorSymbol( \
	std::shared_ptr<Value> other \
) { \
	switch(other->Type()) { \
	case inputClassEnum: \
		return std::make_shared<_OutputClass>( \
			this->GetValue() \
			operatorSymbol std::dynamic_pointer_cast<_InputClass>(other)->GetValue() \
		); \
	default: \
		return std::unexpected( \
			std::string("Cannot use 'operatorSymbol' operator on _InputClass value and ") \
			+ std::string(ValueTypeStr.at(int(other->Type()))) \
		); \
	} \
}


namespace dim {
	namespace interpreter {

		enum class ValueType {
			NONE = 0,
			NUL,
			NUMBER,
			BOOLEAN,
			STRING
		};

		enum class ValueFlag {
			NONE = 0,
			BREAK
		};

		const std::array<std::string_view, 5> ValueTypeStr = {
			"NONE",
			"NULL",
			"NUMBER",
			"BOOLEAN",
			"STRING"
		};

		class Value {
		public:

			virtual bool IsTrue();

			virtual std::string Repr();
			virtual ValueType Type();

			Value() = default;

			ValueFlag GetFlag();
			void SetFlag(
				ValueFlag flag
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

		private:
			ValueFlag m_flag;
		};

		class NullValue : public Value {
		public:

			NullValue();

			bool IsTrue() override;

			ValueType Type() override;
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
		};

		class NumberValue : public Value {
		public:

			NumberValue(
				double value
			);

			double GetValue();
			void SetValue(
				double value
			);

			bool IsTrue() override;

			ValueType Type() override;
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

		private:
			double m_value;
		};

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

			ValueType Type() override;
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

		private:
			bool m_value;
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

			ValueType Type() override;
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

		private:
			std::string m_value;
		};
	}
}
