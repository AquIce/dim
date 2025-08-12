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
			NUMBER,
			BOOLEAN,
			STRING
		};

		enum class ValueFlag {
			NONE = 0,
			BREAK
		};

		const std::array<std::string_view, 4> ValueTypeStr = {
			"NONE",
			"NULL",
			"NUMBER",
			"BOOLEAN"
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

		private:
			ValueFlag m_flag;
		};

		class NullValue : public Value {
		public:

			NullValue();

			bool IsTrue() override;

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

			bool IsTrue() override;

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
			std::string m_value;
		};
	}
}