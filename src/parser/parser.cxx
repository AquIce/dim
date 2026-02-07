#include <parser/parser.hxx>

namespace dim {
	namespace parser {

		std::vector<std::shared_ptr<FunctionDeclarationExpression>> functions = {};

		[[nodiscard]]
		Result<struct lexer::Token> eat(
			std::vector<struct lexer::Token>& tokens
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Trying to eat a token that in empty token list.",
					.type = utils::ErrorType::DEV_UNREACHABLE,
				});
			}
			struct lexer::Token tk = tokens.front();
			tokens.erase(tokens.begin());
			return tk;
		}

		[[nodiscard]]
		Result<struct lexer::Token> expect(
			std::vector<struct lexer::Token>& tokens,
			const struct lexer::Token expected
		) {

			Result<struct lexer::Token> result = eat(tokens);

			if(!result) {
				return std::unexpected(result.error());
			}

			struct lexer::Token tk = result.value();

			if(tk.type != expected.type && expected.value == "") {
				tokens.insert(tokens.begin(), tk);
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = std::string("Invalid token type: got ")
						+ std::string(lexer::TokenTypeStr.at(int(tk.type))) + " expected "
						+ std::string(lexer::TokenTypeStr.at(int(expected.type)))
						+ " (" + tk.value + ")",
					.type = utils::ErrorType::ERROR,
				});
			}

			if(tk.type != expected.type && tk.value != expected.value) {
				tokens.insert(tokens.begin(), tk);
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message =
						std::string("Invalid token value: got ")
						+ tk.value + " expected " + expected.value
						+ std::to_string(tk.ctx.line) + ":"
						+ std::to_string(tk.ctx.column),
					.type = utils::ErrorType::ERROR,
				});
			}

			return tk;
		}

		[[nodiscard]]
		Result<std::string> expect_type(
			std::vector<struct lexer::Token>& tokens
		) {
			Result<struct lexer::Token> result = eat(tokens);
			if(!result) {
				return std::unexpected(result.error());
			}
			const struct lexer::Token tk = result.value();

			if(
				tk.type == lexer::TokenType::TYPE
				|| (
					tk.type == lexer::TokenType::IDENTIFIER
					&& GetDatatypeClass(tk.value)
				)
			) {
				return tk.value;
			}
			
			return std::unexpected(utils::Error{
				.ctx = tk.ctx,
				.message = "Expected type, found " + lexer::TokenRepr(
					tokens.front()
				),
				.type = utils::ErrorType::ERROR
			});
		}

		Result<std::shared_ptr<Expression>> parse_identifier_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}

			struct utils::Context ctx = tokens.front().ctx;

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
					ctx,
					identifierRegister,
					identifier.value
				);
			}

			return std::make_shared<IdentifierExpression>(
				ctx,
				identifierRegister,
				result.value().name,
				result.value().isConst
			);
		}

		Result<std::shared_ptr<Expression>> parse_discard_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::DISCARD) {
				return parse_identifier_expression(tokens, identifierRegister);
			}

			struct utils::Context ctx = tokens.front().ctx;
			(void)eat(tokens);
			return std::make_shared<DiscardExpression>(ctx);
		}

		Result<std::shared_ptr<Expression>> parse_null_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::NUL) {
				return parse_discard_expression(tokens, identifierRegister);
			}

			struct utils::Context ctx = tokens.front().ctx;
			(void)eat(tokens);
			return std::make_shared<NullExpression>(ctx);
		}

		Result<std::shared_ptr<Expression>> parse_boolean_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::BOOLEAN) {
				return parse_null_expression(tokens, identifierRegister);
			}

			struct utils::Context ctx = tokens.front().ctx;
			return std::make_shared<BooleanExpression>(
				ctx,
				eat(tokens).value().value
			);
		}

		Result<std::shared_ptr<Expression>> parse_char_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::CHAR) {
				return parse_boolean_expression(tokens, identifierRegister);
			}

			struct utils::Context ctx = tokens.front().ctx;
			return std::make_shared<CharExpression>(
				ctx,
				eat(tokens).value().value
			);
		}

		Result<std::shared_ptr<Expression>> parse_string_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::STRING) {
				return parse_char_expression(tokens, identifierRegister);
			}

			struct utils::Context ctx = tokens.front().ctx;
			return std::make_shared<StringExpression>(
				ctx,
				eat(tokens).value().value
			);
		}

		Result<std::shared_ptr<Expression>> parse_number_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::NUMBER) {
				return parse_string_expression(tokens, identifierRegister);
			}

			struct utils::Context autoCastCtx = tokens.front().ctx;
			
			std::expected<
				std::shared_ptr<Expression>,
				std::string
			> result = auto_cast(
				std::make_shared<NumberExpression>(
					autoCastCtx,
					eat(tokens).value().value
				)
			);

			if(!result) {
				return std::unexpected(utils::Error{
					.ctx = autoCastCtx,
					.message = result.error(),
					.type = utils::ErrorType::ERROR,
				});
			}
			return result.value();
		}

		Result<std::shared_ptr<Expression>> parse_struct_member_access_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(
				tokens.front().type != lexer::TokenType::IDENTIFIER
				|| tokens.size() < 2
				|| tokens.at(1).type != lexer::TokenType::DOT
			) {
				return parse_number_expression(tokens, identifierRegister);
			}

			std::string typeName;
			struct utils::Context structCtx = tokens.front().ctx;
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
				return std::unexpected(utils::Error{
					.ctx = tokens.front().ctx,
					.message = "Invalid type '" + structClass->GetName() + "' is a native datatype.",
					.type = utils::ErrorType::ERROR,
				});
			}
			auto customDatatypeClass = std::dynamic_pointer_cast<CustomDatatypeClass>(structClass);

			struct utils::Context memberIdentifierCtx = tokens.front().ctx;
			std::shared_ptr<Expression> memberIdentifierExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_identifier_expression,
				tokens,
				identifierRegister,
				memberIdentifierExpression
			)
			auto memberIdentifier = std::dynamic_pointer_cast<IdentifierExpression>(memberIdentifierExpression);
			
			if(tokens.front().type == lexer::TokenType::PARENTHESIS && tokens.front().value == "(") {
				(void)eat(tokens);

				struct utils::Context ctx = tokens.front().ctx;
				CustomDatatypeMemberFunction memberFunction;
				{
					std::expected<CustomDatatypeMemberFunction, std::string> result = customDatatypeClass->GetMemberFunction(
						memberIdentifier->GetName()
					);

					if(!result) {
						return std::unexpected(utils::Error{
							.ctx = memberIdentifierCtx,
							.message =
								std::string("Invalid member function '")
								+ memberIdentifier->GetName() + "' for struct '"
								+ structClass->GetName() + "'",
							.type = utils::ErrorType::ERROR,
						});
					}
					memberFunction = result.value();
				}

				std::vector<std::shared_ptr<Expression>> arguments = {};
				size_t index = 0;
				while(
					tokens.size() > 0
					&& (
						tokens.front().type != lexer::TokenType::PARENTHESIS
						|| tokens.front().value != ")"
					)
				) {
					if(index >= memberFunction.argumentsTypes.size()) {
						return std::unexpected(utils::Error{
							.ctx = ctx,
							.message =
								std::string("Too many arguments (" + std::to_string(index)
								+ ") provided to function ") + memberFunction.name,
							.type = utils::ErrorType::RETERR,
						});
					}
					if(arguments.size() > 0) {
						__TRY_TOKEN_FUNC_WRETERR(
							expect,
							tokens,
							lexer::MakeToken(lexer::TokenType::COMMA)
						)
					}

					std::shared_ptr<Expression> argument;
					__TRY_EXPR_FUNC_WRETERR_WSAVE(
						parse_expression,
						tokens,
						identifierRegister,
						argument
					)
					ctx = argument->ctx;

					// TODO: Add arguments names to memberFunction
			
					if(!parser::isConvertible(argument->GetDatatype(), memberFunction.argumentsTypes.at(index))) {
						return std::unexpected(utils::Error{
							.ctx = ctx,
							.message =
								std::string("Invalid argument datatype ")
								+ argument->GetDatatype()
								+ " (expected " + memberFunction.argumentsTypes.at(index)
								+ ") on argument `" + "[TODO] "+ "` when calling function `"
								+ customDatatypeClass->GetName() + "."
								+ memberFunction.name + "`",
							.type = utils::ErrorType::RETERR,
						});
					}
					index++;

					arguments.push_back(argument);
				}

				if(tokens.size() == 0) {
					return std::unexpected(utils::Error{
						.ctx = utils::Context{ .line = 0, .column = 0 },
						.message = "Unexpected end of file in function declaration.",
						.type = utils::ErrorType::RETERR,
					});
				}
				
				ctx = tokens.front().ctx;
				(void)eat(tokens);

				return std::make_shared<StructMemberFunctionAccessExpression>(
					memberIdentifierCtx,
					std::make_shared<IdentifierExpression>(
						structCtx,
						identifierRegister,
						structInstanceName
					),
					memberIdentifier,
					arguments,
					memberFunction.returnType
				);
			}

			std::expected<CustomDatatypeMember, std::string> result = customDatatypeClass->GetMember(
				memberIdentifier->GetName()
			);

			if(!result) {
				return std::unexpected(utils::Error{
					.ctx = memberIdentifierCtx,
					.message =
						std::string("Invalid member '") + memberIdentifier->GetName()
						+ "' for struct '" + structClass->GetName() + "'",
					.type = utils::ErrorType::ERROR,
				});
			}
			
			return std::make_shared<StructMemberAccessExpression>(
				memberIdentifierCtx,
				std::make_shared<IdentifierExpression>(
					structCtx,
					identifierRegister,
					structInstanceName
				),
				memberIdentifier,
				result.value().type->GetName()
			);
		}

		Result<std::shared_ptr<Expression>> parse_fn_call_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
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

			std::vector<std::shared_ptr<DeclarationExpression>> wantedArgs = (*funcIter)->GetArguments();
			std::vector<std::shared_ptr<Expression>> arguments = {};
			size_t index = 0;
			struct utils::Context ctx = (*funcIter)->ctx;

			while(
				tokens.size() > 0
				&& (
					tokens.front().type != lexer::TokenType::PARENTHESIS
					|| tokens.front().value != ")"
				)
			) {
				if(index >= wantedArgs.size()) {
					return std::unexpected(utils::Error{
						.ctx = ctx,
						.message = std::string("Too many arguments (" + std::to_string(index) + ") provided to function ") + (*funcIter)->GetIdentifier()->GetName(),
						.type = utils::ErrorType::RETERR,
					});
				}

				if(arguments.size() > 0) {
					__TRY_TOKEN_FUNC_WRETERR(
						expect,
						tokens,
						lexer::MakeToken(lexer::TokenType::COMMA)
					)
				}

				std::shared_ptr<Expression> argument;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					identifierRegister,
					argument
				)
				ctx = argument->ctx;

				if(!parser::isConvertible(argument->GetDatatype(), wantedArgs.at(index)->GetDatatype())) {
					return std::unexpected(utils::Error{
						.ctx = ctx,
						.message =
							std::string("Invalid argument datatype ")
							+ argument->GetDatatype()
							+ " (expected " + (*funcIter)->GetIdentifier()->GetName()
							+ ") on argument `" + wantedArgs.at(index)->GetIdentifier()->GetName() + "` when calling function `"
							+ (*funcIter)->GetIdentifier()->GetName() + "`",
						.type = utils::ErrorType::RETERR,
					});
				}
				index++;

				arguments.push_back(argument);
			}

			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file in function declaration.",
					.type = utils::ErrorType::RETERR,
				});
			}
			ctx = tokens.front().ctx;
			(void)eat(tokens);

			return std::make_shared<FunctionCallExpression>(
				ctx,
				(*funcIter)->GetIdentifier(),
				arguments,
				(*funcIter)->GetDatatype()
			);
		}

		Result<std::shared_ptr<Expression>> parse_struct_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::IDENTIFIER) {
				return parse_fn_call_expression(tokens, identifierRegister);
			}

			struct utils::Context structClassCtx = tokens.front().ctx;
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
				return std::unexpected(utils::Error{
					.ctx = structClassCtx,
					.message = "Invalid type '" + structClass->GetName() + "' is a native datatype.",
					.type = utils::ErrorType::ERROR,
				});
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
						return std::unexpected(utils::Error{
							.ctx = tokens.front().ctx,
							.message = result.error(),
							.type = utils::ErrorType::ERROR,
						});
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
						return std::unexpected(utils::Error{
							.ctx = tokens.front().ctx,
							.message =
								std::string("Expected member '") + memberIdentifier->GetName()
								+ "' of type : " + structMemberType + ", got : " + argument->GetDatatype(),
							.type = utils::ErrorType::ERROR,
						});
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
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file in struct declaration.",
					.type = utils::ErrorType::RETERR,
				});
			}
			(void)eat(tokens);

			return std::make_shared<StructExpression>(
				structClassCtx,
				memberExpressions,
				std::make_shared<IdentifierExpression>(
					structClassCtx,
					identifierRegister,
					structClass->GetName()
				)
			);
		}

		Result<std::shared_ptr<Expression>> parse_break_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::BREAK) {
				return parse_struct_expression(tokens, identifierRegister);
			}
			struct utils::Context ctx = tokens.front().ctx;
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
					ctx,
					realBreakExpression,
					std::dynamic_pointer_cast<IdentifierExpression>(
						breakExpression
					)
				);
			}

			return std::make_shared<BreakExpression>(
				ctx,
				breakExpression
			);
		}

		Result<std::shared_ptr<Expression>> parse_return_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::RETURN) {
				return parse_break_expression(tokens, identifierRegister);
			}
			struct utils::Context ctx = tokens.front().ctx;
			(void)eat(tokens);

			if(tokens.front().type == lexer::TokenType::EOL) {
				return std::make_shared<ReturnExpression>(ctx);
			}

			Result<std::shared_ptr<Expression>> result = parse_expression(
				tokens,
				identifierRegister
			);

			if(!result) {
				return std::unexpected(result.error());
			}

			return std::make_shared<ReturnExpression>(
				ctx,
				result.value()
			);
		}

		Result<std::shared_ptr<OrExpression>> parse_or_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::OR) {
				return std::unexpected(utils::Error{
					.ctx = tokens.front().ctx,
					.message = "No or expression found.",
					.type = utils::ErrorType::ERROR,
				});
			}
			struct utils::Context ctx = tokens.front().ctx;
			(void)eat(tokens);

			std::shared_ptr<Expression> orExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_expression,
				tokens,
				identifierRegister,
				orExpression
			)

			return std::make_shared<OrExpression>(
				ctx,
				orExpression
			);
		}

		Result<std::shared_ptr<Expression>> parse_parenthesis_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
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

		Result<std::shared_ptr<Expression>> parse_unary_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
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

			struct utils::Context termCtx = tokens.front().ctx;
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
					return std::unexpected(utils::Error{
						.ctx = termCtx,
						.message = "Trying to apply - operator to non-number expression.",
						.type = utils::ErrorType::ERROR,
					});
				}
				std::string value = std::dynamic_pointer_cast<NumberExpression>(term)->GetValue();
				std::dynamic_pointer_cast<NumberExpression>(term)->SetValue(
					std::string("-") + value
				);
				return term;
			}

			return std::make_shared<UnaryExpression>(
				termCtx,
				term,
				operatorSymbol
			);
		}

		Result<std::shared_ptr<Expression>> parse_logical_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}

			struct utils::Context leftCtx = tokens.front().ctx;
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

				struct utils::Context rightCtx = tokens.front().ctx;
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
					&& !GetBinaryOutputDatatype(
						left->GetDatatype(),
						operatorSymbol,
						right->GetDatatype()
					)
				) {
					return std::unexpected(utils::Error{
						.ctx = rightCtx,
						.message =
							std::string("Got non-matching operands types : ")
							+ left->Repr() + " and " + right->Repr(),
						.type = utils::ErrorType::ERROR,
					});
				}

				left = std::make_shared<BinaryExpression>(
					leftCtx,
					left,
					operatorSymbol,
					right
				);
			}

			return left;
		}



		Result<std::shared_ptr<Expression>> parse_multiplicative_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
				.ctx = utils::Context{ .line = 0, .column = 0 },
				.message = "Unexpected end of file.",
				.type = utils::ErrorType::RETERR,
				});
			}
			struct utils::Context leftCtx = tokens.front().ctx;
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

				struct utils::Context rightCtx = tokens.front().ctx;
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
					return std::unexpected(utils::Error{
						.ctx = rightCtx,
						.message =
							std::string("Got non-matching operand types : ")
							+ left->GetDatatype() + " and " + right->GetDatatype(),
						.type = utils::ErrorType::ERROR,
					});
				}

				left = std::make_shared<BinaryExpression>(
					leftCtx,
					left,
					operatorSymbol,
					right
				);
			}

			return left;
		}

		Result<std::shared_ptr<Expression>> parse_additive_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			struct utils::Context leftCtx = tokens.front().ctx;
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

				struct utils::Context rightCtx = tokens.front().ctx;
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
					return std::unexpected(utils::Error{
						.ctx = rightCtx,
						.message =
							std::string("Got non-matching operand types : ")
							+ left->GetDatatype() + " and " + right->GetDatatype(),
						.type = utils::ErrorType::ERROR,
					});
				}

				left = std::make_shared<BinaryExpression>(
					leftCtx,
					left,
					operatorSymbol,
					right
				);
			}

			return left;
		}

		Result<std::shared_ptr<Expression>> parse_binary_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			return parse_additive_expression(tokens, identifierRegister);
		}

		Result<std::shared_ptr<Expression>> parse_ifelse_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			const bool allow_if
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file in if-else structure.",
					.type = utils::ErrorType::RETERR,
				});
			}

			struct lexer::Token keyword;

			if(
				tokens.front().type == lexer::TokenType::IFELSE
				&& !allow_if && tokens.front().value == "if"
			) {
				return std::unexpected(utils::Error{
					.ctx = tokens.front().ctx,
					.message = "Start of a new if-else structure",
					.type = utils::ErrorType::RETERR,
				});
			}

			struct utils::Context ctx = tokens.front().ctx;
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
				struct utils::Context ctx = tokens.front().ctx;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_expression,
					tokens,
					identifierRegister,
					scope
				)
				if(scope->Type() != NodeType::SCOPE) {
					scope = std::dynamic_pointer_cast<Expression>(
						std::make_shared<ScopeExpression>(
							ctx,
							std::vector<std::shared_ptr<Expression>>{ scope }
						)
					);
				}
			}

			return std::make_shared<IfElseExpression>(
				ctx,
				std::dynamic_pointer_cast<ScopeExpression>(scope),
				condition
			);
		}

		Result<std::shared_ptr<Expression>> parse_ifelse_structure(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			auto innerRegister = std::make_shared<ScopeIdentifierRegister>(identifierRegister);

			struct utils::Context ifElseExpressionCtx = tokens.front().ctx;
			Result<std::shared_ptr<Expression>> result = parse_ifelse_expression(tokens, innerRegister);

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
				return std::unexpected(utils::Error{
					.ctx = ifElseExpressionCtx,
					.message = "Different type if-else structure.",
					.type = utils::ErrorType::ERROR,
				});
			}

			return std::make_shared<IfElseStructure>(
				ifElseExpressionCtx,
				expressions
			);
		}

		Result<std::shared_ptr<Expression>> parse_match_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
			const bool allow_default
		) {
			std::shared_ptr<Expression> condition = nullptr;

			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file in match structure.",
					.type = utils::ErrorType::RETERR,
				});
			}

			struct utils::Context ctx = tokens.front().ctx;
			if(tokens.front().type == lexer::TokenType::DISCARD) {
				if(!allow_default) {
					return std::unexpected(utils::Error{
						.ctx = tokens.front().ctx,
						.message = "Got two default cases in match structure.",
						.type = utils::ErrorType::ERROR,
					});
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
				ctx,
				std::dynamic_pointer_cast<ScopeExpression>(scope),
				condition
			);
		}

		Result<std::shared_ptr<Expression>> parse_match_structure(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::MATCH) {
				return parse_ifelse_structure(tokens, identifierRegister);
			}
			struct utils::Context ctx = tokens.front().ctx;
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
			struct utils::Context matchExpressionCtx;

			while(
				tokens.size() > 0
				&& tokens.front().type != lexer::TokenType::BRACE
				&& tokens.front().value != "}"
			) {
				matchExpressionCtx = tokens.front().ctx;
				Result<std::shared_ptr<Expression>> result = parse_match_expression(
					tokens,
					innerRegister,
					!got_default_case
				);

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
				return std::unexpected(utils::Error{
					.ctx = matchExpressionCtx,
					.message = "Different type match structure.",
					.type = utils::ErrorType::ERROR,
				});
			}

			return std::make_shared<MatchStructure>(
				ctx,
				selectorExpression,
				expressions
			);
		}

		Result<std::shared_ptr<Expression>> parse_loop_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::LOOP) {
				return parse_match_structure(tokens, identifierRegister);
			}
			struct utils::Context ctx = tokens.front().ctx;
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
					struct utils::Context initialCtx = tokens.front().ctx;
					if(initialExpression->Type() != NodeType::IDENTIFIER) {
						return std::unexpected(utils::Error{
							.ctx = initialCtx,
							.message = "Expected identifier in @ loop",
							.type = utils::ErrorType::ERROR,
						});
					}
					(void)eat(tokens);

					struct utils::Context startCtx = tokens.front().ctx;

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
							return std::unexpected(utils::Error{
								.ctx = startCtx,
								.message = "Expected number value for @ loop's start expression.",
								.type = utils::ErrorType::ERROR,
							});
						}
					} else {
						startExpression = std::make_shared<I8Expression>(startCtx, 0);
					}
					__TRY_TOKEN_FUNC_WRETERR(
						expect,
						tokens,
						lexer::MakeToken(lexer::TokenType::DOUBLE_DOT)
					)

					struct utils::Context endCtx = tokens.front().ctx;
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
						return std::unexpected(utils::Error{
							.ctx = endCtx,
							.message = "Expected number value for @ loop's end expression.",
							.type = utils::ErrorType::ERROR,
						});
					}
					std::string identifierName = std::dynamic_pointer_cast<IdentifierExpression>(
						initialExpression
					)->GetName();

					initialExpression = std::make_shared<DeclarationExpression>(
						initialCtx,
						std::dynamic_pointer_cast<IdentifierExpression>(initialExpression),
						startExpression,
						endExpression->GetDatatype(),
						false
					);

					if(innerRegister->Get(identifierName)) {
						return std::unexpected(utils::Error{
							.ctx = initialCtx,
							.message = "Variable name '" + identifierName + "' already exists",
							.type = utils::ErrorType::ERROR,
						});
					}

					innerRegister->Register(
						IdentifierData{
							identifierName,
							false,
							endExpression->GetDatatype()
						}
					);

					auto identifierExpression = std::make_shared<IdentifierExpression>(
						initialCtx,
						innerRegister,
						identifierName
					);

					updateExpression = std::make_shared<AssignationExpression>(
						initialCtx,
						identifierExpression,
						std::make_shared<BinaryExpression>(
							initialCtx,
							identifierExpression,
							"+",
							std::make_shared<I8Expression>(initialCtx, 1)
						)
					);
					condition = std::make_shared<BinaryExpression>(
						endCtx,
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
				struct utils::Context orCtx = tokens.front().ctx;
				__TRY_EXPECTED_FUNC_WRETERR_WSAVE__NEW(
					parse_or_expression,
					std::shared_ptr<OrExpression>,
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
					return std::unexpected(utils::Error{
						.ctx = orCtx,
						.message =
							std::string("Got non-matching operand types : ")
							+ orExpression->GetDatatype() + " and " + scope->GetDatatype(),
						.type = utils::ErrorType::ERROR,
					});
				}
			} else {
				return std::make_shared<LoopExpression>(ctx, scope);
			}

			if(condition) {
				return std::make_shared<ForLoopExpression>(
					ctx,
					std::dynamic_pointer_cast<ScopeExpression>(scope),
					initialExpression,
					condition,
					updateExpression,
					orExpression
				);
			}

			return std::make_shared<WhileLoopExpression>(
				ctx,
				scope,
				initialExpression,
				orExpression
			);
		}

		Result<std::shared_ptr<Expression>> parse_assignation_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::IDENTIFIER) {
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
			struct utils::Context assignableCtx = tokens.front().ctx;
			if(memberOffset == 0) {
				assignable = std::dynamic_pointer_cast<IdentifierExpression>(
					parse_identifier_expression(
						tokens,
						identifierRegister
					).value()
				);
				name = std::dynamic_pointer_cast<IdentifierExpression>(assignable)->GetName();
			} else {
				std::shared_ptr<Expression> structMemberAccessExpression;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_struct_member_access_expression,
					tokens,
					identifierRegister,
					structMemberAccessExpression
				)
				assignable = std::dynamic_pointer_cast<StructMemberAccessExpression>(
					structMemberAccessExpression
				);
				name = std::dynamic_pointer_cast<StructMemberAccessExpression>(
					assignable
				)->GetStruct()->GetName();
			}

			std::string unaryOperator =
				(tokens.size() > 0 && tokens.front().type == lexer::TokenType::UNARY_OPERATOR)
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
				return std::unexpected(utils::Error{
					.ctx = assignableCtx,
					.message = "Variable name '" + name + "' does not exist yet",
					.type = utils::ErrorType::ERROR,
				});
			}

			if(unaryOperator != "") {
				expression = std::make_shared<BinaryExpression>(
					assignableCtx,
					assignable,
					unaryOperator,
					std::make_shared<I8Expression>(assignableCtx, 1)
				);
			}

			if(operatorSymbol != "") {
				expression = std::make_shared<BinaryExpression>(
					assignableCtx,
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
					return std::unexpected(utils::Error{
						.ctx = assignableCtx,
						.message = result.error(),
						.type = utils::ErrorType::ERROR,
					});
				}
			}

			return std::make_shared<AssignationExpression>(
				assignableCtx,
				assignable,
				expression
			);
		}

		Result<std::shared_ptr<Expression>> parse_declaration_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::DECL) {
				return parse_assignation_expression(tokens, identifierRegister);
			}

			struct utils::Context ctx = tokens.front().ctx;

			bool isConst = eat(tokens).value().value == "const";

			struct utils::Context identifierCtx = tokens.front().ctx;
			std::shared_ptr<Expression> identifierExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_identifier_expression,
				tokens,
				identifierRegister,
				identifierExpression
			)

			if(std::dynamic_pointer_cast<IdentifierExpression>(identifierExpression)->GetName() == "this") {
				return std::unexpected(utils::Error{
					.ctx = identifierCtx,
					.message = "Trying to declare 'this' keyword as variable name.",
					.type = utils::ErrorType::ERROR,
				});
			}

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

			struct utils::Context exprCtx = tokens.front().ctx;
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
					return std::unexpected(utils::Error{
						.ctx = exprCtx,
						.message =
							std::string("Expected type ")
							+ datatype + ", got " + gotDatatype,
						.type = utils::ErrorType::ERROR,
					});
				}

				expression = result.value();
			}

			auto identifier = std::dynamic_pointer_cast<IdentifierExpression>(
				identifierExpression
			);

			if(identifierRegister->Get(identifier->GetName())) {
				return std::unexpected(utils::Error{
					.ctx = identifierCtx,
					.message = "Variable name '" + identifier->GetName() + "' already exists",
					.type = utils::ErrorType::ERROR,
				});
			}

			identifierRegister->Register(
				IdentifierData{
					identifier->GetName(),
					isConst,
					datatype
				}
			);

			return std::make_shared<DeclarationExpression>(
				ctx,
				identifier,
				expression,
				datatype,
				isConst
			);
		}

		Result<std::shared_ptr<Expression>> parse_fn_declaration_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::FN) {
				return parse_declaration_expression(tokens, identifierRegister);
			}

			auto innerRegister = std::make_shared<ScopeIdentifierRegister>(identifierRegister);

			struct utils::Context ctx = tokens.front().ctx;
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

			bool skipArguments =
				tokens.size() > 0
				&& tokens.front().type == lexer::TokenType::TYPE
				&& tokens.front().value == "VOID";

			if(skipArguments) {
				(void)eat(tokens);
			}

			std::vector<std::shared_ptr<DeclarationExpression>> arguments = {};

			while(
				!skipArguments
				&& tokens.size() > 0
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
				struct utils::Context argumentCtx = tokens.front().ctx;

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
				__TRY_EXPECTED_FUNC_WRETERR_WSAVE__NEW(
					expect_type,
					std::string,
					argumentDatatype,
					tokens
				)

				// TODO: Add const arguments
				arguments.push_back(
					std::make_shared<DeclarationExpression>(
						argumentCtx,
						argumentIdentifier,
						nullptr,
						argumentDatatype,
						false
					)
				);
			}

			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file in function declaration.",
					.type = utils::ErrorType::RETERR,
				});
			}
			(void)eat(tokens);

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::ARROW)
			)

			DatatypeStr returnDatatype = "";
			__TRY_EXPECTED_FUNC_WRETERR_WSAVE__NEW(
				expect_type,
				std::string,
				returnDatatype,
				tokens
			)

			if(arguments.size() == 0 && !skipArguments) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Missing `void` keyword on function with no arguments (`" + identifier->GetName() + "`)",
					.type = utils::ErrorType::RETERR,
				});
			}

			functions.push_back(
				std::make_shared<FunctionDeclarationExpression>(
					ctx,
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

		Result<std::shared_ptr<Expression>> parse_struct_declaration_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::STRUCT) {
				return parse_fn_declaration_expression(tokens, identifierRegister);
			}
			struct utils::Context ctx = tokens.front().ctx;
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

				struct utils::Context argumentDatatypeCtx = tokens.front().ctx;
				DatatypeStr argumentDatatype = "";
				__TRY_EXPECTED_FUNC_WRETERR_WSAVE__NEW(
					expect_type,
					std::string,
					argumentDatatype,
					tokens
				)

				if(!GetDatatypeClass(argumentDatatype)) {
					return std::unexpected(utils::Error{
						.ctx = argumentDatatypeCtx,
						.message = "Invalid datatype in struct declaration : " + argumentDatatype,
						.type = utils::ErrorType::ERROR,
					});
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
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file in struct declaration.",
					.type = utils::ErrorType::RETERR,
				});
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
				ctx,
				memberExpressions,
				structIdentifier
			);
		}

		Result<std::shared_ptr<Expression>> parse_struct_implementation_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
			}
			if(tokens.front().type != lexer::TokenType::IMPL) {
				return parse_struct_declaration_expression(tokens, identifierRegister);
			}
			struct utils::Context ctx = tokens.front().ctx;
			(void)eat(tokens);

			struct utils::Context structIdentifierCtx = tokens.front().ctx;
			std::shared_ptr<Expression> structIdentifierExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_identifier_expression,
				tokens,
				identifierRegister,
				structIdentifierExpression
			)
			auto structIdentifier = std::dynamic_pointer_cast<IdentifierExpression>(structIdentifierExpression);

			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(
					lexer::TokenType::BRACE,
					"{"
				)
			)

			std::shared_ptr<DatatypeClass> structClass;
			{
				std::expected<
					std::shared_ptr<DatatypeClass>,
					std::string
				> result = GetDatatypeClass(structIdentifier->GetName());
				if(!result) {
					return std::unexpected(utils::Error{
						.ctx = structIdentifierCtx,
						.message = "Invalid typename '" + structIdentifier->GetName() + "' in impl block.",
						.type = utils::ErrorType::ERROR,
					});
				}
					
				structClass = result.value();
			}

			if(structClass->isNative()) {
				return std::unexpected(utils::Error{
					.ctx = structIdentifierCtx,
					.message = "Invalid type '" + structClass->GetName() + "' is a native datatype in impl block.",
					.type = utils::ErrorType::ERROR,
				});
			}
			auto customDatatypeClass = std::dynamic_pointer_cast<CustomDatatypeClass>(structClass);

			auto thisIdentifierRegister = std::make_shared<ScopeIdentifierRegister>(identifierRegister);
			thisIdentifierRegister->Register(IdentifierData{
				.name = "this",
				.isConst = false,
				.datatype = structIdentifier->GetName()
			});

			std::unordered_set<std::shared_ptr<FunctionDeclarationExpression>> memberFunctions;

			while(tokens.size() > 0) {
				if(tokens.front().type == lexer::TokenType::BRACE && tokens.front().value == "}") {
					break;
				}
				std::shared_ptr<Expression> memberFunctionExpression;
				__TRY_EXPR_FUNC_WRETERR_WSAVE(
					parse_fn_declaration_expression,
					tokens,
					thisIdentifierRegister,
					memberFunctionExpression
				)
				auto memberFunction = std::dynamic_pointer_cast<FunctionDeclarationExpression>(
					memberFunctionExpression
				);
				memberFunctions.insert(memberFunction);
				customDatatypeClass->AddMemberFunction(CustomDatatypeMemberFunction{
					.name = memberFunction->GetIdentifier()->GetName(),
					.returnType = memberFunction->GetDatatype(),
					.argumentsTypes =
						utils::map<
							std::shared_ptr<DeclarationExpression>,
							DatatypeStr
						>(
							memberFunction->GetArguments(),
							[](const std::shared_ptr<DeclarationExpression>& argument) {
								return argument->GetDatatype();
							}
						),
				});
			}

			if(tokens.size() == 0) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file in struct implementation.",
					.type = utils::ErrorType::RETERR,
				});
			}
			(void)eat(tokens);

			return std::make_shared<StructImplementationExpression>(
				ctx,
				std::dynamic_pointer_cast<IdentifierExpression>(structIdentifierExpression),
				memberFunctions
			);
		}

		Result<std::shared_ptr<Expression>> parse_scope_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() <= 1) {
				return std::unexpected(utils::Error{
					.ctx = utils::Context{ .line = 0, .column = 0 },
					.message = "Unexpected end of file.",
					.type = utils::ErrorType::RETERR,
				});
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
				return parse_struct_implementation_expression(tokens, identifierRegister);
			}

			struct utils::Context ctx = tokens.front().ctx;

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

			auto scope = std::make_shared<ScopeExpression>(ctx);
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

		Result<std::shared_ptr<Expression>> parse_expression(
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

			auto scope = std::make_shared<ScopeExpression>(utils::Context{ .line = 0, .column = 0 });

			while(tokens.size() > 0) {
				Result<std::shared_ptr<Expression>> result = parse_expression(
					tokens,
					identifierRegister
				);

				if(!result) {
					return std::unexpected(
						std::string("[ERR::PARSER] Got error :\n\t\"") + utils::ErrorRepr(result.error())
						+ "\"\nwhile parsing tokens."
					);
				}

				scope->AddExpression(
					result.value()
				);

				if(result.value()->Type() != NodeType::FN) {
					Result<struct lexer::Token> semicolon;
					if(!(semicolon = expect(tokens, lexer::MakeToken(lexer::TokenType::EOL)))) {
						return std::unexpected(
							"Missing semicolon at " + std::to_string(semicolon.error().ctx.line)
							+ ":" + std::to_string(semicolon.error().ctx.column)
						);
					}
				}
			}

			return scope;
		}
	}
}
