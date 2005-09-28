/**
   \file
   test InputSource for ProductRegistry

   \author Stefano ARGIRO
   \version $Id: TestInputSource4ProductRegistry.cc,v 1.5 2005/07/30 23:41:36 wmtan Exp $
   \date 21 Jul 2005
*/

static const char CVSId[] = "$Id: TestInputSource4ProductRegistry.cc,v 1.5 2005/07/30 23:41:36 wmtan Exp $";

#include <FWCore/Framework/interface/EventPrincipal.h>
#include <FWCore/Framework/interface/InputSource.h>
#include <FWCore/Framework/interface/InputSourceDescription.h>
#include <FWCore/Framework/interface/InputSourceMacros.h>
#include <FWCore/Framework/interface/ProductRegistry.h>
#include <FWCore/Framework/interface/ProductDescription.h>
#include <FWCore/Framework/src/TypeID.h>
#include <FWCore/Framework/src/ToyProducts.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>

namespace edm {

  class DelayedReader;
  class ParameterSet;
  class InputSourceDescription;
} 

  class TestInputSource4ProductRegistry : public edm::InputSource {

  public:
    TestInputSource4ProductRegistry(const edm::ParameterSet& pset, 
      const edm::InputSourceDescription& desc): InputSource(desc) {

      const  edm::ProductRegistry::ProductList& plist=
	desc.preg_->productList();
      edm::ProductRegistry::ProductList::const_iterator pd = plist.begin();

      
      edmtest::StringProduct stringprod;
      edm::TypeID stringID(stringprod);
      CPPUNIT_ASSERT(stringID.friendlyClassName() == 
		     pd->second.friendlyClassName_);
      CPPUNIT_ASSERT(pd->second.module.moduleLabel_=="m1");

      ++pd;

      edmtest::DoubleProduct dprod;
      edm::TypeID dID(dprod);
      CPPUNIT_ASSERT(dID.friendlyClassName() == 
		     pd->second.friendlyClassName_);
      CPPUNIT_ASSERT(pd->second.module.moduleLabel_=="m2");

    }



  private:
    virtual std::auto_ptr<edm::EventPrincipal> read() {
      return std::auto_ptr<edm::EventPrincipal>();
    }

  
  }; 
 
  DEFINE_FWK_INPUT_SOURCE(TestInputSource4ProductRegistry)


