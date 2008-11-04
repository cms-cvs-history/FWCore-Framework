#ifndef FWCore_Framework_GenericObjectOwner_h
#define FWCore_Framework_GenericObjectOwner_h
// -*- C++ -*-
//
// Package:     Framework
// Class  :     GenericObjectOwner
// 
/**\class GenericObjectOwner GenericObjectOwner.h FWCore/Framework/interface/GenericObjectOwner.h

 Description: Helper classed used for doing a 'generic' put into the edm::Event

 Usage:
    

*/
//
// Original Author:  Chris Jones
//         Created:  Sun Feb  3 19:43:16 EST 2008
//

// system include files
#include "Reflex/Object.h"
#include "Reflex/Builder/TypeBuilder.h"

// user include files
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Utilities/interface/WrappedClassName.h"

// forward declarations
namespace edm {
class GenericObjectOwner
{

   public:
      GenericObjectOwner(): m_ownData(false){}
      explicit GenericObjectOwner(ROOT::Reflex::Object const& iObject,
                                  bool iOwnData=true):
         m_object(iObject), m_ownData(iOwnData) {}
      ~GenericObjectOwner();

      // ---------- const member functions ---------------------
      ROOT::Reflex::Object object() const;
   
      // ---------- static member functions --------------------

      // ---------- member functions ---------------------------
      void swap(GenericObjectOwner&);
      void release();
   
   private:
      GenericObjectOwner(GenericObjectOwner const&); // stop default

      GenericObjectOwner const& operator=(GenericObjectOwner const&); // stop default

      // ---------- member data --------------------------------
      ROOT::Reflex::Object m_object;
      bool m_ownData;
};

   //Need to specialize OrphanHandle because we don't actually have a long lived GenericObjectOwner
   template <>
   class OrphanHandle<GenericObjectOwner> {
   public:
      // Default constructed handles are invalid.
      OrphanHandle() {}
      
      OrphanHandle(OrphanHandle<GenericObjectOwner> const& h):
      prod_(h.prod_.object(),false), id_(h.id_) {}
      
      OrphanHandle(ROOT::Reflex::Object const& prod, ProductID const& id):
      prod_(prod,false), id_(id) {}
      
      //~OrphanHandle();
      
      void swap(OrphanHandle<GenericObjectOwner>& other){
         prod_.swap(other.prod_);
         std::swap(id_,other.id_);
      }
      
      
      OrphanHandle<GenericObjectOwner>& operator=(OrphanHandle<GenericObjectOwner> const& rhs)
      {
         OrphanHandle<GenericObjectOwner> temp(rhs);
         swap(temp);
	 return *this;
      }
      
      bool isValid() const {return 0 !=prod_.object().Address();}
         
      GenericObjectOwner const* product() const {return &prod_;}
      GenericObjectOwner const* operator->() const {return product();}
      GenericObjectOwner const& operator*() const {return prod_;}
      
      ProductID id() const {return id_;}
         
         
      private:
         GenericObjectOwner prod_;
         ProductID id_;
      };
      
   template<>
   OrphanHandle<GenericObjectOwner> 
   Event::put<GenericObjectOwner>(std::auto_ptr<GenericObjectOwner> product, std::string const& productInstanceName)
   {
      if (product.get() == 0) {                // null pointer is illegal
         throw edm::Exception(edm::errors::NullPointerError)
         << "Event::put: A null auto_ptr was passed to 'put'.\n"
         << "The pointer is of type " << "GenericObjectOwner" << ".\n"
         << "The specified productInstanceName was '" << productInstanceName << "'.\n";
      }
      
      // The following will call post_insert if T has such a function,
      // and do nothing if T has no such function.
      /*
      typename boost::mpl::if_c<detail::has_postinsert<PROD>::value, 
      DoPostInsert<PROD>, 
      DoNotPostInsert<PROD> >::type maybe_inserter;
      maybe_inserter(product.get());
      */
      ConstBranchDescription const& desc =
      getBranchDescription(TypeID(product->object().TypeOf().TypeInfo()), productInstanceName);
      
      ROOT::Reflex::Type const wrapperType=ROOT::Reflex::Type::ByName(wrappedClassName(desc.fullClassName()));
      if(wrapperType == ROOT::Reflex::Type() ) {
         throw edm::Exception(errors::DictionaryNotFound, "NoWrapperDictionary")
         << "Event::put: the class type '" << desc.fullClassName()
         << "' was passed to put but the Reflex dictionary for the required class '"
         << wrappedClassName(desc.fullClassName()) << "' could not be found./n"
         << "Please change the C++ package which contains the description of '" << desc.fullClassName()
         << "' so that the required class also has a dictionary autogenerated.";
      }
      std::vector<void*> args;
      args.reserve(1);
      args.push_back(product->object().Address());
      //generate the signature of the function we want to call
      std::string s("void(");
      s+=desc.fullClassName();
      s+="*)";
      ROOT::Reflex::Type ptrT = ROOT::Reflex::Type::ByName(s);
      ROOT::Reflex::Object oWrapper(wrapperType.Construct(ptrT,args));
      //ownership was transfered to the wrapper
      product.release();

      static ROOT::Reflex::Type s_edproductType( ROOT::Reflex::Type::ByTypeInfo(typeid(EDProduct)));
      EDProduct *wp(reinterpret_cast<EDProduct*>(oWrapper.CastObject(s_edproductType).Address()));
      putProducts().push_back(std::make_pair(wp, &desc));
      
      // product.release(); // The object has been copied into the Wrapper.
      // The old copy must be deleted, so we cannot release ownership.
      
      return(OrphanHandle<GenericObjectOwner>(oWrapper.Get("obj"), ProductID(currentProcessIndex(), desc.productIndexToAssign())));
   }
   
}
#endif
