/**
   \file
   test InputService for ProductRegistry

   \author Stefano ARGIRO
   \version $Id: TestInputService4ProductRegistry.cc,v 1.1 2005/07/21 21:07:14 argiro Exp $
   \date 21 Jul 2005
*/

static const char CVSId[] = "$Id: TestInputService4ProductRegistry.cc,v 1.1 2005/07/21 21:07:14 argiro Exp $";

#include <FWCore/Framework/interface/EventPrincipal.h>
#include <FWCore/Framework/interface/InputService.h>
#include <FWCore/Framework/interface/InputServiceDescription.h>
#include <FWCore/Framework/interface/InputServiceMacros.h>
#include <FWCore/Framework/interface/ProductRegistry.h>
#include <FWCore/Framework/interface/ProductDescription.h>
#include <FWCore/Framework/src/TypeID.h>
#include <FWCore/Framework/src/ToyProducts.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>

namespace edm {

  class Retriever;
  class ParameterSet;
  class InputServiceDescription;
} 

  class TestInputService4ProductRegistry : public edm::InputService {

  public:
    TestInputService4ProductRegistry(const edm::ParameterSet& pset, 
      const edm::InputServiceDescription& desc): InputService(desc.process_name){

      const  edm::ProductRegistry::ProductList& plist=
	desc.preg_->productList();
      edm::ProductRegistry::ProductList::const_iterator pd = plist.begin();

      
      edmtest::StringProduct stringprod;
      edm::TypeID stringID(stringprod);
      CPPUNIT_ASSERT(stringID.friendlyClassName() == 
		     pd->friendly_product_type_name);
      CPPUNIT_ASSERT(pd->module.module_label=="m1");

      ++pd;

      edmtest::DoubleProduct dprod;
      edm::TypeID dID(dprod);
      CPPUNIT_ASSERT(dID.friendlyClassName() == 
		     pd->friendly_product_type_name);
      CPPUNIT_ASSERT(pd->module.module_label=="m2");

    }



  private:
    virtual std::auto_ptr<edm::EventPrincipal> read() {
      return std::auto_ptr<edm::EventPrincipal>();
    }

  
  }; 
 
  DEFINE_FWK_INPUT_SERVICE(TestInputService4ProductRegistry)


