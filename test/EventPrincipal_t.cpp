/*----------------------------------------------------------------------

Test of the EventPrincipal class.

$Id: EventPrincipal_t.cpp,v 1.9 2005/07/14 22:50:53 wmtan Exp $

----------------------------------------------------------------------*/  
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/EDProduct/interface/EDP_ID.h"
#include "FWCore/Framework/interface/BasicHandle.h"
#include "FWCore/EDProduct/interface/Wrapper.h"
#include "FWCore/Framework/interface/Selector.h"
#include "FWCore/Framework/src/TypeID.h"
#include "FWCore/Framework/src/ToyProducts.h"

#include "FWCore/Framework/interface/EventPrincipal.h"

typedef edm::BasicHandle handle;



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
      handle h = ep.getByLabel(tid, label, std::string());
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

  typedef edmtest::DummyProduct DP;
  typedef edm::Wrapper<DP> WDP;
  DP * pr = new DP;
  std::auto_ptr<edm::EDProduct> pprod(new WDP(*pr));
  std::auto_ptr<edm::Provenance> pprov(new edm::Provenance);
  std::string label("fred");
  std::string processName = "PROD";
  
  edmtest::DummyProduct dp;
  edm::TypeID dummytype(dp);
  std::string className = dummytype.friendlyClassName();
  pprov->product.full_product_type_name = dummytype.userClassName();
  pprov->product.friendly_product_type_name = className;
  pprov->product.module.module_label = label;
  pprov->product.module.process_name = processName;
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

  typedef edmtest::DummyProduct DP;
  typedef edm::Wrapper<DP> WDP;
  DP * pr = new DP;
  std::auto_ptr<edm::EDProduct> pprod(new WDP(*pr));
  std::auto_ptr<edm::Provenance> pprov(new edm::Provenance);
  std::string label("fred");

  edmtest::DummyProduct dp;
  edm::TypeID dummytype(dp);
  std::string className = dummytype.friendlyClassName();

  pprov->product.full_product_type_name = dummytype.userClassName();
  pprov->product.friendly_product_type_name = className;
  pprov->product.module.module_label = label;
  pprov->product.module.process_name = processName;
  ep.put(pprod, pprov);

  edm::EDP_ID id(1);
  
  try
    {
      handle h = ep.get(id);
      assert(h.isValid());
      assert(h.id() == id);
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

  typedef edmtest::DummyProduct DP;
  typedef edm::Wrapper<DP> WDP;
  DP * pr = new DP;
  std::auto_ptr<edm::EDProduct> pprod(new WDP(*pr));
  std::auto_ptr<edm::Provenance> pprov(new edm::Provenance);
  std::string label("fred");
  std::string productInstanceName("Rick");

  edmtest::DummyProduct dp;
  edm::TypeID dummytype(dp);
  std::string className = dummytype.friendlyClassName();

  pprov->product.full_product_type_name = dummytype.userClassName();
  pprov->product.friendly_product_type_name = className;

  pprov->product.module.module_label = label;
  pprov->product.product_instance_name = productInstanceName;
  pprov->product.module.process_name = processName;
  ep.put(pprod, pprov);
  
  try
    {
      edmtest::DummyProduct example;
      edm::TypeID tid(example);

      handle h = ep.getByLabel(tid, label, productInstanceName);
      assert(h.isValid());
      assert(h.provenance()->product.module.module_label == label);
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

  typedef edmtest::DummyProduct DP;
  typedef edm::Wrapper<DP> WDP;
  DP * pr = new DP;
  std::auto_ptr<edm::EDProduct> pprod(new WDP(*pr));
  std::auto_ptr<edm::Provenance> pprov(new edm::Provenance);
  std::string label("fred");

  edmtest::DummyProduct dp;
  edm::TypeID dummytype(dp);
  std::string className = dummytype.friendlyClassName();

  pprov->product.full_product_type_name = dummytype.userClassName();
  pprov->product.friendly_product_type_name = className;

  pprov->product.module.module_label = label;
  pprov->product.module.process_name = processName;
  ep.put(pprod, pprov);

  try
    {
      edmtest::DummyProduct example;
      edm::TypeID tid(example);
      edm::ProcessNameSelector pnsel(processName);

      handle h = ep.getBySelector(tid, pnsel);
      assert(h.isValid());
      assert(h.provenance()->product.module.module_label == label);
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
