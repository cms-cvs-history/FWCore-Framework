// -*- C++ -*-
//
// Package:     CoreFramework
// Class  :     eventsetup_plugin_t
// 
// Implementation:
//     <Notes on implementation>
//
// Author:      Chris Jones
// Created:     Thu May 26 11:01:19 EDT 2005
// $Id: eventsetup_plugin_t.cpp,v 1.2 2005/06/23 20:01:12 wmtan Exp $
//

// system include files
#define BOOST_AUTO_TEST_MAIN
#include "boost/test/auto_unit_test.hpp"

// user include files
#include "PluginManager/PluginManager.h"

#include "FWCore/CoreFramework/interface/SourceFactory.h"
#include "FWCore/CoreFramework/interface/ModuleFactory.h"
#include "FWCore/CoreFramework/interface/EventSetupProvider.h"


//
// constants, enums and typedefs
//
static void doInit() {
   static bool firstTime=true;
   if(firstTime) {
      seal::PluginManager::get()->initialise();
      firstTime = false;
   }
}
using namespace edm::eventsetup;

BOOST_AUTO_UNIT_TEST(finder_test)
{
   doInit();
   
   EventSetupProvider provider;
   
   edm::ParameterSet dummyFinderPSet;
   dummyFinderPSet.insert(true, "module_type", edm::Entry(std::string("LoadableDummyFinder"), true));
   dummyFinderPSet.insert(true, "module_label", edm::Entry(std::string(""), true));
   SourceFactory::get()->addTo(provider, dummyFinderPSet, "RECO", 1, 1);
   
   edm::ParameterSet dummyProviderPSet;
   dummyProviderPSet.insert(true, "module_type", edm::Entry(std::string("LoadableDummyProvider"), true));
   dummyProviderPSet.insert(true, "module_label", edm::Entry(std::string(""), true));
   ModuleFactory::get()->addTo(provider, dummyProviderPSet, "RECO", 1, 1);
   
}
