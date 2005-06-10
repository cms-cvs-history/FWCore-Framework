// $Id: GroupSelector.cc,v 1.1 2005/06/08 21:15:55 wmtan Exp $
#include "FWCore/CoreFramework/interface/EventPrincipal.h"
#include "FWCore/CoreFramework/interface/EventProvenance.h"
#include "FWCore/CoreFramework/interface/GroupSelector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.icc"
#include <vector>

namespace edm {
  GroupSelector::GroupSelector(ParameterSet const& pset) :
    selectAllGroups_(false),
    select_() {
    selectProducts(pset);
  }

  bool GroupSelector::selected(std::string const& label) const {
      SelectMap::const_iterator s = select_.find(label);
      bool const sel = selectAllGroups_ ?
	s == select_.end() || s->second : // select all branches, except those marked "select == false"
	s != select_.end() && s->second;  // select only branches marked "select == true"
      return sel;
  }

  void GroupSelector::selectProducts(ParameterSet const& pset) {
    std::string allString("*");
    std::vector<std::string> all;
    all.push_back(allString);
    std::vector<std::string> none;

    std::vector<std::string> keep = pset.getUntracked("productsSelected", all);

    for(std::vector<std::string>::const_iterator it = keep.begin(); it != keep.end(); ++it) {
      std::string const& label = *it;
      if (allString == label) {
        selectAllGroups_ = true;
        select_.clear();
        break;
      } else {
        select_[ label ] = true;
      }
    }

    std::vector<std::string> skip = pset.getUntracked("productsExcluded", none);

    for(std::vector<std::string>::const_iterator it = skip.begin(); it != skip.end(); ++it) {
      std::string const& label = *it;
      if (allString == label) {
        selectAllGroups_ = false;
        select_.clear();
        break;
      } else {
        select_[ label ] = false;
      }
    }
  }
}
