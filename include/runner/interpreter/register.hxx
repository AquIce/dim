#pragma once

#include <runner/interpreter/value.hxx>
#include <utils/utils.hxx>

#include <memory>
#include <string>
#include <unordered_map>

namespace dim {
	namespace interpreter {

		typedef struct RegisterValue {
			std::shared_ptr<Value> value;
		} RegisterValue;

		class RegisterManager {
		public:
			RegisterManager();

			std::expected<
				Success,
				std::string
			> Register(
				std::string name,
				RegisterValue value
			);

			bool Exists(
				std::string name
			);

			std::expected<
				RegisterValue,
				std::string
			> Get(
				std::string name
			);

			std::expected<
				Success,
				std::string
			> Set(
				std::string name,
				RegisterValue value
			);

		private:
			std::unordered_map<
				std::string,
				RegisterValue
			> m_register;
		};

	}
}
