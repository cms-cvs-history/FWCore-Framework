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
// $Id: TestPRegisterSealModules.cc,v 1.4 2007/04/09 23:13:20 chrjones Exp $
//

// system include files

// user include files
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/test/stubs/TestPRegisterModule1.h"
#include "FWCore/Framework/test/stubs/TestPRegisterModule2.h"

DEFINE_FWK_MODULE(TestPRegisterModule1);
DEFINE_FWK_MODULE(TestPRegisterModule2);
