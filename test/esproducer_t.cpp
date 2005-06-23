/*
 *  proxyfactoryproducer_t.cc
 *  EDMProto
 *
 *  Created by Chris Jones on 4/8/05.
 *
 */
#include <iostream>

#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"

#include "FWCore/CoreFramework/interface/ESProducer.h"

#include "FWCore/CoreFramework/test/DummyData.h"
#include "FWCore/CoreFramework/test/DummyRecord.h"
#include "FWCore/CoreFramework/test/DummyFinder.h"

#include "FWCore/CoreFramework/test/DepRecord.h"

#include "FWCore/CoreFramework/interface/EventSetupProvider.h"
#include "FWCore/CoreFramework/interface/ESHandle.h"

using edm::eventsetup::test::DummyData;
using namespace edm::eventsetup;


class Test1Producer : public ESProducer {
public:
   Test1Producer() {
      data_.value_ = 0;
      setWhatProduced(this);
   }
   const DummyData* produce( const DummyRecord& iRecord ) {
      ++data_.value_;
      std::cout <<"produce called "<<data_.value_<<std::endl;
      return &data_;
   }
private:
   DummyData data_;
};

class ShareProducer : public ESProducer {
public:
   ShareProducer(): ptr_( new DummyData ){
      ptr_->value_ = 0;
      setWhatProduced(this);
   }
   boost::shared_ptr<DummyData> produce( const DummyRecord& iRecord ) {
      ++ptr_->value_;
      std::cout <<"produce called "<<ptr_->value_<<std::endl;
      return ptr_;
   }
private:
   boost::shared_ptr<DummyData> ptr_;
};

BOOST_AUTO_UNIT_TEST( register_test )
{
   Test1Producer testProd;
   EventSetupRecordKey dummyRecordKey = EventSetupRecordKey::makeKey<DummyRecord>();
   BOOST_CHECK(testProd.isUsingRecord( dummyRecordKey ) );

   const DataProxyProvider::KeyedProxies& keyedProxies =
      testProd.keyedProxies( dummyRecordKey  );

   BOOST_CHECK( keyedProxies.size() == 1 );
}

BOOST_AUTO_UNIT_TEST( get_from_test )
{
   EventSetupProvider provider;
   
   boost::shared_ptr<DataProxyProvider> pProxyProv( new Test1Producer );
   provider.add( pProxyProv );
   
   boost::shared_ptr<DummyFinder> pFinder( new DummyFinder);
   provider.add( boost::shared_ptr<EventSetupRecordIntervalFinder>(pFinder) );
   
   for( int iTime=1; iTime != 6; ++iTime) {
      pFinder->setInterval( edm::ValidityInterval(iTime,iTime) );
      const edm::EventSetup& eventSetup = provider.eventSetupForInstance( edm::Timestamp(iTime) );
      ESHandle<DummyData> pDummy;
      eventSetup.get<DummyRecord>().get(pDummy);
      BOOST_CHECK(0 != &(*pDummy) );
      std::cout <<pDummy->value_ << std::endl;
      BOOST_CHECK( iTime == pDummy->value_ );
   }
}

BOOST_AUTO_UNIT_TEST( get_from_share_test )
{
   EventSetupProvider provider;
   
   boost::shared_ptr<DataProxyProvider> pProxyProv( new ShareProducer );
   provider.add( pProxyProv );
   
   boost::shared_ptr<DummyFinder> pFinder( new DummyFinder);
   provider.add( boost::shared_ptr<EventSetupRecordIntervalFinder>(pFinder) );
   
   for( int iTime=1; iTime != 6; ++iTime) {
      pFinder->setInterval( edm::ValidityInterval(iTime,iTime) );
      const edm::EventSetup& eventSetup = provider.eventSetupForInstance( edm::Timestamp(iTime) );
      ESHandle<DummyData> pDummy;
      eventSetup.get<DummyRecord>().get(pDummy);
      BOOST_CHECK(0 != &(*pDummy) );
      std::cout <<pDummy->value_ << std::endl;
      BOOST_CHECK( iTime == pDummy->value_ );
   }
}

struct TestDecorator {
   static int s_pre;
   static int s_post;
   
