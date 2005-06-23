
#include <iostream>

#include "FWCore/CoreFramework/interface/EDProducer.h"
#include "FWCore/CoreFramework/interface/Event.h"
#include "FWCore/CoreFramework/interface/Handle.h"
#include "FWCore/CoreFramework/interface/MakerMacros.h"
#include "FWCore/CoreFramework/src/Factory.h"

#include "PluginManager/PluginManager.h"
#include "FWCore/CoreFramework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/parse.h"
#include "FWCore/ParameterSet/interface/Makers.h"
#include "SealKernel/Exception.h"
#include "SealBase/SharedLibrary.h"

using namespace std;
using namespace edm;


// ----------------------------------------------


int main()
{
  string param1 = 
    "string module_type = \"TestMod\"\n "
    " string module_label = \"t1\"";

  string param2 = 
    "string module_type = \"TestMod\" "
    "string module_label = \"t2\"";
    
  try {

    //seal::SharedLibrary* lib = seal::SharedLibrary::load("libTestMod.so");
    seal::PluginManager::get ()->initialise ();
    Factory* f = Factory::get();

    //Factory::Iterator ib(f->begin()),ie(f->end());
    //for(;ib!=ie;++ib)
    //  {
    //cout << (*ib)->name() << endl;
    // }

    boost::shared_ptr<ParameterSet> p1 = makePSet(*edm::pset::parse(param1.c_str()));;
    boost::shared_ptr<ParameterSet> p2 = makePSet(*edm::pset::parse(param2.c_str()));;

    cerr << p1->getParameter<std::string>("module_type");
    auto_ptr<Worker> w1 = f->makeWorker(*p1,"PROD",0,0);
    auto_ptr<Worker> w2 = f->makeWorker(*p2,"PROD",0,0);
  }
  catch(std::exception& e)
    {
      std::cerr << "std::Exception: " << e.what() << std::endl;
    }
  catch(seal::Exception& e)
    {
      std::cerr << "seal::Exception: " << e.what() << std::endl;
    }
  catch(seal::SharedLibraryError& e)
    {
      std::cerr << "sharedliberror " << e.explainSelf() << std::endl;
    }
  catch(...)
    {
      std::cerr << "weird exception" << endl;
      throw;
    }

  return 0;
}
