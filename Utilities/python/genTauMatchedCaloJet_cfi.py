import FWCore.ParameterSet.Config as cms

genTauMatchedCaloJet = cms.EDProducer(
    "CandidateMatchedCaloJet",
    jetTag = cms.InputTag("ak5CaloJets"),
    candidateTag = cms.InputTag("tauGenJetsSelectorAllHadrons"),
    deltaRMax = cms.double(0.3),
    verbose = cms.untracked.bool(False)
)
