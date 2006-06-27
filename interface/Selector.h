#ifndef Framework_Selector_h
#define Framework_Selector_h

/*----------------------------------------------------------------------
  
Selector: Base class for all "selector" objects, used to select
EDProducts based on information in the associated Provenance.

$Id: Selector.h,v 1.10 2006/06/24 01:46:34 wmtan Exp $

----------------------------------------------------------------------*/

#include <string>

#include "FWCore/Framework/interface/ProvenanceAccess.h"

namespace edm {

  // Abstract base class.
  class Selector {
  public:
    virtual ~Selector();
    bool match(const ProvenanceAccess& p) const;
  private:
    virtual bool doMatch(const ProvenanceAccess& p) const = 0;
  };


  // Select based upon full description of EDProducer.
  class ModuleDescriptionSelector : public Selector {
  public:
    ModuleDescriptionSelector(const Hash<ModuleDescription>& md):md_(md) {}
    
    virtual bool doMatch(const ProvenanceAccess& p) const {
      return p.event().moduleHash() == md_;
    }

  private:
    Hash<ModuleDescription> md_;
  };


  // Select based on process name.
  class ProcessNameSelector : public Selector {
  public:
    ProcessNameSelector(const std::string& pn):pn_(pn) {}
    
    virtual bool doMatch(const ProvenanceAccess& p) const {
      return p.product().processName() == pn_;
    }

  private:
    std::string pn_;
  };

}

#endif
