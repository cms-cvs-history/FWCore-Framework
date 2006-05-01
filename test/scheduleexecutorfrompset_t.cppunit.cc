/**
   \file
   test for ScheduleExecutor

   \author Stefano ARGIRO
   \Changed by Viji Sundararajan on 03-Jul-05.
   \version $Id: scheduleexecutorfrompset_t.cppunit.cc,v 1.23 2006/02/20 01:51:59 wmtan Exp $
   \date 18 May 2005
*/

static const char CVSId[] = "$Id: scheduleexecutorfrompset_t.cppunit.cc,v 1.23 2006/02/20 01:51:59 wmtan Exp $";

#include "FWCore/Framework/interface/ScheduleExecutor.h"
#include "FWCore/Framework/interface/ScheduleBuilder.h"
#include "FWCore/Framework/interface/UnknownModuleException.h"

#include "DataFormats/Common/interface/ProductRegistry.h"
#include "FWCore/Framework/src/WorkerRegistry.h"
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EventSetupProvider.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/Actions.h"
#include "FWCore/Framework/interface/BasicHandle.h"
#include "FWCore/Framework/interface/IOVSyncValue.h"
#include "DataFormats/Common/interface/Wrapper.h"

#include "FWCore/Framework/interface/InputSourceDescription.h"
#include "FWCore/Framework/test/DummyEventSetupRecordRetriever.h"
#include "FWCore/Framework/test/DummySource.h"

#include <FWCore/ParameterSet/interface/ProcessPSetBuilder.h>
#include <FWCore/ParameterSet/interface/Makers.h>
#include <FWCore/ParameterSet/interface/parse.h>

#include "FWCore/Framework/interface/TypeID.h"

#include "DataFormats/TestObjects/interface/ToyProducts.h"

#include "FWCore/Utilities/interface/EDMException.h"

#include <sstream>
#include <string>


#include <cppunit/extensions/HelperMacros.h>

using namespace edm;
using namespace edm::pset;
using namespace std;

auto_ptr<InputSource> setupDummyInputSource(ProductRegistry& preg){

  std::string param1("untracked int32 maxEvents=5");
  boost::shared_ptr<ParameterSet> input_service_pset = 
    makePSet(*edm::pset::parse(param1.c_str()));
  ModuleDescription md;
  md.processName_ = std::string("test");
  md.pass = 1;
  const InputSourceDescription desc(md, preg);
  auto_ptr<InputSource> 
    input(new DummySource(*input_service_pset, desc));
  
  return input;  
}


//Have to pass EventSetupProvider in as argument since EventSetup returned
// only have lifetime as long as the EventSetupProvider from which it comes
const EventSetup& setupDummyEventSetup(edm::eventsetup::EventSetupProvider& cp){

  boost::shared_ptr<DummyEventSetupRecordRetriever> 
    pRetriever(new DummyEventSetupRecordRetriever);
  cp.add(boost::shared_ptr<eventsetup::DataProxyProvider>(pRetriever));
  cp.add(boost::shared_ptr<EventSetupRecordIntervalFinder>(pRetriever)); 
  edm::IOVSyncValue ts(edm::Timestamp(123));
  return cp.eventSetupForInstance(ts);
}

// check that each module wrote its name
void checkProducts(const std::string names, EventPrincipal& pep){

 for (string::const_iterator namesIt = names.begin();
       namesIt!=names.end(); ++namesIt){
    
    stringstream tmp;
    tmp << *namesIt;
    // this is only for testing and should never be done by the user
   
    BasicHandle tmp1 = 
      pep.getByLabel((TypeID(typeid(edmtest::StringProduct))),tmp.str(), std::string());
    Handle<edmtest::StringProduct> p;
    convert_handle(tmp1,p);
    CPPUNIT_ASSERT(p->name_==tmp.str());
  }//for

}


class testScheduleExecutorFromPSet: public CppUnit::TestFixture
{
CPPUNIT_TEST_SUITE(testScheduleExecutorFromPSet);

CPPUNIT_TEST(trivialPathTest);
CPPUNIT_TEST(onePathwithSequenceTest);
CPPUNIT_TEST(multiplePathwithSequenceTest);
//CPPUNIT_TEST(failingToloadModuleTest);
CPPUNIT_TEST_EXCEPTION(failingToloadModuleTest,edm::Exception);


CPPUNIT_TEST_SUITE_END();
public:
  void setUp(){}
  void tearDown(){}

  void trivialPathTest();
  void onePathwithSequenceTest();
  void multiplePathwithSequenceTest();
  void failingToloadModuleTest();
};

///registration of the test so that the runner can find it
CPPUNIT_TEST_SUITE_REGISTRATION(testScheduleExecutorFromPSet);

