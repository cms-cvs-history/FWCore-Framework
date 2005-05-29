/*
 *  interval_t.cc
 *  EDMProto
 *
 *  Created by Chris Jones on 3/30/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */


#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"
#include "FWCore/CoreFramework/interface/Timestamp.h"
#include "FWCore/CoreFramework/interface/ValidityInterval.h"

using edm::Timestamp;
using edm::ValidityInterval;

BOOST_AUTO_UNIT_TEST( comparison_test )
{
   const Timestamp invalid( Timestamp::invalidTimestamp() );
   
   const Timestamp one(1);
   const Timestamp two(2);
   
   BOOST_CHECK( invalid == Timestamp::invalidTimestamp() );
   BOOST_CHECK( one == Timestamp(1) );
   
   BOOST_CHECK( invalid != one);

   BOOST_CHECK( one < two );
   BOOST_CHECK( !( one > two ) );
   BOOST_CHECK( two > one );
   BOOST_CHECK( !( two < one ) );
   
   BOOST_CHECK( one != two );
   BOOST_CHECK( ! ( one == two ) );

   BOOST_CHECK( one <= two );
   BOOST_CHECK( one <= one );
   BOOST_CHECK( one >= one );
   BOOST_CHECK( !( one >= two ) );
}

BOOST_AUTO_UNIT_TEST( timestamp_assignment_test )
{
   const Timestamp one(1);
   
   Timestamp temp( Timestamp::invalidTimestamp() );
   BOOST_CHECK( temp != one );
   temp = one;
   BOOST_CHECK(temp == one );
}

BOOST_AUTO_UNIT_TEST( interval_assignment_test )
{
   ValidityInterval temp;
   const ValidityInterval oneAndTwo( Timestamp(1), Timestamp(2) );
   
   BOOST_CHECK( temp != oneAndTwo );
   BOOST_CHECK( ! (temp == oneAndTwo ) );
   
   temp = oneAndTwo;
   BOOST_CHECK( temp == oneAndTwo );
}
