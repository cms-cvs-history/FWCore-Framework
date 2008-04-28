/**----------------------------------------------------------------------
  $Id: Principal.cc,v 1.29.2.1 2008/04/25 17:20:22 wmtan Exp $
  ----------------------------------------------------------------------*/

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "FWCore/Framework/interface/Principal.h"
#include "DataFormats/Provenance/interface/BranchMapper.h"
#include "DataFormats/Provenance/interface/BranchMapperRegistry.h"
#include "DataFormats/Provenance/interface/ProcessHistory.h"
#include "DataFormats/Provenance/interface/ProcessHistoryRegistry.h"
#include "DataFormats/Provenance/interface/ProductRegistry.h"
#include "DataFormats/Common/interface/BasicHandle.h"
#include "FWCore/Utilities/interface/TypeID.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Framework/src/Group.h"
#include "FWCore/Framework/interface/Selector.h"
//using boost::lambda::_1;

namespace edm {

  Principal::Principal(boost::shared_ptr<ProductRegistry const> reg,
		       ProcessConfiguration const& pc,
		       ProcessHistoryID const& hist,
		       boost::shared_ptr<DelayedReader> rtrv) :
    EDProductGetter(),
    processHistoryID_(hist),
    processHistoryPtr_(boost::shared_ptr<ProcessHistory>(new ProcessHistory)),
    branchMapperID_(),
    branchMapperPtr_(new BranchMapper),
    processConfiguration_(pc),
    processHistoryModified_(false),
    groups_(),
    productStatuses_(reg->maxID(), productstatus::invalid()),
    preg_(reg),
    store_(rtrv)
  {
    if (processHistoryID_.isValid()) {
      ProcessHistoryRegistry& history(*ProcessHistoryRegistry::instance());
      assert(history.notEmpty());
      bool found = history.getMapped(processHistoryID_, *processHistoryPtr_);
      assert(found);
    }
  }

  Principal::~Principal() {
  }

  Group*
  Principal::getExistingGroup(Group const& group) {
    GroupCollection::const_iterator it = groups_.find(group.provenance().branchID());
    if (it == groups_.end()) return 0;
    return it->second.get();
  }

  void 
  Principal::addGroup_(std::auto_ptr<Group> group) {
    BranchDescription const& bd = group->productDescription();
    assert (!bd.className().empty());
    assert (!bd.friendlyClassName().empty());
    assert (!bd.moduleLabel().empty());
    assert (!bd.processName().empty());
    assert (group->provenance().productID().isValid());
    SharedGroupPtr g(group);
    if (g->entryDescription() == 0) g->provenance().setStore(store_);
    groups_.insert(std::make_pair(bd.branchID(), g));
  }

  void 
  Principal::replaceGroup(std::auto_ptr<Group> group) {
    BranchDescription const& bd = group->productDescription();
    assert (!bd.className().empty());
    assert (!bd.friendlyClassName().empty());
    assert (!bd.moduleLabel().empty());
    assert (!bd.processName().empty());
    assert (group->provenance().productID().isValid());
    SharedGroupPtr g(group);
    if (g->entryDescription() == 0) g->provenance().setStore(store_);
    groups_[bd.branchID()]->replace(*g);
  }

  void
  Principal::addGroup(ConstBranchDescription const& bd, ProductStatus status) {
    std::auto_ptr<Group> g(new Group(bd, status));
    addOrReplaceGroup(g);
  }

  void
  Principal::addGroup(std::auto_ptr<EDProduct> prod, std::auto_ptr<Provenance> prov) {
    std::auto_ptr<Group> g(new Group(prod, prov));
    addOrReplaceGroup(g);
  }

