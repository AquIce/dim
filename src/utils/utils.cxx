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

		f128 stof128(
			const std::string& input
		) {
			return strtoflt128(input.c_str(), nullptr);
		}

		std::string f128tos(
			f128 value
		) {
			char buffer[128];
			snprintf(buffer, sizeof(buffer), "%.36Lf", value);
			return std::string(buffer);
		}
	}
}