void testScheduleExecutorFromPSet::trivialPathTest(){

  const char * conf =   "process test ={ \n"                  
  "module a = TestSchedulerModule1 { string module_name = \"a\" }\n"
  "path p = { a}\n" 
  "}\n";

  ProcessPSetBuilder b(conf);
  boost::shared_ptr<ParameterSet> processPSet = b.getProcessPSet();
  
  WorkerRegistry wreg;
  ProductRegistry preg;
  ActionTable table;

  auto_ptr<InputSource> input = setupDummyInputSource(preg);

  ScheduleBuilder builder(*processPSet, wreg, preg, table);
  
  ScheduleExecutor executor(builder.getPathList(), table);
  
  auto_ptr<EventPrincipal> pep = input->readEvent();

  edm::eventsetup::EventSetupProvider cp;
  const EventSetup& c = setupDummyEventSetup(cp);
  
  executor.runOneEvent(*pep,c);
  
  const string names("a");
  checkProducts(names,*pep);
  
}

void testScheduleExecutorFromPSet::onePathwithSequenceTest(){

  const char * conf =   "process test ={ \n"                  
  "module a = TestSchedulerModule1 { string module_name = \"a\" }\n"
  "module b = TestSchedulerModule2 { string module_name = \"b\" }\n"
  "module c = TestSchedulerModule1 { string module_name = \"c\" }\n"
  "module d = TestSchedulerModule2 { string module_name = \"d\" }\n"
  "module e = TestSchedulerModule1 { string module_name = \"e\" }\n" 
  "sequence s1 = { a,b,c}\n"
  "sequence s2 = { d}\n"
  "path p = { s1,s2,e}\n" 
  "}\n";

    
  ProcessPSetBuilder b(conf);
  boost::shared_ptr<ParameterSet> processPSet = b.getProcessPSet();
  WorkerRegistry wreg;
  ProductRegistry preg;
  ActionTable table;

  auto_ptr<InputSource> input = setupDummyInputSource(preg);

  ScheduleBuilder builder(*processPSet, wreg, preg, table);
  
  ScheduleExecutor executor(builder.getPathList(), table);
  
  auto_ptr<EventPrincipal> pep = input->readEvent();
  edm::eventsetup::EventSetupProvider cp;
  const EventSetup& c = setupDummyEventSetup(cp);

  executor.runOneEvent(*pep,c);

  const string names("abcde");
  checkProducts(names,*pep);
 
}

void testScheduleExecutorFromPSet::multiplePathwithSequenceTest(){

  const char * conf =   "process test ={ \n"                  
  "module a = TestSchedulerModule1 { string module_name = \"a\" }\n"
  "module b = TestSchedulerModule2 { string module_name = \"b\" }\n"
  "module c = TestSchedulerModule1 { string module_name = \"c\" }\n"
  "module d = TestSchedulerModule2 { string module_name = \"d\" }\n"
  "module e = TestSchedulerModule1 { string module_name = \"e\" }\n" 
  "sequence s1 = { a,b}\n"
  "sequence s2 = { c,d}\n"
  "path p1 = { s1,e}\n"
  "path p2 = { s2}\n"
  "}\n";

  ProcessPSetBuilder b(conf);
  boost::shared_ptr<ParameterSet> processPSet = b.getProcessPSet();
   
  // actual test of schedule executor
  WorkerRegistry wreg;
  ProductRegistry preg;
  ActionTable table;

  auto_ptr<InputSource> input = setupDummyInputSource(preg);

  ScheduleBuilder builder(*processPSet, wreg, preg, table);

  ScheduleExecutor executor(builder.getPathList(), table);
  
  auto_ptr<EventPrincipal> pep = input->readEvent();
  edm::eventsetup::EventSetupProvider cp;
     const EventSetup& c = setupDummyEventSetup(cp);

  executor.runOneEvent(*pep,c);

  const string names("abecde");
  checkProducts(names,*pep);

}
void testScheduleExecutorFromPSet::failingToloadModuleTest(){

  // register_exception_translator<UnknownModuleException>(&exception1_translator);

const char * conf =   "process test ={ \n"                  
  "module a = NonExistentTestSchedulerModule { uint32 a=1}\n"
  "module b = TestSchedulerModule2 { uint32 a=2}\n"
  "module c = TestSchedulerModule1 { uint32 a=3}\n"
  "module d = TestSchedulerModule2 { uint32 a=4}\n"
  "module e = TestSchedulerModule1 { uint32 a=5}\n" 
  "sequence s1 = { a,b}\n"
  "sequence s2 = { c,d}\n"
  "path p = { s1,s2,e}\n" 
  "}\n";

 ProcessPSetBuilder b(conf);
 boost::shared_ptr<ParameterSet> processPSet = b.getProcessPSet(); 
 
 //BOOST_CHECKPOINT("Going to instanciate a non-implemented module");

 WorkerRegistry wreg;
 ProductRegistry preg; 
 ActionTable table;
 //BOOST_CHECK_THROW(ScheduleBuilder builder(*processPSet, wreg),
//		   edm::Exception);
 ScheduleBuilder builder(*processPSet, wreg, preg, table);
  
}


//test_suite*
//init_unit_test_suite(int /*argc*/, char* /*argv*/[]) 
/*{
  test_suite* test = BOOST_TEST_SUITE("TestScheduler");

  test->add(BOOST_TEST_CASE(&test_trivial_path));
  test->add(BOOST_TEST_CASE(&test_one_path_with_sequence));
  test->add(BOOST_TEST_CASE(&test_multiple_path_with_sequence));
  test->add(BOOST_TEST_CASE(&test_failing_toload_module));
  
  return test;
}*/
