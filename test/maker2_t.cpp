
#include <iostream>

#include "FWCore/CoreFramework/interface/EDProducer.h"
#include "FWCore/CoreFramework/interface/Event.h"
#include "FWCore/CoreFramework/interface/Handle.h"
#include "FWCore/CoreFramework/interface/EventSetup.h"
#include "FWCore/CoreFramework/src/WorkerMaker.h"
#include "FWCore/CoreFramework/src/Factory.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/parse.h"
#include "FWCore/ParameterSet/interface/Makers.h"


using namespace std;
using namespace edm;

class TestMod : public EDProducer
{
 public:
  explicit TestMod(ParameterSet const& p);

  void produce(Event& e, EventSetup const&);
};

TestMod::TestMod(ParameterSet const& p)
{
}

void TestMod::produce(Event& e, EventSetup const&)
{
}

// ----------------------------------------------

int main()
{
  string param1 = "string module_type = \"TestMod\" \n"
    "string module_label = \"t1\" ";
  string param2 = "string module_type = \"TestMod\" \n"
    "string module_label = \"t2\" ";

  auto_ptr<Maker> f(new WorkerMaker<TestMod>);

  boost::shared_ptr<ParameterSet> p1 = makePSet(*edm::pset::parse(param1.c_str()));
  boost::shared_ptr<ParameterSet> p2 = makePSet(*edm::pset::parse(param2.c_str()));

  if(p1==0) cerr << "failed to make p1" << endl;
  if(p2==0) cerr << "failed to make p2" << endl;

  auto_ptr<Worker> w1 = f->makeWorker(*p1,"PROD",0,0);
  auto_ptr<Worker> w2 = f->makeWorker(*p2,"PROD",0,0);

  return 0;
}
