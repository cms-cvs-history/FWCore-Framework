#include "FWCore/Framework/interface/LuminosityBlockPrincipal.h"
#include "FWCore/Framework/interface/RunPrincipal.h"
#include "FWCore/Framework/src/Group.h"

namespace edm {

  LuminosityBlockPrincipal::LuminosityBlockPrincipal(LuminosityBlockAuxiliary const& aux,
	boost::shared_ptr<ProductRegistry const> reg,
	boost::shared_ptr<RunPrincipal> rp,
	ProcessConfiguration const& pc,
	ProcessHistoryID const& hist,
	boost::shared_ptr<BranchMapper> mapper,
	boost::shared_ptr<DelayedReader> rtrv) :
	  Base(reg, pc, hist, rtrv),
	  runPrincipal_(rp),
	  aux_(aux),
          branchMapperPtr_(mapper) {}

  void
  LuminosityBlockPrincipal::addOrReplaceGroup(std::auto_ptr<Group> g) {

    Group* group = getExistingGroup(*g);
    if (group != 0) {

      if (!group->productUnavailable()) {
        assert(group->product() != 0);
      }
      if (!g->productUnavailable()) {
        assert(g->product() != 0);
      }

      group->mergeGroup(g.get());
    } else {
      addGroup_(g);
    }
  }

  void
  LuminosityBlockPrincipal::addGroup(ConstBranchDescription const& bd) {
    std::auto_ptr<Group> g(new Group(bd));
    addOrReplaceGroup(g);
  }

  void
  LuminosityBlockPrincipal::addGroup(std::auto_ptr<EDProduct> prod, std::auto_ptr<Provenance> prov) {
    std::auto_ptr<Group> g(new Group(prod, prov));
    addOrReplaceGroup(g);
  }

  void
  LuminosityBlockPrincipal::addGroup(std::auto_ptr<Provenance> prov) {
    std::auto_ptr<Group> g(new Group(prov));
    addOrReplaceGroup(g);
  }


  void 
  LuminosityBlockPrincipal::put(std::auto_ptr<EDProduct> edp,
		 std::auto_ptr<Provenance> prov) {

    if (edp.get() == 0) {
      throw edm::Exception(edm::errors::InsertFailure,"Null Pointer")
	<< "put: Cannot put because auto_ptr to product is null."
	<< "\n";
    }
    branchMapperPtr_->insert(prov->branchEntryInfo());
    // Group assumes ownership
    this->addGroup(edp, prov);
    this->addToProcessHistory();
  }

  Provenance const&
  LuminosityBlockPrincipal::getProvenance(BranchID const& bid) const {
    SharedConstGroupPtr const& g = getGroup(bid, false, true);
    if (g.get() == 0) {
      throw edm::Exception(edm::errors::ProductNotFound,"InvalidID")
	<< "getProvenance: no product with given branch id: "<< bid << "\n";
    }

    if (g->onDemand()) {
      unscheduledFill(g->provenance()->moduleLabel());
    }
    // We already tried to produce the unscheduled products above
    // If they still are not there, then throw
    if (g->onDemand()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< "getProvenance: no product with given BranchID: "<< bid <<"\n";
    }

    return *g->provenance();
  }

  // This one is mostly for test printout purposes
  // No attempt to trigger on demand execution
  // Skips provenance when the EDProduct is not there
  void
  LuminosityBlockPrincipal::getAllProvenance(std::vector<Provenance const*> & provenances) const {
    provenances.clear();
    for (Principal::const_iterator i = begin(), iEnd = end(); i != iEnd; ++i) {
      if (i->second->provenanceAvailable() && i->second->provenance()->isPresent() && i->second->provenance()->product().present())
	 provenances.push_back(i->second->provenance());
    }
  }

  void
  LuminosityBlockPrincipal::resolveProvenance(Group const& g) const {
    if (!g.provenance()->branchEntryInfoPtr()) {
      // Now fix up the Group
      g.setProvenance(branchMapperPtr_->branchToEntryInfo(g.productDescription().branchID()));
    }
  }

  void
  LuminosityBlockPrincipal::mergeLuminosityBlock(boost::shared_ptr<LuminosityBlockPrincipal> lbp) {

    aux_.mergeAuxiliary(lbp->aux());

    for (Principal::const_iterator i = lbp->begin(), iEnd = lbp->end(); i != iEnd; ++i) {
 
      std::auto_ptr<Group> g(new Group());
      g->swap(*i->second);

      addOrReplaceGroup(g);
    }
  }
}
