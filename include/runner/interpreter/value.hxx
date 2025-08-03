#pragma once

#include <array>
#include <expected>
#include <memory>
#include <string>

namespace dim {
	namespace interpreter {

		enum class ValueType {
			NONE = 0,
			NUL,
			NUMBER
		};

		const std::array<std::string_view, 3> ValueTypeStr = {
			"NONE",
			"NULL",
			"NUMBER"
		};

		class Value {
		public:

			virtual std::string Repr();
			virtual ValueType Type();

			Value() = default;

			virtual std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator+(
				std::shared_ptr<Value> other
			);
			virtual std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator-(
				std::shared_ptr<Value> other
			);
			virtual std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator*(
				std::shared_ptr<Value> other
			);
			virtual std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator/(
				std::shared_ptr<Value> other
			); 
		};

		class NullValue : public Value {
		public:

			NullValue();

			ValueType Type() override;
			std::string Repr() override;

			std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator+(
				std::shared_ptr<Value> other
			) override;
			std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator-(
				std::shared_ptr<Value> other
			) override;
			std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator*(
				std::shared_ptr<Value> other
			) override;
			std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator/(
				std::shared_ptr<Value> other
			) override; 
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

			ValueType Type() override;
			std::string Repr() override;

			std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator+(
				std::shared_ptr<Value> other
			) override;
			std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator-(
				std::shared_ptr<Value> other
			) override;
			std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator*(
				std::shared_ptr<Value> other
			) override;
			std::expected<
				std::shared_ptr<Value>,
				std::string
			> operator/(
				std::shared_ptr<Value> other
			) override; 

		private:
			double m_value;
		};
	}
}