   void pre( const DummyRecord& ) {
      ++s_pre;
   }

   void post( const DummyRecord& ) {
      ++s_post;
   }   
};

int TestDecorator::s_pre = 0;
int TestDecorator::s_post = 0;

class DecoratorProducer : public ESProducer {
public:
   DecoratorProducer(): ptr_( new DummyData ){
      ptr_->value_ = 0;
      setWhatProduced(this, TestDecorator() );
   }
   boost::shared_ptr<DummyData> produce( const DummyRecord& iRecord ) {
      ++ptr_->value_;
      std::cout <<"produce called "<<ptr_->value_<<std::endl;
      return ptr_;
   }
private:
   boost::shared_ptr<DummyData> ptr_;
};

BOOST_AUTO_UNIT_TEST( decorator_test )
{
   EventSetupProvider provider;
   
   boost::shared_ptr<DataProxyProvider> pProxyProv( new DecoratorProducer );
   provider.add( pProxyProv );
   
   boost::shared_ptr<DummyFinder> pFinder( new DummyFinder);
   provider.add( boost::shared_ptr<EventSetupRecordIntervalFinder>(pFinder) );
   
   for( int iTime=1; iTime != 6; ++iTime) {
      pFinder->setInterval( edm::ValidityInterval(iTime,iTime) );
      const edm::EventSetup& eventSetup = provider.eventSetupForInstance( edm::Timestamp(iTime) );
      ESHandle<DummyData> pDummy;
      
      BOOST_CHECK( iTime - 1 == TestDecorator::s_pre );
      BOOST_CHECK( iTime - 1 == TestDecorator::s_post );
      eventSetup.get<DummyRecord>().get(pDummy);
      BOOST_CHECK(0 != &(*pDummy) );
      std::cout <<"pre "<<TestDecorator::s_pre << " post " << TestDecorator::s_post << std::endl;
      BOOST_CHECK( iTime  == TestDecorator::s_pre );
      BOOST_CHECK( iTime  == TestDecorator::s_post );
      BOOST_CHECK( iTime == pDummy->value_ );
   }
}

class DepProducer : public ESProducer {
public:
   DepProducer(): ptr_( new DummyData ){
      ptr_->value_ = 0;
      setWhatProduced(this , dependsOn( &DepProducer::callWhenDummyChanges, 
                                        &DepProducer::callWhenDummyChanges2,
                                        &DepProducer::callWhenDummyChanges3 ) );
   }
   boost::shared_ptr<DummyData> produce( const DepRecord& iRecord ) {
      return ptr_;
   }
   void callWhenDummyChanges( const DummyRecord& ) {
      ++ptr_->value_;
      std::cout <<"callWhenDummyChanges called "<<ptr_->value_<<std::endl;
   }
   void callWhenDummyChanges2( const DummyRecord& ) {
      ++ptr_->value_;
      std::cout <<"callWhenDummyChanges2 called "<<ptr_->value_<<std::endl;
   }
   void callWhenDummyChanges3( const DummyRecord& ) {
      ++ptr_->value_;
      std::cout <<"callWhenDummyChanges3 called "<<ptr_->value_<<std::endl;
   }
   
private:
   boost::shared_ptr<DummyData> ptr_;
};

BOOST_AUTO_UNIT_TEST( dependsOn_test )
{
   EventSetupProvider provider;
   
   boost::shared_ptr<DataProxyProvider> pProxyProv( new DepProducer );
   provider.add( pProxyProv );
   
   boost::shared_ptr<DummyFinder> pFinder( new DummyFinder);
   provider.add( boost::shared_ptr<EventSetupRecordIntervalFinder>(pFinder) );
   
   for( int iTime=1; iTime != 6; ++iTime) {
      pFinder->setInterval( edm::ValidityInterval(iTime,iTime) );
      const edm::EventSetup& eventSetup = provider.eventSetupForInstance( edm::Timestamp(iTime) );
      ESHandle<DummyData> pDummy;
      
      eventSetup.get<DepRecord>().get(pDummy);
      BOOST_CHECK(0 != &(*pDummy) );
      BOOST_CHECK( 3*iTime == pDummy->value_ );
   }
}
