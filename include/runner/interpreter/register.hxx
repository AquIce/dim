#pragma once

#include <runner/interpreter/value.hxx>
#include <utils/utils.hxx>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace dim {
	namespace interpreter {

		typedef struct RegisterValue {
			std::shared_ptr<Value> value;
		} RegisterValue;

		class RegisterManager {
		public:
			RegisterManager(
				std::shared_ptr<RegisterManager> parent = nullptr
			);

			std::expected<
				Success,
				std::string
			> Register(
				const std::string name,
				RegisterValue value
			);

			bool Exists(
				const std::string name
			);

			std::expected<
				RegisterValue,
				std::string
			> Get(
				const std::string name
			);

			std::expected<
				Success,
				std::string
			> Set(
				const std::string name,
				RegisterValue value
			);

			std::string Repr();

		private:
			bool _Exists(
				const std::string name
			);

		private:
			std::unordered_map<
				std::string,
				RegisterValue
			> m_register;

			std::shared_ptr<RegisterManager> m_parent;
		};

	}
}
