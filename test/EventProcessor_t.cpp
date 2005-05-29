/*----------------------------------------------------------------------

Test of the EventProcessor class.

$Id: EventProcessor_t.cpp,v 1.5 2005/05/28 05:53:03 wmtan Exp $

----------------------------------------------------------------------*/  
#include <exception>
#include <iostream>
#include <string>

#include "FWCore/CoreFramework/interface/EventProcessor.h"

void work()
{
  std::string configuration("process p = {\n"
			    "source = EmptyInputService { int32 MaxEvents=5 }\n"
			    "module m1 = TestMod { int32 ivalue = 10 }\n"
			    "module m2 = TestMod { int32 ivalue = -3 }\n"
                            "path p1 = { m1,m2 }\n"
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
