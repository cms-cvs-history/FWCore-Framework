/*----------------------------------------------------------------------
  
$Id: TypeID.cc,v 1.11 2005/11/11 20:55:49 chrjones Exp $

----------------------------------------------------------------------*/
#include <ostream>
#include "FWCore/Framework/src/TypeID.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "Reflex/Type.h"
#include <string>

namespace edm {
  void
  TypeID::print(std::ostream& os) const {
    os << className();
  }

  std::string
  TypeID::className() const {
    seal::reflex::Type t = seal::reflex::Type::byTypeInfo(typeInfo());
    if (!bool(t)) {
      throw edm::Exception(errors::ProductNotFound,"NoMatch")
        << "TypeID::className: No dictionary for class " << name() << '\n';
    }
    return t.name(seal::reflex::SCOPED);
  }

  std::string 
  TypeID::userClassName() const {
    std::string name = className();
    if (name.find("edm::Wrapper") == 0) {
      stripTemplate(name);
    }
    return name;
  }

  std::string 
  TypeID::friendlyClassName() const {
    std::string name = userClassName();
    while (stripTemplate(name)) {}
    stripNamespace(name);
    return name;
  }

  bool
  TypeID::stripTemplate(std::string& name) {
    std::string const spec("<,>");
    char const space = ' ';
    std::string::size_type idx = name.find_first_of(spec);
    if (idx == std::string::npos) {
      return false;
    }
    std::string::size_type first = 0;
    std::string::size_type after = idx;
    if (name[idx] == '<') {
      after = name.rfind('>');
      assert (after != std::string::npos);
      first = ++idx;
    } else {
      name = name.substr(0, idx);
    }
    std::string::size_type idxa = after;
    while (space == name[--idxa]) --after;
    std::string::size_type idxf = first;
    while (space == name[idxf++]) ++first;
    name = name.substr(first, after - first);
    return true;
  }

  bool
  TypeID::stripNamespace(std::string& name) {
    std::string::size_type idx = name.rfind(':');
    bool ret = (idx != std::string::npos);
    if (ret) {
      ++idx;
      name = name.substr(idx);
    }
    return ret;
  }

  std::ostream&
  operator<<(std::ostream& os, const TypeID& id) {
    id.print(os);
    return os;
  }
}

