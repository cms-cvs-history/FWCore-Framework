#ifndef FWCore_Framework_FileBlock_h
#define FWCore_Framework_FileBlock_h

/*----------------------------------------------------------------------

FileBlock: Properties of an input file.

$Id: FileBlock.h,v 1.4 2007/11/30 07:06:30 wmtan Exp $

----------------------------------------------------------------------*/

#include "DataFormats/Provenance/interface/FileFormatVersion.h"
class TTree;
#include <map>
#include <string>
#include <vector>

namespace edm {
  class BranchDescription;
  class FileBlock {
  public:
    FileBlock() : fileFormatVersion_(),
	tree_(0), metaTree_(0),
	lumiTree_(0), lumiMetaTree_(0),
	runTree_(0), runMetaTree_(0),
	fastClonable_(false), sortedNewBranchNames_(), oldBranchNames_() {}
    FileBlock(FileFormatVersion const& version,
	TTree const* ev, TTree const* meta,
	TTree const* lumi, TTree const* lumiMeta,
	TTree const* run, TTree const* runMeta,
	bool fastClone,
	std::vector<std::string> const& newNames,
	std::vector<std::string> const& oldNames) :
	  fileFormatVersion_(version),
	  tree_(const_cast<TTree *>(ev)), 
	  metaTree_(const_cast<TTree *>(meta)), 
	  lumiTree_(const_cast<TTree *>(lumi)), 
	  lumiMetaTree_(const_cast<TTree *>(lumiMeta)), 
	  runTree_(const_cast<TTree *>(run)), 
	  runMetaTree_(const_cast<TTree *>(runMeta)), 
	  fastClonable_(fastClone), 
	  sortedNewBranchNames_(newNames),
	  oldBranchNames_(oldNames) {}
    ~FileBlock() {}

    FileFormatVersion const& fileFormatVersion() const {return fileFormatVersion_;}
    TTree * const tree() const {return tree_;}
    TTree * const metaTree() const {return metaTree_;}
    TTree * const lumiTree() const {return lumiTree_;}
    TTree * const lumiMetaTree() const {return lumiMetaTree_;}
    TTree * const runTree() const {return runTree_;}
    TTree * const runMetaTree() const {return runMetaTree_;}
    bool fastClonable() const {return fastClonable_;}
    std::vector<std::string> const& sortedNewBranchNames() const {return sortedNewBranchNames_;}
    std::vector<std::string> const& oldBranchNames() const {return oldBranchNames_;}

  private:
    FileFormatVersion fileFormatVersion_;
    // We use bare pointers because ROOT owns these.
    TTree * tree_;
    TTree * metaTree_;
    TTree * lumiTree_;
    TTree * lumiMetaTree_;
    TTree * runTree_;
    TTree * runMetaTree_;
    bool fastClonable_;
    std::vector<std::string> sortedNewBranchNames_;
    std::vector<std::string> oldBranchNames_;
  };
}
#endif
