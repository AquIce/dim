#include <string>

namespace dim {
	namesapce utils {
		char peek(
			const std::string& str
		);

		[[nodiscard]] char shift(
			std::string& str
		);
	}
}
