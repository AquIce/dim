#include <lexer/lexer.hxx>

namespace dim {
	namespace lexer {

		struct Token MakeToken(
			const TokenType type,
			const std::string value,
      const struct utils::Context ctx
		) noexcept {
			return Token{
				.type = type,
        .value = value,
        .ctx = ctx,
			};
		}

		std::string TokenRepr(
			const struct Token& token
		) noexcept {
      std::string ctx = std::to_string(token.ctx.line) + ":" + std::to_string(token.ctx.column) + " ";
			switch(token.type) {
				case TokenType::NONE:
					return ctx + "NONE";
				
				case TokenType::NUL:
					return ctx + "NULL";

				case TokenType::NUMBER:
					return ctx + "NUMBER(" + token.value + ")";
				
				case TokenType::BOOLEAN:
					return ctx + "BOOLEAN(" + token.value + ")";

				case TokenType::CHAR:
					return ctx + "CHAR('" + token.value + "')";

				case TokenType::STRING:
					return ctx + "STRING(\"" + token.value + "\")";
				
				case TokenType::EOL:
				case TokenType::UNARY_OPERATOR:
				case TokenType::BINARY_OPERATOR:
				case TokenType::PARENTHESIS:
				case TokenType::BRACE:
				case TokenType::DOT:
				case TokenType::COMMA:
				case TokenType::COLON:
				case TokenType::EQUALS:
				case TokenType::ARROW:
				case TokenType::AT:
				case TokenType::DOUBLE_DOT:
				case TokenType::IFELSE:
				case TokenType::MATCH:
				case TokenType::LOOP:
				case TokenType::BREAK:
				case TokenType::OR:
				case TokenType::DECL:
				case TokenType::TYPE:
				case TokenType::FN:
				case TokenType::RETURN:
				case TokenType::STRUCT:
				case TokenType::IMPL:
				case TokenType::DISCARD:
					return ctx + token.value;

				case TokenType::IDENTIFIER:
					return ctx + "ID(" + token.value + ")";
				
				default:
					return ctx + "UNKNOWN";
			}
		}

    LexTracker::LexTracker(
      const std::vector<std::string> src
    ) :
      src(src),
      ctx({ .line = 0, .column = 0 }),
      tokens()
    {}

		std::expected<std::string, std::string> LexTracker::peek(
			size_t length
		) {
			try {
				const std::string& lineStr = this->src.at(ctx.line);

				if(this->ctx.column + length > lineStr.size()) {
					return std::unexpected("Peek exceeds line length.");
				}

				return lineStr.substr(this->ctx.column, length);
			} catch(...) {
				return std::unexpected("Peek failed: line index out of bounds.");
			}
		}

		std::expected<std::string, std::string> LexTracker::shift(
			size_t length
		) {
			try {
				const std::string& lineStr = this->src.at(ctx.line);

				if(this->ctx.column + length > lineStr.size()) {
					return std::unexpected("Shift exceeds end of line.");
				}

				std::string result = lineStr.substr(this->ctx.column, length);
				this->ctx.column += length;

				if(this->ctx.column == lineStr.size()) {
          while(
            this->ctx.line < this->src.size()
            && this->src.at(++this->ctx.line).size() == 0
          ) {}
					this->ctx.column = 0;
				}

				return result;
			} catch(...) {
				return std::unexpected("Shift failed: line index out of bounds.");
			}
		}

    std::expected<char, utils::Error> LexTracker::lex_char() {
      if(this->src.size() == 0) {
         return std::unexpected(utils::Error{
           .ctx = this->ctx,
           .message = "Trying to lex character in empty source.",
           .type = utils::ErrorType::RETERR,
        });
      }

      char first = this->src.at(this->ctx.line).at(this->ctx.column);

      if(first == '\\') {
        if(this->src.size() < 2) {
          return std::unexpected(utils::Error{
            .ctx = this->ctx,
            .message = "Trying to lex invalid escaped character in source, nothing after \\.",
            .type = utils::ErrorType::RETERR,
          });
        }
			  // peek 2 chars for escape sequence
        std::expected<std::string, std::string> peekRes = this->shift(2);
				if(!peekRes) {
           return std::unexpected(utils::Error{
             .ctx = this->ctx,
             .message = peekRes.error(),
             .type = utils::ErrorType::RETERR,
          });
        }

				std::expected<char, std::string> result = to_escaped_char(peekRes.value());
				if(!result) {
           return std::unexpected(utils::Error{
             .ctx = this->ctx,
             .message = result.error(),
             .type = utils::ErrorType::RETERR,
          });
				}

        return result.value();
      }

      (void)this->shift();
      return first;
    }

