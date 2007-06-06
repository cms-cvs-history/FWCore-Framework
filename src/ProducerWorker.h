#ifndef Framework_ProducerWorker_h
#define Framework_ProducerWorker_h

/*----------------------------------------------------------------------
  
ProducerWorker: The EDProducer as the schedule sees it.  The job of
this object is to call the producer, collect up the results, and
feed them into the event.
According to our current definition, a single producer can only
appear in one worker.

$Id: ProducerWorker.h,v 1.18 2006/06/20 23:13:27 paterno Exp $

----------------------------------------------------------------------*/

#include <memory>

#include "boost/shared_ptr.hpp"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/src/Worker.h"

namespace edm
{

  class ProducerWorker : public Worker
  {
  public:
    ProducerWorker(std::auto_ptr<EDProducer>,
		   const ModuleDescription&,
		   const WorkerParams&);

    virtual ~ProducerWorker();

    template <class ModType>
    static std::auto_ptr<EDProducer> makeOne(const ModuleDescription& md,
					     const WorkerParams& wp);
  private:
    virtual bool implDoWork(EventPrincipal& e, EventSetup const& c,
			    CurrentProcessingContext const* cpc);

    virtual void implBeginJob(EventSetup const&) ;
    virtual void implEndJob() ;
    virtual bool implBeginRun(RunPrincipal& rp, EventSetup const& c,
			    CurrentProcessingContext const* cpc);
    virtual bool implEndRun(RunPrincipal& rp, EventSetup const& c,
			    CurrentProcessingContext const* cpc);
    virtual bool implBeginLuminosityBlock(LuminosityBlockPrincipal& lbp, EventSetup const& c,
			    CurrentProcessingContext const* cpc);
    virtual bool implEndLuminosityBlock(LuminosityBlockPrincipal& lbp, EventSetup const& c,
			    CurrentProcessingContext const* cpc);
    virtual std::string workerType() const;
    
    boost::shared_ptr<EDProducer> producer_;
  };

  template <> 
  struct WorkerType<EDProducer>
  {
    typedef EDProducer ModuleType;
    typedef ProducerWorker worker_type;
  };

  template <class ModType>
  std::auto_ptr<EDProducer> ProducerWorker::makeOne(const ModuleDescription&,
						const WorkerParams& wp)
  {
    return std::auto_ptr<EDProducer>(new ModType(*wp.pset_));
  }

}

#endif
