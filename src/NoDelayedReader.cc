/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

#include "FWCore/Framework/interface/NoDelayedReader.h"
#include "DataFormats/Provenance/interface/BranchKey.h"
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/RunPrincipal.h"
#include "FWCore/Framework/interface/LuminosityBlockPrincipal.h"
#include "FWCore/Utilities/interface/Exception.h"

namespace edm {
  NoDelayedReader::~NoDelayedReader() {}

  std::auto_ptr<EDProduct>
  NoDelayedReader::getProduct_(BranchKey const& k, EDProductGetter const* ep) const {
    EventPrincipal const* epr = dynamic_cast<EventPrincipal const*>(ep);
    if (epr) {
      throw cms::Exception("LogicError","NoDelayedReader")
        << "getProduct() called for branchkey: " << k << " EventID: " << epr->id() << "\n";
    }
    RunPrincipal const* rpr = dynamic_cast<RunPrincipal const*>(ep);
    if (rpr) {
      throw cms::Exception("LogicError","NoDelayedReader")
        << "getProduct() called for branchkey: " << k << " RunID: " << epr->id() << "\n";
    }
    LuminosityBlockPrincipal const* lpr = dynamic_cast<LuminosityBlockPrincipal const*>(ep);
    if (lpr) {
      throw cms::Exception("LogicError","NoDelayedReader")
        << "getProduct() called for branchkey: " << k << " LuminosityBlockNumber_t: " << lpr->id() << "\n";
    }
    throw cms::Exception("LogicError","NoDelayedReader")
      << "getProduct() called for branchkey: " << k << "\n";
  }

  std::auto_ptr<BranchEntryInfo>
  NoDelayedReader::getProvenance_(BranchDescription const& desc) const {
    throw cms::Exception("LogicError","NoDelayedReader")
      << "getProvenance() called for branchkey: " << BranchKey(desc) << "\n";
  }
}
