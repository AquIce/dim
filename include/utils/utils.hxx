#pragma once

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>

#define LOG(text) std::cerr << (text) << "\n"

#define FLOG(text) std::cerr << (text) << std::endl

#define NOIMP return std::unexpected("NOIMP")

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

typedef struct Success {} Success;

namespace dim {
	namespace utils {

		[[nodiscard]] char shift(
			std::string& str
		);

		[[nodiscard]] std::string shift(
			std::string& str,
			size_t num
		);

		template <typename Iter>
		size_t indexOfUnsafe(
			Iter first,
			Iter last,
			const typename std::iterator_traits<Iter>::value_type& x
		) {
		    size_t i = 0;
		    while(first != last && *first != x) {
		      	++first, ++i;
		    }
		    return i;
		}
		
		template <typename Iter>
		std::optional<size_t> indexOf(
		    Iter first,
		    Iter last,
		    const typename std::iterator_traits<Iter>::value_type& x
		) {
		    size_t i = 0;
		    while (first != last) {
		        if (*first == x) {
		            return i;
		        }
		        ++first; ++i;
		    }
		    return std::nullopt;
		}
	}
}
