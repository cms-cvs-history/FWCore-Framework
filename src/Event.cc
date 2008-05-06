#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "DataFormats/Provenance/interface/BranchType.h"
#include "FWCore/Framework/interface/EventPrincipal.h"

namespace edm {

    Event::Event(EventPrincipal& ep, ModuleDescription const& md) :
	DataViewImpl(ep, md, InEvent),
	aux_(ep.aux()),
	luminosityBlock_(new LuminosityBlock(ep.luminosityBlockPrincipal(), md)),
	gotProductIDs_(),
	gotViews_() {
    }

    EventPrincipal &
    Event::eventPrincipal() {
      return dynamic_cast<EventPrincipal &>(principal());
    }

    EventPrincipal const &
    Event::eventPrincipal() const {
      return dynamic_cast<EventPrincipal const&>(principal());
    }

    Run const&
    Event::getRun() const {
      return getLuminosityBlock().getRun();
    }

//   History const& 
//   Event::history() const {
//     DataViewImpl const& dvi = me();
//     EDProductGetter const* pg = dvi.prodGetter(); // certain to be non-null
//     assert(pg);
//     EventPrincipal const& ep = dynamic_cast<EventPrincipal const&>(*pg);
//     return ep.history();
//   }
  History const&
  Event::history() const {
    return eventPrincipal().history();
  }


  Provenance const&
  Event::getProvenance(BranchID const& bid) const
  {
    return eventPrincipal().getProvenance(bid);
  }

  void
  Event::getAllProvenance(std::vector<Provenance const*> & provenances) const
  {
    eventPrincipal().getAllProvenance(provenances);
  }

  BasicHandle
  Event::getByProductID_(ProductID const& oid) const
  {
    return eventPrincipal().getByProductID(oid);
  }


  void
  Event::commit_() {
    // fill in guts of provenance here
    EventPrincipal & ep = eventPrincipal();
    ProductPtrVec::iterator pit(putProducts().begin());
    ProductPtrVec::iterator pie(putProducts().end());

    while(pit!=pie) {
	std::auto_ptr<EDProduct> pr(pit->first);
	// note: ownership has been passed - so clear the pointer!
	pit->first = 0;

	// set provenance
	boost::shared_ptr<EntryDescription> entryDescriptionPtr(new EntryDescription);
	entryDescriptionPtr->parents_ = gotProductIDs_;
	entryDescriptionPtr->moduleDescriptionID_ = pit->second->moduleDescriptionID();
	boost::shared_ptr<BranchEntryInfo> branchEntryInfoPtr(
		new BranchEntryInfo(pit->second->branchID(),
				    pit->second->productIDtoAssign(),
				    productstatus::present(),
				    entryDescriptionPtr));
	std::auto_ptr<Provenance> pv(
	    new Provenance(*pit->second, branchEntryInfoPtr));
	ep.put(pr,pv);
	++pit;
    }

    // the cleanup is all or none
    putProducts().clear();
  }

}
