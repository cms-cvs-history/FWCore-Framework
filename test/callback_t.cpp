/*
 *  callback_t.cc
 *  EDMProto
 *
 *  Created by Chris Jones on 4/17/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include <memory>

#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"


#include "FWCore/CoreFramework/interface/Callback.h"
#include "FWCore/CoreFramework/interface/ESProducts.h"

namespace callbacktest {
   struct Data {
      Data() : value_(0) {}
      Data(int iValue) : value_(iValue) {}
      virtual ~Data() {}
      int value_;
   };
   
   struct Double {
      Double() : value_(0) {}
      Double(double iValue) : value_(iValue) {}
      virtual ~Double() {}
      double value_;
   };

   struct Record { };
   
   struct ConstPtrProd {
      const Data* method(const Record&) {
         ++data_.value_;
         return &data_;
      }      
      Data data_;
   };

   struct AutoPtrProd {
      AutoPtrProd() : value_(0) {}
      std::auto_ptr<Data> method(const Record&) {
         std::auto_ptr<Data> temp(new Data(++value_));
         return temp;
      }
      
      int value_;
   };

   struct SharedPtrProd {
      SharedPtrProd() : ptr_(new Data()) {}
      boost::shared_ptr<Data> method(const Record&) {
         ++ptr_->value_;
         return ptr_;
      }      
      boost::shared_ptr<Data> ptr_;
   };
   
   struct PtrProductsProd {
      edm::eventsetup::produce::ESProducts<const Data*, const Double*> method(const Record&) {
         using namespace edm::eventsetup::produce;
         const Data* dataT = &data_;
         const Double* doubleT = &double_;
         ++data_.value_;
         ++double_.value_;
         return products(dataT, doubleT);
      }
      Data data_;
      Double double_;
   };
}

using namespace callbacktest;
using namespace edm::eventsetup;
typedef Callback<ConstPtrProd, const Data*, Record> ConstPtrCallback;

BOOST_AUTO_UNIT_TEST(const_ptr_test)
{
   ConstPtrProd prod;

   ConstPtrCallback callback(&prod, &ConstPtrProd::method);
   const Data* handle;


   callback.holdOntoPointer(&handle);
   
   Record record;
   callback.newRecordComing();
   callback(record);
   BOOST_CHECK(handle == &(prod.data_));
   BOOST_CHECK(prod.data_.value_ == 1);

   //since haven't cleared, should not have changed
   callback(record);
   BOOST_CHECK(handle == &(prod.data_));
   BOOST_CHECK(prod.data_.value_ == 1);

   callback.newRecordComing();

   callback(record);
   BOOST_CHECK(handle == &(prod.data_));
   BOOST_CHECK(prod.data_.value_ == 2);
   
}

typedef Callback<AutoPtrProd, std::auto_ptr<Data>, Record> AutoPtrCallback;

BOOST_AUTO_UNIT_TEST(auto_ptr_test)
{
   AutoPtrProd prod;
   
   AutoPtrCallback callback(&prod, &AutoPtrProd::method);
   std::auto_ptr<Data> handle;
   
   
   callback.holdOntoPointer(&handle);
   
   Record record;
   callback.newRecordComing();
   callback(record);
   BOOST_CHECK(0 != handle.get());
   BOOST_CHECK(prod.value_ == 1);
   assert(0 != handle.get());
   BOOST_CHECK(prod.value_ == handle->value_);
   
   //since haven't cleared, should not have changed
   callback(record);
   BOOST_CHECK(prod.value_ == 1);
   BOOST_CHECK(prod.value_ == handle->value_);
   
   handle.release();

   callback.newRecordComing();
   
   callback(record);
   BOOST_CHECK(0 != handle.get());
   BOOST_CHECK(prod.value_ == 2);
   assert(0 != handle.get());
   BOOST_CHECK(prod.value_ == handle->value_);
   
}

typedef Callback<SharedPtrProd, boost::shared_ptr<Data>, Record> SharedPtrCallback;

BOOST_AUTO_UNIT_TEST(shared_ptr_test)
{
   SharedPtrProd prod;
   
   SharedPtrCallback callback(&prod, &SharedPtrProd::method);
   boost::shared_ptr<Data> handle;
   
   
   callback.holdOntoPointer(&handle);
   
   Record record;
   callback.newRecordComing();
   callback(record);
   BOOST_CHECK(handle.get() == prod.ptr_.get());
   BOOST_CHECK(prod.ptr_->value_ == 1);
   
   //since haven't cleared, should not have changed
   callback(record);
   BOOST_CHECK(handle.get() == prod.ptr_.get());
   BOOST_CHECK(prod.ptr_->value_ == 1);
   
   handle = boost::shared_ptr<Data>() ;
   callback.newRecordComing();
   
   callback(record);
   BOOST_CHECK(handle.get() == prod.ptr_.get());
   BOOST_CHECK(prod.ptr_->value_ == 2);
   
}

typedef Callback<PtrProductsProd, edm::eventsetup::produce::ESProducts<const Data*, const Double*>, Record> PtrProductsCallback;

BOOST_AUTO_UNIT_TEST(ptr_products_test)
{
   PtrProductsProd prod;
   
   PtrProductsCallback callback(&prod, &PtrProductsProd::method);
   const Data* handle;
   const Double* doubleHandle;
   
   callback.holdOntoPointer(&handle);
   callback.holdOntoPointer(&doubleHandle);
   
   Record record;
   callback.newRecordComing();
   callback(record);
   BOOST_CHECK(handle == &(prod.data_));
   BOOST_CHECK(prod.data_.value_ == 1);
   
   //since haven't cleared, should not have changed
   callback(record);
   BOOST_CHECK(handle == &(prod.data_));
   BOOST_CHECK(prod.data_.value_ == 1);
   
   callback.newRecordComing();
   
   callback(record);
   BOOST_CHECK(handle == &(prod.data_));
   BOOST_CHECK(prod.data_.value_ == 2);
   
}
