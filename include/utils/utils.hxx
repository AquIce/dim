#include <string>

#define FILEINFO std::string(__FILE__) + ":" + std::to_string(__LINE__)

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
