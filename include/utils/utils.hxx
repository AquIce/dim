#include <string>

#define FILEINFO std::string(__FILE__) + ":" + std::to_string(__LINE__)

#define __TRY_EXPECTED_FUNC_WRETERR(func, _Value, _Err, ...) \
{ \
	std::expected<_Value, _Err> result = func(__VA_ARGS__); \
	if(!result) { \
		return std::unexpected(result.error()); \
	} \
}
#define __TRY_EXPECTED_FUNC_WRETERR_WSAVE(func, _Value, _Err, resvalue, ...) \
{ \
	std::expected<_Value, _Err> result = func(__VA_ARGS__); \
	if(!result) { \
		return std::unexpected(result.error()); \
	} \
	resvalue = result.value(); \
}

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
