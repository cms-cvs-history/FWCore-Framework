/*----------------------------------------------------------------------

Test of the EventProcessor class.

$Id: EventProcessor2_t.cpp,v 1.2 2005/06/02 20:20:00 wmtan Exp $

----------------------------------------------------------------------*/  
#include <exception>
#include <iostream>
#include <string>

#include "FWCore/CoreFramework/interface/EventProcessor.h"

void work()
{
  std::string configuration("process PROD = {\n"
		    "source = EmptyInputService { untracked int32 maxEvents = 5 }\n"
		    "module m1 = IntProducer { int32 ivalue = 10 }\n"
		    "module m2 = DoubleProducer { double dvalue = 3.3 }\n"
		    "module out = AsciiOutputModule { untracked bool keepOnlySpecifiedProducts = false untracked vstring skipProducts = {} }\n"
                    "path p1 = { m1,m2,out }\n"
		    "}\n");
  edm::EventProcessor proc(configuration);

  proc.run(0);
}

int main()
{
  int rc = -1;                // we should never return this value!
  try { work(); rc = 0;}
  catch ( std::exception& e )
    {
      std::cerr << "Exception caught: " << e.what() << std::endl;
      rc = 1;
    }
  catch ( ... )
    {
      std::cerr << "Unknown exception caught" << std::endl;
      rc = 2;
    }
  return rc;
}
