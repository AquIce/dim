#include <utils/utils.hxx>

namespace dim {
	namespace utils {

		[[nodiscard]] char shift(
			std::string& str
		) {
			char first = str.at(0);
			str = str.substr(1, str.length() - 1);
			return first;
		}

		[[nodiscard]] std::string shift(
			std::string& str,
			size_t num
		) {
			std::string result = "";
			for(; num > 0; num--) {
				result += shift(str);
			}
			return result;
		}
	}
}
