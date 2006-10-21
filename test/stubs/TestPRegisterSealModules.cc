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
// $Id: TestPRegisterSealModules.cc,v 1.2 2005/12/28 00:51:29 wmtan Exp $
//

// system include files

// user include files
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/test/stubs/TestPRegisterModule1.h"
#include "FWCore/Framework/test/stubs/TestPRegisterModule2.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(TestPRegisterModule1);
DEFINE_ANOTHER_FWK_MODULE(TestPRegisterModule2);
