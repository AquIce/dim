#include <parser/parser.hxx>

namespace dim {
	namespace parser {

		std::vector<std::shared_ptr<FunctionDeclarationExpression>> functions = {};

		[[nodiscard]]
		std::expected<struct lexer::Token, std::string> eat(
			std::vector<struct lexer::Token>& tokens
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Trying to eat a token that in empty token list.");
			}
			struct lexer::Token tk = tokens.front();
			tokens.erase(tokens.begin());
			return tk;
		}

		[[nodiscard]]
		std::expected<struct lexer::Token, std::string> expect(
			std::vector<struct lexer::Token>& tokens,
			const struct lexer::Token expected
		) {

			std::expected<struct lexer::Token, std::string> result = eat(tokens);

			if(!result) {
				return std::unexpected(
					result.error() + " Expected "
					+ std::string(lexer::TokenTypeStr.at(int(expected.type)))
					+ " (" + expected.value + ")"
				);
			}

			struct lexer::Token tk = result.value();

			if(tk.type != expected.type && expected.value == "") {
				tokens.insert(tokens.begin(), tk);
				return std::unexpected(
					std::string("Invalid token type: got ")
					+ std::string(lexer::TokenTypeStr.at(int(tk.type))) + " expected "
					+ std::string(lexer::TokenTypeStr.at(int(expected.type)))
					+ " (" + tk.value + ")"
				);
			}

			if(tk.type != expected.type && tk.value != expected.value) {
				tokens.insert(tokens.begin(), tk);
				return std::unexpected(
					std::string("Invalid token value: got ")
					+ tk.value + " expected " + expected.value
				);
			}

			return tk;
		}

		[[nodiscard]]
		std::expected<std::string, std::string> expect_type(
			std::vector<struct lexer::Token>& tokens
		) {
			if(
				tokens.size() > 0 && (
					tokens.front().type == lexer::TokenType::TYPE
					|| (
						tokens.front().type == lexer::TokenType::IDENTIFIER
						&& GetDatatypeClass(tokens.front().value)
					)
				)
			) {
				return eat(tokens).value().value;
			}
			return std::unexpected("Expected type, found " + lexer::TokenRepr(
				tokens.front()
			));
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_identifier_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}

			lexer::Token identifier;
			__TRY_TOKEN_FUNC_WRETERR_WSAVE(
				expect,
				identifier,
				tokens,
				lexer::MakeToken(lexer::TokenType::IDENTIFIER)
			)

			std::expected<
				IdentifierData,
				std::string
			> result = identifierRegister->Get(identifier.value);

			if(!result) {
				return std::make_shared<IdentifierExpression>(
					identifierRegister,
					identifier.value
				);
			}

			return std::make_shared<IdentifierExpression>(
				identifierRegister,
				result.value().name,
				result.value().isConst
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_discard_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::DISCARD) {
				return parse_identifier_expression(tokens, identifierRegister);
			}

