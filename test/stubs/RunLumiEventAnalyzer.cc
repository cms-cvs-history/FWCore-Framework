
#include "FWCore/Framework/test/stubs/RunLumiEventAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include <cassert>
#include <iostream>

namespace edmtest {

  RunLumiEventAnalyzer::RunLumiEventAnalyzer(edm::ParameterSet const& pset) :
    expectedRunLumisEvents_(pset.getUntrackedParameter<std::vector<unsigned int> >("expectedRunLumiEvents", std::vector<unsigned int>())),
    index_(0),
    verbose_(pset.getUntrackedParameter<bool>("verbose", false)),
    dumpTriggerResults_(pset.getUntrackedParameter<bool>("dumpTriggerResults", false))
  {
  }

  void RunLumiEventAnalyzer::analyze(edm::Event const& event, edm::EventSetup const& es) {

    if (verbose_) {
      edm::LogAbsolute("RunLumiEvent") << "RUN_LUMI_EVENT "
                                       << event.run() << ", " 
                                       << event.luminosityBlock() << ", "
                                       << event.id().event();
    }

    if (dumpTriggerResults_) {
      edm::Handle<edm::TriggerResults> triggerResults;
      event.getByLabel("TriggerResults", triggerResults);
      if(triggerResults.isValid()) {
        edm::LogAbsolute("RunLumiEvent") << "TestFailuresAnalyzer dumping TriggerResults";
        edm::LogAbsolute("RunLumiEvent") << *triggerResults;
      }
      else {
        edm::LogAbsolute("RunLumiEvent") << "TriggerResults not found\n";
      }
    }

    if ((index_ + 2U) < expectedRunLumisEvents_.size()) {
      assert(expectedRunLumisEvents_[index_++] == event.run());
      assert(expectedRunLumisEvents_[index_++] == event.luminosityBlock());
      assert(expectedRunLumisEvents_[index_++] == event.id().event());
    }
  }

  void RunLumiEventAnalyzer::beginRun(edm::Run const& run, edm::EventSetup const& es) {

    if (verbose_) {
      edm::LogAbsolute("RunLumiEvent") << "RUN_LUMI_EVENT "
                                       << run.run() << ", " 
                                       << 0 << ", "
                                       << 0;
    }

    if ((index_ + 2U) < expectedRunLumisEvents_.size()) {
      assert(expectedRunLumisEvents_[index_++] == run.run());
      assert(expectedRunLumisEvents_[index_++] == 0);
      assert(expectedRunLumisEvents_[index_++] == 0);
    }
  }

  void RunLumiEventAnalyzer::endRun(edm::Run const& run, edm::EventSetup const& es) {

    if (verbose_) {
      edm::LogAbsolute("RunLumiEvent") << "RUN_LUMI_EVENT "
                                       << run.run() << ", " 
                                       << 0 << ", "
                                       << 0;
    }

    if ((index_ + 2U) < expectedRunLumisEvents_.size()) {
      if (!(expectedRunLumisEvents_[index_++] == run.run())) {
        throw cms::Exception("UnexpectedRun", "RunLumiEventAnalyzer::endRun unexpected run");
      }
      if (!(expectedRunLumisEvents_[index_++] == 0)) {
        throw cms::Exception("UnexpectedLumi", "RunLumiEventAnalyzer::endRun unexpected lumi");
      }
      if (!(expectedRunLumisEvents_[index_++] == 0)) {
        throw cms::Exception("UnexpectedEvent", "RunLumiEventAnalyzer::endRun unexpected event");
      }
    }
  }

  void RunLumiEventAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& es) {

    if (verbose_) {
      edm::LogAbsolute("RunLumiEvent") << "RUN_LUMI_EVENT "
                                       << lumi.run() << ", " 
                                       << lumi.luminosityBlock() << ", "
                                       << 0;
    }

    if ((index_ + 2U) < expectedRunLumisEvents_.size()) {
      assert(expectedRunLumisEvents_[index_++] == lumi.run());
      assert(expectedRunLumisEvents_[index_++] == lumi.luminosityBlock());
      assert(expectedRunLumisEvents_[index_++] == 0);
    }
  }

  void RunLumiEventAnalyzer::endLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& es) {

    if (verbose_) {
      edm::LogAbsolute("RunLumiEvent") << "RUN_LUMI_EVENT "
                                       << lumi.run() << ", " 
                                       << lumi.luminosityBlock() << ", "
                                       << 0;
    }

    if ((index_ + 2U) < expectedRunLumisEvents_.size()) {
      if (!(expectedRunLumisEvents_[index_++] == lumi.run())) {
        throw cms::Exception("UnexpectedRun", "RunLumiEventAnalyzer::endLuminosityBlock unexpected run");
      }
      if (!(expectedRunLumisEvents_[index_++] == lumi.luminosityBlock())) {
        throw cms::Exception("UnexpectedLumi", "RunLumiEventAnalyzer::endLuminosityBlock unexpected lumi");
      }
      if (!(expectedRunLumisEvents_[index_++] == 0)) {
        throw cms::Exception("UnexpectedEvent", "RunLumiEventAnalyzer::endLuminosityBlock unexpected event");
      }
    }
  }
}

using edmtest::RunLumiEventAnalyzer;
DEFINE_FWK_MODULE(RunLumiEventAnalyzer);
