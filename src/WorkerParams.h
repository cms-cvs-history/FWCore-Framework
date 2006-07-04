#ifndef Framework_WorkerParams_h
#define Framework_WorkerParams_h

/** ----------------------

This struct is used to communication parameters into the worker factory.

---------------------- **/

#include "DataFormats/Common/interface/PassID.h"
#include "DataFormats/Common/interface/ReleaseVersion.h"
#include "FWCore/Utilities/interface/GetPassID.h"
#include "FWCore/Utilities/interface/GetReleaseVersion.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <string>

namespace edm
{
  class ProductRegistry;
  class ActionTable;

  struct WorkerParams
  {
    WorkerParams(): 
      pset_(),reg_(),actions_(),
      processName_(),releaseVersion_(),passID_() { }

    WorkerParams(ParameterSet const& pset,
		 ProductRegistry& reg,
		 ActionTable& actions,
		 std::string const& processName,
		 std::string releaseVersion=getReleaseVersion(),
		 std::string passID=getPassID()):
      pset_(&pset),reg_(&reg),actions_(&actions),
      processName_(processName),releaseVersion_(releaseVersion),passID_(passID) { }

    ParameterSet const* pset_;
    ProductRegistry* reg_;
    ActionTable* actions_;
    std::string processName_;
    ReleaseVersion releaseVersion_;
    PassID passID_;
  };
}

#endif
