#ifndef LLRAnalysis_HadTauStudies_RunLumiSectionEventNumberSelector_h
#define LLRAnalysis_HadTauStudies_RunLumiSectionEventNumberSelector_h

/** \class RunLumiSectionEventNumberSelector
 *
 * Select events based on run + luminosity section + event number pairs
 * written (a three columns separated by white-space character) into an ASCII file
 * 
 * \author Christian Veelken, LLR
 *
 */

#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <TObject.h>

#include <string>
#include <map>

class RunLumiSectionEventNumberSelector 
{
 public:
  // constructor 
  explicit RunLumiSectionEventNumberSelector(const edm::ParameterSet&);
    
  // destructor
  virtual ~RunLumiSectionEventNumberSelector();
    
  bool operator()(ULong_t, ULong_t, ULong_t) const;

 private:

//--- read ASCII file containing run and event numbers
  void readInputFile();
  
  std::string inputFileName_;

  std::string separator_;

  typedef std::set<ULong_t> eventNumberSet;
  typedef std::map<ULong_t, eventNumberSet> lumiSectionEventNumberMap;
  std::map<ULong_t, lumiSectionEventNumberMap> runLumiSectionEventNumbers_;
  
  typedef std::map<ULong_t, int> matchedEventNumbersMap;
  typedef std::map<ULong_t, matchedEventNumbersMap> matchedLumiSectionEventNumberMap;
  mutable std::map<ULong_t, matchedLumiSectionEventNumberMap> matchedRunLumiSectionEventNumbers_;

  mutable long numEventsProcessed_;
  mutable long numEventsToBeSelected_;
  mutable long numEventsSelected_;
};

#endif   
