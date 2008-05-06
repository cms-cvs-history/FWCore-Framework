#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "DataFormats/Provenance/interface/BranchType.h"
#include "FWCore/Framework/interface/LuminosityBlockPrincipal.h"
#include "FWCore/Framework/interface/Run.h"

namespace edm {

  LuminosityBlock::LuminosityBlock(LuminosityBlockPrincipal& lbp, ModuleDescription const& md) :
	DataViewImpl(lbp, md, InLumi),
	aux_(lbp.aux()),
	run_(new Run(lbp.runPrincipal(), md)) {
  }

  LuminosityBlockPrincipal &
  LuminosityBlock::luminosityBlockPrincipal() {
    return dynamic_cast<LuminosityBlockPrincipal &>(principal());
  }

  LuminosityBlockPrincipal const &
  LuminosityBlock::luminosityBlockPrincipal() const {
    return dynamic_cast<LuminosityBlockPrincipal const&>(principal());
  }

  Provenance const&
  LuminosityBlock::getProvenance(BranchID const& bid) const
  {
    return luminosityBlockPrincipal().getProvenance(bid);
  }

  void
  LuminosityBlock::getAllProvenance(std::vector<Provenance const*> & provenances) const
  {
    luminosityBlockPrincipal().getAllProvenance(provenances);
  }


  void
  LuminosityBlock::commit_() {
    // fill in guts of provenance here
    LuminosityBlockPrincipal & lbp = luminosityBlockPrincipal();
    ProductPtrVec::iterator pit(putProducts().begin());
    ProductPtrVec::iterator pie(putProducts().end());

    while(pit!=pie) {
	std::auto_ptr<EDProduct> pr(pit->first);
	// note: ownership has been passed - so clear the pointer!
	pit->first = 0;

	// set provenance
	boost::shared_ptr<EntryDescription> entryDescriptionPtr(new EntryDescription);
	entryDescriptionPtr->moduleDescriptionID_ = pit->second->moduleDescriptionID();
	boost::shared_ptr<BranchEntryInfo> branchEntryInfoPtr(
		new BranchEntryInfo(pit->second->branchID(),
				    pit->second->productIDtoAssign(),
				    productstatus::present(),
				    entryDescriptionPtr));
	std::auto_ptr<Provenance> pv(
	    new Provenance(*pit->second, branchEntryInfoPtr));
	lbp.put(pr,pv);
	++pit;
    }

    // the cleanup is all or none
    putProducts().clear();
  }

}
