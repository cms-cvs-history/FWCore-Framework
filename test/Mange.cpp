/*----------------------------------------------------------------------

This is a generic main that can be used with any plugin and a 
PSet script.   See notes in EventProcessor.cpp for details about
it.

$Id: Mange.cpp,v 1.3 2005/06/28 04:46:02 jbk Exp $

----------------------------------------------------------------------*/  

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "FWCore/CoreFramework/interface/EventProcessor.h"
#include "FWCore/FWUtilities/interface/Exception.h"

using namespace std;

int main(int argc, char* argv[])
{
  // Right now option processing is not really present.
  // The EventProcessor should handle this in the future.

  std::cerr << "Mange has been renamed cmsRun. "
            << " Mange will be removed in the next prerelease. "
            << " To avoid this annoying message, switch to cmsRun now. " << std::endl;
  int rc = -1; // we should never return this value!
  try
    {
      edm::EventProcessor proc(argc,argv);
      proc.run();
      rc = 0;
    }
  catch (seal::Error& e)
    {
      std::cerr << "Exception caught in " << argv[0] << ": "
		<< e.explainSelf()
		<< std::endl;
      rc = 1;
    }
  catch (std::exception& e)
    {
      std::cerr << "Standard library exception caught in " << argv[0] << ": "
		<< e.what()
		<< std::endl;
      rc = 1;
    }
  catch (...)
    {
      std::cerr << "Unknown exception caught in " << argv[0]
		<< std::endl;
      rc = 2;
    }
  
  return rc;
}
