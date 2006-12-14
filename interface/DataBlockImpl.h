#ifndef Framework_DataBlockImpl_h
#define Framework_DataBlockImpl_h

/*----------------------------------------------------------------------
  
DataBlockImpl: This is the implementation of the classes
responsible for management of EDProducts. It is not seen by reconstruction code;

The major internal component of the DataBlockImpl is the Group, which
contains an EDProduct and its associated Provenance, along with
ancillary transient information regarding the two. Groups are handled
through shared pointers.

The DataBlockImpl returns BasicHandle, rather than a shared
pointer to a Group, when queried.

$Id: DataBlockImpl.h,v 1.4 2006/12/01 03:29:52 wmtan Exp $

----------------------------------------------------------------------*/
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "boost/shared_ptr.hpp"

#include "DataFormats/Common/interface/BranchKey.h"
#include "DataFormats/Common/interface/ProductID.h"
#include "DataFormats/Common/interface/EDProduct.h"
#include "DataFormats/Common/interface/EDProductGetter.h"
#include "DataFormats/Common/interface/ProcessHistory.h"
#include "DataFormats/Common/interface/ProcessHistoryID.h"
#include "FWCore/Framework/interface/BasicHandle.h"
#include "FWCore/Framework/interface/NoDelayedReader.h"
#include "FWCore/Framework/interface/DelayedReader.h"
#include "FWCore/Framework/interface/SelectorBase.h"

#include "FWCore/Framework/src/Group.h"
#include "FWCore/Framework/interface/TypeID.h"

namespace edm {
  class ProductRegistry;
  class UnscheduledHandler;
  class DataBlockImpl : public EDProductGetter {
  public:
    typedef std::vector<boost::shared_ptr<Group> > GroupVec;
    typedef GroupVec::const_iterator               const_iterator;
    typedef ProcessHistory::const_iterator         ProcessNameConstIterator;
    typedef boost::shared_ptr<const Group>         SharedConstGroupPtr;
    typedef std::vector<BasicHandle>               BasicHandleVec;

    DataBlockImpl(ProductRegistry const& reg,
                   ProcessHistoryID const& hist = ProcessHistoryID(),
                   boost::shared_ptr<DelayedReader> rtrv = boost::shared_ptr<DelayedReader>(new NoDelayedReader));

    virtual ~DataBlockImpl();
    size_t  size() const { return groups_.size(); }

    EDProductGetter const* prodGetter() const {return this;}

    DataBlockImpl const& groupGetter() const {return *this;}

    DataBlockImpl & groupGetter() {return *this;}

    // Return the number of EDProducts contained.
    unsigned long numEDProducts() const;
    
    void put(std::auto_ptr<EDProduct> edp,
	     std::auto_ptr<Provenance> prov);

    SharedConstGroupPtr const getGroup(ProductID const& oid, bool resolve = true) const;

    BasicHandle  get(ProductID const& oid) const;

    BasicHandle  getBySelector(TypeID const& tid, SelectorBase const& s) const;

    BasicHandle  getByLabel(TypeID const& tid,
			    std::string const& label,
                            std::string const& productInstanceName) const;

    BasicHandle  getByLabel(TypeID const& tid,
			    std::string const& label,
			    std::string const& productInstanceName,
			    std::string const& processName) const;

    void getMany(TypeID const& tid, 
		 SelectorBase const&,
		 BasicHandleVec& results) const;

    BasicHandle  getByType(TypeID const& tid) const;

    void getManyByType(TypeID const& tid, 
		 BasicHandleVec& results) const;

    Provenance const&
    getProvenance(ProductID const& oid) const;

    void
    getAllProvenance(std::vector<Provenance const *> & provenances) const;

    // ----- access to all products

    const_iterator begin() const { return groups_.begin(); }
    const_iterator end() const { return groups_.end(); }

    ProcessNameConstIterator beginProcess() const {
      return processHistory().begin();
    }

    ProcessNameConstIterator endProcess() const {
      return processHistory().end();
    }

    ProcessHistory const& processHistory() const;    

    ProcessHistoryID const& processHistoryID() const {
      return processHistoryID_;   
    }

    // ----- manipulation of provenance


    // ----- Add a new Group
    // *this takes ownership of the Group, which in turn owns its
    // data.
    void addGroup(std::auto_ptr<Group> g);

    // ----- Mark this DataBlockImpl as having been updated in the
    // given Process.
    void addToProcessHistory(ProcessConfiguration const& processConfiguration);

    ProductRegistry const& productRegistry() const {return *preg_;}

    boost::shared_ptr<DelayedReader> store() const {return store_;}

    virtual EDProduct const* getIt(ProductID const& oid) const;

  private:

    virtual bool unscheduledFill(Group const& group) const = 0;

    virtual bool fillAndMatchSelector(Provenance& prov, SelectorBase const& selector) const = 0;

    typedef boost::shared_ptr<Group> SharedGroupPtr;

    SharedConstGroupPtr const getInactiveGroup(ProductID const& oid) const;

    // Make my DelayedReader get the EDProduct for a Group.  The Group is
    // a cache, and so can be modified through the const reference.
    // We do not change the *number* of groups through this call, and so
    // *this is const.
    void resolve_(Group const& g, bool unconditional = false) const;

    ProcessHistoryID processHistoryID_;

    boost::shared_ptr<ProcessHistory> processHistoryPtr_;

    // A vector of active groups.
    GroupVec groups_; // products and provenances are persistent

    // users need to vary the info in the BranchKey object
    // to store the output of different code versions for the
    // same configured module (e.g. change processName_)

    // indices are to product/provenance slot
    typedef std::map<BranchKey, int> BranchDict;
    BranchDict branchDict_; // 1->1

    typedef std::map<ProductID, int> ProductDict;
    ProductDict productDict_; // 1->1

    typedef std::map<std::string, std::vector<int> > TypeDict;
    TypeDict typeDict_; // 1->many

    // A vector of inactive groups (product not present or inaccessible).
    GroupVec inactiveGroups_; // products and provenances are persistent

    // indices are to product/provenance slot
    BranchDict inactiveBranchDict_; // 1->1
    ProductDict inactiveProductDict_; // 1->1
    TypeDict inactiveTypeDict_; // 1->many

    // it is probably straightforward to load the BranchKey
    // dictionary above with information from the input source - 
    // mostly because this is information from provenance.
    // The product provanance are likewise easily filled.
    // The typeid index is more of a problem. Here
    // the I/O subsystem will need to take the friendly product
    // name string from provenance and get back a TypeID object.
    // Getting the products loaded (from the file) is another
    // issue. Here we may need some sort of hook into the I/O
    // system unless we want to preload all products (probably
    // not a good idea).
    // At MiniBooNE, this products object was directly part of
    // ROOT and the "gets" caused things to load properly - and
    // this is where the reservation for an object came into
    // the picture i.e. the "maker" function of the event.
    // should eventprincipal be pure interface?
    // should ROOT just be present here?
    // What goes into the event header(s)? Which need to be persistent?

    // Pointer to the product registry. There is one entry in the registry
    // for each EDProduct in the event.
    ProductRegistry const* preg_;

    // Pointer to the 'source' that will be used to obtain EDProducts
    // from the persistent store.
    boost::shared_ptr<DelayedReader> store_;
  };
}
#endif
