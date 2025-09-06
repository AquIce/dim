#pragma once

#include <runner/interpreter/value.hxx>
#include <parser/ast.hxx>
#include <utils/utils.hxx>

#include <memory>
#include <string>
#include <unordered_map>

namespace dim {
	namespace interpreter {

		typedef struct {
			std::shared_ptr<Value> value;
		} RegisterValue;

		typedef struct {
			std::shared_ptr<parser::FunctionDeclarationExpression> function;
		} FunctionRegisterValue;

		class RegisterManager {
		public:
			RegisterManager(
				std::shared_ptr<RegisterManager> parent = nullptr,
				std::shared_ptr<Value> discard = nullptr
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

			std::shared_ptr<Value> GetDiscard();
			void SetDiscard(
				std::shared_ptr<Value> discard
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

			std::shared_ptr<Value> m_discard;

			std::shared_ptr<RegisterManager> m_parent;
		};

		class FunctionRegisterManager {
		public:
			FunctionRegisterManager();

			std::expected<
				Success,
				std::string
			> Register(
				FunctionRegisterValue value
			);

			bool Exists(
				const std::string name
			);

			std::expected<
				FunctionRegisterValue,
				std::string
			> Get(
				const std::string name
			);

			std::string Repr();

		private:
			std::unordered_map<
				std::string,
				FunctionRegisterValue
			> m_register;
		};
	}
}
