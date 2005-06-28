/*
 *  eventsetuprecord_t.cc
 *  EDMProto
 *
 *  Created by Chris Jones on 3/29/05.
 *
 */

#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"
#include "FWCore/CoreFramework/interface/EventSetupRecordImplementation.h"

#include "FWCore/CoreFramework/interface/EventSetupRecordProviderTemplate.h"
#include "FWCore/CoreFramework/interface/EventSetupRecordProviderFactoryManager.h"
#include "FWCore/CoreFramework/interface/EventSetupRecordProviderFactoryTemplate.h"

using namespace edm;
using namespace edm::eventsetup;

class DummyRecord : public edm::eventsetup::EventSetupRecordImplementation<DummyRecord> { public:
   const DataProxy* find(const edm::eventsetup::DataKey& iKey ) const {
      return edm::eventsetup::EventSetupRecord::find( iKey );
   }
};

#include "FWCore/CoreFramework/interface/HCMethods.icc"
//#include "FWCore/CoreFramework/interface/HCTypeTag.icc"
#include "FWCore/CoreFramework/interface/HCTypeTagTemplate.icc"
//HCMethods<T, T, EventSetup, EventSetupRecordKey, EventSetupRecordKey::IdTag >
template<>
const char*
edm::eventsetup::heterocontainer::HCTypeTagTemplate<DummyRecord, edm::eventsetup::EventSetupRecordKey>::className() {
   return "DummyRecord";
}

//create an instance of the factory
static eventsetup::EventSetupRecordProviderFactoryTemplate<DummyRecord> s_factory;

BOOST_AUTO_UNIT_TEST(factory_test)
{
   std::auto_ptr<EventSetupRecordProvider> dummyProvider =
   EventSetupRecordProviderFactoryManager::instance().makeRecordProvider(
                              EventSetupRecordKey::makeKey<DummyRecord>());
   
   BOOST_CHECK(0 != dynamic_cast<EventSetupRecordProviderTemplate<DummyRecord>*>(&(*dummyProvider)));

}   

namespace eventsetuprecord_t {
   class Dummy {};
}
using eventsetuprecord_t::Dummy;

template<>
const char*
edm::eventsetup::heterocontainer::HCTypeTagTemplate<Dummy, edm::eventsetup::DataKey>::className() {
   return "Dummy";
}

#include "FWCore/CoreFramework/interface/DataProxyTemplate.h"

class FailingDummyProxy : public eventsetup::DataProxyTemplate<DummyRecord, Dummy> {
protected:
   const value_type* make(const record_type&, const DataKey&) {
      return 0 ;
   }
   void invalidateCache() {
   }   
};

class WorkingDummyProxy : public eventsetup::DataProxyTemplate<DummyRecord, Dummy> {
public:
   WorkingDummyProxy(const Dummy* iDummy) : data_(iDummy) {}

protected:
   
   const value_type* make(const record_type&, const DataKey&) {
      return data_ ;
   }
   void invalidateCache() {
   }   
private:
   const Dummy* data_;
};

BOOST_AUTO_UNIT_TEST(proxy_test)
{
   DummyRecord dummyRecord;
   FailingDummyProxy dummyProxy;
   
   const DataKey dummyDataKey(DataKey::makeTypeTag<FailingDummyProxy::value_type>(),
                              "");
   
   BOOST_CHECK(0 == dummyRecord.find(dummyDataKey));
   //BOOST_CHECK_THROW(dummyRecord.get(dummyPtr), edm::eventsetup::MakeDataException<DummyRecord,Dummy>);

   
   dummyRecord.add(dummyDataKey,
                    &dummyProxy);
   
   BOOST_CHECK(&dummyProxy == dummyRecord.find(dummyDataKey));

   const DataKey dummyFredDataKey(DataKey::makeTypeTag<FailingDummyProxy::value_type>(),
                                  "fred");
   BOOST_CHECK(0 == dummyRecord.find(dummyFredDataKey));

}

#include "FWCore/CoreFramework/interface/ESHandle.h"
#include "FWCore/CoreFramework/interface/recordGetImplementation.icc"

BOOST_AUTO_UNIT_TEST(get_test)
{
   DummyRecord dummyRecord;
   FailingDummyProxy dummyProxy;

   const DataKey dummyDataKey(DataKey::makeTypeTag<FailingDummyProxy::value_type>(),
                              "");

   ESHandle<Dummy> dummyPtr;
   typedef edm::eventsetup::NoDataException<Dummy> NoDataExceptionType;
   BOOST_CHECK_THROW(dummyRecord.get(dummyPtr), NoDataExceptionType) ;
   
   dummyRecord.add(dummyDataKey,
                    &dummyProxy);

   typedef edm::eventsetup::MakeDataException<DummyRecord,Dummy> ExceptionType;
   BOOST_CHECK_THROW(dummyRecord.get(dummyPtr), ExceptionType);

   Dummy myDummy;
   WorkingDummyProxy workingProxy(&myDummy);
   
   const DataKey workingDataKey(DataKey::makeTypeTag<WorkingDummyProxy::value_type>(),
                              "working");

   dummyRecord.add(workingDataKey,
                    &workingProxy);

   dummyRecord.get(dummyPtr, "working");
   
   BOOST_CHECK(&(*dummyPtr) == &myDummy);

   const std::string workingString("working");
   
   dummyRecord.get(dummyPtr, workingString);
   BOOST_CHECK(&(*dummyPtr) == &myDummy);
   
}

BOOST_AUTO_UNIT_TEST(doGet_test)
{
   DummyRecord dummyRecord;
   FailingDummyProxy dummyProxy;
   
   const DataKey dummyDataKey(DataKey::makeTypeTag<FailingDummyProxy::value_type>(),
                              "");
   
   BOOST_CHECK(!dummyRecord.doGet(dummyDataKey)) ;
   
   dummyRecord.add(dummyDataKey,
                   &dummyProxy);
   
   typedef edm::eventsetup::MakeDataException<DummyRecord,Dummy> ExceptionType;
   BOOST_CHECK_THROW(dummyRecord.doGet(dummyDataKey), ExceptionType);
   
   Dummy myDummy;
   WorkingDummyProxy workingProxy(&myDummy);
   
   const DataKey workingDataKey(DataKey::makeTypeTag<WorkingDummyProxy::value_type>(),
                                "working");
   
   dummyRecord.add(workingDataKey,
                   &workingProxy);
   
   BOOST_CHECK(dummyRecord.doGet(workingDataKey) );
   
}

