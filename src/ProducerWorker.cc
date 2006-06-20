

/*----------------------------------------------------------------------
$Id: ProducerWorker.cc,v 1.21 2006/04/20 22:33:22 wmtan Exp $
----------------------------------------------------------------------*/

#include "FWCore/Framework/src/ProducerWorker.h"

#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/src/WorkerParams.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/CurrentProcessingContext.h"
#include "FWCore/Framework/src/CPCSentry.h"

namespace edm
{
  ProducerWorker::ProducerWorker(std::auto_ptr<EDProducer> ed,
				 const ModuleDescription& md,
				 const WorkerParams& wp) :
    Worker(md,wp),
    producer_(ed) {
    producer_->registerProducts(producer_, wp.reg_, md, true);
  }

  ProducerWorker::~ProducerWorker() {
  }

  bool ProducerWorker::implDoWork(EventPrincipal& ep, EventSetup const& c,
				  CurrentProcessingContext const* cpc) {

    bool rc = false;

    Event e(ep,description());
    producer_->doProduce(e, c, cpc);
    e.commit_();
    rc = true;
    return rc;
  }

  void ProducerWorker::implBeginJob(EventSetup const& es) {
    producer_->beginJob(es);
  }

  void ProducerWorker::implEndJob() {
    producer_->endJob();
  }
  
  std::string ProducerWorker::workerType() const {
    return "EDProducer";
  }
  
}
