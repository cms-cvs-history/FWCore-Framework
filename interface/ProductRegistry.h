#ifndef Framework_ProductRegistry_h
#define Framework_ProductRegistry_h

/**
   \file
   Implementation of ProductRegistry

   \author Stefano ARGIRO
   \co-author Bill Tanenbaum
   \version $Id: ProductRegistry.h,v 1.9 2005/09/01 05:08:50 wmtan Exp $
   \date 19 Jul 2005
*/

#include "FWCore/Framework/interface/BranchKey.h"
#include "FWCore/Framework/interface/ProductDescription.h"

namespace edm {

  /**
     \class ProductRegistry ProductRegistry.h "edm/ProductRegistry.h"

     \brief 

     \author Stefano ARGIRO
     \co-author Bill Tanenbaum
     \date 19 Jul 2005
  */
  class ProductRegistry {

  public:
    ProductRegistry() : productList_(), nextID_(0) {}

    ~ProductRegistry() {}
  
    typedef std::map<BranchKey, ProductDescription> ProductList;

    void addProduct(ProductDescription const& productdesc);

    void copyProduct(ProductDescription const& productdesc);

    void setProductIDs();

    ProductList const& productList() const {return productList_;}
    
    unsigned long nextID() const {return nextID_;}

    void setNextID(unsigned long next) {nextID_ = next;}

  private:
    ProductList productList_;
    unsigned long nextID_;
  };
} // edm


#endif

