#ifndef FWCore_Framework_NoDelayedReader_h
#define FWCore_Framework_NoDelayedReader_h

/*----------------------------------------------------------------------
$Id: NoDelayedReader.h,v 1.10 2008/02/06 06:24:44 wmtan Exp $
----------------------------------------------------------------------*/

#include <memory>
#include "FWCore/Framework/interface/DelayedReader.h"

namespace edm {
  class NoDelayedReader : public DelayedReader {
  public:
    virtual ~NoDelayedReader();
  private:
    virtual std::auto_ptr<EDProduct> getProduct_(BranchKey const& k, EDProductGetter const* ep) const;
    virtual std::auto_ptr<EntryDescription> getProvenance_(BranchDescription const& desc) const;
  };
}
#endif
