/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
#include <string>
#include "DataFormats/Provenance/interface/ProductStatus.h"
#include "DataFormats/Common/interface/BasicHandle.h"
#include "FWCore/Framework/src/Group.h"
#include "FWCore/Utilities/interface/ReflexTools.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using ROOT::Reflex::Type;
using ROOT::Reflex::TypeTemplate;

namespace edm {

  Group::Group() :
    product_(),
    provenance_(),
    dropped_(false),
    onDemand_(false) {}


  Group::Group(std::auto_ptr<Provenance> prov) :
    product_(),
    provenance_(prov.release()),
    dropped_(!provenance_->product().present()),
    onDemand_(false) {
  }

  Group::Group(ConstBranchDescription const& bd, bool demand) :
    product_(),
    provenance_(new Provenance(bd)),
    dropped_(!bd.present()),
    onDemand_(demand) {
  }

  Group::Group(ConstBranchDescription const& bd) :
    product_(),
    provenance_(new Provenance(bd)),
    dropped_(!bd.present()),
    onDemand_(false) {
  }

  Group::Group(std::auto_ptr<EDProduct> edp,
	       std::auto_ptr<Provenance> prov) :
    product_(edp.release()),
    provenance_(prov.release()),
    dropped_(false),
    onDemand_(false) {
  }

  Group::~Group() {
  }

  ProductStatus
  Group::status() const {
    // for backward compatibility
    if (product_) {
      product_->isPresent() ? provenance_->setPresent() : provenance_->setNotPresent();
    }
    return provenance_->productStatus();
  }

  bool
  Group::onDemand() const {
    return onDemand_;
  }

  bool 
  Group::productUnavailable() const { 
    if (onDemand()) return false;
    if (dropped_) return true;
    if (productstatus::unknown(status())) return false;
    return not productstatus::present(status());

  }

  bool 
  Group::provenanceAvailable() const { 
    if (onDemand()) return false;
    return true;    
  }

  void 
  Group::setProduct(std::auto_ptr<EDProduct> prod) const {
    assert (product() == 0);
    product_ = boost::shared_ptr<EDProduct>(prod.release());  // Group takes ownership
  }
  
  void 
  Group::setProvenance(std::auto_ptr<BranchEntryInfo> beiPtr) const {
    assert (branchEntryInfoPtr() == 0);
    provenance_->setBranchEntryInfo(boost::shared_ptr<BranchEntryInfo>(beiPtr.release()));  // Group takes ownership
  }

  void  
  Group::swap(Group& other) {
    std::swap(product_, other.product_);
    std::swap(provenance_, other.provenance_);
    std::swap(dropped_, other.dropped_);
    std::swap(onDemand_, other.onDemand_);
  }

  void
  Group::replace(Group& g) {
    this->swap(g);
  }

  Type
  Group::productType() const
  {
    return Type::ByTypeInfo(typeid(*product()));
  }

  bool
  Group::isMatchingSequence(Type const& wantedElementType) const
  {
    Type value_type;
    bool is_sequence = is_sequence_wrapper(productType(), value_type);
        
    // If our product is not a sequence, we can't match...
    if (!is_sequence) return false;

    Type elementType = value_type; // this is not true for RefVector...

    TypeTemplate valueTypeTemplate = value_type.TemplateFamily();

    return 
      is_sequence 
      ? (elementType==wantedElementType || 
	 elementType.HasBase(wantedElementType))
      : false;      
  }

  BasicHandle
  Group::makeBasicHandle() const
  {
    return BasicHandle(product_.get(), provenance_.get());
  }

  void
  Group::write(std::ostream& os) const 
  {
    // This is grossly inadequate. It is also not critical for the
    // first pass.
    os << std::string("Group for product with ID: ")
       << provenance_->productID();
  }

  void
  Group::mergeGroup(Group * newGroup) {

    if (status() != newGroup->status()) {
      throw edm::Exception(edm::errors::Unknown, "Merging")
        << "Group::mergeGroup(), Trying to merge two run products or two lumi products.\n"
        << "The products have different creation status's.\n"
        << "For example \"present\" and \"notCreated\"\n"
        << "The Framework does not currently support this and probably\n"
        << "should not support this case.\n"
        << "Likely a problem exists in the producer that created these\n"
        << "products.  If this problem cannot be reasonably resolved by\n"
        << "fixing the producer module, then contact the Framework development\n"
        << "group with details so we can discuss whether and how to support this\n"
        << "use case.\n"
        << "className = " << provenance()->className() << "\n"
        << "moduleLabel = " << moduleLabel() << "\n"
        << "instance = " << productInstanceName() << "\n"
        << "process = " << processName() << "\n";
    }

    // Don't support specifying multple modules.  So just null the description
    // if they are different.    

    if (provenance_->moduleDescriptionID() != newGroup->provenance_->moduleDescriptionID()) {
	// Not just yet
      // provenance_->moduleDescriptionID_ = ModuleDescriptionID();
    }

    if (!productUnavailable() && !newGroup->productUnavailable()) {

      if (product_->isMergeable()) {
        product_->mergeProduct(newGroup->product_.get());
      }
      else if (product_->hasIsProductEqual()) {

        if (!product_->isProductEqual(newGroup->product_.get())) {
          edm::LogWarning  ("RunLumiMerging") 
            << "Group::mergeGroup\n" 
            << "Two run/lumi products for the same run/lumi which should be equal are not\n"
            << "Using the first, ignoring the second\n"
            << "className = " << provenance()->className() << "\n"
            << "moduleLabel = " << moduleLabel() << "\n"
            << "instance = " << productInstanceName() << "\n"
            << "process = " << processName() << "\n";
        }
      }
      else {
        edm::LogWarning  ("RunLumiMerging") 
          << "Group::mergeGroup\n" 
          << "Run/lumi product has neither a mergeProduct nor isProductEqual function\n"
          << "Using the first, ignoring the second in merge\n"
          << "className = " << provenance()->className() << "\n"
          << "moduleLabel = " << moduleLabel() << "\n"
          << "instance = " << productInstanceName() << "\n"
          << "process = " << processName() << "\n";
      }
    }
  }
}
