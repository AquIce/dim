#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <stdfloat>
#include <string>
#include <type_traits>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = std::float32_t;
using f64 = std::float64_t;

#ifdef __GNUC__
	#include <quadmath.h>
	using f128 = __float128;
#else
	#error "GCC/Clang and 128-bit float type required"
#endif

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

#define __GEN__STOIU_FN_BODY(name, _Datatype) \
inline _Datatype name(const std::string& input) { \
	_Datatype value; \
	std::from_chars(input.data(), input.data() + input.size(), value); \
	return value; \
}

typedef struct {} Success;

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

		__GEN__STOIU_FN_BODY(stoi8, i8)
		__GEN__STOIU_FN_BODY(stoi16, i16)
		__GEN__STOIU_FN_BODY(stoi32, i32)
		__GEN__STOIU_FN_BODY(stoi64, i64)

		__GEN__STOIU_FN_BODY(stou8, u8)
		__GEN__STOIU_FN_BODY(stou16, u16)
		__GEN__STOIU_FN_BODY(stou32, u32)
		__GEN__STOIU_FN_BODY(stou64, u64)

		inline f32 stof32(const std::string& input) {
			return std::stof(input);
		}
		inline f64 stof64(const std::string& input) {
			return std::stod(input);
		}

		#ifdef __GNUC__
			inline f128 stof128(
				const std::string& input
			) {
				return strtoflt128(input.c_str(), nullptr);
			}

			inline std::string f128tos(
				f128 value
			) {
				char buffer[128];
				snprintf(buffer, sizeof(buffer), "%.36Lf", value);
				return std::string(buffer);
			}

			inline bool f128isint(
				f128 value
			) {
				return value == floorq(value);
			}
		#endif

		template <typename T, T beginValue, T endValue>
		class Iterator {
		public:
			Iterator(
				const T& value
			) :
				m_value(static_cast<ValueType>(value))
			{}

			Iterator() :
				m_value(static_cast<ValueType>(beginValue))
			{}

			Iterator operator++() {
				++m_value;
				return *this;
			}
			T operator*() {
				return static_cast<T>(m_value);
			}
			Iterator begin() {
				return *this;
			}
			Iterator end() {
				static const Iterator endIterator = ++Iterator(endValue);
				return endIterator;
			}

			bool operator!=(
				const Iterator& i
			) {
				return m_value != i.m_value;
			}

		public:
			typedef typename std::underlying_type<T>::type ValueType;
			size_t m_value;
		};
	}
}
