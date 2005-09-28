#ifndef test_TestInputSource4ProductRegistry_h
#define test_TestInputSource4ProductRegistry_h
// -*- C++ -*-
//
// Package:     test
// Class  :     TestInputSource4ProductRegistry
// 
/**\class TestInputSource4ProductRegistry TestInputSource4ProductRegistry.h Framework/test/interface/TestInputSource4ProductRegistry.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Original Author:  Chris Jones
//         Created:  Sat Sep 24 10:58:04 CEST 2005
// $Id$
//

// system include files
#include <memory>

// user include files
#include <FWCore/Framework/interface/InputSource.h>

// forward declarations

namespace edm {
   
   class Retriever;
   class ParameterSet;
   class InputSourceDescription;
} 

class TestInputSource4ProductRegistry : public edm::InputSource {
   
public:
   TestInputSource4ProductRegistry(const edm::ParameterSet& pset, 
                                    const edm::InputSourceDescription& desc);
   
private:
   virtual std::auto_ptr<edm::EventPrincipal> read();
      
}; 

#endif
