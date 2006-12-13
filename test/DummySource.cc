/*----------------------------------------------------------------------
$Id: DummySource.cc,v 1.4 2006/10/21 17:18:56 wmtan Exp $
----------------------------------------------------------------------*/

#include <stdexcept>
#include <memory>

#include "FWCore/Framework/test/DummySource.h"
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
DEFINE_FWK_INPUT_SOURCE(DummySource);

