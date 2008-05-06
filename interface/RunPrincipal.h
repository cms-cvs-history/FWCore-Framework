#ifndef FWCore_Framework_RunPrincipal_h
#define FWCore_Framework_RunPrincipal_h

/*----------------------------------------------------------------------
  
RunPrincipal: This is the class responsible for management of
per run EDProducts. It is not seen by reconstruction code;
such code sees the Run class, which is a proxy for RunPrincipal.

The major internal component of the RunPrincipal
is the DataBlock.

$Id: RunPrincipal.h,v 1.23.2.1 2008/05/04 03:18:25 wmtan Exp $

----------------------------------------------------------------------*/

#include "boost/shared_ptr.hpp"

#include "DataFormats/Provenance/interface/BranchMapper.h"
#include "DataFormats/Provenance/interface/RunAuxiliary.h"
#include "FWCore/Framework/interface/Principal.h"

namespace edm {
  class UnscheduledHandler;
  class RunPrincipal : public Principal {
  typedef Principal Base;
  public:
    RunPrincipal(RunAuxiliary const& aux,
	boost::shared_ptr<ProductRegistry const> reg,
	ProcessConfiguration const& pc,
	ProcessHistoryID const& hist = ProcessHistoryID(),
	boost::shared_ptr<BranchMapper> mapper = boost::shared_ptr<BranchMapper>(new BranchMapper),
	boost::shared_ptr<DelayedReader> rtrv = boost::shared_ptr<DelayedReader>(new NoDelayedReader)) :
	  Base(reg, pc, hist, rtrv),
	  aux_(aux),
	  branchMapperPtr_(mapper) {}
    ~RunPrincipal() {}

    RunAuxiliary const& aux() const {
      aux_.processHistoryID_ = processHistoryID();
      return aux_;
    }

    RunNumber_t run() const {
      return aux().run();
    }

    RunID const& id() const {
      return aux().id();
    }

    Timestamp const& beginTime() const {
      return aux().beginTime();
    }

    Timestamp const& endTime() const {
      return aux().endTime();
    }

    void setEndTime(Timestamp const& time) {
      aux_.setEndTime(time);
    }

    void setUnscheduledHandler(boost::shared_ptr<UnscheduledHandler>) {}

    void mergeRun(boost::shared_ptr<RunPrincipal> rp);

    Provenance const&
    getProvenance(BranchID const& bid) const;

    void
    getAllProvenance(std::vector<Provenance const *> & provenances) const;

    void put(std::auto_ptr<EDProduct> edp,
	     std::auto_ptr<Provenance> prov);

    void addGroup(ConstBranchDescription const& bd);

    void addGroup(std::auto_ptr<EDProduct> prod, std::auto_ptr<Provenance> prov);

    void addGroup(std::auto_ptr<Provenance> prov);

  private:

    virtual void addOrReplaceGroup(std::auto_ptr<Group> g);

    virtual void resolveProvenance(Group const& g) const;

    virtual bool unscheduledFill(std::string const&) const {return false;}

    RunAuxiliary aux_;

    boost::shared_ptr<BranchMapper> branchMapperPtr_;
  };
}
#endif

