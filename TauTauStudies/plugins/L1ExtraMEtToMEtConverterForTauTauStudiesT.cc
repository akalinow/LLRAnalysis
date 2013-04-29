#include "LLRAnalysis/TauTauStudies/plugins/L1ExtraMEtToMEtConverterForTauTauStudiesT.h"

#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/PatCandidates/interface/MET.h"

template <>
reco::CaloMET L1ExtraMEtToMEtConverterForTauTauStudiesT<reco::CaloMET>::createMEt(double sumEt, const reco::Candidate::LorentzVector& p4, const reco::Candidate::Point& vertex)
{    
  SpecificCaloMETData caloMEtData;
  reco::CaloMET caloMEt(caloMEtData, sumEt, p4, vertex);
  return caloMEt;
}

typedef L1ExtraMEtToMEtConverterForTauTauStudiesT<reco::CaloMET> L1ExtraMEtToCaloMEtConverterForTauTauStudies;

template <>
pat::MET L1ExtraMEtToMEtConverterForTauTauStudiesT<pat::MET>::createMEt(double sumEt, const reco::Candidate::LorentzVector& p4, const reco::Candidate::Point& vertex)
{    
  pat::MET patMEt(reco::MET(sumEt, p4, vertex));
  return patMEt;
}

typedef L1ExtraMEtToMEtConverterForTauTauStudiesT<pat::MET> L1ExtraMEtToPATMEtConverterForTauTauStudies;

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(L1ExtraMEtToCaloMEtConverterForTauTauStudies);
DEFINE_FWK_MODULE(L1ExtraMEtToPATMEtConverterForTauTauStudies);
