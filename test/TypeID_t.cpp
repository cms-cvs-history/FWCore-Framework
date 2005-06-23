/*----------------------------------------------------------------------

Test program for edm::TypeID class.

$Id: TypeID_t.cpp,v 1.1 2005/05/29 02:29:54 wmtan Exp $
 ----------------------------------------------------------------------*/
#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"

#include <cassert>
#include <iostream>
#include <string>

#include "FWCore/CoreFramework/src/TypeID.h"

namespace edmtest {
  struct empty { };
}

using std::cerr;
using std::endl;

BOOST_AUTO_UNIT_TEST(equality_test)
{
  edmtest::empty e;
  edm::TypeID id1(e);
  edm::TypeID id2(e);
  
  BOOST_CHECK(!(id1 < id2));
  BOOST_CHECK(!(id2 < id1));

  std::string n1(id1.name());
  std::string n2(id2.name());

  BOOST_CHECK(n1==n2);
}

BOOST_AUTO_UNIT_TEST(copy_test)
{
  edmtest::empty e;
  edm::TypeID id1(e);

  edm::TypeID id3=id1;
  BOOST_CHECK(!(id1 < id3));
  BOOST_CHECK(!(id3 < id1));

  std::string n1(id1.name());
  std::string n3(id3.name());
  BOOST_CHECK(n1== n3);
}
