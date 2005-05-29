/**
   \file
   test for ScheduleExecutor

   \author Stefano ARGIRO
   \version $Id: ScheduleExecutorFromPSet_t.cc,v 1.7 2005/05/28 06:04:36 wmtan Exp $
   \date 18 May 2005
*/

static const char CVSId[] = "$Id: ScheduleExecutorFromPSet_t.cc,v 1.7 2005/05/28 06:04:36 wmtan Exp $";

#include "FWCore/CoreFramework/interface/EDProducer.h"
#include "FWCore/CoreFramework/interface/ScheduleExecutor.h"

#include "FWCore/CoreFramework/interface/ScheduleBuilder.h"
#include "FWCore/ParameterSet/interface/Nodes.h"
#include "FWCore/ParameterSet/interface/ProcessDesc.h"


#include "FWCore/CoreFramework/interface/EventSetup.h"
#include "FWCore/CoreFramework/interface/EventSetupProvider.h"
#include "FWCore/CoreFramework/interface/Timestamp.h"

#include "FWCore/CoreFramework/interface/InputServiceDescription.h"
#include "FWCore/CoreFramework/test/DummyEventSetupRecordRetriever.h"
#include "FWCore/FWCoreServices/src/EmptyInputService.h"

#include "FWCore/ParameterSet/interface/parse.h"
#include "FWCore/ParameterSet/interface/Makers.h"
#include "FWCore/ParameterSet/interface/MakeProcessPSet.h"

#include <iostream>
#include <string>
#include <fstream>
#include <exception>
#include "PluginManager/PluginManager.h"

using namespace edm;
using namespace edm::pset;
using namespace std;




int main(){

  seal::PluginManager::get ()->initialise ();




  int length;
  char * buffer;

  ifstream is;
  is.open ("testScheduler.txt");

  // get length of file:
  is.seekg (0, ios::end);
  length = is.tellg();
  is.seekg (0, ios::beg);

  // allocate memory:
  buffer = new char [length];

  // read data as a block:
  is.read (buffer,length);
  is.close();

  cout << buffer << endl;

  string bufferStr(buffer);
  

  boost::shared_ptr<edm::ParameterSet> processPSet= makeProcessPSet(bufferStr);
  
  // actual test of schedule executor

  ScheduleBuilder builder(*processPSet);

  ScheduleExecutor executor(builder.getPathList());

  edm::eventsetup::EventSetupProvider cp;
  boost::shared_ptr<DummyEventSetupRecordRetriever> 
    pRetriever( new DummyEventSetupRecordRetriever );
  cp.add( boost::shared_ptr<eventsetup::DataProxyProvider>(pRetriever) );
  cp.add( boost::shared_ptr<eventsetup::EventSetupRecordIntervalFinder>(pRetriever));

  std::string param1("int32 MaxEvents=5");
  boost::shared_ptr<ParameterSet> input_service_pset = makePSet( *edm::pset::parse(param1.c_str() ) );
  const InputServiceDescription desc("test",1);

  auto_ptr<InputService> input(new EmptyInputService(*input_service_pset,desc));
  auto_ptr<EventPrincipal> pep = input->readEvent();

  edm::Timestamp ts(123);
  EventSetup const& c = cp.eventSetupForInstance(ts);


  int ret = executor.runOneEvent(*pep,c);
  cout << "Result " << ret << endl;  

  ret = executor.runOneEvent(*pep,c);
  cout << "Result " << ret << endl;  


}
