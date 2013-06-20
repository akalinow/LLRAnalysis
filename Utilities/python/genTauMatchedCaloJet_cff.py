import FWCore.ParameterSet.Config as cms

from PhysicsTools.JetMCAlgos.TauGenJets_cfi import *
from PhysicsTools.JetMCAlgos.TauGenJetsDecayModeSelectorAllHadrons_cfi import *
from LLRAnalysis.Utilities.genTauMatchedCaloJet_cfi import *

tauGenJetsSelectorElectron = tauGenJetsSelectorAllHadrons.clone()
tauGenJetsSelectorElectron.select=['electron']
tauGenJetsSelectorMuon = tauGenJetsSelectorAllHadrons.clone()
tauGenJetsSelectorMuon.select=['muon']
genTauMatchedCaloJetElec = genTauMatchedCaloJet.clone()
genTauMatchedCaloJetElec.candidateTag = "tauGenJetsSelectorElectron"
genTauMatchedCaloJetMu = genTauMatchedCaloJet.clone()
genTauMatchedCaloJetMu.candidateTag = "tauGenJetsSelectorMuon"

makeTauMatchedCaloJets = cms.Sequence(
    tauGenJets +
    tauGenJetsSelectorAllHadrons + genTauMatchedCaloJet +
    tauGenJetsSelectorElectron + genTauMatchedCaloJetElec +
    tauGenJetsSelectorMuon + genTauMatchedCaloJetMu
)
