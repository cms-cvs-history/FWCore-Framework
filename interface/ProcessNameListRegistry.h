#ifndef FWCoreFrameworkProcessNameListRegistry_h
#define FWCoreFrameworkProcessNameListRegistry_h

#include "FWCore/Utilities/interface/ThreadSafeRegistry.h"
#include "DataFormats/Common/interface/ProcessNameList.h"
#include "DataFormats/Common/interface/ProcessNameListID.h"

namespace edm
{
  typedef edm::detail::ThreadSafeRegistry<edm::ProcessNameListID,edm::ProcessNameList> ProcessNameListRegistry;
  typedef ProcessNameListRegistry::collection_type ProcessNameListMap;
}

#endif
