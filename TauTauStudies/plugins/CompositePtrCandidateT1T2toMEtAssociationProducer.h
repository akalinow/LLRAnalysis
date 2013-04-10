#ifndef LLRAnalysis_TauTauStudies_CompositePtrCandidateT1T2toMEtAssociationProducer_h
#define LLRAnalysis_TauTauStudies_CompositePtrCandidateT1T2toMEtAssociationProducer_h

/** \class CompositePtrCandidateT1T2toMEtAssociationProducer
 *
 * Produce collection of MET objects, diTau objects
 * and association between MET and diTau collections
 * 
 * \author Christian Veelken, LLR
 *
 * \version $Revision: 1.1 $
 *
 * $Id: CompositePtrCandidateT1T2toMEtAssociationProducer.h,v 1.1 2011/08/17 13:58:06 veelken Exp $
 *
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEt.h"
#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEtFwd.h"

#include "DataFormats/Common/interface/AssociationVector.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/Ref.h"

template <typename T1, typename T2>
class CompositePtrCandidateT1T2toMEtAssociationProducer : public edm::EDProducer 
{
  typedef CompositePtrCandidateT1T2MEt<T1,T2> CompositePtrCandidate;
  typedef std::vector<CompositePtrCandidate> CompositePtrCandidateCollection;
  typedef edm::RefProd<CompositePtrCandidateCollection> CompositePtrCandidateRefProd;
  typedef std::vector<int> vint;
  typedef edm::AssociationVector<CompositePtrCandidateRefProd, vint> diTauToMEtAssociation;

 public:
  explicit CompositePtrCandidateT1T2toMEtAssociationProducer(const edm::ParameterSet&);
  ~CompositePtrCandidateT1T2toMEtAssociationProducer();

  void produce(edm::Event&, const edm::EventSetup&);

 private:

  std::string moduleLabel_;

  struct inputEntryType
  {
    edm::InputTag srcDiTau_;
    edm::InputTag srcMET_;
  };
  std::vector<inputEntryType> inputs_;
};

#endif


