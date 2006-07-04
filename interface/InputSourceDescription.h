#ifndef Framework_InputSourceDescription_h
#define Framework_InputSourceDescription_h

/*----------------------------------------------------------------------

InputSourceDescription : the stuff that is needed to configure an
input source that does not come in through the ParameterSet  

$Id: InputSourceDescription.h,v 1.3 2006/05/02 02:15:06 wmtan Exp $
----------------------------------------------------------------------*/
#include <string>
#include "DataFormats/Common/interface/ModuleDescription.h"
#include "DataFormats/Common/interface/ProcessHistoryItem.h"

namespace edm {
  class ProductRegistry;

  struct InputSourceDescription {
    InputSourceDescription() : moduleDescription_(), processHistoryItem_(), productRegistry_(0) { }
    InputSourceDescription(ModuleDescription const& md,
			    ProcessHistoryItem const& ph,
			    ProductRegistry& preg) :
      moduleDescription_(md),
      processHistoryItem_(ph),
      productRegistry_(&preg)
	 
    {}

    ModuleDescription moduleDescription_;
    ProcessHistoryItem processHistoryItem_;
    ProductRegistry * productRegistry_;
  };
}

#endif
