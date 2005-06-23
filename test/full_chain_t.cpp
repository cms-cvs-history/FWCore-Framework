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

#include "FWCore/CoreFramework/interface/EventSetup.h"
#include "FWCore/CoreFramework/interface/EventSetupProvider.h"
#include "FWCore/CoreFramework/interface/Timestamp.h"
#include "FWCore/CoreFramework/interface/DataProxyProvider.h"

#include "FWCore/CoreFramework/interface/HCMethods.icc"

#include "FWCore/CoreFramework/interface/recordGetImplementation.icc"

#include "FWCore/CoreFramework/interface/ESHandle.h"

#include "FWCore/CoreFramework/interface/DataProxyTemplate.h"

#include "FWCore/CoreFramework/interface/EventSetupRecordIntervalFinder.h"

#include "FWCore/CoreFramework/test/DummyRecord.h"
#include "FWCore/CoreFramework/test/DummyData.h"
#include "FWCore/CoreFramework/test/DummyFinder.h"
#include "FWCore/CoreFramework/test/DummyProxyProvider.h"

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