		std::expected<char, std::string> to_escaped_char(
			const std::string& chr
		) noexcept {
			if(chr.size() == 1) {
				return chr[0];
			}

			if(chr.size() == 2 && chr[0] == '\\') {
				switch(chr[1]) {
					case 'n': return '\n';
					case 't': return '\t';
					case 'r': return '\r';
					case 'b': return '\b';
					case 'f': return '\f';
					case 'v': return '\v';
					case 'a': return '\a';
					case '\\': return '\\';
					case '\'': return '\'';
					case '\"': return '\"';
					case '0': return '\0';
					default:
						return std::unexpected("Unsupported escape sequence: " + chr);
				}
			}

			return std::unexpected("Invalid input: " + chr);
		}

		Result<struct Token> LexEOL(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
				return std::unexpected(utils::Error{
					.ctx = tracker.ctx,
					.message = result.error(),
					.type = utils::ErrorType::RETERR,
				});
			}
			if(result.value() == ";") {
        (void)tracker.shift();
				return MakeToken(
					TokenType::EOL,
					";", //tracker.shift().value(),
          tracker.ctx
				);
			}

			return std::unexpected(utils::Error{
		    .ctx = tracker.ctx,
				.message = "No EOL token found.",
				.type = utils::ErrorType::RETERR,
			});
		}

		Result<struct Token> LexNull(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(4);
			if(!result) {
				return std::unexpected(utils::Error{
					.ctx = tracker.ctx,
					.message = result.error(),
					.type = utils::ErrorType::RETERR,
				});
			}
			if(result.value() == "null") {
				return MakeToken(
					TokenType::NUL,
					tracker.shift(4).value(),
          tracker.ctx
				);
			}

			return std::unexpected(utils::Error{
		        .ctx = tracker.ctx,
				.message = "No null token found.",
				.type = utils::ErrorType::RETERR,
			});
		}

		Result<struct Token> LexNumber(
		  LexTracker& tracker
	    ) noexcept {
			std::string number = "";
			bool isDecimal = false;

			size_t line = tracker.ctx.line;
			std::expected<std::string, std::string> peekRes = tracker.peek();
			if(!peekRes) {
				return std::unexpected(utils::Error{
					.ctx = tracker.ctx,
					.message = peekRes.error(),
					.type = utils::ErrorType::RETERR,
				});
			}
			if(peekRes.value().at(0) == '.') {
				return std::unexpected(utils::Error{
					.ctx = tracker.ctx,
					.message = ". at the start of number token.",
					.type = utils::ErrorType::RETERR,
				});
			}

			while(true) {
				char current = peekRes.value().at(0);
				if(isDecimal && (current == '.')) {
					return std::unexpected(utils::Error{
						.ctx = tracker.ctx,
						.message = std::string("Invalid number litteral : '") + number + std::string(1, current) + "' has two decimal separators.",
						.type = utils::ErrorType::ERROR,
					});
				}
				if(!std::isdigit(current) && (current != '.')) {
					break;
				}
        if(current == '.') {
          isDecimal = true;
        }

				number += current;
        (void)tracker.shift();

				peekRes = tracker.peek();
				if(!peekRes || line != tracker.ctx.line) {
					break;
				}
			}
			
			if(number.length() == 0) {
				return std::unexpected(utils::Error{
					.ctx = tracker.ctx,
					.message = "No number token found.",
					.type = utils::ErrorType::RETERR,
				});
			}
			return MakeToken(
				TokenType::NUMBER,
				number,
        tracker.ctx
			);
		}


