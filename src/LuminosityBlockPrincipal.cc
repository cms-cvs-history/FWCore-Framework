#include "FWCore/Framework/interface/LuminosityBlockPrincipal.h"
#include "FWCore/Framework/interface/RunPrincipal.h"
#include "FWCore/Framework/interface/Group.h"
#include "DataFormats/Provenance/interface/ProductRegistry.h"
#include "DataFormats/Provenance/interface/ProductID.h"

namespace edm {

  LuminosityBlockPrincipal::LuminosityBlockPrincipal(LuminosityBlockAuxiliary const& aux,
	boost::shared_ptr<ProductRegistry const> reg,
	ProcessConfiguration const& pc,
	boost::shared_ptr<BranchMapper> mapper, 
	boost::shared_ptr<DelayedReader> rtrv) :
	  Base(reg, pc, aux.processHistoryID_, mapper, rtrv),
	  runPrincipal_(),
    aux_(aux) {
      if (reg->productProduced(InLumi)) {
        addToProcessHistory();
      }
  }

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
    std::auto_ptr<Group> g(new Group(bd, ProductID()));
    addOrReplaceGroup(g);
  }

  void
  LuminosityBlockPrincipal::addGroup(std::auto_ptr<EDProduct> prod,
	ConstBranchDescription const& bd,
	std::auto_ptr<ProductProvenance> productProvenance) {
    std::auto_ptr<Group> g(new Group(prod, bd, ProductID(), productProvenance));
    addOrReplaceGroup(g);
  }

  void
  LuminosityBlockPrincipal::addGroup(ConstBranchDescription const& bd,
	std::auto_ptr<ProductProvenance> productProvenance) {
    std::auto_ptr<Group> g(new Group(bd, ProductID(), productProvenance));
    addOrReplaceGroup(g);
  }

  void 
  LuminosityBlockPrincipal::put(std::auto_ptr<EDProduct> edp,
		ConstBranchDescription const& bd,
		std::auto_ptr<ProductProvenance> productProvenance) {

    if (edp.get() == 0) {
      throw edm::Exception(edm::errors::InsertFailure,"Null Pointer")
	<< "put: Cannot put because auto_ptr to product is null."
	<< "\n";
    }
    branchMapperPtr()->insert(*productProvenance);
    // Group assumes ownership
    this->addGroup(edp, bd, productProvenance);
  }

  Provenance
  LuminosityBlockPrincipal::getProvenance(BranchID const& bid) const {
    SharedConstGroupPtr const& g = getGroup(bid, false, true, true);
    if (g.get() == 0) {
      throw edm::Exception(edm::errors::ProductNotFound,"InvalidID")
	<< "getProvenance: no product with given branch id: "<< bid << "\n";
    }

    if (g->onDemand()) {
      unscheduledFill(g->productDescription().moduleLabel());
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
    for (Base::const_iterator i = begin(), iEnd = end(); i != iEnd; ++i) {
      if (i->second->provenanceAvailable()) {
        resolveProvenance(*i->second);
        if (i->second->provenance()->productProvenanceSharedPtr() &&
            i->second->provenance()->isPresent() &&
            i->second->provenance()->product().present())
           provenances.push_back(i->second->provenance());
        }
    }
  }

  void
  LuminosityBlockPrincipal::mergeLuminosityBlock(boost::shared_ptr<LuminosityBlockPrincipal> lbp) {

    aux_.mergeAuxiliary(lbp->aux());

    for (Base::const_iterator i = lbp->begin(), iEnd = lbp->end(); i != iEnd; ++i) {
 
      std::auto_ptr<Group> g(new Group());
      g->swap(*i->second);

      addOrReplaceGroup(g);
    }
  }
}