			(void)eat(tokens);
			return std::make_shared<DiscardExpression>();
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_null_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::NUL) {
				return parse_discard_expression(tokens, identifierRegister);
			}

			(void)eat(tokens);
			return std::make_shared<NullExpression>();
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_boolean_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::BOOLEAN) {
				return parse_null_expression(tokens, identifierRegister);
			}

			return std::make_shared<BooleanExpression>(
				eat(tokens).value().value
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_char_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::CHAR) {
				return parse_boolean_expression(tokens, identifierRegister);
			}

			return std::make_shared<CharExpression>(
				eat(tokens).value().value
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_string_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::STRING) {
				return parse_char_expression(tokens, identifierRegister);
			}

			return std::make_shared<StringExpression>(
				eat(tokens).value().value
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_number_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::NUMBER) {
				return parse_string_expression(tokens, identifierRegister);
			}

			return auto_cast(
				std::make_shared<NumberExpression>(
					eat(tokens).value().value
				)
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_struct_member_access_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(
				tokens.front().type != lexer::TokenType::IDENTIFIER
				|| tokens.size() < 2
				|| tokens.at(1).type != lexer::TokenType::DOT
			) {
				return parse_number_expression(tokens, identifierRegister);
			}

			std::string typeName;
      std::string structInstanceName = tokens.front().value;
			{
				std::expected<
					IdentifierData,
					std::string
				> result = identifierRegister->Get(tokens.front().value);
				if(!result) {
					return parse_number_expression(tokens, identifierRegister);
				}
				
				typeName = result.value().datatype;
			}
			std::shared_ptr<DatatypeClass> structClass;
			{
				std::expected<
					std::shared_ptr<DatatypeClass>,
					std::string
				> result = GetDatatypeClass(typeName);
				if(!result) {
					return parse_number_expression(tokens, identifierRegister);
				}
				
				structClass = result.value();
			}

			(void)eat(tokens);
			(void)eat(tokens);

			if(structClass->isNative()) {
				return std::unexpected("Invalid type '" + structClass->GetName() + "' is a native datatype.");
			}
			auto customDatatypeClass = std::dynamic_pointer_cast<CustomDatatypeClass>(structClass);

			std::shared_ptr<Expression> memberIdentifierExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_identifier_expression,
				tokens,
				identifierRegister,
				memberIdentifierExpression
			)
			auto memberIdentifier = std::dynamic_pointer_cast<IdentifierExpression>(memberIdentifierExpression);
			
			std::expected<CustomDatatypeMember, std::string> result = customDatatypeClass->GetMember(
				memberIdentifier->GetName()
			);
			if(!result) {
				return std::unexpected("Invalid member '" + memberIdentifier->GetName() + "' for struct '" + structClass->GetName() + "'");
			}
			return std::make_shared<StructMemberAccessExpression>(
				std::make_shared<IdentifierExpression>(
					identifierRegister,
					structInstanceName
				),
				memberIdentifier,
				result.value().type->GetName()
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_fn_call_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			std::string potentialIdentifierName = tokens.front().value;
			std::vector<std::shared_ptr<FunctionDeclarationExpression>>::iterator funcIter = std::find_if(
				functions.begin(),
				functions.end(),
				[&potentialIdentifierName](const std::shared_ptr<FunctionDeclarationExpression>& function) {
					return function->GetIdentifier()->GetName() == potentialIdentifierName;
				}
			);
			if(
				tokens.front().type != lexer::TokenType::IDENTIFIER
				|| funcIter == functions.end()
			) {
				return parse_struct_member_access_expression(tokens, identifierRegister);
			}
			(void)eat(tokens);

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::PARENTHESIS, "(")
			)

			std::vector<std::shared_ptr<Expression>> arguments = {};
			while(
				tokens.size() > 0
				&& (
					tokens.front().type != lexer::TokenType::PARENTHESIS
					|| tokens.front().value != ")"
				)
			) {
				if(arguments.size() > 0) {
					__TRY_TOKEN_FUNC_WRETERR(
						expect,
						tokens,
						lexer::MakeToken(lexer::TokenType::COMMA)
					)
				}

				// TODO: Add datatypes check

				std::shared_ptr<Expression> argument;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					identifierRegister,
					argument
				)
				arguments.push_back(argument);
			}

			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file in function declaration expression.");
			}
			(void)eat(tokens);

			return std::make_shared<FunctionCallExpression>(
				(*funcIter)->GetIdentifier(),
				arguments,
				(*funcIter)->GetDatatype()
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_struct_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::IDENTIFIER) {
				return parse_fn_call_expression(tokens, identifierRegister);
			}

			std::shared_ptr<DatatypeClass> structClass;
			{
				std::expected<
					std::shared_ptr<DatatypeClass>,
					std::string
				> result = GetDatatypeClass(tokens.front().value);
				if(!result) {
					return parse_fn_call_expression(tokens, identifierRegister);
				}
				structClass = result.value();
			}
			(void)eat(tokens);

			if(structClass->isNative()) {
				return std::unexpected("Invalid type '" + structClass->GetName() + "' is a native datatype.");
			}
			auto customDatatypeClass = std::dynamic_pointer_cast<CustomDatatypeClass>(structClass);

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(
					lexer::TokenType::BRACE,
					"{"
				)
			)

			std::vector<std::shared_ptr<IdentifierExpression>> memberExpressions = {};

			while(tokens.size() > 0) {
				if(tokens.front().type == lexer::TokenType::BRACE && tokens.front().value == "}") {
					break;
				}
				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::DOT)
				)
				std::shared_ptr<Expression> memberIdentifierExpression;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_identifier_expression,
					tokens,
					identifierRegister,
					memberIdentifierExpression
				)
				auto memberIdentifier = std::dynamic_pointer_cast<IdentifierExpression>(memberIdentifierExpression);

				DatatypeStr structMemberType;
				{
					std::expected<
						CustomDatatypeMember,
						std::string
					> result = customDatatypeClass->GetMember(memberIdentifier->GetName());
					if(!result) {
						return std::unexpected(result.error());
					}
					structMemberType = result.value().type->GetName();
				}

				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::EQUALS)
				)

				std::shared_ptr<Expression> argument;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					identifierRegister,
					argument
				)

				if(argument->GetDatatype() != structMemberType) {
					std::expected<
						std::shared_ptr<Expression>,
						std::string
					> result = try_cast(argument, structMemberType);

					if(!result) {
						return std::unexpected(
							"Expected member '" + memberIdentifier->GetName()
							+ "' of type : " + structMemberType + ", got : " + argument->GetDatatype()
						);
					}

					argument = result.value();
				}
				
				memberIdentifier->SetDatatype(structMemberType);
				memberIdentifier->SetExpression(argument);

				memberExpressions.push_back(memberIdentifier);

				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::COMMA)
				)
			}
		
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file in struct declaration.");
			}
			(void)eat(tokens);

			return std::make_shared<StructExpression>(
				memberExpressions,
				std::make_shared<IdentifierExpression>(
					identifierRegister,
					structClass->GetName()
				)
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_break_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::BREAK) {
				return parse_struct_expression(tokens, identifierRegister);
			}
			(void)eat(tokens);

			std::shared_ptr<Expression> breakExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_expression,
				tokens,
				identifierRegister,
				breakExpression
			)

			if(
				breakExpression->Type() == NodeType::IDENTIFIER
				&& tokens.size() > 0
				&& tokens.front().type != lexer::TokenType::EOL
			) {
				std::shared_ptr<Expression> realBreakExpression;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					identifierRegister,
					realBreakExpression
				)
				return std::make_shared<BreakExpression>(
					realBreakExpression,
					std::dynamic_pointer_cast<IdentifierExpression>(
						breakExpression
					)
				);
			}

			return std::make_shared<BreakExpression>(
				breakExpression
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_return_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::RETURN) {
				return parse_break_expression(tokens, identifierRegister);
			}
			(void)eat(tokens);

			std::expected<
				std::shared_ptr<Expression>,
				std::string
			> result = parse_expression(
				tokens,
				identifierRegister
			);

			if(!result) {
				return std::make_shared<ReturnExpression>();
			}

			return std::make_shared<ReturnExpression>(
				result.value()
			);
		}

		std::expected<
			std::shared_ptr<OrExpression>,
			std::string
		> parse_or_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::OR) {
				return std::unexpected("No or expression found.");
			}
			(void)eat(tokens);

			std::shared_ptr<Expression> orExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_expression,
				tokens,
				identifierRegister,
				orExpression
			)

			return std::make_shared<OrExpression>(
				orExpression
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_parenthesis_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(
				tokens.front().type != lexer::TokenType::PARENTHESIS
				|| tokens.front().value != "("
			) {
				return parse_return_expression(tokens, identifierRegister);
			}

			__TRY_TOKEN_FUNC_WRETERR(
				eat,
				tokens
			)

			std::shared_ptr<Expression> inner;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_expression,
				tokens,
				identifierRegister,
				inner
			)

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(
					lexer::TokenType::PARENTHESIS,
					")"
				)
			)

			return inner;
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_unary_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(
				tokens.front().type != lexer::TokenType::UNARY_OPERATOR
				&& (
					tokens.front().type != lexer::TokenType::BINARY_OPERATOR
					|| tokens.front().value != "-"
				)
			) {
				return parse_parenthesis_expression(tokens, identifierRegister);
			}
			std::string operatorSymbol = eat(tokens).value().value;

			std::shared_ptr<Expression> term;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_unary_expression,
				tokens,
				identifierRegister,
				term
			)

			if(operatorSymbol == "-") {
				if(
					term->Type() != NodeType::I8
					&& term->Type() != NodeType::I16
					&& term->Type() != NodeType::I32
					&& term->Type() != NodeType::I64
					&& term->Type() != NodeType::U8
					&& term->Type() != NodeType::U16
					&& term->Type() != NodeType::U32
					&& term->Type() != NodeType::U64
					&& term->Type() != NodeType::F32
					&& term->Type() != NodeType::F64
					&& term->Type() != NodeType::F128
				) {
					return std::unexpected("Trying to apply - operator to non-number expression.");
				}
				std::string value = std::dynamic_pointer_cast<NumberExpression>(term)->GetValue();
				std::dynamic_pointer_cast<NumberExpression>(term)->SetValue(
					std::string("-") + value
				);
				return term;
			}

			return std::make_shared<UnaryExpression>(
				term,
				operatorSymbol
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_logical_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			std::shared_ptr<Expression> left;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_unary_expression,
				tokens,
				identifierRegister,
				left
			)

			while(
				tokens.size() != 0 &&
				tokens.front().type == lexer::TokenType::BINARY_OPERATOR &&
				(
					tokens.front().value == "<"
					|| tokens.front().value == ">"
					|| tokens.front().value == "<="
					|| tokens.front().value == ">="
					|| tokens.front().value == "&&"
					|| tokens.front().value == "||"
					|| tokens.front().value == "=="
					|| tokens.front().value == "!="
					|| tokens.front().value == "&"
					|| tokens.front().value == "|"
					|| tokens.front().value == "^"
				)
			) {
				std::string operatorSymbol = eat(tokens).value().value;

				std::shared_ptr<Expression> right;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_logical_expression,
					tokens,
					identifierRegister,
					right
				)

				if(
           left->GetDatatype() != "INFER"
           && right->GetDatatype() != "INFER"
           && !GetBinaryOutputDatatype(left->GetDatatype(), operatorSymbol, right->GetDatatype())
        ) {
					return std::unexpected(
						std::string("Got non-matching operands types : ")
						+ left->GetDatatype() + " and " + right->GetDatatype()
					);
				}

				left = std::make_shared<BinaryExpression>(
					left,
					operatorSymbol,
					right
				);
			}

			return left;
		}



		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_multiplicative_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			std::shared_ptr<Expression> left;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_logical_expression,
				tokens,
				identifierRegister,
				left
			)

			while(
				tokens.size() != 0 &&
				tokens.front().type == lexer::TokenType::BINARY_OPERATOR &&
				(tokens.front().value == "*" || tokens.front().value == "/")
			) {
				std::string operatorSymbol = eat(tokens).value().value;

				std::shared_ptr<Expression> right;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_multiplicative_expression,
					tokens,
					identifierRegister,
					right
				)
        
        if(
          left->GetDatatype() != "INFER"
          && right->GetDatatype() != "INFER"
          && !GetBinaryOutputDatatype(left->GetDatatype(), operatorSymbol, right->GetDatatype())
        ) {
					return std::unexpected(
						std::string("Got non-matching operand types : ")
						+ left->GetDatatype() + " and " + right->GetDatatype()
					);
				}

				left = std::make_shared<BinaryExpression>(
					left,
					operatorSymbol,
					right
				);
			}

			return left;
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_additive_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			std::shared_ptr<Expression> left;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_multiplicative_expression,
				tokens,
				identifierRegister,
				left
			)

			while(
				tokens.size() != 0 &&
				tokens.front().type == lexer::TokenType::BINARY_OPERATOR &&
				(tokens.front().value == "+" || tokens.front().value == "-")
			) {
				std::string operatorSymbol = eat(tokens).value().value;

				std::shared_ptr<Expression> right;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_multiplicative_expression,
					tokens,
					identifierRegister,
					right
				)

        if(
          left->GetDatatype() != "INFER"
          && right->GetDatatype() != "INFER"
          && !GetBinaryOutputDatatype(left->GetDatatype(), operatorSymbol, right->GetDatatype())
        ) {
					return std::unexpected(
						std::string("Got non-matching operand types : ")
						+ left->GetDatatype() + " and " + right->GetDatatype()
					);
				}

				left = std::make_shared<BinaryExpression>(
					left,
					operatorSymbol,
					right
				);
			}

			return left;
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_binary_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			return parse_additive_expression(tokens, identifierRegister);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_ifelse_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			const bool allow_if
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file in if-else structure.");
			}

			struct lexer::Token keyword;

			if(
				tokens.front().type == lexer::TokenType::IFELSE
				&& !allow_if && tokens.front().value == "if"
			) {
				return std::unexpected("Start of a new structure");
			}

			__TRY_TOKEN_FUNC_WRETERR_WSAVE(
				expect,
				keyword,
				tokens,
				lexer::MakeToken(lexer::TokenType::IFELSE)
			)

			std::shared_ptr<Expression> condition = nullptr;
			std::shared_ptr<Expression> scope;
			if(keyword.value != "else") {
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_parenthesis_expression,
					tokens,
					identifierRegister,
					condition
				)

				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_scope_expression,
					tokens,
					identifierRegister,
					scope
				)
			} else {
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					identifierRegister,
					scope
				)
				if(scope->Type() != NodeType::SCOPE) {
					scope = std::dynamic_pointer_cast<Expression>(
						std::make_shared<ScopeExpression>(
							std::vector<std::shared_ptr<Expression>>{ scope }
						)
					);
				}
			}

			return std::make_shared<IfElseExpression>(
				std::dynamic_pointer_cast<ScopeExpression>(scope),
				condition
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_ifelse_structure(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			auto innerRegister = std::make_shared<ScopeIdentifierRegister>(identifierRegister);

			std::expected<
				std::shared_ptr<Expression>,
				std::string
			> result = parse_ifelse_expression(tokens, innerRegister);

			if(!result) {
				return parse_binary_expression(tokens, identifierRegister);
			}

			std::vector<std::shared_ptr<IfElseExpression>> expressions = {};
			while(result) {
				expressions.push_back(
					std::dynamic_pointer_cast<IfElseExpression>(result.value())
				);
				if(!expressions.back()->GetCondition()) {
					break;
				}

				result = parse_ifelse_expression(tokens, innerRegister, false);
			}

			auto expressionsUp = std::vector<std::shared_ptr<Expression>>();
			for(std::shared_ptr<IfElseExpression> expr : expressions) {
				expressionsUp.push_back(expr);
			}

			if(!try_n_cast(expressionsUp)) {
				return std::unexpected("Different type if-else structure.");
			}

			return std::make_shared<IfElseStructure>(
				expressions
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_match_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			const bool allow_default
		) {
			std::shared_ptr<Expression> condition = nullptr;

			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file in match structure.");
			}

			if(tokens.front().type == lexer::TokenType::DISCARD) {
				if(!allow_default) {
					return std::unexpected("Got two default cases in match structure.");
				}
				(void)eat(tokens);
			} else if(
				tokens.front().type == lexer::TokenType::PARENTHESIS
				&& tokens.front().value == "("
			) {
				tokens.insert(
					tokens.begin() + 1,
					MakeToken(
						lexer::TokenType::DISCARD,
						"_"
					)
				);
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_parenthesis_expression,
					tokens,
					identifierRegister,
					condition
				)
			} else {
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					identifierRegister,
					condition
				)
			}

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::ARROW)
			)

			std::shared_ptr<Expression> scope;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_scope_expression,
				tokens,
				identifierRegister,
				scope
			)

			return std::make_shared<MatchExpression>(
				std::dynamic_pointer_cast<ScopeExpression>(scope),
				condition
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_match_structure(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::MATCH) {
				return parse_ifelse_structure(tokens, identifierRegister);
			}
			(void)eat(tokens);

			auto innerRegister = std::make_shared<ScopeIdentifierRegister>(identifierRegister);

			std::shared_ptr<Expression> selectorExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_parenthesis_expression,
				tokens,
				innerRegister,
				selectorExpression
			)

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(
					lexer::TokenType::BRACE,
					"{"
				)
			)

			std::vector<std::shared_ptr<MatchExpression>> expressions = {};
			bool got_default_case = false;

			while(
				tokens.size() > 0
				&& tokens.front().type != lexer::TokenType::BRACE
				&& tokens.front().value != "}"
			) {
				std::expected<
					std::shared_ptr<Expression>,
					std::string
				> result = parse_match_expression(tokens, innerRegister, !got_default_case);

				if(!result) {
					return std::unexpected(result.error());
				}

				expressions.push_back(
					std::dynamic_pointer_cast<MatchExpression>(
						result.value()
					)
				);
				if(!expressions.back()->GetCondition()) {
					got_default_case = true;
				}
			}
			(void)eat(tokens);

			auto expressionsUp = std::vector<std::shared_ptr<Expression>>();
			for(std::shared_ptr<MatchExpression> expr : expressions) {
				expressionsUp.push_back(expr);
			}

			if(!try_n_cast(expressionsUp)) {
				return std::unexpected("Different type match structure.");
			}

			return std::make_shared<MatchStructure>(
				selectorExpression,
				expressions
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_loop_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::LOOP) {
				return parse_match_structure(tokens, identifierRegister);
			}
			(void)eat(tokens);

			auto innerRegister = std::make_shared<ScopeIdentifierRegister>(identifierRegister);

			std::shared_ptr<Expression> initialExpression = nullptr;
			std::shared_ptr<Expression> condition = nullptr;
			std::shared_ptr<Expression> updateExpression = nullptr;

			if(
				tokens.size() > 0
				&& tokens.front().type == lexer::TokenType::PARENTHESIS
				&& tokens.front().value == "("
			) {
				(void)eat(tokens);
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					innerRegister,
					initialExpression
				)	

				if(tokens.size() > 0 && tokens.front().type == lexer::TokenType::EOL) {
					(void)eat(tokens);
					
					__TRY_EXPR_FUNC_WRETERR_WSAVE(
						parse_expression,
						tokens,
						innerRegister,
						condition
					)

					__TRY_TOKEN_FUNC_WRETERR(
						expect,
						tokens,
						lexer::MakeToken(lexer::TokenType::EOL)
					)

					__TRY_EXPR_FUNC_WRETERR_WSAVE(
						parse_expression,
						tokens,
						innerRegister,
						updateExpression
					)
				}
				else if(tokens.size() > 0 && tokens.front().type == lexer::TokenType::AT) {
					if(initialExpression->Type() != NodeType::IDENTIFIER) {
						return std::unexpected("Expected identifier in @ loop");
					}
					(void)eat(tokens);
					std::shared_ptr<Expression> startExpression;
					if(tokens.size() > 1 && tokens.front().type != lexer::TokenType::DOUBLE_DOT) {
						__TRY_EXPR_FUNC_WRETERR_WSAVE(
							parse_expression,
							tokens,
							innerRegister,
							startExpression
						)
						if(
							startExpression->GetDatatype() == "BOOLEAN"
							|| startExpression->GetDatatype() == "CHAR"
							|| startExpression->GetDatatype() == "STRING"
						) {
							return std::unexpected("Expected number value for @ loop's start expression.");
						}
					} else {
						startExpression = std::make_shared<I8Expression>(0);
					}
					__TRY_TOKEN_FUNC_WRETERR(
						expect,
						tokens,
						lexer::MakeToken(lexer::TokenType::DOUBLE_DOT)
					)
					std::shared_ptr<Expression> endExpression;
					__TRY_EXPR_FUNC_WRETERR_WSAVE(
						parse_expression,
						tokens,
						innerRegister,
						endExpression
					)
					if(
						endExpression->GetDatatype() == "BOOLEAN"
						|| endExpression->GetDatatype() == "CHAR"
						|| endExpression->GetDatatype() == "STRING"
					) {
						return std::unexpected("Expected number value for @ loop's end expression.");
					}
					std::string identifierName = std::dynamic_pointer_cast<IdentifierExpression>(
						initialExpression
					)->GetName();
					initialExpression = std::make_shared<DeclarationExpression>(
						std::dynamic_pointer_cast<IdentifierExpression>(initialExpression),
						startExpression,
						endExpression->GetDatatype(),
						false
					);

          if(innerRegister->Get(identifierName)) {
    				return std::unexpected("Variable name '" + identifierName + "' already exists");
    			}

	    		innerRegister->Register(
	    			IdentifierData{
	    				identifierName,
	    				false,
	    				endExpression->GetDatatype()
            }
	    		);

					auto identifierExpression = std::make_shared<IdentifierExpression>(
						innerRegister,
						identifierName
					);

					updateExpression = std::make_shared<AssignationExpression>(
						identifierExpression,
						std::make_shared<BinaryExpression>(
							identifierExpression,
							"+",
              std::make_shared<I8Expression>(1)
						)
					);
					condition = std::make_shared<BinaryExpression>(
						identifierExpression,
						"<=",
						endExpression
					);
				}

				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::PARENTHESIS, ")")
				)
			}

			std::shared_ptr<Expression> scopeExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_scope_expression,
				tokens,
				innerRegister,
				scopeExpression
			)
			auto scope = std::dynamic_pointer_cast<ScopeExpression>(scopeExpression);

			std::shared_ptr<OrExpression> orExpression;

			if(initialExpression != nullptr) {
				__TRY_EXPECTED_FUNC_WRETERR_WSAVE(
					parse_or_expression,
					std::shared_ptr<OrExpression>,
					std::string,
					orExpression,
					tokens,
					innerRegister
				)

				if(
					!try_n_cast(std::vector({
						orExpression->GetExpression(),
						scope->GetExpressions().back()
					}))
				) {
					return std::unexpected(
						std::string("Got non-matching operand types : ")
						+ orExpression->GetDatatype() + " and " + scope->GetDatatype()
					);
				}
			} else {
				return std::make_shared<LoopExpression>(scope);
			}

			if(condition) {
				return std::make_shared<ForLoopExpression>(
					std::dynamic_pointer_cast<ScopeExpression>(scope),
					initialExpression,
					condition,
					updateExpression,
					orExpression
				);
			}

			return std::make_shared<WhileLoopExpression>(
				scope,
				initialExpression,
				orExpression
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_assignation_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			// TODO: Add struct member access as a possiblity
			if(
				tokens.front().type != lexer::TokenType::IDENTIFIER
      ) {
        return parse_loop_expression(tokens, identifierRegister);
      }
      size_t memberOffset = 0;
      if(
        tokens.at(1).type == lexer::TokenType::DOT
        && tokens.at(2).type == lexer::TokenType::IDENTIFIER
      ) {
        memberOffset = 2;
      }
      if(tokens.size() <= 2 + memberOffset) {
        return parse_loop_expression(tokens, identifierRegister);
      }
			if(
			  tokens.at(1 + memberOffset).type != lexer::TokenType::EQUALS
				&& (
					tokens.at(1 + memberOffset).type != lexer::TokenType::BINARY_OPERATOR
					|| tokens.at(2 + memberOffset).type != lexer::TokenType::EQUALS
				)
				&& (
					tokens.at(1 + memberOffset).type != lexer::TokenType::UNARY_OPERATOR
					|| (tokens.at(1 + memberOffset).value != "++" && tokens.at(1).value == "--")
				)
			) {
				return parse_loop_expression(tokens, identifierRegister);
			}

			std::shared_ptr<AssignableExpression> assignable;
      std::string name;
      if(memberOffset == 0) {
        assignable = std::dynamic_pointer_cast<IdentifierExpression>(
          parse_identifier_expression(
		  		  tokens,
		  		  identifierRegister
		  	  ).value()
        );
        name = std::dynamic_pointer_cast<IdentifierExpression>(assignable)->GetName();
      } else {
        assignable = std::dynamic_pointer_cast<StructMemberAccessExpression>(
          parse_struct_member_access_expression(
            tokens,
            identifierRegister
          ).value()
        );
        LOG(assignable->Repr());
        name = std::dynamic_pointer_cast<StructMemberAccessExpression>(assignable)->GetStruct()->GetName();
      }

			std::string unaryOperator = (tokens.size() > 0 && tokens.front().type == lexer::TokenType::UNARY_OPERATOR)
				? std::string(1, eat(tokens).value().value.at(1))
				: "";

			std::string operatorSymbol = "";
			std::shared_ptr<Expression> expression;

			if(unaryOperator == "") {
				if(tokens.size() > 0 && tokens.front().type == lexer::TokenType::BINARY_OPERATOR) {
					operatorSymbol = eat(tokens).value().value;
				}

				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::EQUALS)
				)

				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					identifierRegister,
					expression
				)
			}

			if(!identifierRegister->Get(name)) {
				return std::unexpected("Variable name '" + name + "' does not exist yet");
			}

			if(unaryOperator != "") {
				expression = std::make_shared<BinaryExpression>(
					assignable,
					unaryOperator,
					std::make_shared<I8Expression>(1)
				);
			}

			if(operatorSymbol != "") {
				expression = std::make_shared<BinaryExpression>(
					assignable,
					operatorSymbol,
					expression
				);
			}

      {
        std::expected<
          Success,
          std::string
        > result = assignable->TryAssign(
          identifierRegister,
          expression
        );
        if(!result) {
          return std::unexpected(result.error());
        }
      }

			return std::make_shared<AssignationExpression>(
				assignable,
				expression
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_declaration_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::DECL) {
				return parse_assignation_expression(tokens, identifierRegister);
			}

			bool isConst = eat(tokens).value().value == "const";

			std::shared_ptr<Expression> identifierExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_identifier_expression,
				tokens,
				identifierRegister,
				identifierExpression
			)

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::COLON)
			)

			DatatypeStr datatype = "INFER";
			if(
				tokens.size() > 0 && (
					tokens.front().type == lexer::TokenType::TYPE
					|| (
						tokens.front().type == lexer::TokenType::IDENTIFIER
						&& GetDatatypeClass(tokens.front().value)
					)
				)
			) {
				datatype = eat(tokens).value().value;
			}

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::EQUALS)
			)

			std::shared_ptr<Expression> expression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_expression,
				tokens,
				identifierRegister,
				expression
			)

			DatatypeStr gotDatatype = expression->GetDatatype();

			if(datatype == "INFER") {
				datatype = gotDatatype;
			} else if(datatype != gotDatatype) {
				std::expected<
					std::shared_ptr<Expression>,
					std::string
				> result = try_cast(expression, datatype);

				if(!result) {
					return std::unexpected(
						std::string("Expected type ")
						+ datatype + ", got " + gotDatatype
					);
				}

				expression = result.value();
			}

			auto identifier = std::dynamic_pointer_cast<IdentifierExpression>(
				identifierExpression
			);

			if(identifierRegister->Get(identifier->GetName())) {
				return std::unexpected("Variable name '" + identifier->GetName() + "' already exists");
			}

			identifierRegister->Register(
				IdentifierData{
					identifier->GetName(),
					isConst,
					datatype
				}
			);

			return std::make_shared<DeclarationExpression>(
				identifier,
				expression,
				datatype,
				isConst
			);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_fn_declaration_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file.");
			}
			if(tokens.front().type != lexer::TokenType::FN) {
				return parse_declaration_expression(tokens, identifierRegister);
			}

			auto innerRegister = std::make_shared<ScopeIdentifierRegister>(identifierRegister);

			(void)eat(tokens);

			std::shared_ptr<Expression> identifierExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_identifier_expression,
				tokens,
				identifierRegister,
				identifierExpression
			)
			auto identifier = std::dynamic_pointer_cast<IdentifierExpression>(identifierExpression);

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(
					lexer::TokenType::PARENTHESIS,
					"("
				)
			)

			std::vector<std::shared_ptr<DeclarationExpression>> arguments = {};

			while(
				tokens.size() > 0
				&& (
					tokens.front().type != lexer::TokenType::PARENTHESIS
					|| tokens.front().value != ")"
				)
			) {
				if(arguments.size() > 0) {
					__TRY_TOKEN_FUNC_WRETERR(
						expect,
						tokens,
						lexer::MakeToken(lexer::TokenType::COMMA)
					)
				}

				std::shared_ptr<Expression> argumentIdentifierExpression;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_identifier_expression,
					tokens,
					innerRegister,
					argumentIdentifierExpression
				)
				auto argumentIdentifier = std::dynamic_pointer_cast<IdentifierExpression>(argumentIdentifierExpression);
				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::COLON)
				)
				DatatypeStr argumentDatatype = "";
				__TRY_EXPECTED_FUNC_WRETERR_WSAVE(
					expect_type,
					std::string,
					std::string,
					argumentDatatype,
					tokens
				)

				// TODO: Add const arguments
				arguments.push_back(
					std::make_shared<DeclarationExpression>(
						argumentIdentifier,
						nullptr,
						argumentDatatype,
						false
					)
				);
			}

			if(tokens.size() == 0) {
				return std::unexpected("Unexpected end of file in function declaration expression.");
			}
			(void)eat(tokens);

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::ARROW)
			)

			DatatypeStr returnDatatype = "";
			__TRY_EXPECTED_FUNC_WRETERR_WSAVE(
				expect_type,
				std::string,
				std::string,
				returnDatatype,
				tokens
			)

			functions.push_back(
				std::make_shared<FunctionDeclarationExpression>(
					identifier,
					arguments,
					nullptr,
					returnDatatype
				)
			);

			std::shared_ptr<Expression> scopeExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_scope_expression,
				tokens,
				innerRegister,
				scopeExpression
			)
			functions.back()->SetScope(
				std::dynamic_pointer_cast<ScopeExpression>(scopeExpression)
			);

			return functions.back();
		}

    std::expected<
      std::shared_ptr<Expression>,
      std::string
    > parse_struct_declaration_expression(
      std::vector<struct lexer::Token>& tokens,
      std::shared_ptr<ScopeIdentifierRegister> identifierRegister
    ) {
		if(tokens.size() == 0) {
			return std::unexpected("Unexpected end of file.");
		}
		if(tokens.front().type != lexer::TokenType::STRUCT) {
			return parse_fn_declaration_expression(tokens, identifierRegister);
		}
		(void)eat(tokens);

		__TRY_TOKEN_FUNC_WRETERR(
			expect,
			tokens,
			lexer::MakeToken(
				lexer::TokenType::BRACE,
				"{"
			)
		)

		std::vector<std::shared_ptr<IdentifierExpression>> memberExpressions = {};

		while(tokens.size() > 0) {
			if(tokens.front().type == lexer::TokenType::BRACE && tokens.front().value == "}") {
				break;
			}
			std::shared_ptr<Expression> memberIdentifierExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_identifier_expression,
				tokens,
				identifierRegister,
				memberIdentifierExpression
			)
			auto memberIdentifier = std::dynamic_pointer_cast<IdentifierExpression>(memberIdentifierExpression);

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::COLON)
			)

			DatatypeStr argumentDatatype = "";
			__TRY_EXPECTED_FUNC_WRETERR_WSAVE(
				expect_type,
				std::string,
				std::string,
				argumentDatatype,
				tokens
			)

			if(!GetDatatypeClass(argumentDatatype)) {
				return std::unexpected("Invalid datatype in struct declaration : " + argumentDatatype);
			}

			memberIdentifier->SetDatatype(argumentDatatype);
			memberExpressions.push_back(memberIdentifier);

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::EOL)
			)
		}
		if(tokens.size() == 0) {
			return std::unexpected("Unexpected end of file in struct declaration.");
		}
		(void)eat(tokens);

		std::shared_ptr<Expression> structIdentifierExpression;
		__TRY_EXPR_FUNC_WRETERR_WSAVE(
			parse_identifier_expression,
			tokens,
			identifierRegister,
			structIdentifierExpression
		)
		auto structIdentifier = std::dynamic_pointer_cast<IdentifierExpression>(structIdentifierExpression);

		datatypes.push_back(
			std::make_shared<CustomDatatypeClass>(
				structIdentifier->GetName(),
				dim::utils::to_unordered_set<CustomDatatypeMember>(
					dim::utils::map<std::shared_ptr<IdentifierExpression>, CustomDatatypeMember>(
						memberExpressions,
						[](const std::shared_ptr<IdentifierExpression>& member) {
							return CustomDatatypeMember{
								.type = GetDatatypeClass(member->GetDatatype()).value(),
								.name = member->GetName()
							};
						}
	    			)
    			)
    		)
		);

		return std::make_shared<StructDeclarationExpression>(
			memberExpressions,
			structIdentifier
		);
    }

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_scope_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() <= 1) {
				return std::unexpected("Unexpected end of file.");
			}
			if(
				(
					tokens.front().type != lexer::TokenType::BRACE
					|| tokens.front().value != "{"
				)
				&& (
					tokens.front().type != lexer::TokenType::IDENTIFIER
					|| GetDatatypeClass(tokens.front().value)
					|| tokens.at(1).type != lexer::TokenType::BRACE
					|| tokens.at(1).value != "{"
				)
			) {
				return parse_struct_declaration_expression(tokens, identifierRegister);
			}

			std::shared_ptr<Expression> scopeName = nullptr;
			if(
				tokens.size() > 0
				&& tokens.front().type != lexer::TokenType::BRACE
				&& tokens.front().value != "{"
			) {
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_identifier_expression,
					tokens,
					identifierRegister,
					scopeName
				)
			}

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::BRACE, "{")
			)

			auto scope = std::make_shared<ScopeExpression>();
			scope->SetName(std::dynamic_pointer_cast<IdentifierExpression>(scopeName));
			auto innerRegister = std::make_shared<ScopeIdentifierRegister>(identifierRegister);

			bool closingBraceFound = false;

			while(tokens.size() > 0) {

				if(
					tokens.front().type == lexer::TokenType::BRACE
					&& tokens.front().value == "}"
				) {
					(void)eat(tokens);
					closingBraceFound = true;
					break;
				}

				std::shared_ptr<Expression> expr;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					identifierRegister,
					expr
				)

				scope->AddExpression(
					expr
				);
				
				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::EOL)
				)
			}

			if(!closingBraceFound) {
				__TRY_TOKEN_FUNC_WRETERR(
					expect,
					tokens,
					lexer::MakeToken(lexer::TokenType::BRACE, "}")
				)
			}

			return scope;
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			return parse_scope_expression(tokens, identifierRegister);
		}

		std::expected<
			std::shared_ptr<ScopeExpression>,
			std::string
		> Parse(
			std::vector<struct lexer::Token>& tokens
		) {
			auto identifierRegister = std::make_shared<ScopeIdentifierRegister>();

			auto scope = std::make_shared<ScopeExpression>();

			while(tokens.size() > 0) {
				std::expected<
					std::shared_ptr<Expression>,
					std::string
				> result = parse_expression(
					tokens,
					identifierRegister
				);

				if(!result) {
					return std::unexpected(
						std::string("[ERR::PARSER] Got error :\n\t\"") + result.error()
						+ "\"\nwhile parsing tokens."
					);
				}

				scope->AddExpression(
					result.value()
				);

				if(result.value()->Type() != NodeType::FN) {
					__TRY_TOKEN_FUNC_WRETERR(
						expect,
						tokens,
						lexer::MakeToken(lexer::TokenType::EOL)
					)
				}
			}

			return scope;
		}
	}
}
