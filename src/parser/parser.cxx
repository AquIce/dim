#include <parser/parser.hxx>

namespace dim {
	namespace parser {

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

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_identifier_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
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
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::DISCARD) {
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
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::NUL) {
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
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::BOOLEAN) {
				return parse_null_expression(tokens, identifierRegister);
			}

			return std::make_shared<BooleanExpression>(
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
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::STRING) {
				return parse_boolean_expression(tokens, identifierRegister);
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
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::NUMBER) {
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
		> parse_break_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::BREAK) {
				return parse_number_expression(tokens, identifierRegister);
			}
			(void)eat(tokens);

			std::shared_ptr<Expression> breakExpression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_number_expression,
				tokens,
				identifierRegister,
				breakExpression
			)

			return std::make_shared<BreakExpression>(
				breakExpression
			);
		}

		std::expected<
			std::shared_ptr<OrExpression>,
			std::string
		> parse_or_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::OR) {
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
			if(
				tokens.size() > 0 &&
				(
					tokens.front().type != lexer::TokenType::PARENTHESIS ||
					tokens.front().value != "("
				)
			) {
				return parse_break_expression(tokens, identifierRegister);
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
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::UNARY_OPERATOR) {
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

				if(!try_n_cast(std::vector({left, right}))) {
					return std::unexpected(
						std::string("Got non-matching operands types : ")
						+ std::string(DatatypeToStr.at(int(left->GetDatatype())))
						+ " and " + std::string(DatatypeToStr.at(int(right->GetDatatype())))
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

				if(!try_n_cast(std::vector({left, right}))) {
					return std::unexpected(
						std::string("Got non-matching operand types : ")
						+ std::string(DatatypeToStr.at(int(left->GetDatatype())))
						+ " and " + std::string(DatatypeToStr.at(int(right->GetDatatype())))
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

				if(!try_n_cast(std::vector({left, right}))) {
					return std::unexpected(
						std::string("Got non-matching operand types : ")
						+ std::string(DatatypeToStr.at(int(left->GetDatatype())))
						+ " and " + std::string(DatatypeToStr.at(int(right->GetDatatype())))
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
			struct lexer::Token keyword;

			if(
				tokens.size() > 0 && tokens.front().type == lexer::TokenType::IFELSE
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
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::MATCH) {
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
			)  {
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
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::LOOP) {
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
							startExpression->GetDatatype() == Datatype::BOOLEAN
							|| startExpression->GetDatatype() == Datatype::STRING
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
						endExpression->GetDatatype() == Datatype::BOOLEAN
						|| endExpression->GetDatatype() == Datatype::STRING
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
						+ std::string(DatatypeToStr.at(int(orExpression->GetDatatype())))
						+ " and " + std::string(DatatypeToStr.at(int(scope->GetDatatype())))
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
			if(
				tokens.size() > 0 && tokens.front().type != lexer::TokenType::IDENTIFIER
				|| (tokens.size() > 1 && tokens.at(1).type != lexer::TokenType::EQUALS)
			) {
				return parse_loop_expression(tokens, identifierRegister);
			}

			// We know it's an identifier
			std::shared_ptr<Expression> identifierExpression = parse_identifier_expression(
				tokens,
				identifierRegister
			).value();

			// We know it's an EQUALS token
			(void)eat(tokens);

			std::shared_ptr<Expression> expression;
			__TRY_EXPR_FUNC_WRETERR_WSAVE(
				parse_expression,
				tokens,
				identifierRegister,
				expression
			)

			std::string name = std::dynamic_pointer_cast<IdentifierExpression>(identifierExpression)->GetName();

			std::expected<
				IdentifierData,
				std::string
			> result = identifierRegister->Get(name);

			if(!result) {
				return std::unexpected("Variable name '" + name + "' does not exist yet");
			}

			auto identifier = std::make_shared<IdentifierExpression>(
				identifierRegister,
				result.value().name,
				result.value().isConst,
				nullptr,
				result.value().datatype
			);

			if(identifier->GetIsConst()) {
				return std::unexpected("Trying to set constant '" + name + "'");
			}

			Datatype expectedDatatype = identifier->GetDatatype();
			Datatype gotDatatype = expression->GetDatatype();
			if(expectedDatatype != gotDatatype) {
				std::expected<
					std::shared_ptr<Expression>,
					std::string
				> castResult = try_cast(expression, expectedDatatype);

				if(!castResult) {
					return std::unexpected(
						std::string("Expected type ")
						+ std::string(DatatypeToStr.at(int(expectedDatatype)))
						+ ", got " + std::string(DatatypeToStr.at(int(gotDatatype)))
					);
				}

				expression = castResult.value();
			}

			identifier->SetExpression(expression);

			return std::make_shared<AssignationExpression>(
				identifier,
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
			if(tokens.size() > 0 && tokens.front().type != lexer::TokenType::DECL) {
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

			Datatype datatype = Datatype::INFER;
			if(tokens.size() > 0 && tokens.front().type == lexer::TokenType::TYPE) {
				datatype = Datatype(
					utils::indexOfUnsafe(
						std::begin(DatatypeToStr),
						std::end(DatatypeToStr),
						eat(tokens).value().value
					)
				);
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

			Datatype gotDatatype = expression->GetDatatype();

			if(datatype == Datatype::INFER) {
				datatype = gotDatatype;
			} else if(datatype != gotDatatype) {
				std::expected<
					std::shared_ptr<Expression>,
					std::string
				> result = try_cast(expression, datatype);

				if(!result) {
					return std::unexpected(
						std::string("Expected type ")
						+ std::string(DatatypeToStr.at(int(datatype)))
						+ ", got " + std::string(DatatypeToStr.at(int(gotDatatype)))
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
		> parse_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			return parse_declaration_expression(tokens, identifierRegister);
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> parse_scope_expression(
			std::vector<struct lexer::Token>& tokens,
			std::shared_ptr<ScopeIdentifierRegister> identifierRegister
		) {
			__TRY_TOKEN_FUNC_WRETERR(
				expect,
				tokens,
				lexer::MakeToken(lexer::TokenType::BRACE, "{")
			)

			auto scope = std::make_shared<ScopeExpression>();
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
			std::shared_ptr<ScopeExpression>,
			std::string
		> Parse(
			std::vector<struct lexer::Token>& tokens
		) {

			auto identifierRegister = std::make_shared<ScopeIdentifierRegister>();
		
			std::expected<
				std::shared_ptr<Expression>,
				std::string
			> result = parse_scope_expression(
				tokens,
				identifierRegister
			);

			if(!result) {
				return std::unexpected(
					std::string("[ERR::PARSER] Got error :\n\t\"") + result.error()
					+ "\"\nwhile parsing tokens."
				);
			}

			return std::dynamic_pointer_cast<ScopeExpression>(result.value());
		}
	}
}
