/*
 *  full_chain_test.cc
 *  EDMProto
 *
 *  Created by Chris Jones on 4/3/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EventSetupProvider.h"
#include "FWCore/Framework/interface/Timestamp.h"
#include "FWCore/Framework/interface/DataProxyProvider.h"

#include "FWCore/Framework/interface/HCMethods.icc"

#include "FWCore/Framework/interface/recordGetImplementation.icc"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/DataProxyTemplate.h"

#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"

#include "FWCore/Framework/test/DummyRecord.h"
#include "FWCore/Framework/test/DummyData.h"
#include "FWCore/Framework/test/DummyFinder.h"
#include "FWCore/Framework/test/DummyProxyProvider.h"

using namespace edm;
using namespace edm::eventsetup;
using namespace edm::eventsetup::test;


BOOST_AUTO_UNIT_TEST(get_from_dataproxyprovider_test)
{
   eventsetup::EventSetupProvider provider;

   boost::shared_ptr<DataProxyProvider> pProxyProv(new DummyProxyProvider);
   provider.add(pProxyProv);
   
   boost::shared_ptr<DummyFinder> pFinder(new DummyFinder);
   provider.add(boost::shared_ptr<EventSetupRecordIntervalFinder>(pFinder));

   pFinder->setInterval(ValidityInterval(1,5));
   for(unsigned int iTime=1; iTime != 6; ++iTime) {
      EventSetup const& eventSetup = provider.eventSetupForInstance(Timestamp(iTime));
      ESHandle<DummyData> pDummy;
      eventSetup.get<DummyRecord>().get(pDummy);
      BOOST_CHECK(0 != &(*pDummy));
      
      eventSetup.getData(pDummy);
   
      BOOST_CHECK(0 != &(*pDummy));
   }
}
