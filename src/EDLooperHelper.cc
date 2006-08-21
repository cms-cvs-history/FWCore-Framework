// -*- C++ -*-
//
// Package:     <FWCore/Framework>
// Module:      EDLooperHelper
// 
// Description: <one line class summary>
//
// Implementation:
//     <Notes on implementation>
//
// Author:      Valentin Kuznetsov
// Created:     Wed Jul 12 11:38:09 EDT 2006
// $Id$
//
// Revision history
//
// $Log$

// user include files
#include "FWCore/Framework/interface/EDLooperHelper.h"
#include "FWCore/Framework/interface/EventSetupProvider.h"
#include "FWCore/Framework/interface/EventSetupRecordKey.h"

namespace edm {

//
// constants, enums and typedefs
//

static const char* const kFacilityString = "FWCore.Framework.EDLooperHelper" ;

// ---- cvs-based strings (Id and Tag with which file was checked out)
static const char* const kIdString  = "$Id: skeleton.cc,v 1.7 2004/02/12 02:14:38 pcs Exp $";
static const char* const kTagString = "$Name:  $";

//
// static data member definitions
//

//
// constructors and destructor
//
//EDLooperHelper::EDLooperHelper()
//{
//}

// EDLooperHelper::EDLooperHelper( const EDLooperHelper& rhs )
// {
//    // do actual copying here; if you implemented
//    // operator= correctly, you may be able to use just say      
//    *this = rhs;
// }

EDLooperHelper::~EDLooperHelper()
{
}

//
// assignment operators
//
// const EDLooperHelper& EDLooperHelper::operator=( const EDLooperHelper& rhs )
// {
//   if( this != &rhs ) {
//      // do actual copying here, plus:
//      // "SuperClass"::operator=( rhs );
//   }
//
//   return *this;
// }

//
// member functions
//
EventHelperDescription
EDLooperHelper::runOnce(unsigned long numberToProcess)
{
    return eventProcessor_->runOnce(numberToProcess);
}

void
EDLooperHelper::rewind(const std::set<edm::eventsetup::EventSetupRecordKey>& keys)
{
   std::for_each(keys.begin(),keys.end(), 
        boost::bind(&eventsetup::EventSetupProvider::resetRecordPlusDependentRecords,
                     eventProcessor_->esp_.get(), _1));
   return eventProcessor_->rewind();
}

//
// const member functions
//

//
// static member functions
//

} // end of namespace
