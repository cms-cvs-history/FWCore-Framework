/**
   \file
   Implementation of class DummyProduct

   \author Stefano ARGIRO
   \version $Id$
   \date 06 Jun 2005
*/

static const char CVSId[] = "$Id$";


#include "DummyProduct.h"

using namespace edmtest;

void DummyProduct::setName(const std::string& name){dummyname_=name;}
std::string DummyProduct::getName() const {return dummyname_;}

