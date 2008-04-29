#ifndef FWCore_Framework_DelayedReader_h
#define FWCore_Framework_DelayedReader_h

/*----------------------------------------------------------------------
  
DelayedReader: The abstract interface through which the EventPrincipal
uses input sources to retrieve EDProducts from external storage.

$Id: DelayedReader.h,v 1.9 2008/02/06 06:24:44 wmtan Exp $

----------------------------------------------------------------------*/

#include <memory>
#include "boost/shared_ptr.hpp"
#include "DataFormats/Provenance/interface/Provenance.h"
#include "DataFormats/Common/interface/EDProduct.h"

namespace edm {
  class BranchDescription;
  class EDProductGetter;
  class DelayedReader {
  public:
    virtual ~DelayedReader();
    std::auto_ptr<EDProduct> getProduct(BranchKey const& k, EDProductGetter const* ep) {
      return getProduct_(k, ep);
    }
    std::auto_ptr<Provenance> getProvenance(BranchDescription const& desc) {
      return getProvenance_(desc);
    }
    void mergeReaders(boost::shared_ptr<DelayedReader> other) {mergeReaders_(other);}
  private:
    virtual std::auto_ptr<EDProduct> getProduct_(BranchKey const& k, EDProductGetter const* ep) const = 0;
    virtual std::auto_ptr<Provenance> getProvenance_(BranchDescription const& desc) const = 0;
    virtual void mergeReaders_(boost::shared_ptr<DelayedReader>) {}
  };
}

#endif
