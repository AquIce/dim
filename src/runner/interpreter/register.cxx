#include <runner/interpreter/register.hxx>

namespace dim {
	namespace interpreter {

		RegisterManager::RegisterManager(
			std::shared_ptr<RegisterManager> parent
		) :
			m_register(),
			m_parent(parent)
		{}

		std::expected<
			Success,
			std::string
		> RegisterManager::Register(
			const std::string name,
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
			const std::string name
		) {
			return (
				this->_Exists(name)
				||
				(m_parent != nullptr && m_parent->Exists(name))
			);
		}

		std::expected<
			RegisterValue,
			std::string
		> RegisterManager::Get(
			const std::string name
		) {
			if(!this->Exists(name)) {
				return std::unexpected("Trying to get non existing value.");
			}
			
			try {
				return m_register.at(name);
			} catch(...) {
				return m_parent->Get(name);
			}
		}

		std::expected<
			Success,
			std::string
		> RegisterManager::Set(
			const std::string name,
			RegisterValue value
		) {
			if(!this->Exists(name)) {
				return std::unexpected("Trying to set non existing value.");
			}

			if(m_register.find(name) != m_register.end()) {
				m_register[name] = value;
				return Success{};
			}

			return m_parent->Set(name, value);
		}

		std::string RegisterManager::Repr() {
			std::string repr = "RegMan\n";
			for(const auto& [name, registerValue] : m_register) {
				repr += name + ": " + registerValue.value->Repr() + "\n";
			}
			return repr;
		}

		bool RegisterManager::_Exists(
			const std::string name
		) {
			return m_register.find(name) != m_register.end();
		}
	}
}
