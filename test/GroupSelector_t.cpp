#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "FWCore/Framework/interface/GroupSelector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/ModuleDescription.h"
#include "DataFormats/Common/interface/BranchDescription.h"
#include "FWCore/Utilities/interface/EDMException.h"

using std::string;
using std::vector;

typedef std::vector<edm::BranchDescription const*> VCBDP;

void apply_gs(edm::GroupSelector const& gs,
	      VCBDP const&  allbranches,
	      vector<bool>& results)
{
  VCBDP::const_iterator it  = allbranches.begin();
  VCBDP::const_iterator end = allbranches.end();
  for ( ; it != end; ++it ) results.push_back(gs.selected(**it));
}

int doTest(edm::ParameterSet const& params,
	     const char* testname,
	     VCBDP const&  allbranches,
	     vector<bool>& expected)
{
  edm::GroupSelector gs(params, allbranches);
  std::cout << "GroupSelector from "
	    << testname
	    << ": "
	    << gs
	    << std::endl;

  vector<bool> results;
  apply_gs(gs, allbranches, results);

  int rc = 0;
  if ( expected != results ) rc = 1;
  if (rc == 1) std::cerr << "FAILURE: " << testname << '\n';
  std::cout << "---" << std::endl;
  return rc;
}

int work()
{
  int rc = 0;
  // We pretend to have one module, with two products. The products
  // are of the same and, type differ in instance name.
  edm::ModuleDescription modA;
  edm::ParameterSet modAparams;
  modAparams.addParameter<int>("i", 2112);
  modAparams.addParameter<string>("s", "hi");
  modA.pid = modAparams.id();
  modA.moduleName_    = "typeA";
  modA.moduleLabel_   = "modA";
  modA.versionNumber_ = 3UL;
  modA.processName_   = "PROD";
  modA.pass           = 1UL;

  // The trailing 0 is a null EDProduct pointer. Sure hope it ain't
  // used!
  edm::BranchDescription b1(modA, "UglyProdTypeA", "ProdTypeA", "i1", 0);
  edm::BranchDescription b2(modA, "UglyProdTypeA", "ProdTypeA", "i2", 0);

  // Our second pretend module has only one product, and gives it no
  // instance name.
  edm::ModuleDescription modB;
  edm::ParameterSet modBparams;
  modBparams.addParameter<double>("d", 2.5);
  modB.pid = modBparams.id();
  modB.moduleName_    = "typeB";
  modB.moduleLabel_   = "modB";
  modB.versionNumber_ = 1UL;
  modB.processName_   = "HLT";
  modB.pass           = 1UL;

  edm::BranchDescription b3(modB, "UglyProdTypeB", "ProdTypeB", "", 0);

  // Our third pretend is like modA, except it hass processName_ of
  // "USER"
  edm::ModuleDescription modC;
  modC.pid            = modA.pid;
  modC.moduleName_    = modA.moduleName_;
  modC.moduleLabel_   = modA.moduleLabel_;
  modC.versionNumber_ = modA.versionNumber_;
  modC.processName_   = "USER";
  modC.pass           = modA.pass;

  edm::BranchDescription b4(modC, "UglyProdTypeA", "ProdTypeA", "i1", 0);
  edm::BranchDescription b5(modC, "UglyProdTypeA", "ProdTypeA", "i2", 0);

  
  // These are pointers to all the branches that are available. In a
  // framework program, these would come from the ProductRegistry
  // which is used to initialze the OutputModule being configured.
  VCBDP allbranches;
  allbranches.push_back(&b1); // ProdTypeA_modA_i1. (PROD)
  allbranches.push_back(&b2); // ProdTypeA_modA_i2. (PROD)
  allbranches.push_back(&b3); // ProdTypeB_modB_HLT. (no instance name)
  allbranches.push_back(&b4); // ProdTypeA_modA_i1_USER.
  allbranches.push_back(&b5); // ProdTypeA_modA_i2_USER.
  
  // Test default parameters
  {
    bool wanted[] = { true, true, true, true, true };
    vector<bool> expected(wanted, wanted+sizeof(wanted)/sizeof(bool));
    edm::ParameterSet noparams;

    rc += doTest(noparams, "default parameters", allbranches, expected);
  }

  // Keep all branches with instance name i2.
  {
    bool wanted[] = { false, true, false, false, true };
    vector<bool> expected(wanted, wanted+sizeof(wanted)/sizeof(bool));

    edm::ParameterSet keep_i2;
    const string keep_i2_rule = "keep *_*_i2_*";
    vector<string> cmds;
    cmds.push_back(keep_i2_rule);
    keep_i2.addUntrackedParameter<vector<string> >("outputCommands", cmds);

    rc += doTest(keep_i2, "keep_i2 parameters", allbranches, expected);
  }

  // Drop all branches with instance name i2.
  {
    bool wanted[] = { true, false, true, true, false };
    vector<bool> expected(wanted, wanted+sizeof(wanted)/sizeof(bool));

    edm::ParameterSet drop_i2;
    const string drop_i2_rule1 = "keep *";
    const string drop_i2_rule2 = "drop *_*_i2_*";
    vector<string> cmds;
    cmds.push_back(drop_i2_rule1);
    cmds.push_back(drop_i2_rule2);
    drop_i2.addUntrackedParameter<vector<string> >("outputCommands", cmds);

    rc += doTest(drop_i2, "drop_i2 parameters", allbranches, expected);
  }

  // Now try dropping all branches with product type "foo". There are
  // none, so all branches should be written.
  {
    bool wanted[] = { true, true, true, true, true };
    vector<bool> expected(wanted, wanted+sizeof(wanted)/sizeof(bool));

    edm::ParameterSet drop_foo;
    const string drop_foo_rule1 = "keep *_*_*_*"; // same as "keep *"
    const string drop_foo_rule2 = "drop foo_*_*_*";
    vector<string> cmds;
    cmds.push_back(drop_foo_rule1);
    cmds.push_back(drop_foo_rule2);
    drop_foo.addUntrackedParameter<vector<string> >("outputCommands", cmds);

    rc += doTest(drop_foo, "drop_foo parameters", allbranches, expected);
  }

  // Now try dropping all branches with product type "ProdTypeA".
  {
    bool wanted[] = { false, false, true, false, false };
    vector<bool> expected(wanted, wanted+sizeof(wanted)/sizeof(bool));

    edm::ParameterSet drop_ProdTypeA;
    const string drop_ProdTypeA_rule1 = "keep *";
    const string drop_ProdTypeA_rule2 = "drop ProdTypeA_*_*_*";
    vector<string> cmds;
    cmds.push_back(drop_ProdTypeA_rule1);
    cmds.push_back(drop_ProdTypeA_rule2);
    drop_ProdTypeA.addUntrackedParameter<vector<string> >("outputCommands", cmds);

    rc += doTest(drop_ProdTypeA,
		 "drop_ProdTypeA",
		 allbranches, expected);   
  }

  // Keep only branches with instance name 'i1', from Production.
  {
    bool wanted[] = { true, false, false, false, false };
    vector<bool> expected(wanted, wanted+sizeof(wanted)/sizeof(bool));

    edm::ParameterSet keep_i1prod;
    const string keep_i1prod_rule = "keep *_*_i1_PROD";
    vector<string> cmds;
    cmds.push_back(keep_i1prod_rule);
    keep_i1prod.addUntrackedParameter<vector<string> >("outputCommands", cmds);

    rc += doTest(keep_i1prod,
		 "keep_i1prod",
		 allbranches, expected);   
  }

  // First say to keep everything,  then  to drop everything, then  to
  // keep it again. The end result should be to keep everything.
  {
    bool wanted[] = { true, true, true, true, true };
    vector<bool> expected(wanted, wanted+sizeof(wanted)/sizeof(bool));

    edm::ParameterSet indecisive;
    const string indecisive_rule1 = "keep *";
    const string indecisive_rule2 = "drop *";
    const string indecisive_rule3 = "keep *";
    vector<string> cmds;
    cmds.push_back(indecisive_rule1);
    cmds.push_back(indecisive_rule2);
    cmds.push_back(indecisive_rule3);
    indecisive.addUntrackedParameter<vector<string> >("outputCommands", cmds);

    rc += doTest(indecisive,
		 "indecisive",
		 allbranches, expected);
  }

  // Keep all things, bu drop all things from modA, but later keep all
  // things from USER.
  {
    bool wanted[] = { false, false, true, true, true };
    vector<bool> expected(wanted, wanted+sizeof(wanted)/sizeof(bool));

    edm::ParameterSet params;
    const string rule1 = "keep *";
    const string rule2 = "drop *_modA_*_*";
    const string rule3 = "keep *_*_*_USER";
    vector<string> cmds;
    cmds.push_back(rule1);
    cmds.push_back(rule2);
    cmds.push_back(rule3);
    params.addUntrackedParameter<vector<string> >("outputCommands", cmds);

    rc += doTest(params,
		 "drop_modA_keep_user",
		 allbranches, expected);
  }

  {
    // Now try an illegal specification: not starting with 'keep' or 'drop'
    try
      {
	edm::ParameterSet bad;
	const string bad_rule = "beep *_*_i2_*";
	vector<string> cmds;
	cmds.push_back(bad_rule);
	bad.addUntrackedParameter<vector<string> >("outputCommands", cmds);
	edm::GroupSelector gs(bad, allbranches);
      }
    catch ( edm::Exception const& x )
      {
	// OK, we should be here... now check exception type
	assert ( x.categoryCode() == edm::errors::Configuration );
      }
    catch ( ... )
      {
	std::cerr << "Wrong exception type\n";
	rc += 1;
      }
    
  }

  // Now try an illegal specification: too short
  {
    try
      {
	edm::ParameterSet bad;
	const string bad_rule = "drop ";
	vector<string> cmds;
	cmds.push_back(bad_rule);
	bad.addUntrackedParameter<vector<string> >("outputCommands", cmds);
	edm::GroupSelector gs(bad, allbranches);	
	std::cerr << "Failed to throw required exception\n";
	rc += 1;
      }
    catch ( edm::Exception const& x )
      {
	// OK, we should be here... now check exception type
	assert ( x.categoryCode() == edm::errors::Configuration );
      }
    catch ( ... )
      {
	std::cerr << "Wrong exception type\n";
	rc += 1;
      }    
  }

  return rc;
}

int main()
{
  int rc = 0;
  try
    {
      rc = work();
    }
  catch ( edm::Exception& x )
    {
      std::cerr << "edm::Exception caught:\n" << x << '\n';
      rc = 1;
    }
  catch ( ... )
    {
      std::cerr << "Unknown exception caught\n";
      rc = 2;
    }
  return rc;
}