  void
  Principal::addToProcessHistory() const {
    if (processHistoryModified_) return;
    ProcessHistory& ph = *processHistoryPtr_;
    std::string const& processName = processConfiguration_.processName();
    for (ProcessHistory::const_iterator it = ph.begin(), itEnd = ph.end(); it != itEnd; ++it) {
      if (processName == it->processName()) {
	throw edm::Exception(errors::Configuration, "Duplicate Process")
	  << "The process name " << processName << " was previously used on these products.\n"
	  << "Please modify the configuration file to use a distinct process name.\n";
      }
    }
    ph.push_back(processConfiguration_);
    //OPTIMIZATION NOTE:  As of 0_9_0_pre3
    // For very simple Sources (e.g. EmptySource) this routine takes up nearly 50% of the time per event.
    // 96% of the time for this routine is being spent in computing the
    // ProcessHistory id which happens because we are reconstructing the ProcessHistory for each event.
    // (The process ID is first computed in the call to 'insertMapped(..)' below.)
    // It would probably be better to move the ProcessHistory construction out to somewhere
    // which persists for longer than one Event
    ProcessHistoryRegistry::instance()->insertMapped(ph);
    processHistoryID_ = ph.id();
    processHistoryModified_ = true;
  }

  ProcessHistory const&
  Principal::processHistory() const {
    return *processHistoryPtr_;
  }

  void 
  Principal::put(std::auto_ptr<EDProduct> edp,
		 std::auto_ptr<Provenance> prov) {

    if (!prov->productID().isValid()) {
      throw edm::Exception(edm::errors::InsertFailure,"Null Product ID")
	<< "put: Cannot put product with null Product ID."
	<< "\n";
    }
    if (edp.get() == 0) {
      throw edm::Exception(edm::errors::InsertFailure,"Null Pointer")
	<< "put: Cannot put because auto_ptr to product is null."
	<< "\n";
    }
    // Group assumes ownership
    this->addGroup(edp, prov);
    this->addToProcessHistory();
  }

  Principal::SharedConstGroupPtr const
  Principal::getGroup(BranchID const& bid, bool resolveProd, bool resolveProv, bool fillOnDemand) const {
    GroupCollection::const_iterator it = groups_.find(bid);
    if (it == groups_.end()) {
      return SharedConstGroupPtr();
    }
    SharedConstGroupPtr const& g = it->second;
    if (resolveProd && !g->productUnavailable()) {
      this->resolveProduct(*g, fillOnDemand);
    }
    if (resolveProv && g->provenanceAvailable()) {
      this->resolveProvenance(*g);
    }
    return g;
  }

  BasicHandle
  Principal::get(ProductID const& oid) const {
    BranchID bid = branchMapperPtr_->productToBranch(oid);
    SharedConstGroupPtr const& g = getGroup(bid, true, false, true);
    if (g.get() == 0) {
      if (!oid.isValid()) {
        throw edm::Exception(edm::errors::ProductNotFound,"InvalidID")
	  << "get by product ID: invalid ProductID supplied\n";
      }
      boost::shared_ptr<cms::Exception> whyFailed( new edm::Exception(edm::errors::ProductNotFound,"InvalidID") );
      *whyFailed
	<< "get by product ID: no product with given id: "<< oid << "\n";
      return BasicHandle(whyFailed);
    }

    // Check for case where we tried on demand production and
    // it failed to produce the object
    if (g->onDemand()) {
      boost::shared_ptr<cms::Exception> whyFailed( new edm::Exception(edm::errors::ProductNotFound,"InvalidID") );
      *whyFailed
	<< "get by product ID: no product with given id: " << oid << "\n"
        << "onDemand production failed to produce it.\n";
      return BasicHandle(whyFailed);
    }
    return BasicHandle(g->product(), &g->provenance());
  }

