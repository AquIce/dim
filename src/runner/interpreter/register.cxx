#include <runner/interpreter/register.hxx>

namespace dim {
	namespace interpreter {

		RegisterManager::RegisterManager() :
			m_register()
		{}

		std::expected<
			Success,
			std::string
		> RegisterManager::Register(
			std::string name,
			RegisterValue value
		) {
			if(this->Exists(name)) {
				return std::unexpected("Trying to register existing value");
			}
			
			m_register.insert(
				{ name, value }
			);
			return Success{};
		}

		bool RegisterManager::Exists(
			std::string name
		) {
			return m_register.find(name) != m_register.end();
		}

		std::expected<
			RegisterValue,
			std::string
		> RegisterManager::Get(
			std::string name
		) {
			if(!this->Exists(name)) {
				return std::unexpected("Trying to get non existing value.");
			}

			return m_register.at(name);
		}

		std::expected<
			Success,
			std::string
		> RegisterManager::Set(
			std::string name,
			RegisterValue value
		) {
			if(!this->Exists(name)) {
				return std::unexpected("Trying to set non existing value.");
			}

			m_register[name] = value;

			return Success{};
		}

	}
}
