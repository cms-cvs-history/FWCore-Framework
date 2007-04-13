/*----------------------------------------------------------------------
$Id: DummySource.cc,v 1.5 2006/12/13 22:15:22 wmtan Exp $
----------------------------------------------------------------------*/

#include <stdexcept>
#include <memory>

#include "FWCore/Framework/test/DummySource.h"

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
