/*----------------------------------------------------------------------
  $Id: SelectorBase.cc,v 1.2 2007/04/09 22:18:56 wdd Exp $
  ----------------------------------------------------------------------*/

#include "FWCore/Framework/interface/SelectorBase.h"
#include "DataFormats/Provenance/interface/ConstBranchDescription.h"

namespace edm
{

  //------------------------------------------------------------------
  //
  // SelectorBase
  //  
  //------------------------------------------------------------------
  SelectorBase::~SelectorBase()
  { }

  bool
  SelectorBase::match(ConstBranchDescription const& p) const
  {
    return doMatch(p);
  }
}
