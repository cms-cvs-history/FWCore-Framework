/*
 *  esproducts_t.cc
 *  EDMProto
 *
 *  Created by Chris Jones on 4/18/05.
 *
 */

#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"

#include "FWCore/CoreFramework/interface/ESProducts.h"
using namespace edm::eventsetup::produce;

ESProducts<const int*, const float*>
returnPointers(const int* iInt, const float* iFloat)
{
   return products(iInt, iFloat);
}

BOOST_AUTO_UNIT_TEST(const_ptr_test)
{
   int int_ = 0;
   float float_ = 0;
   
   ESProducts<const int*, const float*> product = 
      returnPointers(&int_, &float_);
   
   const int* readInt = 0;
   const float* readFloat = 0;

   product.assignTo(readInt);
   product.assignTo(readFloat);
   
   BOOST_CHECK(readInt == &int_);
   BOOST_CHECK(readFloat == &float_);
}

   
