/*----------------------------------------------------------------------
$Id: DummySource.cc,v 1.1 2005/10/15 02:06:56 wmtan Exp $
----------------------------------------------------------------------*/

#include <stdexcept>
#include <memory>
#include <string>


#include "FWCore/Framework/test/DummySource.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/InputSourceDescription.h"
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/EDProduct/interface/EventID.h"
#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/InputSourceMacros.h"
#include "FWCore/Framework/interface/SourceFactory.h"

namespace edm {
  class BranchKey;

  //used for defaults
  static const unsigned long kNanoSecPerSec = 1000000000;
  static const unsigned long kAveEventPerSec = 200;
  
  DummySource::DummySource(ParameterSet const& pset,
				       InputSourceDescription const& desc) :
    InputSource(desc),
    remainingEvents_(pset.getUntrackedParameter<int>("maxEvents", -1)),
    nextID_(0)   
  { }

  DummySource::~DummySource() {
  }

  std::auto_ptr<EventPrincipal>
  DummySource::read() {
    std::auto_ptr<EventPrincipal> result(0);
    
    if (remainingEvents_-- != 0) {
      result = std::auto_ptr<EventPrincipal>(new EventPrincipal(nextID_.next(), Timestamp(), *preg_));
    }
    return result;
  }
}
using edm::DummySource;
DEFINE_FWK_INPUT_SOURCE(DummySource)
