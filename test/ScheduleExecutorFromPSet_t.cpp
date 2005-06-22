/**
   \file
   test for ScheduleExecutor

   \author Stefano ARGIRO
   \version $Id: ScheduleExecutorFromPSet_t.cpp,v 1.7 2005/06/20 15:18:56 argiro Exp $
   \date 18 May 2005
*/

static const char CVSId[] = "$Id: ScheduleExecutorFromPSet_t.cpp,v 1.7 2005/06/20 15:18:56 argiro Exp $";


#include "FWCore/CoreFramework/interface/ScheduleExecutor.h"
#include "FWCore/CoreFramework/interface/ScheduleBuilder.h"
#include "FWCore/CoreFramework/interface/UnknownModuleException.h"


#include "FWCore/CoreFramework/interface/EventPrincipal.h"
#include "FWCore/CoreFramework/interface/EventSetup.h"
#include "FWCore/CoreFramework/interface/EventSetupProvider.h"
#include "FWCore/CoreFramework/interface/Timestamp.h"

#include "FWCore/CoreFramework/interface/InputServiceDescription.h"
#include "FWCore/CoreFramework/test/DummyEventSetupRecordRetriever.h"
#include "FWCore/FWCoreServices/src/EmptyInputService.h"

#include <FWCore/ParameterSet/interface/ProcessPSetBuilder.h>
#include <FWCore/ParameterSet/interface/Makers.h>
#include <FWCore/ParameterSet/interface/parse.h>

#include "FWCore/CoreFramework/src/TypeID.h"

#include "FWCore/CoreFramework/src/ToyProducts.h"


#include <sstream>
#include <string>


#include "boost/test/unit_test.hpp"

using namespace edm;
using namespace edm::pset;
using namespace std;
using namespace boost::unit_test_framework;

auto_ptr<InputService> setupDummyInputService(){

  std::string param1("int32 maxEvents=5");
  boost::shared_ptr<ParameterSet> input_service_pset = 
    makePSet( *edm::pset::parse(param1.c_str() ) );
  const InputServiceDescription desc("test",1);
  auto_ptr<InputService> 
    input(new EmptyInputService(*input_service_pset,desc));
  
  return input;  
}

const EventSetup& setupDummyEventSetup(){

  edm::eventsetup::EventSetupProvider cp;
  boost::shared_ptr<DummyEventSetupRecordRetriever> 
    pRetriever( new DummyEventSetupRecordRetriever );
  cp.add( boost::shared_ptr<eventsetup::DataProxyProvider>(pRetriever) );
  cp.add( boost::shared_ptr<eventsetup::EventSetupRecordIntervalFinder>(pRetriever)); 
  edm::Timestamp ts(123);
  return cp.eventSetupForInstance(ts);
}

// check that each module wrote its name
void checkProducts(const std::string names, EventPrincipal& pep){

 for (string::const_iterator namesIt = names.begin();
       namesIt!=names.end(); ++namesIt){
    
    stringstream tmp;
    tmp<< *namesIt;
    // this is only for testing and should never be done by the user
   
    Handle<EDProduct> tmp1 = 
      pep.getByLabel((TypeID(typeid(edmtest::StringProduct))),tmp.str());
    Handle<edmtest::StringProduct> p;
    convert_handle(tmp1,p);

    BOOST_CHECK(p->name_==tmp.str());
  }//for

}

void test_trivial_path(){

  const char * conf =   "process test ={ \n"                  
    "module a = TestSchedulerModule1 { string module_name = \"a\" }\n"
    "path p = { a}\n";
  
  ProcessPSetBuilder b(conf);
  boost::shared_ptr<ParameterSet> processPSet = b.getProcessPSet();
  
  ScheduleBuilder builder(*processPSet);
  
  ScheduleExecutor executor(builder.getPathList());
  
  auto_ptr<InputService> input = setupDummyInputService();
  auto_ptr<EventPrincipal> pep = input->readEvent();
  const EventSetup& c = setupDummyEventSetup();
  
  executor.runOneEvent(*pep,c);
  
  const string names("a");
  checkProducts(names,*pep);
  
}

void test_one_path_with_sequence(){

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

  ScheduleBuilder builder(*processPSet);
  
  ScheduleExecutor executor(builder.getPathList());
  
  auto_ptr<InputService> input = setupDummyInputService();
  auto_ptr<EventPrincipal> pep = input->readEvent();
  const EventSetup& c = setupDummyEventSetup();

  executor.runOneEvent(*pep,c);

  const string names("abcde");
  checkProducts(names,*pep);
 
}

void test_multiple_path_with_sequence(){

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
  
  ScheduleBuilder builder(*processPSet);
  
  ScheduleExecutor executor(builder.getPathList());
  
  auto_ptr<InputService> input = setupDummyInputService();
  auto_ptr<EventPrincipal> pep = input->readEvent();
  const EventSetup& c = setupDummyEventSetup();

  executor.runOneEvent(*pep,c);

  const string names("abecde");
  checkProducts(names,*pep);

}
void test_failing_toload_module(){

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
 
 BOOST_CHECKPOINT("Going to instanciate a non-implemented module");
 BOOST_CHECK_THROW(ScheduleBuilder builder(*processPSet),
		   UnknownModuleException);

}


test_suite*
init_unit_test_suite( int /*argc*/, char* /*argv*/[] ) {
  test_suite* test = BOOST_TEST_SUITE("TestScheduler");
  
  test->add( BOOST_TEST_CASE( &test_one_path_with_sequence ) );
  test->add( BOOST_TEST_CASE( &test_multiple_path_with_sequence ) );
  test->add( BOOST_TEST_CASE( &test_failing_toload_module ) );
  
  return test;
}
