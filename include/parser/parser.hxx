#pragma once

#include "ast.hxx"
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
__TRY_EXPECTED_FUNC_WRETERR__NEW( \
	func, \
	lexer::Token, \
	__VA_ARGS__ \
)
#define __TRY_TOKEN_FUNC_WRETERR_WSAVE(func, value, ...) \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE__NEW( \
	func, \
	lexer::Token, \
	value, \
	__VA_ARGS__ \
)

#define __TRY_EXPR_FUNC_WRETERR(func, tokens, identifierRegister) \
__TRY_EXPECTED_FUNC_WRETERR__NEW( \
	func, \
	std::shared_ptr<Expression>, \
	tokens, \
	identifierRegister \
)
#define __TRY_EXPR_FUNC_WRETERR_WSAVE(func, tokens, identifierRegister, value) \
__TRY_EXPECTED_FUNC_WRETERR_WSAVE__NEW( \
	func, \
	std::shared_ptr<Expression>, \
	value, \
	tokens, \
	identifierRegister \
)

namespace dim {
	namespace parser {

		extern std::vector<std::shared_ptr<FunctionDeclarationExpression>> functions;

		[[nodiscard]]
		Result<struct lexer::Token> eat(
			std::vector<struct lexer::Token>& tokens
		);

		[[nodiscard]]
		Result<struct lexer::Token> expect(
			std::vector<struct lexer::Token>& tokens,
			const struct lexer::Token expected
		);

		[[nodiscard]]
		Result<std::string> expect_type(
			std::vector<struct lexer::Token>& tokens
		);

		Result<std::shared_ptr<Expression>> parse_identifier_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_discard_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_null_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_boolean_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_char_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_string_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_number_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_struct_member_access_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_fn_call_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_struct_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_break_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_return_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<OrExpression>> parse_or_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_parenthesis_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_unary_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_and_logical_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_or_logical_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_comparison_logical_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_equality_logical_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_multiplicative_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_additive_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_binary_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_ifelse_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			const bool allow_if = true
		);

		Result<std::shared_ptr<Expression>> parse_ifelse_structure(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_match_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			const bool allow_default = true
		);

		Result<std::shared_ptr<Expression>> parse_struct_structure(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_loop_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_assignation_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_declaration_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_scope_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_fn_declaration_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_interface_declaration_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		Result<std::shared_ptr<Expression>> parse_struct_declaration_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

    Result<std::shared_ptr<Expression>> parse_struct_implementation_expression(
      std::vector<struct lexer::Token>& tokens,
      std::shared_ptr<ScopeIdentifierRegister> identifierRegister
    );

		Result<std::shared_ptr<Expression>> parse_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		);

		std::expected<
			std::shared_ptr<ScopeExpression>,
			std::string
		> Parse(
			std::vector<struct lexer::Token>& tokens
		);
	}
}
