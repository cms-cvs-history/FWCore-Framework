#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/UnscheduledHandler.h"
#include "FWCore/Framework/interface/LuminosityBlockPrincipal.h"
#include "FWCore/Framework/interface/Group.h"
#include "FWCore/Utilities/interface/Algorithms.h"
#include "DataFormats/Common/interface/BasicHandle.h"
#include "DataFormats/Provenance/interface/BranchListIndex.h"
#include "DataFormats/Provenance/interface/BranchIDList.h"
#include "DataFormats/Provenance/interface/BranchIDListRegistry.h"
#include "DataFormats/Provenance/interface/ParameterSetIDListRegistry.h"
#include "DataFormats/Provenance/interface/Provenance.h"
#include "DataFormats/Provenance/interface/ProductRegistry.h"

#include <algorithm>

namespace edm {
  EventPrincipal::EventPrincipal(EventAuxiliary const& aux,
	boost::shared_ptr<ProductRegistry const> reg,
	ProcessConfiguration const& pc,
	boost::shared_ptr<History> history,
	boost::shared_ptr<BranchMapper> mapper,
	boost::shared_ptr<DelayedReader> rtrv) :
	  Base(reg, pc, history->processHistoryID(), mapper, rtrv),
	  aux_(aux),
	  luminosityBlockPrincipal_(),
	  unscheduledHandler_(),
	  moduleLabelsRunning_(),
	  history_(history),
	  branchToProductIDHelper_() {
	    if (reg->productProduced(InEvent)) {
	      addToProcessHistory();
	      // Add list of producted BranchIDs to BranchIDListRegistry;
	      history_->addEntry(BranchIDListRegistry::instance()->extra().currentIndex());
	    }
	    // Fill in helper map for Branch to ProductID mapping
	    for (BranchListIndexes::const_iterator
		 it = history->branchListIndexes().begin(),
		 itEnd = history->branchListIndexes().end();
		 it != itEnd; ++it) {
	      ProcessIndex pix = it - history->branchListIndexes().begin();
	      branchToProductIDHelper_.insert(std::make_pair(*it, pix));
	    }
	  }

  RunPrincipal const&
  EventPrincipal::runPrincipal() const {
    return luminosityBlockPrincipal().runPrincipal();
  }

  RunPrincipal &
  EventPrincipal::runPrincipal() {
    return luminosityBlockPrincipal().runPrincipal();
  }

  void
  EventPrincipal::addOnDemandGroup(ConstBranchDescription const& desc) {
    std::auto_ptr<Group> g(new Group(desc, branchIDToProductID(desc.branchID()), true));
    addOrReplaceGroup(g);
  }

  void
  EventPrincipal::addOrReplaceGroup(std::auto_ptr<Group> g) {
    Group const* group = getExistingGroup(*g);
    if (group != 0) {
      if(!group->onDemand()) {
        ConstBranchDescription const& bd = group->productDescription();
	throw edm::Exception(edm::errors::InsertFailure,"AlreadyPresent")
	  << "addGroup_: Problem found while adding product provenance, "
	  << "product already exists for ("
	  << bd.friendlyClassName() << ","
	  << bd.moduleLabel() << ","
	  << bd.productInstanceName() << ","
	  << bd.processName()
	  << ")\n";
      }
      replaceGroup(g);
    } else {
      addGroup_(g);
    }
  }

  void
  EventPrincipal::addGroup(ConstBranchDescription const& bd) {
    std::auto_ptr<Group> g(new Group(bd, branchIDToProductID(bd.branchID())));
    addOrReplaceGroup(g);
  }

  void
  EventPrincipal::addGroup(std::auto_ptr<EDProduct> prod,
	 ConstBranchDescription const& bd,
	 std::auto_ptr<ProductProvenance> productProvenance) {
    std::auto_ptr<Group> g(new Group(prod, bd, branchIDToProductID(bd.branchID()), productProvenance));
    addOrReplaceGroup(g);
  }

  void
  EventPrincipal::addGroup(ConstBranchDescription const& bd,
	 std::auto_ptr<ProductProvenance> productProvenance) {
    std::auto_ptr<Group> g(new Group(bd, branchIDToProductID(bd.branchID()), productProvenance));
    addOrReplaceGroup(g);
  }

  void
  EventPrincipal::addGroup(std::auto_ptr<EDProduct> prod,
	 ConstBranchDescription const& bd,
	 boost::shared_ptr<ProductProvenance> productProvenance) {
    std::auto_ptr<Group> g(new Group(prod, bd, branchIDToProductID(bd.branchID()), productProvenance));
    addOrReplaceGroup(g);
  }

  void
  EventPrincipal::addGroup(ConstBranchDescription const& bd,
	 boost::shared_ptr<ProductProvenance> productProvenance) {
    std::auto_ptr<Group> g(new Group(bd, branchIDToProductID(bd.branchID()), productProvenance));
    addOrReplaceGroup(g);
  }

  void 
  EventPrincipal::put(std::auto_ptr<EDProduct> edp,
		ConstBranchDescription const& bd,
		std::auto_ptr<ProductProvenance> productProvenance) {

    if (edp.get() == 0) {
      throw edm::Exception(edm::errors::InsertFailure,"Null Pointer")
	<< "put: Cannot put because auto_ptr to product is null."
	<< "\n";
    }
    ProductID pid = branchIDToProductID(bd.branchID());
    // Group assumes ownership
    if (!pid.isValid()) {
      throw edm::Exception(edm::errors::InsertFailure,"Null Product ID")
	<< "put: Cannot put product with null Product ID."
	<< "\n";
    }
    branchMapperPtr()->insert(*productProvenance);
    this->addGroup(edp, bd, productProvenance);
  }

