#include <parser/types.hxx>

namespace dim {
  namespace parser {

    std::vector<std::shared_ptr<DatatypeClass>> datatypes = {
      std::make_shared<DatatypeClass>("INFER"),
      std::make_shared<DatatypeClass>("VOID"),
      std::make_shared<DatatypeClass>("I8"),
      std::make_shared<DatatypeClass>("I16"),
      std::make_shared<DatatypeClass>("I32"),
      std::make_shared<DatatypeClass>("I64"),
      std::make_shared<DatatypeClass>("U8"),
      std::make_shared<DatatypeClass>("U16"),
      std::make_shared<DatatypeClass>("U32"),
      std::make_shared<DatatypeClass>("U64"),
      std::make_shared<DatatypeClass>("F32"),
      std::make_shared<DatatypeClass>("F64"),
      std::make_shared<DatatypeClass>("F128"),
      std::make_shared<DatatypeClass>("BOOL"),
      std::make_shared<DatatypeClass>("CHAR"),
      std::make_shared<DatatypeClass>("STRING"),
    };

    std::expected<
      std::shared_ptr<DatatypeClass>,
      std::string
    > GetDatatypeClass(
      const std::string& name
    ) {
      std::vector<std::shared_ptr<DatatypeClass>>::iterator result = std::find_if(
        datatypes.begin(), datatypes.end(),
        [&name](std::shared_ptr<DatatypeClass> datatype) {
          return datatype->GetName() == name;
        }
      );
      if(result == datatypes.end()) {
        return std::unexpected("Invalid datatype for lookup.");
      }
      return *result;
    }

    std::expected<
      DatatypeStr,
      std::string
    > GetUnaryOutputDatatype(
      std::string operatorSymbol, 
      DatatypeStr termType
    ) {
      try {
        return UnaryOutputType.at(operatorSymbol).at(termType);
      } catch(...) {
        return std::unexpected("Invalid operator type.");
      }
    }

    std::expected<
      DatatypeStr,
      std::string
    > GetBinaryOutputDatatype(
      DatatypeStr leftType,
      std::string operatorSymbol,
      DatatypeStr rightType
    ) {
      try {
        return BinaryOutputType.at(operatorSymbol).at({ leftType, rightType });
      } catch(...) {
        return std::unexpected("Invalid operator types.");
      }
    }

    bool isConvertible(
      const DatatypeStr& from,
      const DatatypeStr& to
    ) {
      try {
        (void)ConversionTable.at(from);
        (void)ConversionTable.at(to);
      } catch(...) {
        return false;
      }

      size_t toIndex = std::find_if(datatypes.begin(), datatypes.end(), [to](const std::shared_ptr<DatatypeClass>& datatype) {
        return datatype->GetName() == to;
      }) - datatypes.begin();

      return (ConversionTable.at(from) & (1 << toIndex)) != 0;
    }



    DatatypeClass::DatatypeClass(
      std::string name
    ) :
      m_name(name)
    {}

    std::string DatatypeClass::GetName() {
      return m_name;
    }

    bool DatatypeClass::operator==(
      const std::shared_ptr<DatatypeClass>& other
    ) {
      return other->isNative() && isConvertible(
        m_name,
        other->GetName()
      );
    }

    bool DatatypeClass::isNative() {
      return true;
    }



    CustomDatatypeClass::CustomDatatypeClass(
      std::string name,
      std::unordered_set<CustomDatatypeMember> members
    ) :
      DatatypeClass(name),
      m_members(members),
      m_memberFunctions()
    {}

    std::unordered_set<CustomDatatypeMember> CustomDatatypeClass::GetMembers() {
      return m_members;
    }

    std::expected<
      CustomDatatypeMember,
      std::string
    > CustomDatatypeClass::GetMember(
      const std::string& name
    ) {
      std::unordered_set<CustomDatatypeMember>::iterator iter = std::find_if(
        m_members.begin(),
        m_members.end(),
        [&name](const CustomDatatypeMember& member) {
          return member.name == name;
        }
      );
      if(iter == m_members.end()) {
        return std::unexpected("Member '" + name + "' does not exists in struct " + m_name);
      }
      return *iter;
    }

    std::unordered_set<CustomDatatypeMemberFunction> CustomDatatypeClass::GetMemberFunctions() {
      return m_memberFunctions;
    }

    std::expected<
      CustomDatatypeMemberFunction,
      std::string
    > CustomDatatypeClass::GetMemberFunction(
      const std::string& name
    ) {
      std::unordered_set<CustomDatatypeMemberFunction>::iterator iter = std::find_if(
        m_memberFunctions.begin(),
        m_memberFunctions.end(),
        [&name](const CustomDatatypeMemberFunction& member) {
          return member.name == name;
        }
      );
      if(iter == m_memberFunctions.end()) {
        return std::unexpected("Member method '" + name + "' does not exists in struct " + m_name);
      }
      return *iter;
    }

    std::expected<
      Success,
      std::string
    > CustomDatatypeClass::AddMemberFunction(
      CustomDatatypeMemberFunction memberFunction
    ) {
      if(this->GetMemberFunction(memberFunction.name)) {
        return std::unexpected("Member function '" + memberFunction.name + "' already exists in struct " + m_name);
      }
      m_memberFunctions.insert(memberFunction);
      return Success{};
    }

    bool CustomDatatypeClass::operator==(
      const std::shared_ptr<DatatypeClass>& other
    ) {
      if(other->isNative()) {
        return false;
      }
      return *this == std::dynamic_pointer_cast<CustomDatatypeClass>(other);
    }

    bool CustomDatatypeClass::operator==(
      const std::shared_ptr<CustomDatatypeClass>& other
    ) {
      return m_members == other->GetMembers();
    }

    bool CustomDatatypeClass::isNative() {
      return false;
    }
  }
}
