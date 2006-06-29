#include <cassert>
#include "FWCore/Framework/interface/ModuleDescriptionRegistry.h"
#include "FWCore/Framework/interface/ProcessNameListRegistry.h"

int main()
{
  edm::ModuleDescriptionRegistry* mreg = 
    edm::ModuleDescriptionRegistry::instance();

  edm::ProcessNameListRegistry* pnlreg = 
    edm::ProcessNameListRegistry::instance();

  assert( mreg );
  assert( pnlreg );
}
