#ifndef Modules_DummySource_h
#define Modules_DummySource_h

/*----------------------------------------------------------------------
$Id: DummySource.h,v 1.1 2005/10/17 19:22:41 wmtan Exp $
----------------------------------------------------------------------*/

#include "FWCore/Framework/interface/GeneratedInputSource.h"

namespace edm {
  class DummySource : public GeneratedInputSource {
  public:
    explicit DummySource(ParameterSet const&, InputSourceDescription const&);
    ~DummySource();
  private:
    virtual void produce(Event &);
  };
}
#endif
