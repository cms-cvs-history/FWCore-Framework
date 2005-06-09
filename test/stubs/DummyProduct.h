/**
   \file
   declaration of class DummyProduct

   \author Stefano ARGIRO
   \version $Id$
   \date 06 Jun 2005
*/

#ifndef _edm_DummyProduct_h_
#define _edm_DummyProduct_h_

static const char CVSId_edmtest_DummyProduct[] = 
"$Id$";

#include "FWCore/EDProduct/interface/EDProduct.h"
#include <string>

namespace edmtest {

/**
   A test product 
*/
  
  class DummyProduct:public edm::EDProduct{
    
  public:

    void setName(const std::string& name);
    std::string getName() const ;
  
  private:
    std::string dummyname_;
    //double dummyname_;
    
  };
  
} // edmtest


#endif // _edmtest_DummyProduct_h_

