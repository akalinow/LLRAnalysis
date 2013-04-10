#include "LLRAnalysis/Utilities/interface/SingleCandidatePicker.h"

#include "DataFormats/Candidate/interface/Candidate.h" 
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

//typedef SingleCandidatePicker<reco::Candidate> SingleRecoCandidatePicker;
typedef SingleCandidatePicker<pat::Electron>   SinglePatElectronPicker; 
typedef SingleCandidatePicker<pat::Muon>       SinglePatMuonPicker; 
typedef SingleCandidatePicker<pat::Tau>        SinglePatTauPicker; 

//////////////////////////
#include "FWCore/Framework/interface/MakerMacros.h"

//DEFINE_FWK_MODULE(SingleRecoCandidatePicker);
DEFINE_FWK_MODULE(SinglePatElectronPicker);
DEFINE_FWK_MODULE(SinglePatMuonPicker);
DEFINE_FWK_MODULE(SinglePatTauPicker);

