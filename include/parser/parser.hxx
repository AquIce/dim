#include <parser/ast.hxx>

#include <lexer/lexer.hxx>

#include <expected>
#include <memory>
#include <string>
#include <vector>

#define __TRY_TOKEN_FUNC_WRETERR(func, ...) \
__TRY_EXPECTED_FUNC_WRETERR( \
	func, \
	lexer::Token, \
	std::string, \
	__VA_ARGS__ \
)
#define __TRY_TOKEN_FUNC_WRETERR_WSAVE(func, value, ...) \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE( \
	func, \
	lexer::Token, \
	std::string, \
	value, \
	__VA_ARGS__ \
)

#define __TRY_EXPR_FUNC_WRETERR(func, tokens) \
__TRY_EXPECTED_FUNC_WRETERR( \
	func, \
	std::shared_ptr<Expression>, \
	std::string, \
	tokens \
)
#define __TRY_EXPR_FUNC_WRETERR_WSAVE(func, tokens, value) \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE( \
	func, \
	std::shared_ptr<Expression>, \
	std::string, \
	value, \
	tokens \
)

namespace dim {
	namespace parser {

		[[nodiscard]]
		std::expected<struct lexer::Token, std::string> eat(
			std::vector<struct lexer::Token>& tokens
		);

		[[nodiscard]]
		std::expected<struct lexer::Token, std::string> expect(
			std::vector<struct lexer::Token>& tokens,
			struct lexer::Token expected
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_null_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_number_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_boolean_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_parenthesis_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_multiplicative_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_additive_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_binary_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<ScopeExpression>,
			std::string
		> Parse(
			std::vector<struct lexer::Token>& tokens
		);
	}
}
