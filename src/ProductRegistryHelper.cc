/*----------------------------------------------------------------------
  
$Id: ProductRegistryHelper.cc,v 1.8.2.1 2006/06/27 02:14:12 wmtan Exp $

----------------------------------------------------------------------*/

#include "FWCore/Framework/interface/ModuleDescriptionRegistry.h"
#include "FWCore/Framework/interface/ProductRegistryHelper.h"
#include "DataFormats/Common/interface/ProductRegistry.h"
#include "DataFormats/Common/interface/BranchDescription.h"
#include "DataFormats/Common/interface/ModuleDescription.h"

namespace edm {
  ProductRegistryHelper::~ProductRegistryHelper() { }

  ProductRegistryHelper::TypeLabelList & ProductRegistryHelper::typeLabelList() {
    return typeLabelList_;
  }

  void
  ProductRegistryHelper::addToRegistry(TypeLabelList::const_iterator const& iBegin,
                             TypeLabelList::const_iterator const& iEnd,
                             ModuleDescription const& iDesc,
                             ProductRegistry& iReg,
                             bool iIsListener) {
    for (TypeLabelList::const_iterator p = iBegin; p != iEnd; ++p) {
      BranchDescription pdesc(iDesc.moduleLabel_,
                              iDesc.processName_,
                              p->typeID_.userClassName(),
                              p->typeID_.friendlyClassName(), 
                              p->productInstanceName_,
                              iDesc.id());
      pdesc.psetIDs_.insert(iDesc.pid);
      pdesc.branchAliases_.insert(p->branchAlias_);
      iReg.addProduct(pdesc, iIsListener);
      ModuleDescriptionRegistry::instance()->insertMapped(iDesc);
    }//for
  }
}
