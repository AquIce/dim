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
		
	}
}