  BasicHandle
  Principal::getForOutput(BranchID const& bid, bool getProd, bool getProv) const {
    ProductID oid = branchMapperPtr_->branchToProduct(bid);

    ProductStatus & status = const_cast<ProductStatus &>(productStatuses_[oid.id() - 1]);

    SharedConstGroupPtr const& g = getGroup(bid, getProd, getProv, false);
    if (g.get() == 0) {
      if (!oid.isValid()) {
        throw edm::Exception(edm::errors::ProductNotFound,"InvalidID")
	  << "getForOutput: invalid ProductID supplied\n";
      }
      status = productstatus::neverCreated();
      return BasicHandle();
    }
    status = g->status();
    if (productstatus::onDemand(status)) {
      status = productstatus::neverCreated();
      return BasicHandle();
    }
    return BasicHandle(g->product(), &g->provenance());
  }

  BasicHandle
  Principal::getBySelector(TypeID const& productType, 
			   SelectorBase const& sel) const {

    BasicHandleVec results;

    int nFound = findGroups(productType,
                            preg_->productLookup(),
                            sel,
                            results,
                            true);

    if (nFound == 0) {
      boost::shared_ptr<cms::Exception> whyFailed( new edm::Exception(edm::errors::ProductNotFound) );
      *whyFailed
	<< "getBySelector: Found zero products matching all criteria\n"
	<< "Looking for type: " << productType << "\n";
      return BasicHandle(whyFailed);
    }
    if (nFound > 1) {
      throw edm::Exception(edm::errors::ProductNotFound)
        << "getBySelector: Found "<<nFound<<" products rather than one which match all criteria\n"
	<< "Looking for type: " << productType << "\n";
    }
    return results[0];
  }

  BasicHandle
  Principal::getByLabel(TypeID const& productType, 
			std::string const& label,
			std::string const& productInstanceName) const {
    BasicHandleVec results;

    edm::Selector sel(edm::ModuleLabelSelector(label) &&
                      edm::ProductInstanceNameSelector(productInstanceName));

    int nFound = findGroups(productType,
                            preg_->productLookup(),
                            sel,
                            results,
                            true);

    if (nFound == 0) {
      boost::shared_ptr<cms::Exception> whyFailed( new edm::Exception(edm::errors::ProductNotFound) );
      *whyFailed
	<< "getByLabel: Found zero products matching all criteria\n"
	<< "Looking for type: " << productType << "\n"
	<< "Looking for module label: " << label << "\n"
	<< "Looking for productInstanceName: " << productInstanceName << "\n";
      return BasicHandle(whyFailed);
    }
    if (nFound > 1) {
      throw edm::Exception(edm::errors::ProductNotFound)
        << "getByLabel: Found "<<nFound<<" products rather than one which match all criteria\n"
	<< "Looking for type: " << productType << "\n"
	<< "Looking for module label: " << label << "\n"
	<< "Looking for productInstanceName: " << productInstanceName << "\n";
    }
    return results[0];
  }

  BasicHandle
  Principal::getByLabel(TypeID const& productType,
			std::string const& label,
			std::string const& productInstanceName,
			std::string const& processName) const
  {

    BasicHandleVec results;

    edm::Selector sel(edm::ModuleLabelSelector(label) &&
                      edm::ProductInstanceNameSelector(productInstanceName) &&
                      edm::ProcessNameSelector(processName));

    int nFound = findGroups(productType,
                            preg_->productLookup(),
                            sel,
                            results,
                            true);

    if (nFound == 0) {
      boost::shared_ptr<cms::Exception> whyFailed( new edm::Exception(edm::errors::ProductNotFound) );
      *whyFailed
	<< "getByLabel: Found zero products matching all criteria\n"
	<< "Looking for type: " << productType << "\n"
	<< "Looking for module label: " << label << "\n"
	<< "Looking for productInstanceName: " << productInstanceName << "\n"
	<< "Looking for process: " << processName << "\n";
      return BasicHandle(whyFailed);
    }
    if (nFound > 1) {
      throw edm::Exception(edm::errors::ProductNotFound)
        << "getByLabel: Found "<<nFound<<" products rather than one which match all criteria\n"
	<< "Looking for type: " << productType << "\n"
	<< "Looking for module label: " << label << "\n"
	<< "Looking for productInstanceName: " << productInstanceName << "\n"
	<< "Looking for process: " << processName << "\n";
    }
    return results[0];
  }
 

