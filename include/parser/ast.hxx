#pragma once

#include <parser/types.hxx>
#include <utils/utils.hxx>

#include <algorithm>
#include <array>
#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#define __GEN__SUB_NUMBER_CLASS(_ClassName, _ValueType) \
class _ClassName : public NumberExpression { \
public: \
	_ClassName( \
		_ValueType value \
	); \
	\
	std::string Repr( \
	  size_t indent = 0 \
	) override; \
	NodeType Type() override; \
	DatatypeStr GetDatatype() override; \
\
private: \
	_ValueType m_value; \
};

#define __GEN__SUB_NUMBER_CLASS_IMPL(_ClassName, _ValueType, ENUM_MEMBER) \
_ClassName::_ClassName( \
	_ValueType value \
) : \
	NumberExpression(std::to_string(value)), \
	m_value(value) \
{} \
\
std::string _ClassName::Repr( \
	size_t indent \
) { \
	std::string repr = std::string(#_ClassName) + "(" + std::to_string(m_value) + ")"; \
	repr.insert(0, indent, '\t'); \
	return repr; \
} \
\
NodeType _ClassName::Type() { \
	return NodeType::ENUM_MEMBER; \
} \
DatatypeStr _ClassName::GetDatatype() { \
	return std::string(#ENUM_MEMBER); \
}

namespace dim {
	namespace parser {

		class Expression;
		class NestedExpression;
		class ScopeExpression;
		class NullExpression;
		class NumberExpression;
		class I8Expression;
		class I16Expression;
		class I32Expression;
		class I64Expression;
		class U8Expression;
		class U16Expression;
		class U32Expression;
		class U64Expression;
		class F32Expression;
		class F64Expression;
		class F128Expression;
		class BooleanExpression;
		class CharExpression;
		class StringExpression;
    class AssignableExpression;
		class UnaryExpression;
		class BinaryExpression;
		class IfElseExpression;
		class IfElseStructure;
		class MatchExpression;
		class MatchStructure;
		class LoopExpression;
		class WhileLoopExpression;
		class ForLoopExpression;
		class BreakExpression;
		class OrExpression;
		class IdentifierExpression;
		class DiscardExpression;
		class AssignationExpression;
		class DeclarationExpression;
		class FunctionDeclarationExpression;
		class FunctionCallExpression;
		class StructDeclarationExpression;
		class StructExpression;
		class StructMemberAccessExpression;
    class StructImplementationExpression;
		
		enum class NodeType {
			NONE = 0,
			NESTED,
			SCOPE,
			NUL,
			NUMBER,
			VOID,
			I8, I16, I32, I64,
			U8, U16, U32, U64,
			F32, F64, F128,
			BOOLEAN,
			CHAR,
			STRING,
			ASSIGNABLE,
			UNARY,
			BINARY,
			IFELSE_EXPR,
			IFELSE_STRUCT,
			MATCH_EXPR,
			MATCH_STRUCT,
			LOOP,
			WHILE,
			FOR,
			BREAK,
			RETURN,
			OR,
			IDENTIFIER,
			DISCARD,
			ASSIGN,
			DECL,
			FN,
			FN_CALL,
			STRUCT_DECL,
			STRUCT,
			STRUCT_ACCESS,
      STRUCT_IMPL,
      STRUCT_ACCESS_FN,
		};

		const std::array<std::string_view, 44> NodeTypeToStr = {
			"NONE",
			"NESTED",
			"SCOPE",
			"NUL",
			"NUMBER",
			"VOID",
			"I8", "I16", "I32", "I64",
			"U8", "U16", "U32", "U64",
			"F32", "F64", "F128",
			"BOOLEAN",
			"CHAR",
			"STRING",
			"ASSIGNABLE",
			"UNARY",
			"BINARY",
			"IFELSE_EXPR",
			"IFELSE_STRUCT",
			"MATCH_EXPR",
			"MATCH_STRUCT",
			"LOOP",
			"WHILE",
			"FOR",
			"BREAK",
			"RETURN",
			"OR",
			"IDENTIFIER",
			"DISCARD",
			"ASSIGN",
			"DECL",
			"FN",
			"FN_CALL",
			"STRUCT_DECL",
			"STRUCT",
			"STRUCT_ACCESS",
      "STRUCT_IMPL",
      "STRUCT_ACCESS_FN",
		};

		typedef struct {
			const std::string name;
			const bool isConst;
			const DatatypeStr datatype;
		} IdentifierData;
	}
}

#include <parser/cast.hxx>

namespace dim {
	namespace parser {

		class ScopeIdentifierRegister {
		public:
			ScopeIdentifierRegister(
				std::shared_ptr<ScopeIdentifierRegister> parent = nullptr
			);

			std::expected<
				IdentifierData,
				std::string
			> Get(
				const std::string name
			);
			void Register(
				IdentifierData identifier
			);

		private:
			std::shared_ptr<ScopeIdentifierRegister> m_parent;
			std::vector<IdentifierData> m_identifiers;
		};

		class Expression {
		public:
			virtual std::string Repr(
				const size_t indent = 0
			);
			virtual NodeType Type();
			virtual DatatypeStr GetDatatype();

			Expression() = default;
		};

		class NestedExpression : public Expression {
		public:
			NestedExpression(
				std::shared_ptr<Expression> expression
			);

			std::shared_ptr<Expression> GetExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		protected:
			std::shared_ptr<Expression> m_expression;
		};

    class AssignableExpression : public Expression {
    public:
      AssignableExpression();

      std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;
      virtual std::expected<
        Success,
        std::string
      > TryAssign(
        std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
        std::shared_ptr<Expression> expression
      );
    };

		class IdentifierExpression : public AssignableExpression {
		public:
			IdentifierExpression(
				std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
				std::string name,
				bool isConst = true,
				std::shared_ptr<Expression> expression = nullptr,
				DatatypeStr datatype = "INFER"
			);

			std::string GetName();
			bool GetIsConst();
			void SetIsConst(
				bool isConst
			);
      std::shared_ptr<Expression> GetExpression();
			void SetExpression(
				std::shared_ptr<Expression> expression
			);
			void SetDatatype(
				DatatypeStr datatype
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;
      std::expected<
        Success,
        std::string
      > TryAssign(
        std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
        std::shared_ptr<Expression> expression
      ) override;

		private:
      std::shared_ptr<Expression> m_expression;
			std::string m_name;
			bool m_isConst;
			DatatypeStr m_datatype;
		};

		class ScopeExpression : public Expression {
		public:
			ScopeExpression(
				std::vector<std::shared_ptr<Expression>> expressions = {},
				std::shared_ptr<IdentifierExpression> name = nullptr
			);

			std::vector<std::shared_ptr<Expression>> GetExpressions();
			void AddExpression(
				std::shared_ptr<Expression> expression
			);

			std::shared_ptr<IdentifierExpression> GetName();
			void SetName(
				std::shared_ptr<IdentifierExpression> name
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::vector<std::shared_ptr<Expression>> m_expressions;
			std::shared_ptr<IdentifierExpression> m_name;
		};

		class NullExpression : public Expression {
		public:
			NullExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;
		};

		class NumberExpression : public Expression {
		public:
			NumberExpression(
				std::string value
			);

			std::string GetValue();
			void SetValue(
				std::string value
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		protected:
			std::string m_value;
		};

		__GEN__SUB_NUMBER_CLASS(I8Expression, i8)
		__GEN__SUB_NUMBER_CLASS(I16Expression, i16)
		__GEN__SUB_NUMBER_CLASS(I32Expression, i32)
		__GEN__SUB_NUMBER_CLASS(I64Expression, i64)

		__GEN__SUB_NUMBER_CLASS(U8Expression, u8)
		__GEN__SUB_NUMBER_CLASS(U16Expression, u16)
		__GEN__SUB_NUMBER_CLASS(U32Expression, u32)
		__GEN__SUB_NUMBER_CLASS(U64Expression, u64)

		__GEN__SUB_NUMBER_CLASS(F32Expression, f32)
		__GEN__SUB_NUMBER_CLASS(F64Expression, f64)
		__GEN__SUB_NUMBER_CLASS(F128Expression, f128)

		class BooleanExpression : public Expression {
		public:
			BooleanExpression(
				std::string value
			);

			std::string GetValue();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::string m_value;
		};

		class CharExpression : public Expression {
		public:
			CharExpression(
				std::string value
			);

			std::string GetValue();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::string m_value;
		};

		class StringExpression : public Expression {
		public:
			StringExpression(
				std::string value
			);

			std::string GetValue();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::string m_value;
		};

		class UnaryExpression : public Expression {
		public:
			UnaryExpression(
				std::shared_ptr<Expression> term,
				std::string operatorSymbol
			);

			std::shared_ptr<Expression> GetTerm();
			std::string GetOperator();

			std::shared_ptr<Expression> GetSampleExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::shared_ptr<Expression> m_term;
			std::string m_operatorSymbol;
		};

		class BinaryExpression : public Expression {
		public:
			BinaryExpression(
				std::shared_ptr<Expression> left,
				std::string operatorSymbol,
				std::shared_ptr<Expression> right
			);

			std::shared_ptr<Expression> GetLeft();
			std::string GetOperator();
			std::shared_ptr<Expression> GetRight();

			std::shared_ptr<Expression> GetSampleExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::shared_ptr<Expression> m_left;
			std::string m_operatorSymbol;
			std::shared_ptr<Expression> m_right;
		};

		class IfElseExpression : public Expression {
		public:
			IfElseExpression(
				std::shared_ptr<ScopeExpression> scope,
				std::shared_ptr<Expression> condition
			);

			std::shared_ptr<ScopeExpression> GetScope();
			std::shared_ptr<Expression> GetCondition();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::shared_ptr<ScopeExpression> m_scope;
			std::shared_ptr<Expression> m_condition;
		};

		class IfElseStructure : public Expression {
		public:
			IfElseStructure(
				std::vector<std::shared_ptr<IfElseExpression>> expressions
			);

			std::vector<std::shared_ptr<IfElseExpression>> GetExpressions();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::vector<std::shared_ptr<IfElseExpression>> m_expressions;
		};

		class MatchExpression : public Expression {
		public:
			MatchExpression(
				std::shared_ptr<ScopeExpression> scope,
				std::shared_ptr<Expression> condition
			);

			std::shared_ptr<ScopeExpression> GetScope();
			std::shared_ptr<Expression> GetCondition();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::shared_ptr<ScopeExpression> m_scope;
			std::shared_ptr<Expression> m_condition;
		};

		class MatchStructure : public NestedExpression {
		public:
			MatchStructure(
				std::shared_ptr<Expression> expression,
				std::vector<std::shared_ptr<MatchExpression>> expressions
			);

			std::vector<std::shared_ptr<MatchExpression>> GetExpressions();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::vector<std::shared_ptr<MatchExpression>> m_expressions;
		};

		class LoopExpression : public Expression {
		public:
			LoopExpression(
				std::shared_ptr<ScopeExpression> scope
			);

			std::shared_ptr<ScopeExpression> GetScope();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		protected:
			std::shared_ptr<ScopeExpression> m_scope;
		};

		class WhileLoopExpression : public LoopExpression {
		public:
			WhileLoopExpression(
				std::shared_ptr<ScopeExpression> scope,
				std::shared_ptr<Expression> condition,
				std::shared_ptr<OrExpression> orExpression
			);

			std::shared_ptr<Expression> GetCondition();
			std::shared_ptr<Expression> GetOrExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		protected:
			std::shared_ptr<Expression> m_condition;
			std::shared_ptr<Expression> m_orExpression;
		};

		class ForLoopExpression : public WhileLoopExpression {
		public:
			ForLoopExpression(
				std::shared_ptr<ScopeExpression> scope,
				std::shared_ptr<Expression> initialExpression,
				std::shared_ptr<Expression> condition,
				std::shared_ptr<Expression> updateExpression,
				std::shared_ptr<OrExpression> orExpression
			);

			std::shared_ptr<Expression> GetInitialExpression();
			std::shared_ptr<Expression> GetUpdateExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::shared_ptr<Expression> m_initialExpression;
			std::shared_ptr<Expression> m_updateExpression;
		};

		class BreakExpression : public NestedExpression {
		public:
			BreakExpression(
				std::shared_ptr<Expression> expression,
				std::shared_ptr<IdentifierExpression> scopeName = nullptr
			);

			std::shared_ptr<IdentifierExpression> GetScopeName();
			void SetScopeName(
				std::shared_ptr<IdentifierExpression> scopeName
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;
		
		private:
			std::shared_ptr<IdentifierExpression> m_name;
		};

		class ReturnExpression : public NestedExpression {
		public:
			ReturnExpression(
				std::shared_ptr<Expression> expression = nullptr
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;
		};

		class OrExpression : public NestedExpression {
		public:
			OrExpression(
				std::shared_ptr<Expression> expression
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;
		};

		class DiscardExpression : public AssignableExpression {
		public:
			DiscardExpression();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;
      std::expected<
        Success,
        std::string
      > TryAssign(
        std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
        std::shared_ptr<Expression> expression
      );
		};

		class AssignationExpression : public NestedExpression {
		public:
			AssignationExpression(
				std::shared_ptr<AssignableExpression> destination,
				std::shared_ptr<Expression> expression
			);

			std::shared_ptr<AssignableExpression> GetDestination();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::shared_ptr<AssignableExpression> m_destination;
		};

		class DeclarationExpression : public Expression {
		public:
			DeclarationExpression(
				std::shared_ptr<IdentifierExpression> identifier,
				std::shared_ptr<Expression> expression,
				DatatypeStr datatype,
				bool isConst
			);

			std::shared_ptr<IdentifierExpression> GetIdentifier();

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::shared_ptr<IdentifierExpression> m_identifier;
		};

		class FunctionDeclarationExpression : public Expression {
		public:
			FunctionDeclarationExpression(
				std::shared_ptr<IdentifierExpression> identifier,
				std::vector<std::shared_ptr<DeclarationExpression>> arguments,
				std::shared_ptr<ScopeExpression> scope,
				DatatypeStr returnDatatype
			);

			std::shared_ptr<IdentifierExpression> GetIdentifier();
			std::vector<std::shared_ptr<DeclarationExpression>> GetArguments();
			std::shared_ptr<ScopeExpression> GetScope();
			void SetScope(
				std::shared_ptr<ScopeExpression> scope
			);

			std::string Repr(
				const size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::shared_ptr<IdentifierExpression> m_identifier;
			std::vector<std::shared_ptr<DeclarationExpression>> m_arguments;
			std::shared_ptr<ScopeExpression> m_scope;
			DatatypeStr m_returnDatatype;
		};

		class FunctionCallExpression : public Expression {
		public:
			FunctionCallExpression(
				std::shared_ptr<IdentifierExpression> identifier,
				std::vector<std::shared_ptr<Expression>> arguments,
				DatatypeStr returnDatatype
			);

			std::shared_ptr<IdentifierExpression> GetIdentifier();
			std::vector<std::shared_ptr<Expression>> GetArguments();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::shared_ptr<IdentifierExpression> m_identifier;
			std::vector<std::shared_ptr<Expression>> m_arguments;
			DatatypeStr m_returnDatatype;
		};

		class StructDeclarationExpression : public Expression {
		public:
			StructDeclarationExpression(
				std::vector<std::shared_ptr<IdentifierExpression>> members,
				std::shared_ptr<IdentifierExpression> name
			);

			std::vector<std::shared_ptr<IdentifierExpression>> GetMembers();
			std::shared_ptr<IdentifierExpression> GetName();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::vector<std::shared_ptr<IdentifierExpression>> m_members;
			std::shared_ptr<IdentifierExpression> m_name;
		};

		class StructExpression : public Expression {
		public:
			StructExpression(
				std::vector<std::shared_ptr<IdentifierExpression>> members,
				std::shared_ptr<IdentifierExpression> name
			);

			std::vector<std::shared_ptr<IdentifierExpression>> GetMembers();
			std::shared_ptr<IdentifierExpression> GetName();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

		private:
			std::vector<std::shared_ptr<IdentifierExpression>> m_members;
			std::shared_ptr<IdentifierExpression> m_name;
		};

		class StructMemberAccessExpression : public AssignableExpression {
		public:
			StructMemberAccessExpression(
				std::shared_ptr<IdentifierExpression> structIdentifier,
				std::shared_ptr<IdentifierExpression> memberIdentifier,
				DatatypeStr datatype
			);

			std::shared_ptr<IdentifierExpression> GetStruct();
			std::shared_ptr<IdentifierExpression> GetMember();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;
      std::expected<
        Success,
        std::string
      > TryAssign(
        std::shared_ptr<ScopeIdentifierRegister> identifierRegister,
        std::shared_ptr<Expression> expression
      ) override;

		private:
			std::shared_ptr<IdentifierExpression> m_structIdentifier;
			std::shared_ptr<IdentifierExpression> m_memberIdentifier;
		};

    class StructImplementationExpression : public Expression {
    public:
      StructImplementationExpression(
        std::shared_ptr<IdentifierExpression> structIdentifier,
        std::unordered_set<std::shared_ptr<FunctionDeclarationExpression>> memberFunctions
      );

      std::shared_ptr<IdentifierExpression> GetStruct();
      std::unordered_set<std::shared_ptr<FunctionDeclarationExpression>> GetMemberFunctions();
      std::expected<
        std::shared_ptr<FunctionDeclarationExpression>,
        std::string
      > GetMemberFunction(
        const std::string& name
      );

      std::string Repr(
        size_t indent = 0
      ) override;
      NodeType Type() override;
      DatatypeStr GetDatatype() override;

    private:
      std::shared_ptr<IdentifierExpression> m_structIdentifier;
      std::unordered_set<std::shared_ptr<FunctionDeclarationExpression>> m_memberFunctions;
    };

    class StructMemberFunctionAccessExpression : public Expression {
		public:
			StructMemberFunctionAccessExpression(
				std::shared_ptr<IdentifierExpression> structIdentifier,
				std::shared_ptr<IdentifierExpression> memberFunctionIdentifier,
        std::vector<std::shared_ptr<Expression>> arguments,
				DatatypeStr returnDatatype
			);

			std::shared_ptr<IdentifierExpression> GetStruct();
			std::shared_ptr<IdentifierExpression> GetMemberFunction();
      std::vector<std::shared_ptr<Expression>> GetArguments();

			std::string Repr(
				size_t indent = 0
			) override;
			NodeType Type() override;
			DatatypeStr GetDatatype() override;

  	private:
			std::shared_ptr<IdentifierExpression> m_structIdentifier;
			std::shared_ptr<IdentifierExpression> m_memberFunctionIdentifier;
      std::vector<std::shared_ptr<Expression>> m_arguments;
      DatatypeStr m_returnDatatype;
		};
  }
}
