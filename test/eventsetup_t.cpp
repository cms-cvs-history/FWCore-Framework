/*
 *  eventsetup_t.cc
 *  EDMProto
 *
 *  Created by Chris Jones on 3/24/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EventSetupRecordImplementation.h"
#include "FWCore/Framework/interface/EventSetupProvider.h"
#include "FWCore/Framework/interface/Timestamp.h"

#include "FWCore/Framework/interface/eventSetupGetImplementation.icc"

#include "FWCore/Framework/test/DummyRecord.h"
//class DummyRecord : public edm::eventsetup::EventSetupRecordImplementation<DummyRecord> {};

#include "FWCore/Framework/interface/HCMethods.icc"
//#include "FWCore/Framework/interface/HCTypeTag.icc"
#include "FWCore/Framework/interface/HCTypeTagTemplate.h"
/*
template<>
const char*
edm::eventsetup::heterocontainer::HCTypeTagTemplate<DummyRecord, edm::eventsetup::EventSetupRecordKey>::className() {
   return "DummyRecord";
}
*/
using namespace edm;


BOOST_AUTO_UNIT_TEST(construct_test)
{
   eventsetup::EventSetupProvider provider;
   const Timestamp timestamp = 1;
   EventSetup const& eventSetup = provider.eventSetupForInstance(timestamp);
   BOOST_CHECK(&eventSetup != 0);
   BOOST_CHECK(eventSetup.timestamp() == timestamp);
}

BOOST_AUTO_UNIT_TEST(get_test)
{
   eventsetup::EventSetupProvider provider;
   EventSetup const& eventSetup = provider.eventSetupForInstance(Timestamp::invalidTimestamp());
   BOOST_CHECK(&eventSetup != 0);
   BOOST_CHECK_THROW(eventSetup.get<DummyRecord>(), edm::eventsetup::NoRecordException<DummyRecord>);
   
   DummyRecord dummyRecord;
   provider.addRecordToEventSetup(dummyRecord);
   const DummyRecord& gottenRecord = eventSetup.get<DummyRecord>();
   BOOST_CHECK(0 != &gottenRecord);
   BOOST_CHECK(&dummyRecord == &gottenRecord);
}

#include "FWCore/Framework/interface/EventSetupRecordProviderTemplate.h"

class DummyEventSetupProvider : public edm::eventsetup::EventSetupProvider {
public:
   template<class T>
   void insert(std::auto_ptr<T> iRecord) {
      edm::eventsetup::EventSetupProvider::insert(iRecord);
   }
};

BOOST_AUTO_UNIT_TEST(record_provider_test)
{
   DummyEventSetupProvider provider;
   typedef eventsetup::EventSetupRecordProviderTemplate<DummyRecord> DummyRecordProvider;
   std::auto_ptr<DummyRecordProvider > dummyRecordProvider(new DummyRecordProvider());
   
   provider.insert(dummyRecordProvider);
   
   //NOTE: use 'invalid' timestamp since the default 'interval of validity'
   //       for a Record is presently an 'invalid' timestamp on both ends.
   //       Since the EventSetup::get<> will only retrieve a Record if its
   //       interval of validity is 'valid' for the present 'instance'
   //       this is a 'hack' to have the 'get' succeed
   EventSetup const& eventSetup = provider.eventSetupForInstance(Timestamp::invalidTimestamp());
   const DummyRecord& gottenRecord = eventSetup.get<DummyRecord>();
   BOOST_CHECK(0 != &gottenRecord);
}

#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"

class DummyFinder : public eventsetup::EventSetupRecordIntervalFinder {
public:
   DummyFinder() {
      this->findingRecord<DummyRecord>();
   }

   void setInterval(const ValidityInterval& iInterval) {
      interval_ = iInterval;
   }
protected:
   virtual void setIntervalFor(const eventsetup::EventSetupRecordKey&,
                                const Timestamp& iTime, 
                                ValidityInterval& iInterval) {
      if(interval_.validFor(iTime)) {
         iInterval = interval_;
      } else {
         iInterval = ValidityInterval();
      }
   }
private:
   ValidityInterval interval_;   
};


BOOST_AUTO_UNIT_TEST(record_validity_test)
{
   DummyEventSetupProvider provider;
   typedef eventsetup::EventSetupRecordProviderTemplate<DummyRecord> DummyRecordProvider;
   std::auto_ptr<DummyRecordProvider > dummyRecordProvider(new DummyRecordProvider());

   boost::shared_ptr<DummyFinder> finder(new DummyFinder);
   dummyRecordProvider->addFinder(finder);
   
   provider.insert(dummyRecordProvider);
   
   {
      EventSetup const& eventSetup = provider.eventSetupForInstance(Timestamp(1));
      BOOST_CHECK_THROW(eventSetup.get<DummyRecord>(), edm::eventsetup::NoRecordException<DummyRecord>);
   }

   finder->setInterval(ValidityInterval(Timestamp(2), Timestamp(3)));
   {
      EventSetup const& eventSetup = provider.eventSetupForInstance(Timestamp(2));
      eventSetup.get<DummyRecord>();
   }
   {
      EventSetup const& eventSetup = provider.eventSetupForInstance(Timestamp(3));
      eventSetup.get<DummyRecord>();
   }
   {
      EventSetup const& eventSetup = provider.eventSetupForInstance(Timestamp(4));
      BOOST_CHECK_THROW(eventSetup.get<DummyRecord>(), edm::eventsetup::NoRecordException<DummyRecord>);
   }
   
   
}

#include "FWCore/Framework/interface/DataProxyProvider.h"

class DummyProxyProvider : public eventsetup::DataProxyProvider {
public:
   DummyProxyProvider() {
      usingRecord<DummyRecord>();
   }
   void newInterval(const eventsetup::EventSetupRecordKey& iRecordType,
                     const ValidityInterval& iInterval) {
      //do nothing
   }
protected:
   void registerProxies(const eventsetup::EventSetupRecordKey&, KeyedProxies& iHolder) {
   }

};

#include "FWCore/Framework/interface/EventSetupRecordProviderFactoryTemplate.h"
//create an instance of the factory
static eventsetup::EventSetupRecordProviderFactoryTemplate<DummyRecord> s_factory;

BOOST_AUTO_UNIT_TEST(proxy_provider_test)
{
   eventsetup::EventSetupProvider provider;
   boost::shared_ptr<eventsetup::DataProxyProvider> dummyProv(new DummyProxyProvider());
   provider.add(dummyProv);
   
   EventSetup const& eventSetup = provider.eventSetupForInstance(Timestamp::invalidTimestamp());
   const DummyRecord& gottenRecord = eventSetup.get<DummyRecord>();
   BOOST_CHECK(0 != &gottenRecord);
}
