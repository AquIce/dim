#include <parser/cast.hxx>

namespace dim {
	namespace parser {

		bool isConvertible(
			Datatype first,
			Datatype second
		) noexcept {
			return (
				(ConversionTable.at(int(first)) & (1 << int(second))) != 0
			);
		}

		std::expected<
			std::shared_ptr<NumberExpression>,
			std::string
		> try_cast_number(
			std::shared_ptr<Expression> expression
		) noexcept {
			auto numberExpression = std::dynamic_pointer_cast<NumberExpression>(
				expression
			);
			if(numberExpression == nullptr) {
				return std::unexpected("Invalid cast to number.");
			}

			return numberExpression;
		}

		std::expected<
			Success,
			std::string
		> try_cast_integer(
			std::shared_ptr<NumberExpression> numberExpression
		) noexcept {
			std::string value = numberExpression->GetValue();
			if(
				std::find(
					std::begin(value),
					std::end(value),
					'.'
				) != std::end(value)
			) {
				return std::unexpected("Invalid cast of decimal value to integer.");
			}
			return Success{};
		}

		std::expected<
			std::shared_ptr<BooleanExpression>,
			std::string
		> try_cast_bool(
			std::shared_ptr<Expression> expression
		) noexcept {
			auto boolExpression = std::dynamic_pointer_cast<BooleanExpression>(
				expression
			);
			if(boolExpression == nullptr) {
				return std::unexpected("Invalid cast to boolean.");
			}

			return boolExpression;
		}

		std::expected<
			std::shared_ptr<StringExpression>,
			std::string
		> try_cast_str(
			std::shared_ptr<Expression> expression
		) noexcept {
			auto strExpression = std::dynamic_pointer_cast<StringExpression>(
				expression
			);
			if(strExpression == nullptr) {
				return std::unexpected("Invalid cast to string.");
			}

			return strExpression;
		}

		std::shared_ptr<Expression> get_root_expression(
			std::shared_ptr<Expression> expression
		) noexcept {
			if(auto nestedExpression = std::dynamic_pointer_cast<NestedExpression>(expression)) {
				if(!nestedExpression->GetExpression()) {
					return expression;
				}
				return nestedExpression->GetExpression();
			} else if(auto binaryExpression = std::dynamic_pointer_cast<BinaryExpression>(expression)) {
				return binaryExpression->GetSampleExpression();
			} else if(auto unaryExpression = std::dynamic_pointer_cast<UnaryExpression>(expression)) {
				return unaryExpression->GetSampleExpression();
			} else if(auto scopeExpression = std::dynamic_pointer_cast<ScopeExpression>(expression)) {
				return scopeExpression->GetExpressions().back();
			} else if(auto loopExpression = std::dynamic_pointer_cast<LoopExpression>(expression)) {
				return loopExpression->GetScope()->GetExpressions().back();
			} else if(auto ifElseStructure = std::dynamic_pointer_cast<IfElseStructure>(expression)) {
				return ifElseStructure->GetExpressions().back()->GetScope()->GetExpressions().back();
			}
			return expression;
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> try_cast(
			std::shared_ptr<Expression> expressionRef,
			Datatype datatype
		) noexcept {
			auto expressions = std::vector<std::shared_ptr<Expression>>({ expressionRef, get_root_expression(expressionRef) });
			while(expressions.back() != expressions.at(expressions.size() - 2)) {
				expressions.push_back(get_root_expression(expressions.back()));
			}
			std::shared_ptr<Expression> expression = expressions.back();

			switch(datatype) {
			case Datatype::I8: { __GEN__TRY_CAST_ITYPE(i8, I8Expression) }
			case Datatype::I16: { __GEN__TRY_CAST_ITYPE(i16, I16Expression) }
			case Datatype::I32: { __GEN__TRY_CAST_ITYPE(i32, I32Expression) }
			case Datatype::I64: { __GEN__TRY_CAST_ITYPE(i64, I64Expression)}
			case Datatype::U8: { __GEN__TRY_CAST_UTYPE(u8, U8Expression)}
			case Datatype::U16: { __GEN__TRY_CAST_UTYPE(u16, U16Expression)}
			case Datatype::U32: { __GEN__TRY_CAST_UTYPE(u32, U32Expression)}
			case Datatype::U64: { __GEN__TRY_CAST_UTYPE(u64, U64Expression)}
			case Datatype::F32: { __GEN__TRY_CAST_FTYPE(f32, F32Expression)}
			case Datatype::F64: { __GEN__TRY_CAST_FTYPE(f64, F64Expression)}
			case Datatype::F128: {
				std::shared_ptr<NumberExpression> numberExpression;
				__TRY_EXPECTED_FUNC_WRETERR_WSAVE(
					try_cast_number,
					std::shared_ptr<NumberExpression>,
					std::string,
					numberExpression,
					expression
				)

				try {
					if(expressions.size() != 2) {
						return expressionRef;
					}
					return std::make_shared<F128Expression>(
						utils::stof128(numberExpression->GetValue())
					);
				} catch(...) {
					return std::unexpected(
						"Value '" + numberExpression->GetValue()
						+ "' out of global bounds"
					);
				}
			}
			case Datatype::BOOLEAN: {
				std::shared_ptr<BooleanExpression> boolExpression;
				__TRY_EXPECTED_FUNC_WRETERR_WSAVE(
					try_cast_bool,
					std::shared_ptr<BooleanExpression>,
					std::string,
					boolExpression,
					expression
				)
				if(expressions.size() != 2) {
					return expressionRef;
				}
				return boolExpression;
			}
			/*case Datatype::CHAR:
				NOIMP;*/
			case Datatype::STRING: {
				std::shared_ptr<StringExpression> strExpression;
				__TRY_EXPECTED_FUNC_WRETERR_WSAVE(
					try_cast_str,
					std::shared_ptr<StringExpression>,
					std::string,
					strExpression,
					expression
				)
				if(expressions.size() != 2) {
					return expressionRef;
				}
				return strExpression;
			}
			default:
				return std::unexpected("Invalid datatype.");
			}
		}

		std::expected<
			std::shared_ptr<Expression>,
			std::string
		> auto_cast(
			std::shared_ptr<Expression> expression
		) noexcept {
			std::expected<
				std::shared_ptr<Expression>,
				std::string
			> result;
			for(const auto& datatype : DatatypeIterator()) {
				result = try_cast(expression, datatype);
				if(result) {
					return result;
				}
			}
			return std::unexpected(
				std::string("Invalid value (cannot be cast to any type) : ")
				+ expression->Repr()
			);
		}

		bool try_n_cast(
			std::vector<std::shared_ptr<Expression>> expressions
		) noexcept {
			for(size_t i = 0; i < expressions.size(); i++) {
				bool isCastPossible = true;
				for(size_t j = 0; j < expressions.size(); j++) {
					if(!isConvertible(
						expressions.at(i)->GetDatatype(),
						expressions.at(j)->GetDatatype()
					)) {
						isCastPossible = false;
						break;
					};
				}
				if(isCastPossible) {
					return true;
				}
			}
			return false;
		}
	}
}
