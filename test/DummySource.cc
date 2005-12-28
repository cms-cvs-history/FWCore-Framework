/*----------------------------------------------------------------------
$Id: DummySource.cc,v 1.2 2005/12/28 00:51:28 wmtan Exp $
----------------------------------------------------------------------*/

#include <stdexcept>
#include <memory>

#include "FWCore/Framework/test/DummySource.h"
#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/InputSourceMacros.h"

namespace edm {
  DummySource::DummySource(ParameterSet const& pset,
				       InputSourceDescription const& desc) :
    GeneratedInputSource(pset, desc)
  { }

  DummySource::~DummySource() {
  }

  bool
  DummySource::produce(edm::Event &) {
    return true;
  }
}
using edm::DummySource;
DEFINE_FWK_INPUT_SOURCE(DummySource)