		Result<struct Token> LexBoolean(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(4);
			if(!result) {
				return std::unexpected(utils::Error{
					.ctx = tracker.ctx,
					.message = result.error(),
					.type = utils::ErrorType::RETERR,
				});
			}
			if(result.value() == "true") {
				return MakeToken(
					TokenType::BOOLEAN,
					tracker.shift(4).value(),
          tracker.ctx
				);
			}

			result = tracker.peek(5);
			if(!result) {
				return std::unexpected(utils::Error{
					.ctx = tracker.ctx,
					.message = result.error(),
					.type = utils::ErrorType::RETERR,
				});
			}
			if(result.value() == "true") {
				return MakeToken(
					TokenType::BOOLEAN,
					tracker.shift(5).value(),
          tracker.ctx
				);
			}

			return std::unexpected(utils::Error{
				.ctx = tracker.ctx,
				.message = "No boolean token found.",
				.type = utils::ErrorType::RETERR,
			});
		}

		Result<struct Token> LexChar(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> peekRes = tracker.peek();
			if(!peekRes) {
				return std::unexpected(utils::Error{
					.ctx = tracker.ctx,
					.message = peekRes.error(),
					.type = utils::ErrorType::RETERR,
				});
			}
			if(peekRes.value().at(0) != '\'') {
				return std::unexpected(utils::Error{
					.ctx = tracker.ctx,
					.message = "No char token found.",
					.type = utils::ErrorType::RETERR,
				});
			}
			(void)tracker.shift();

      std::expected<char, utils::Error> lexedChar = tracker.lex_char();
      if(!lexedChar) {
        return std::unexpected(lexedChar.error());
      }
			
      peekRes = tracker.peek();
			if(peekRes.value().at(0) != '\'') {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = "Invalid character found: Missing closing ' symbol.",
          .type = utils::ErrorType::ERROR,
        });
			}
			(void)tracker.shift();

			return MakeToken(
				TokenType::CHAR,
				std::string(1, lexedChar.value()),
        tracker.ctx
			);
		}


		Result<struct Token> LexString(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> peekRes = tracker.peek();
			if(!peekRes) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = peekRes.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(peekRes.value().at(0) != '"') {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = "No string token found.",
          .type = utils::ErrorType::RETERR,
        });
			}
			(void)tracker.shift();

			std::string str = "";

			while(true) {
        std::expected<char, utils::Error> result = tracker.lex_char();
				if(!result) {
					return std::unexpected(result.error());
				}
				
        if(result.value() == '"') {
	  			break;
	  		}

				str += result.value();
			}

			return MakeToken(
				TokenType::STRING,
				str,
        tracker.ctx
			);
		}


		Result<struct Token> LexBinaryOperator(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(
				result.value() == "+"
				|| result.value() == "-"
				|| result.value() == "*"
				|| result.value() == "/"
				|| result.value() == ">"
				|| result.value() == "<"
				|| result.value() == "&"
				|| result.value() == "|"
				|| result.value() == "^"
			) {
				return MakeToken(
					TokenType::BINARY_OPERATOR,
					tracker.shift().value(),
          tracker.ctx
				);
			}
			
			result = tracker.peek(2);
			if(!result) {
			  return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(
				result.value() == ">="
				|| result.value() == "<="
				|| result.value() == "&&"
				|| result.value() == "||"
				|| result.value() == "=="
				|| result.value() == "!="
			) {
				return MakeToken(
					TokenType::BINARY_OPERATOR,
					tracker.shift(2).value(),
          tracker.ctx
				);
			}
			
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No binary operator token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexUnaryOperator(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "!" || result.value() == "~") {
				return MakeToken(
					TokenType::UNARY_OPERATOR,
					tracker.shift().value(),
          tracker.ctx
				);
			}

			result = tracker.peek(2);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "++" || result.value() == "--") {
				return MakeToken(
					TokenType::UNARY_OPERATOR,
					tracker.shift(2).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No unary operator token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexParenthesis(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "(" || result.value() == ")") {
				return MakeToken(
					TokenType::PARENTHESIS,
					tracker.shift().value(),
          tracker.ctx
				);
			}
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No parenthesis token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexBrace(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "{" || result.value() == "}") {
				return MakeToken(
					TokenType::BRACE,
					tracker.shift().value(),
          tracker.ctx
				);
			}
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No null token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexDot(
			LexTracker& tracker
		) noexcept {
			
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == ".") {
				return MakeToken(
					TokenType::DOT,
					tracker.shift().value(),
          tracker.ctx
				);
			}
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No dot token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexComma(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == ",") {
				return MakeToken(
					TokenType::COMMA,
					tracker.shift().value(),
          tracker.ctx
				);
			}
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No comma token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexColon(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == ":") {
				return MakeToken(
					TokenType::COLON,
					tracker.shift().value(),
          tracker.ctx
				);
			}
			return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No colon token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexEquals(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "=") {
				return MakeToken(
					TokenType::EQUALS,
					tracker.shift().value(),
          tracker.ctx
				);
			}
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No equals token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexArrow(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "->") {
				return MakeToken(
					TokenType::ARROW,
					tracker.shift(2).value(),
          tracker.ctx
				);
			}
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No arrow token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexAt(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek();
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "@") {
				return MakeToken(
					TokenType::AT,
					tracker.shift().value(),
          tracker.ctx
				);
			}
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No at token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexDoubleDot(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "..") {
				return MakeToken(
					TokenType::DOUBLE_DOT,
					tracker.shift(2).value(),
          tracker.ctx
				);
			}
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No double dot token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexIfElse(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "if") {
				return MakeToken(
					TokenType::IFELSE,
					tracker.shift(2).value(),
          tracker.ctx
				);
			}
			result = tracker.peek(4);
			if(!result) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "else") {
				return MakeToken(
					TokenType::IFELSE,
					tracker.shift(4).value(),
          tracker.ctx
				);
			}
			result = tracker.peek(6);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "elseif") {
				return MakeToken(
					TokenType::IFELSE,
					tracker.shift(6).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No if-else token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexMatch(
			LexTracker& tracker
		) noexcept {
			std::expected<std::string, std::string> result = tracker.peek(5);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "match") {
				return MakeToken(
					TokenType::MATCH,
					tracker.shift(5).value(),
          tracker.ctx
				);
			}
	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No match token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexLoop(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(4);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "loop") {
				return MakeToken(
					TokenType::LOOP,
					tracker.shift(4).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No loop token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexBreak(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(5);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "break") {
				return MakeToken(
					TokenType::BREAK,
					tracker.shift(5).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No break token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexOr(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "or") {
				return MakeToken(
					TokenType::OR,
					tracker.shift(2).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No or token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexDecl(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(3);
			if(!result) {
	      return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "var") {
				return MakeToken(
					TokenType::DECL,
					tracker.shift(3).value(),
          tracker.ctx
				);
			}
			result = tracker.peek(5);
			if(!result) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "const") {
				return MakeToken(
					TokenType::DECL,
					tracker.shift(5).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No const var/const token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexType(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
      }
			if(result.value() == "i8") {
				(void)tracker.shift(2);
				return MakeToken(
					TokenType::DOT,
					"I8",
          tracker.ctx
				);
			}
			if(result.value() == "u8") {
				(void)tracker.shift(2);
				return MakeToken(
					TokenType::DOT,
					"U8",
          tracker.ctx
				);
			}

			result = tracker.peek(3);
			if(!result) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "i16") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"I16",
          tracker.ctx
				);
			}
			if(result.value() == "i32") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"I32",
          tracker.ctx
				);
			}
			if(result.value() == "i64") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"I64",
          tracker.ctx
				);
			}
			if(result.value() == "u16") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"U16",
          tracker.ctx
				);
			}
			if(result.value() == "u32") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"U32",
          tracker.ctx
				);
			}
			if(result.value() == "u64") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"U64",
          tracker.ctx
				);
			}
			if(result.value() == "f32") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"F32",
          tracker.ctx
				);
			}
			if(result.value() == "f64") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"F64",
          tracker.ctx
				);
			}
			if(result.value() == "str") {
				(void)tracker.shift(3);
				return MakeToken(
					TokenType::TYPE,
					"STRING",
          tracker.ctx
				);
			}

			result = tracker.peek(4);
			if(!result) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "void") {
				(void)tracker.shift(4);
				return MakeToken(
					TokenType::TYPE,
					"VOID",
          tracker.ctx
				);
			}
			if(result.value() == "char") {
				(void)tracker.shift(4);
				return MakeToken(
					TokenType::TYPE,
					"CHAR",
          tracker.ctx
				);
			}
			if(result.value() == "bool") {
				(void)tracker.shift(4);
				return MakeToken(
					TokenType::TYPE,
					"BOOL",
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No type token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexFn(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(2);
			if(!result) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "fn") {
				return MakeToken(
					TokenType::FN,
					tracker.shift(2).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No fn token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexReturn(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(6);
			if(!result) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "return") {
				return MakeToken(
					TokenType::RETURN,
					tracker.shift(6).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No return token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexStruct(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(6);
			if(!result) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "struct") {
				return MakeToken(
					TokenType::STRUCT,
					tracker.shift(6).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No struct token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexImpl(
			LexTracker& tracker
		) noexcept {

			std::expected<std::string, std::string> result = tracker.peek(4);
			if(!result) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = result.error(),
          .type = utils::ErrorType::RETERR,
        });
			}
			if(result.value() == "impl") {
				return MakeToken(
					TokenType::IMPL,
					tracker.shift(4).value(),
          tracker.ctx
				);
			}

	    return std::unexpected(utils::Error{
        .ctx = tracker.ctx,
        .message = "No impl token found.",
        .type = utils::ErrorType::RETERR,
      });
		}

		Result<struct Token> LexIdentifier(
			LexTracker& tracker
		) noexcept {
			std::string identifier = "";

      std::expected<std::string, std::string> result;
			while(
        (result = tracker.peek()) && (
					std::isalpha(result.value().front()) ||
					std::isdigit(result.value().front()) ||
					result.value().front() == '_'
				)
			) {
				identifier += tracker.shift().value();
			}
			
			if(
				identifier.size() == 0
			) {
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = "Invalid identifier found (length 0).",
          .type = utils::ErrorType::RETERR,
        });
			}

			if(identifier == "_") {
				return MakeToken(
					TokenType::DISCARD,
					identifier,
          tracker.ctx
				);
			}

			return MakeToken(
				TokenType::IDENTIFIER,
				identifier,
        tracker.ctx
			);
		}

		void StripWhitespaces(
			LexTracker& tracker
		) noexcept {
      std::expected<std::string, std::string> result = tracker.peek();
			while(
        (result = tracker.peek()) && std::isspace(result.value().front())
      ) {
        (void)tracker.shift();
			}
		}

		Result<Success> StripComments(
			LexTracker& tracker
		) noexcept {
      std::expected<std::string, std::string> result = tracker.peek(2);
      if(!result) {
        return Success{};
      }
			if(result.value() == "/*") {
				// To avoid /*/
				(void)tracker.shift(2);
				
				while(result = tracker.peek(2)) {
					if(result.value() == "*/") {
						(void)tracker.shift(2);
						return Success{};
					}
					(void)tracker.shift();
				}
				return std::unexpected(utils::Error{
          .ctx = tracker.ctx,
          .message = "Unclosed multiline comment.",
          .type = utils::ErrorType::ERROR,
        });
			}
			if(result.value() == "//") {
        std::expected<std::string, std::string> shiftRes = tracker.peek();
				while(
          (shiftRes = tracker.peek()) && shiftRes.value().front() != '\n'
        ) {
					(void)tracker.shift();
				}
			}
			return Success{};
		}

		std::expected<Success, std::string> Lex(
			std::vector<struct Token>& tokens,
      const std::vector<std::string>& src
		) noexcept {
  
      LexTracker tracker = LexTracker(src);

			while(tracker.peek()) {
				
				StripWhitespaces(tracker);

				Result<Success> stripResult = StripComments(tracker);
				if(!stripResult) {
					return std::unexpected(
						"[ERR::LEXER] Got error :\n\t\"" + utils::ErrorRepr(stripResult.error()) + "\"\nwhile lexing source. \""
					);
				}

				StripWhitespaces(tracker);

        utils::Error error = utils::Error{
          .ctx = tracker.ctx,
          .message = "",
          .type = utils::ErrorType::NONE,
        };

        if(!tracker.peek()) {
          break;
        }

				for(const auto& lexFunction : LexFunctionsList) {
					
					Result<struct Token> result = lexFunction(tracker);

          if(!result) {
						error = result.error();
						continue;
					}
					tracker.tokens.push_back(result.value());
          error = utils::Error{
            .ctx = tracker.ctx,
            .message = "",
            .type = utils::ErrorType::NONE,
          };
					break;
				}

				if(error.type != utils::ErrorType::NONE) {
					return std::unexpected(
						"[ERR::LEXER] Got error :\n\t\"" + utils::ErrorRepr(error) + "\"\nwhile lexing source. \""
					);
				}
			}
      tokens = tracker.tokens;
			return Success{};
		}
	}
}
