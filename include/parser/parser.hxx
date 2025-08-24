#pragma once

#include <parser/ast.hxx>
#include <parser/cast.hxx>

#include <lexer/lexer.hxx>

#include<utils/utils.hxx>

#include <expected>
#include <iterator>
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

		extern std::vector<IdentifierData> Identifiers;

		std::expected<
			IdentifierData,
			std::string
		> GetIdentifier(
			const std::string name
		);

		[[nodiscard]]
		std::expected<struct lexer::Token, std::string> eat(
			std::vector<struct lexer::Token>& tokens
		);

		[[nodiscard]]
		std::expected<struct lexer::Token, std::string> expect(
			std::vector<struct lexer::Token>& tokens,
			const struct lexer::Token expected
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_identifier_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_discard_expression(
			std::vector<struct lexer::Token>& tokens
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
		> parse_string_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_break_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<OrExpression>,
			std::string
		> parse_or_expression(
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
		> parse_unary_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_logical_expression(
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
		> parse_ifelse_expression(
			std::vector<struct lexer::Token>& tokens,
			const bool allow_if = true
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_ifelse_structure(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_loop_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_assignation_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_declaration_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_expression(
			std::vector<struct lexer::Token>& tokens
		);

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_scope_expression(
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