  ParameterSetID const&
  EventPrincipal::productIDToParameterSetID(ProductID const& pid) const {
    if (!pid.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound,"InvalidID")
        << "get by product ID: invalid ProductID supplied\n";
    }
    BranchListIndex blix = history().branchListIndexes().at(pid.processIndex());
    ParameterSetIDList const& blist = ParameterSetIDListRegistry::instance()->data().at(blix);
    return blist.at(pid.productIndex()-1);
  }

  BranchID
  EventPrincipal::productIDToBranchID(ProductID const& pid) const {
    if (!pid.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound,"InvalidID")
        << "get by product ID: invalid ProductID supplied\n";
    }
    BranchID::value_type bid = 0;
    try {
      BranchListIndex blix = history().branchListIndexes().at(pid.processIndex());
      BranchIDList const& blist = BranchIDListRegistry::instance()->data().at(blix);
      bid = blist.at(pid.productIndex()-1);
    }
    catch (std::exception) {
      return BranchID();
    }
    return BranchID(bid);
  }

  ProductID
  EventPrincipal::branchIDToProductID(BranchID const& bid) const {
    if (!bid.isValid()) {
      throw edm::Exception(edm::errors::NotFound,"InvalidID")
        << "branchIDToProductID: invalid BranchID supplied\n";
    }
    BranchIDListHelper::BranchIDToIndexMap const& branchIDToIndexMap =
      BranchIDListRegistry::instance()->extra().branchIDToIndexMap();   
    BranchIDListHelper::BranchIDToIndexMap::const_iterator it = branchIDToIndexMap.find(bid);
    if (it == branchIDToIndexMap.end()) {
      throw edm::Exception(edm::errors::NotFound,"Bad BranchID")
        << "branchIDToProductID: productID cannot be determined from BranchID\n";
    }
    BranchListIndex blix = it->second.first;
    ProductIndex productIndex = it->second.second;
    std::map<BranchListIndex, ProcessIndex>:: const_iterator i = branchToProductIDHelper_.find(blix);
    if (i == branchToProductIDHelper_.end()) {
      throw edm::Exception(edm::errors::NotFound,"Bad branch ID")
        << "branchIDToProductID: productID cannot be determined from BranchID\n";
    }
    ProcessIndex processIndex = i->second;
    return ProductID(processIndex, productIndex + 1);
  }

  BasicHandle
  EventPrincipal::getByProductID(ProductID const& pid) const {
    BranchID bid = productIDToBranchID(pid);
    SharedConstGroupPtr const& g = getGroup(bid, true, true, true);
    if (g.get() == 0) {
      boost::shared_ptr<cms::Exception> whyFailed( new edm::Exception(edm::errors::ProductNotFound,"InvalidID") );
      *whyFailed
	<< "get by product ID: no product with given id: "<< pid << "\n";
      return BasicHandle(whyFailed);
    }

    // Check for case where we tried on demand production and
    // it failed to produce the object
    if (g->onDemand()) {
      boost::shared_ptr<cms::Exception> whyFailed( new edm::Exception(edm::errors::ProductNotFound,"InvalidID") );
      *whyFailed
	<< "get by product ID: no product with given id: " << pid << "\n"
        << "onDemand production failed to produce it.\n";
      return BasicHandle(whyFailed);
    }
    return BasicHandle(g->product(), g->provenance(), pid);
  }

  EDProduct const *
  EventPrincipal::getIt(ProductID const& pid) const {
    return getByProductID(pid).wrapper();
  }

  Provenance
  EventPrincipal::getProvenance(BranchID const& bid) const {
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

  Provenance
  EventPrincipal::getProvenance(ProductID const& pid) const {
    BranchID bid = productIDToBranchID(pid);
    return getProvenance(bid);
  }

  // This one is mostly for test printout purposes
  // No attempt to trigger on demand execution
  // Skips provenance when the EDProduct is not there
  void
  EventPrincipal::getAllProvenance(std::vector<Provenance const*> & provenances) const {
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
  EventPrincipal::resolveProvenance(Group const& g) const {
    if (!g.productProvenancePtr()) {
      // Now fix up the Group
      g.setProvenance(branchMapperPtr()->branchToEntryInfo(g.productDescription().branchID()));
    }
  }

  void
  EventPrincipal::setUnscheduledHandler(boost::shared_ptr<UnscheduledHandler> iHandler) {
    unscheduledHandler_ = iHandler;
  }

  EventSelectionIDVector const&
  EventPrincipal::eventSelectionIDs() const
  {
    return history_->eventSelectionIDs();
  }

  History const&
  EventPrincipal::history() const
  {
    return *history_;
  }

  bool
  EventPrincipal::unscheduledFill(std::string const& moduleLabel) const {

    // If it is a module already currently running in unscheduled
    // mode, then there is a circular dependency related to which
    // EDProducts modules require and produce.  There is no safe way
    // to recover from this.  Here we check for this problem and throw
    // an exception.
    std::vector<std::string>::const_iterator i =
      find_in_all(moduleLabelsRunning_, moduleLabel);

    if (i != moduleLabelsRunning_.end()) {
      throw edm::Exception(errors::LogicError)
        << "Hit circular dependency while trying to run an unscheduled module.\n"
        << "Current implementation of unscheduled execution cannot always determine\n"
        << "the proper order for module execution.  It is also possible the modules\n"
        << "have a built in circular dependence that will not work with any order.\n"
        << "In the first case, scheduling some or all required modules in paths will help.\n"
        << "In the second case, the modules themselves will have to be fixed.\n";
    }

    moduleLabelsRunning_.push_back(moduleLabel);

    if (unscheduledHandler_) {
      unscheduledHandler_->tryToFill(moduleLabel, *const_cast<EventPrincipal *>(this));
    }
    moduleLabelsRunning_.pop_back();
    return true;
  }

}
