
#include "FWCore/CoreFramework/interface/Exception.h"

#include <iostream>
#include <string>
#include <iomanip>

using namespace cms;
using namespace std;

struct Thing : public Exception
{
  Thing(const std::string& msg):Exception("Thing",msg) { }
};

ostream& operator<<(ostream& os, const Thing& t)
{
  os << "Thing(" << t.what() << ")";
  return os;
}

void func3()
{
  throw Thing("Data Corrupt") 
    << " Low level error"
    << endl;
}

void func2()
{
  func3();
}

void func1()
{
  try 
    {
      func2();
    }
  catch (Exception& e)
    {
      throw Exception("InfiniteLoop","In func2",e) << "Gave up";
    }
  
}

int main()
{
  try
    {
      func1();
    }
  catch (Exception& e)
    {
      cerr << "*** main caught Exception, output is ***\n"
	   << "(" << e.what() << ")"
	   << "*** After exception output ***"
	   << endl;

      cerr << "\nCategory name list:\n";

#if 0
      if(e.what() != answer)
	{
	  cerr << "not right answer\n(" << answer << ")\n"
	       << endl;
	  abort();
	}
#endif

    }
  return 0; 
}
