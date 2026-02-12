#include <utils/utils.hxx>

namespace dim {
	namespace utils {

    std::string ErrorTypeRepr(
      ErrorType type
    ) {
      switch(type) {
        case ErrorType::NONE:
          return "NONE";
        case ErrorType::FATAL:
          return "FATAL";
        case ErrorType::ERROR:
          return "ERROR";
        case ErrorType::WARNING:
          return "WARNING";
        case ErrorType::RETERR:
          return "RETERR";
        default:
          return "UNKNOWN";
      }
    }

    std::string ErrorRepr(
      struct Error& err
    ) {
      return std::string("[") + ErrorTypeRepr(err.type)
        + "]" + std::to_string(err.ctx.line)
        + ":" + std::to_string(err.ctx.column)
        + " > " + err.message;
    }

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
