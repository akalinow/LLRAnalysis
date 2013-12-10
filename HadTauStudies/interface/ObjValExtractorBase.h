#ifndef LLRAnalysis_HadTauStudies_ObjValExtractorBase_h
#define LLRAnalysis_HadTauStudies_ObjValExtractorBase_h

/** \class ObjValExtractorBase
 *
 * Base-class for extracting scalar values from PAT objects
 * (derived classes are auxiliary classes used for binning and Ntuple filling purposes)
 * 
 * \author Christian Veelken
 *
 */

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class ObjValExtractorBase
{
 public:
  // constructor 
  explicit ObjValExtractorBase() {}
  
  // destructor
  virtual ~ObjValExtractorBase() {}

  // method for extracting scalar value
  virtual double operator()(const edm::Event&) const = 0;
};

#include "FWCore/PluginManager/interface/PluginFactory.h"

typedef edmplugin::PluginFactory<ObjValExtractorBase* (const edm::ParameterSet&)> ObjValExtractorPluginFactory;

#endif  

