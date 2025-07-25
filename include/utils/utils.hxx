#include <string>

namespace dim {
	namespace utils {
		char peek(
			const std::string& str
		);

		[[nodiscard]] char shift(
			std::string& str
		);
	}
}
