#ifndef Framework_RunPrincipal_h
#define Framework_RunPrincipal_h

/*----------------------------------------------------------------------
  
RunPrincipal: This is the class responsible for management of
per run EDProducts. It is not seen by reconstruction code;
such code sees the Run class, which is a proxy for RunPrincipal.

The major internal component of the RunPrincipal
is the DataBlock.

$Id: RunPrincipal.h,v 1.7 2007/03/04 06:00:22 wmtan Exp $

----------------------------------------------------------------------*/

#include "DataFormats/Provenance/interface/RunAuxiliary.h"
#include "FWCore/Framework/interface/Principal.h"

namespace edm {
  class RunPrincipal : private Principal {
  typedef Principal Base;
  public:
    RunPrincipal(RunNumber_t const& id,
	ProductRegistry const& reg,
	ProcessConfiguration const& pc,
	ProcessHistoryID const& hist = ProcessHistoryID(),
	boost::shared_ptr<DelayedReader> rtrv = boost::shared_ptr<DelayedReader>(new NoDelayedReader)) :
	  Base(reg, pc, hist, rtrv), aux_(id) {}
    ~RunPrincipal() {}

    RunAuxiliary const& aux() const {
      return aux_;
    }

    RunNumber_t run() const {
      return aux().run();
    }

    RunID const& id() const {
      return aux().id();
    }

    using Base::addGroup;
    using Base::addToProcessHistory;
    using Base::begin;
    using Base::end;
    using Base::getAllProvenance;
    using Base::getByLabel;
    using Base::get;
    using Base::getBySelector;
    using Base::getByType;
    using Base::getGroup;
    using Base::getIt;
    using Base::getMany;
    using Base::getManyByType;
    using Base::getProvenance;
    using Base::groupGetter;
    using Base::numEDProducts;
    using Base::processHistory;
    using Base::processHistoryID;
    using Base::prodGetter;
    using Base::productRegistry;
    using Base::put;
    using Base::size;
    using Base::store;

  private:
    virtual bool unscheduledFill(Group const&) const {return false;}
    virtual bool fillAndMatchSelector(Provenance &, SelectorBase const&) const {return false;}

    RunAuxiliary aux_;
  };
}
#endif

