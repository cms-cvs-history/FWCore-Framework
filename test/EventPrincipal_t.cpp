/*----------------------------------------------------------------------

Test of the EventPrincipal class.

$Id: EventPrincipal_t.cpp,v 1.5 2005/06/23 20:01:12 wmtan Exp $

----------------------------------------------------------------------*/  
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include "FWCore/FWUtilities/interface/EDMException.h"
#include "FWCore/EDProduct/interface/EDP_ID.h"
#include "FWCore/CoreFramework/interface/Handle.h"
#include "FWCore/CoreFramework/interface/Selector.h"
#include "FWCore/CoreFramework/src/TypeID.h"
#include "FWCore/CoreFramework/src/ToyProducts.h"

#include "FWCore/CoreFramework/interface/EventPrincipal.h"

typedef edm::Handle<edm::EDProduct> handle;



void fail_get_by_id()
{
  edm::EventPrincipal ep;
  ep.addToProcessHistory("PROD");
  try
    {
      edm::EDP_ID id(1);
      handle h = ep.get(id);
      assert("Failed to throw required exception" == 0);
    }
  catch (edm::Exception& x)
    {
      // nothing to do
    }
  catch (...)
    {
      assert("Threw wrong kind of exception" == 0);
    }
}

void fail_get_by_selector()
{
  edm::EventPrincipal ep;
  ep.addToProcessHistory("PROD");
  try
    {
      edm::TypeID tid(ep);   // sure not to match any product
      edm::ProcessNameSelector sel("PROD");
      handle h = ep.getBySelector(tid, sel);
      assert("Failed to throw required exception" == 0);      
    }
  catch (edm::Exception& x)
    {
      // nothing to do
    }
  catch (...)
    {
      assert("Threw wrong kind of exception" == 0);
    }
}

void fail_get_by_label()
{
  edm::EventPrincipal ep;
  ep.addToProcessHistory("PROD");
  try
    {
      edm::TypeID tid(ep);   // sure not to match any product
      std::string label("this does not exist");
      handle h = ep.getByLabel(tid, label);
      assert("Failed to throw required exception" == 0);      
    }
  catch (edm::Exception& x)
    {
      // nothing to do
    }
  catch (...)
    {
      assert("Threw wrong kind of exception" == 0);
    }
}

void fail_get_many()
{
  edm::EventPrincipal ep;
  ep.addToProcessHistory("PROD");
  try
    {
      edm::TypeID tid(ep);   // sure not to match any product
      edm::ProcessNameSelector sel("PROD");
      std::vector<handle> handles;
      ep.getMany(tid, sel, handles);
      assert("Failed to throw required exception" == 0);      
    }
  catch (edm::Exception& x)
    {
      // nothing to do
    }
  catch (...)
    {
      assert("Threw wrong kind of exception" == 0);
    }

}

void fail_get_by_invalid_id()
{
  edm::EventPrincipal ep;
  ep.addToProcessHistory("PROD");

  std::auto_ptr<edm::EDProduct> pprod(new edmtest::DummyProduct);
  std::auto_ptr<edm::Provenance> pprov(new edm::Provenance);
  std::string label("fred");
  std::string processName = "PROD";
  
  edmtest::DummyProduct dp;
  edm::TypeID dummytype(dp);
  std::string className = dummytype.friendlyClassName();
  pprov->full_product_type_name = dummytype.reflectionClassName();
  pprov->friendly_product_type_name = className;
  pprov->module.module_label = label;
  pprov->module.process_name = processName;
  ep.put(pprod, pprov);
  edm::EDP_ID id(1);

  try
    {
      handle h = ep.get(id-1);
      assert("Failed to throw required exception" == 0);      
    }
  catch (edm::Exception& x)
    {
      // nothing to do
    }
  catch (...)
    {
      assert("Threw wrong kind of exception" == 0);
    }
}



void get_by_id()
{
  std::string processName = "PROD";
  edm::EventPrincipal ep;
  ep.addToProcessHistory(processName);

  std::auto_ptr<edm::EDProduct> pprod(new edmtest::DummyProduct());
  std::auto_ptr<edm::Provenance> pprov(new edm::Provenance);
  std::string label("fred");

  edmtest::DummyProduct dp;
  edm::TypeID dummytype(dp);
  std::string className = dummytype.friendlyClassName();

  pprov->full_product_type_name = dummytype.reflectionClassName();
  pprov->friendly_product_type_name = className;
  pprov->module.module_label = label;
  pprov->module.process_name = processName;
  ep.put(pprod, pprov);

  edm::EDP_ID id(1);
  
  try
    {
      handle h = ep.get(id);
      assert(h.isValid());
      assert(h->id() == id);
    }
  catch (edm::Exception& x)
    {
      std::cerr << x.what()<< std::endl;
      assert("Threw exception unexpectedly" == 0);
    }
  catch (...)
    {
      std::cerr << "Unknown exception type\n";
      assert("Threw exception unexpectedly" == 0);
    }
}

void get_by_label()
{
  edm::EventPrincipal ep;
  std::string processName = "PROD";
  ep.addToProcessHistory(processName);

  std::auto_ptr<edm::EDProduct> pprod(new edmtest::DummyProduct());
  std::auto_ptr<edm::Provenance> pprov(new edm::Provenance);
  std::string label("fred");

  edmtest::DummyProduct dp;
  edm::TypeID dummytype(dp);
  std::string className = dummytype.friendlyClassName();

  pprov->full_product_type_name = dummytype.reflectionClassName();
  pprov->friendly_product_type_name = className;


  pprov->module.module_label = label;
  pprov->module.process_name = processName;
  ep.put(pprod, pprov);
  
  try
    {
      edmtest::DummyProduct example;
      edm::TypeID tid(example);

      handle h = ep.getByLabel(tid, label);
      assert(h.isValid());
      assert(h.provenance()->module.module_label == label);
    }
  catch (edm::Exception& x)
    {
      std::cerr << x.what()<< std::endl;
      assert("Threw exception unexpectedly" == 0);
    }
  catch (...)
    {
      std::cerr << "Unknown exception type\n";
      assert("Threw exception unexpectedly" == 0);
    }
}



void get_by_selector()
{
  edm::EventPrincipal ep;
  std::string processName("PROD");
  ep.addToProcessHistory(processName);

  std::auto_ptr<edm::EDProduct> pprod(new edmtest::DummyProduct());
  std::auto_ptr<edm::Provenance> pprov(new edm::Provenance);
  std::string label("fred");

  edmtest::DummyProduct dp;
  edm::TypeID dummytype(dp);
  std::string className = dummytype.friendlyClassName();

  pprov->full_product_type_name = dummytype.reflectionClassName();
  pprov->friendly_product_type_name = className;

  pprov->module.module_label = label;
  pprov->module.process_name = processName;
  ep.put(pprod, pprov);

  try
    {
      edmtest::DummyProduct example;
      edm::TypeID tid(example);
      edm::ProcessNameSelector pnsel(processName);

      handle h = ep.getBySelector(tid, pnsel);
      assert(h.isValid());
      assert(h.provenance()->module.module_label == label);
    }
  catch (edm::Exception& x)
    {
      std::cerr << x.what()<< std::endl;
      assert("Threw exception unexpectedly" == 0);
    }
  catch (...)
    {
      std::cerr << "Unknown exception type\n";
      assert("Threw exception unexpectedly" == 0);
    }
}


int main()
{
  fail_get_by_id();
  fail_get_by_selector();
  fail_get_by_label();
  fail_get_many(); 
  fail_get_by_invalid_id(); 

  get_by_id();
  get_by_label();
  get_by_selector();
}