  void 
  Principal::getMany(TypeID const& productType, 
		     SelectorBase const& sel,
		     BasicHandleVec& results) const {

    findGroups(productType,
               preg_->productLookup(),
               sel,
               results,
               false);

    return;
  }

  BasicHandle
  Principal::getByType(TypeID const& productType) const {

    BasicHandleVec results;

    edm::MatchAllSelector sel;

    int nFound = findGroups(productType,
                            preg_->productLookup(),
                            sel,
                            results,
                            true);

    if (nFound == 0) {
      boost::shared_ptr<cms::Exception> whyFailed( new edm::Exception(edm::errors::ProductNotFound) );
      *whyFailed
	<< "getByType: Found zero products matching all criteria\n"
	<< "Looking for type: " << productType << "\n";
      return BasicHandle(whyFailed);
    }
    if (nFound > 1) {
      throw edm::Exception(edm::errors::ProductNotFound)
        << "getByType: Found "<<nFound <<" products rather than one which match all criteria\n"
	<< "Looking for type: " << productType << "\n";
    }
    return results[0];
  }

  void 
  Principal::getManyByType(TypeID const& productType, 
			   BasicHandleVec& results) const {

    edm::MatchAllSelector sel;

    findGroups(productType,
               preg_->productLookup(),
               sel,
               results,
               false);
    return;
  }

  size_t
  Principal::getMatchingSequence(TypeID const& typeID,
				 SelectorBase const& selector,
				 BasicHandleVec& results,
				 bool stopIfProcessHasMatch) const {

    // One new argument is the element lookup container
    // Otherwise this just passes through the arguments to findGroups
    return findGroups(typeID,
                      preg_->elementLookup(),
                      selector,
                      results,
                      stopIfProcessHasMatch);
  }

  Provenance const&
  Principal::getProvenance(BranchID const& bid) const {
    SharedConstGroupPtr const& g = getGroup(bid, false, true, true);
    if (g.get() == 0) {
      throw edm::Exception(edm::errors::ProductNotFound,"InvalidID")
	<< "getProvenance: no product with given branch id: "<< bid << "\n";
    }

    if (g->onDemand()) {
      unscheduledFill(g->provenance());
    }
    // We already tried to produce the unscheduled products above
    // If they still are not there, then throw
    if (g->onDemand()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< "getProvenance: no product with given BranchID: "<< bid <<"\n";
    }

    return g->provenance();
  }

  // This one is mostly for test printout purposes
  // No attempt to trigger on demand execution
  // Skips provenance when the EDProduct is not there
  void
  Principal::getAllProvenance(std::vector<Provenance const*> & provenances) const {
    provenances.clear();
    for (Principal::const_iterator i = begin(), iEnd = end(); i != iEnd; ++i) {
      if (i->second->provenanceAvailable() && i->second->provenance().isPresent() && i->second->provenance().product().present())
	 provenances.push_back(&i->second->provenance());
    }
  }

  void
  Principal::readImmediate() const {
    for (Principal::const_iterator i = begin(), iEnd = end(); i != iEnd; ++i) {
      if (i->second->productUnavailable()) {
        resolveProduct(*i->second, false);
      }
      if (i->second->provenanceAvailable()) {
	resolveProvenance(*i->second);
      }
    }
  }

  EDProduct const *
  Principal::getIt(ProductID const& oid) const {
    return get(oid).wrapper();
  }

