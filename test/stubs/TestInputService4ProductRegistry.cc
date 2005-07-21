/**
   \file
   test InputService for ProductRegistry

   \author Stefano ARGIRO
   \version $Id$
   \date 21 Jul 2005
*/

static const char CVSId[] = "$Id$";

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
	desc.preg_->getProductList();
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
    virtual std::auto_ptr<edm::EventPrincipal> read(){}

  
  }; 
 
  DEFINE_FWK_INPUT_SERVICE(TestInputService4ProductRegistry)


