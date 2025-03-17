#include <dim/utils/utils.hpp>

char dim::utils::peek(
	const std::string& str
) {
	return str.at(0);
}

[[nodiscard]] char dim::utils::shift(
	std::string& str
) {
	char first = str.at(0);
	str = str.substr(1, str.length() - 1);
	return first;
}
