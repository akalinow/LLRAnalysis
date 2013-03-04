import FWCore.ParameterSet.Config as cms

genTauMatchedCaloJet = cms.EDProducer(
    "GenTauMatchedCaloJet",
    jetTag = cms.InputTag("ak5CaloJets"),
    genTauJetTag = cms.InputTag("tauGenJetsSelectorAllHadrons"),
    deltaRMax = cms.double(0.3),
    verbose = cms.untracked.bool(False)
)
