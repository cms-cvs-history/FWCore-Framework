/*
 *  proxyfactoryproducer_t.cc
 *  EDMProto
 *
 *  Created by Chris Jones on 4/8/05.
 *
 */

#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"

#include "FWCore/Framework/interface/ProxyFactoryProducer.h"
#include "FWCore/Framework/interface/ProxyFactoryTemplate.h"

#include "FWCore/Framework/interface/DataProxyTemplate.h"
#include "FWCore/Framework/test/DummyData.h"
#include "FWCore/Framework/test/DummyRecord.h"


using edm::eventsetup::test::DummyData;
using namespace edm::eventsetup;

class DummyProxy : public DataProxyTemplate<DummyRecord, DummyData> {
public:
   DummyProxy() {}
   
protected:
   
   const value_type* make(const record_type&, const DataKey&) {
      return static_cast<const value_type*>(0) ;
   }
   void invalidateCache() {
   }   
private:
};


class Test1Producer : public ProxyFactoryProducer {
public:
   Test1Producer() {
      std::auto_ptr<ProxyFactoryTemplate<DummyProxy> > pFactory(new 
                                                                 ProxyFactoryTemplate<DummyProxy>());
      registerFactory(pFactory);
   }
};

BOOST_AUTO_UNIT_TEST(register_proxyfactorytemplate_test)
{
   Test1Producer testProd;
   EventSetupRecordKey dummyRecordKey = EventSetupRecordKey::makeKey<DummyRecord>();
   BOOST_CHECK(testProd.isUsingRecord(dummyRecordKey));

   const DataProxyProvider::KeyedProxies& keyedProxies =
      testProd.keyedProxies(dummyRecordKey);

   BOOST_CHECK(keyedProxies.size() == 1);
   BOOST_CHECK(0 != dynamic_cast<DummyProxy*>(&(*(keyedProxies.front().second))));
}