  size_t
  Principal::findGroups(TypeID const& typeID,
			TypeLookup const& typeLookup,
			SelectorBase const& selector,
			BasicHandleVec& results,
			bool stopIfProcessHasMatch) const {
    assert(results.empty());

    // A class without a dictionary cannot be in an Event/Lumi/Run.
    // First, we check if the class has a dictionary.  If it does not,
    // we return immediately.  This is necessary to avoid an exception
    // being thrown inside TypeID::friendlyClassName().
    if (!typeID.hasDictionary()) {
      return 0;
    }

    TypeLookup::const_iterator i = typeLookup.find(typeID.friendlyClassName());

    if (i == typeLookup.end()) {
      return 0;
    }

    const ProcessLookup& processLookup = i->second;

    // Handle groups for current process, note that we need to
    // look at the current process even if it is not in the processHistory
    // because of potential unscheduled (onDemand) production
    findGroupsForProcess(processConfiguration_.processName(),
                         processLookup,
                         selector,
                         results);

    // Loop over processes in reverse time order.  Sometimes we want to stop
    // after we find a process with matches so check for that at each step.
    for (ProcessHistory::const_reverse_iterator iproc = processHistory().rbegin(),
	   eproc = processHistory().rend();
         iproc != eproc && (results.empty() || !stopIfProcessHasMatch);
         ++iproc) {

      // We just dealt with the current process before the loop so skip it
      if (iproc->processName() == processConfiguration_.processName()) continue;

      findGroupsForProcess(iproc->processName(),
                           processLookup,
                           selector,
                           results);
    }
    return results.size();
  }

  void 
  Principal::findGroupsForProcess(std::string const& processName,
				  ProcessLookup const& processLookup,
				  SelectorBase const& selector,
				  BasicHandleVec& results) const {

    ProcessLookup::const_iterator j = processLookup.find(processName);

    if (j == processLookup.end()) return;

    // This is a vector of indexes into the productID vector
    // These indexes point to groups with desired process name (and
    // also type when this function is called from findGroups)
    std::vector<BranchID> const& vindex = j->second;

    for (std::vector<BranchID>::const_iterator ib(vindex.begin()), ie(vindex.end());
	 ib != ie;
	 ++ib) {
      SharedConstGroupPtr const& group = getGroup(*ib, false, false, false);
      if(group.get() == 0) {
        continue;
      }

      if (selector.match(group->provenance())) {

	// Skip product if not available.
        if (!group->productUnavailable()) {
          this->resolveProduct(*group, true);
	  // If the product is a dummy filler, group will now be marked unavailable.
          // Unscheduled execution can fail to produce the EDProduct so check
          if (!group->productUnavailable() && !group->onDemand()) {
            // Found a good match, save it
            results.push_back(BasicHandle(group->product(), &group->provenance()));
          }
        }
      }
    }
    return;
  }

  void
  Principal::resolveProduct(Group const& g, bool fillOnDemand) const {
    if (g.productUnavailable()) {
      throw edm::Exception(errors::ProductNotFound,"InaccessibleProduct")
	<< "resolve_: product is not accessible\n"
	<< g.provenance() << '\n';
    }

    if (g.product()) return; // nothing to do.

    // Try unscheduled production.
    if (g.onDemand()) {
      if (fillOnDemand) unscheduledFill(g.provenance());
      return;
    }

    // must attempt to load from persistent store
    BranchKey const bk = BranchKey(g.productDescription());
    std::auto_ptr<EDProduct> edp(store_->getProduct(bk, this));

    // Now fix up the Group
    g.setProduct(edp);
  }

  void
  Principal::resolveProvenance(Group const& g) const {
    if (g.entryDescription()) return;

    // must attempt to load from persistent store
    std::auto_ptr<EntryDescription> prov(store_->getProvenance(g.productDescription()));

    // Now fix up the Group
    g.setProvenance(prov);
  }

  void
  Principal::recombine(Principal & other, std::vector<BranchID> const& bids) {
    for (std::vector<BranchID>::const_iterator it = bids.begin(), itEnd = bids.end(); it != itEnd; ++it) {
      groups_[*it].swap(other.groups_[*it]);
    }
    store_->mergeReaders(other.store());
  }
}
