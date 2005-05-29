/*----------------------------------------------------------------------

Test of the EventProcessor class.

$Id: EventProcessor2_t.cpp,v 1.3 2005/05/28 05:53:03 wmtan Exp $

----------------------------------------------------------------------*/  
#include <exception>
#include <iostream>
#include <string>

#include "FWCore/CoreFramework/interface/EventProcessor.h"

void work()
{
  std::string configuration("process PROD = {\n"
		    "source = EmptyInputService { int32 MaxEvents=5 }\n"
		    "module m1 = IntProducer { int32 ivalue = 10 }\n"
		    "module m2 = DoubleProducer { double dvalue = 3.3 }\n"
		    "module out = AsciiOutputModule { int32 x = 1 }\n"
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
