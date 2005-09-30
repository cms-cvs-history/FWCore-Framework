#include <cassert>
#include <exception>
#include <iostream>
#include <memory>

#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ModuleDescription.h"
#include "FWCore/Framework/interface/Handle.h"

#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Utilities/interface/CodedException.h"
#include "FWCore/Utilities/interface/Exception.h"

// This bit of hackery is here so that we can call 'commit_' on the
// Event. Don't do this in real code, or disasters may ensue!

namespace edm
{
  class ProducerWorker
  {
  public:
    static void commit(Event* p) { p->commit_(); }
  };
}

void work()
{

  edm::ModuleDescription      md;
  md.pid = edm::PS_ID("deadbeef");
  md.moduleName_    = "NoClass";
  md.moduleLabel_   = "NoLabel";
  md.versionNumber_ = 1;
  md.processName_   = "TEST";
  md.pass           = 2112;      

  edm::EventPrincipal         ep;
  assert ( ep.numEDProducts() == 0 );

  
  {
    std::auto_ptr<edm::Event>   pe(new edm::Event(ep, md));
    std::auto_ptr<int>          prod(new int(10));
    pe->put(prod);
    edm::ProducerWorker::commit(pe.get());
    assert ( ep.numEDProducts() == 1 );
  }

  {
    std::auto_ptr<edm::Event>   pe(new edm::Event(ep, md));
    edm::Handle<int>            h;
    assert( !h.isValid() );
    pe->getByType(h);
    assert( h.isValid() );
  }

}

int main()
{
  int status = -1;
  try 
    { 
      work(); 
      status = 0;
    }
  catch ( edm::Exception& x )
    {
      std::cerr << x.what() << '\n';
    }
  catch ( std::exception& x )
    {
      std::cerr << x.what() << '\n';
    }
  catch ( ... )
    {
      std::cerr << "Caught an unidentifiable exception!\n";
    }
  return status;
}
