#ifndef Framework_InputSource_h
#define Framework_InputSource_h


/*----------------------------------------------------------------------
  
InputSource: Abstract interface for all primary input sources. Input
sources are responsible for creating an EventPrincipal, using data
controlled by the source, and external to the EventPrincipal itself.

The InputSource is also responsible for dealing with the "process
name list" contained within the EventPrincipal. Each InputSource has
to know what "process" (HLT, PROD, USER, USER1, etc.) the program is
part of. The InputSource is repsonsible for pushing this process name
onto the end of the process name list.

For now, we specify this process name to the constructor of the
InputSource. This should be improved.

 Some questions about this remain:

   1. What should happen if we "rerun" a process? i.e., if "USER1" is
   already last in our input file, and we run again a job which claims
   to be "USER1", what should happen? For now, we just quietly add
   this to the history.

   2. Do we need to detect a problem with a history like:
         HLT PROD USER1 PROD
   or is it up to the user not to do something silly? Right now, there
   is no protection against such sillyness.

Some examples of InputSource subclasses may be:

 1) EmptySource: creates EventPrincipals which contain no EDProducts.
 2) PoolSource: creates EventPrincipals which "contain" the data
    read from a POOL file. This source should provide for delayed loading
    of data, thus the quotation marks around contain.
 3) DAQInputSource: creats EventPrincipals which contain raw data, as
    delivered by the L1 trigger and event builder. 

$Id: InputSource.h,v 1.7 2006/04/18 21:55:21 lsexton Exp $

----------------------------------------------------------------------*/

#include <memory>
#include <string>

#include "DataFormats/Common/interface/EventID.h"
#include "DataFormats/Common/interface/ModuleDescription.h"
#include "FWCore/Framework/interface/ProductRegistryHelper.h"

namespace edm {
  class EventPrincipal;
  class ProductRegistry;
  class InputSourceDescription;
  class EventID;
  class ParameterSet;
  class InputSource : public ProductRegistryHelper {
  public:
    /// Constructor
    explicit InputSource(ParameterSet const&, InputSourceDescription const&);

    /// Destructor
    virtual ~InputSource();

    /// Indicate inability to get a new event by returning a null auto_ptr.

    /// Read next event
    std::auto_ptr<EventPrincipal> readEvent();

    /// Read a specific event
    std::auto_ptr<EventPrincipal> readEvent(EventID const&);

    /// Skip the number of events specified.
    /// Offset may be negative.
    void skipEvents(int offset);

    /// Begin again at the first event
    void rewind() {rewind_();}

    /// Wake up the input source
    void wakeUp() {wakeUp_();}

    /// Set the run number
    void setRunNumber(RunNumber_t r) {setRun(r);}

    /// issue an event report
    void issueReports(EventID const&);

    /// Set the module description,
    /// and register any products to be created.
    // FIX THIS:  The module description should be
    // set at construction time by being contained 
    // the input source description.
    void addToRegistry(ModuleDescription const& md);

    /// Accessor for product registry.
    ProductRegistry & productRegistry() const {return *preg_;}
    
    /// Reset the remaining number of events to the maximum number.
    void repeat() {remainingEvents_ = maxEvents_;}

    /// Accessor for maximum number of events to be read.
    /// -1 is used for unlimited.
    int maxEvents() const {return maxEvents_;}

    /// Accessor for remaining number of events to be read.
    int remainingEvents() const {return remainingEvents_;}

    /// Accessor for 'module' description.
    ModuleDescription const& module() const {return module_;}

  private:

    // Indicate inability to get a new event by returning a null
    // auto_ptr.
    std::auto_ptr<EventPrincipal> readEvent_();

    std::auto_ptr<EventPrincipal> readEvent_(EventID const&);

    virtual std::auto_ptr<EventPrincipal> read() = 0;

    virtual std::auto_ptr<EventPrincipal> readIt(EventID const&);

    virtual void skip(int);

    virtual void setRun(RunNumber_t r);

    virtual void rewind_();

    virtual void wakeUp_() {}

  private:

    int const maxEvents_;

    int remainingEvents_;

    int readCount_;

    bool const unlimited_;

    ModuleDescription module_;

    // A pointer to the ProductRegistry;
    ProductRegistry * preg_;

    // The process name we add to each EventPrincipal.
    std::string const process_;
  };
}

#endif
