/**
   \file
   test InputSource for ProductRegistry

   \author Stefano ARGIRO
   \version $Id: TestInputSource4ProductRegistry.cc,v 1.5 2005/07/30 23:41:36 wmtan Exp $
   \date 21 Jul 2005
*/

static const char CVSId[] = "$Id: TestInputSource4ProductRegistry.cc,v 1.5 2005/07/30 23:41:36 wmtan Exp $";

#include <FWCore/Framework/interface/EventPrincipal.h>
#include <FWCore/Framework/interface/InputSourceDescription.h>
#include <FWCore/Framework/interface/ProductRegistry.h>
#include <FWCore/Framework/interface/ProductDescription.h>
#include <FWCore/Framework/src/TypeID.h>
#include <FWCore/Framework/src/ToyProducts.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>

#include "FWCore/Framework/test/stubs/TestInputSource4ProductRegistry.h"

TestInputSource4ProductRegistry::TestInputSource4ProductRegistry(
                           const edm::ParameterSet& pset, 
                           const edm::InputSourceDescription& desc): 
InputSource(desc) 
{   
}

std::auto_ptr<edm::EventPrincipal> TestInputSource4ProductRegistry::read() {
   return std::auto_ptr<edm::EventPrincipal>();
}


