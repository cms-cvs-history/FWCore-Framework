// -*- C++ -*-
//
// Package:     test
// Class  :     TestPRegisterSealModules
// 
// Implementation:
//     <Notes on implementation>
//
// Original Author:  Chris Jones
//         Created:  Sat Sep 24 10:58:40 CEST 2005
// $Id$
//

// system include files

// user include files
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/InputSourceMacros.h"
#include "FWCore/Framework/test/stubs/TestPRegisterModule1.h"
#include "FWCore/Framework/test/stubs/TestPRegisterModule2.h"
#include "FWCore/Framework/test/stubs/TestInputSource4ProductRegistry.h"

DEFINE_FWK_INPUT_SOURCE(TestInputSource4ProductRegistry)
DEFINE_ANOTHER_FWK_MODULE(TestPRegisterModule1)
DEFINE_ANOTHER_FWK_MODULE(TestPRegisterModule2)
