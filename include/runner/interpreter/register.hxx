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

			std::string Repr();

		private:
			bool _Exists(
				std::string name
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
