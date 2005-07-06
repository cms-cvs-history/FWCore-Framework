/*----------------------------------------------------------------------

Test of the EventProcessor class.

$Id: EventProcessor2_t.cpp,v 1.6 2005/06/28 04:46:02 jbk Exp $

----------------------------------------------------------------------*/  
#include <exception>
#include <iostream>
#include <string>
#include <stdexcept>
#include "FWCore/FWUtilities/interface/EDMException.h"
#include "FWCore/CoreFramework/interface/EventProcessor.h"

void work()
{
  std::string configuration("process PROD = {\n"
		    "source = EmptyInputService { untracked int32 maxEvents = 5 }\n"
		    "module m1 = IntProducer { int32 ivalue = 10 }\n"
		    "module m2 = DoubleProducer { double dvalue = 3.3 }\n"
		    "module out = AsciiOutputModule { }\n"
                    "path p1 = { m1,m2,out }\n"
		    "}\n");
  edm::EventProcessor proc(configuration);

  proc.run(0);
}

int main()
{
  int rc = -1;                // we should never return this value!
  try { work(); rc = 0;}
  catch (seal::Error& e)
    {
      std::cerr << "Application exception caught: "
		<< e.explainSelf() << std::endl;
      rc = 1;
    }
  catch (std::runtime_error& e)
    {
      std::cerr << "Standard library exception caught: "
		<< e.what() << std::endl;
      rc = 1;
    }
  catch (...)
    {
      std::cerr << "Unknown exception caught" << std::endl;
      rc = 2;
    }
  return rc;
}
