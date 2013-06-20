import FWCore.ParameterSet.Config as cms

from LLRAnalysis.Utilities.genTauMatchedCaloJet_cfi import *

selectedPatTausForCaloJetMatching = cms.EDFilter("PATTauSelector",
                                                 src = cms.InputTag("selectedPatTaus"),
                                                 cut = cms.string("")
                                                 )

patTauMatchedCaloJet = genTauMatchedCaloJet.clone()
patTauMatchedCaloJet.candidateTag = "selectedPatTausForCaloJetMatching"

makeRecoTauMatchedCaloJets = cms.Sequence(
    selectedPatTausForCaloJetMatching + patTauMatchedCaloJet
)
