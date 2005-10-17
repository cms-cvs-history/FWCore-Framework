#ifndef Framework_DummySource_h
#define Framework_DummySource_h

/*----------------------------------------------------------------------
$Id: DummySource.h,v 1.1 2005/10/15 02:06:56 wmtan Exp $
----------------------------------------------------------------------*/

#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/InputSource.h"
#include "FWCore/EDProduct/interface/EventID.h"

namespace edm {
  class ParameterSet;

  class DummySource : public InputSource {
  public:
    explicit DummySource(ParameterSet const&, const InputSourceDescription&);
    ~DummySource();
  private:
    std::auto_ptr<EventPrincipal> read();
    int remainingEvents_;
    EventID nextID_;
  };
}
#endif
