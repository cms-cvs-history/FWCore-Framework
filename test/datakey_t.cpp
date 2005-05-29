/*
 *  datakey_t.cc
 *  EDMProto
 *
 *  Created by Chris Jones on 3/31/05.
 *
 */

#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"
#include <cstring>

#include "FWCore/CoreFramework/interface/DataKey.h"

using namespace edm;
using namespace edm::eventsetup;

BOOST_AUTO_UNIT_TEST( name_tag_construction_test )
{
   const NameTag defaultTag;
   BOOST_CHECK( 0 == std::strcmp("", defaultTag.value() ) );
   
   const NameTag namedTag("fred");
   BOOST_CHECK( 0 == std::strcmp("fred", namedTag.value() ) );
}

BOOST_AUTO_UNIT_TEST( name_tag_comparison_test )
{
   const NameTag defaultTag;
   BOOST_CHECK( defaultTag == defaultTag );
   
   const NameTag fredTag("fred");
   BOOST_CHECK( fredTag == fredTag );
   
   BOOST_CHECK( ! (defaultTag == fredTag ) );
   
   const NameTag barneyTag("barney");
   
   BOOST_CHECK( barneyTag < fredTag );
}

BOOST_AUTO_UNIT_TEST( name_tag_copy_test )
{
   const NameTag defaultTag;
   NameTag tester( defaultTag );
   BOOST_CHECK( tester == defaultTag );
   
   const NameTag fredTag("fred");
   tester = fredTag;
   BOOST_CHECK( tester == fredTag );
}

namespace datakey_t {
   class Dummy {};
   class Dummy2 {};
}
using datakey_t::Dummy;
using datakey_t::Dummy2;

#include "FWCore/CoreFramework/interface/HCTypeTagTemplate.icc"
template<>
const char*
edm::eventsetup::heterocontainer::HCTypeTagTemplate<Dummy, edm::eventsetup::DataKey>::className() {
   return "Dummy";
}

template<>
const char*
edm::eventsetup::heterocontainer::HCTypeTagTemplate<Dummy2, edm::eventsetup::DataKey>::className() {
   return "Dummy2";
}

BOOST_AUTO_UNIT_TEST( construction_test )
{
   DataKey defaultKey;
   BOOST_CHECK( TypeTag() == defaultKey.type() );
   BOOST_CHECK( 0 == std::strcmp( "", defaultKey.name().value() ) );

   DataKey dummyKey( DataKey::makeTypeTag<Dummy>(), "");
   BOOST_CHECK( DataKey::makeTypeTag<Dummy>() == dummyKey.type() );
   BOOST_CHECK( 0 == std::strcmp( "", dummyKey.name().value() ) );

   DataKey namedDummyKey( DataKey::makeTypeTag<Dummy>(), "fred");
   BOOST_CHECK( DataKey::makeTypeTag<Dummy>() == namedDummyKey.type() );
   BOOST_CHECK( 0 == std::strcmp( "fred", namedDummyKey.name().value() ) );
}

BOOST_AUTO_UNIT_TEST( comparison_test )
{
   const DataKey defaultKey;
   BOOST_CHECK( defaultKey == defaultKey );
   BOOST_CHECK( !( defaultKey < defaultKey ) );
   
   const DataKey dummyKey( DataKey::makeTypeTag<Dummy>(), "");
   const DataKey fredDummyKey( DataKey::makeTypeTag<Dummy>(), "fred");
   const DataKey barneyDummyKey( DataKey::makeTypeTag<Dummy>(), "barney");

   BOOST_CHECK( ! ( defaultKey == dummyKey ) );
   BOOST_CHECK( dummyKey == dummyKey );
   BOOST_CHECK( ! (dummyKey == fredDummyKey ) );
   
   BOOST_CHECK( barneyDummyKey == barneyDummyKey );
   BOOST_CHECK( barneyDummyKey < fredDummyKey );
   BOOST_CHECK( !( fredDummyKey < barneyDummyKey ) );
   BOOST_CHECK( !(barneyDummyKey == fredDummyKey ) );
   
   const DataKey dummy2Key( DataKey::makeTypeTag<Dummy2>(), "");

   BOOST_CHECK( ! (dummy2Key == dummyKey ) );
}

BOOST_AUTO_UNIT_TEST( copy_test )
{
   const DataKey defaultKey;
   DataKey tester(defaultKey);
   BOOST_CHECK( tester == defaultKey );
   
   const DataKey dummyKey( DataKey::makeTypeTag<Dummy>(), "");
   tester = dummyKey;
   BOOST_CHECK( tester == dummyKey );
   const DataKey fredDummyKey( DataKey::makeTypeTag<Dummy>(), "fred");
   tester = fredDummyKey;
   BOOST_CHECK( tester == fredDummyKey );

   DataKey tester2( fredDummyKey );
   BOOST_CHECK( tester2 == fredDummyKey );
}

BOOST_AUTO_UNIT_TEST( nocopy_construction_test )
{
   const DataKey fredDummyKey( DataKey::makeTypeTag<Dummy>(), "fred");
   const DataKey noCopyFredDummyKey( DataKey::makeTypeTag<Dummy>(), "fred", DataKey::kDoNotCopyMemory );

   BOOST_CHECK( fredDummyKey == noCopyFredDummyKey );
   
   const DataKey copyFredDummyKey( noCopyFredDummyKey );
   BOOST_CHECK( copyFredDummyKey == noCopyFredDummyKey );
   
   DataKey copy2FredDummyKey;
   copy2FredDummyKey = noCopyFredDummyKey;
   BOOST_CHECK( copy2FredDummyKey == noCopyFredDummyKey );
   
   DataKey noCopyBarneyDummyKey( DataKey::makeTypeTag<Dummy>(), "barney", DataKey::kDoNotCopyMemory );

   noCopyBarneyDummyKey = noCopyFredDummyKey;
   BOOST_CHECK( noCopyBarneyDummyKey == noCopyFredDummyKey );
}
