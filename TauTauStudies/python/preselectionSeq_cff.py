import FWCore.ParameterSet.Config as cms

arturMuon = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
    cut = cms.string("pt > 5 && abs(eta) < 2.4 && isGlobalMuon"),
    filter = cms.bool(False)
    )
arturTau = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausTriggerMatch"),
    cut = cms.string("pt > 17 && abs(eta) < 2.4 && tauID('decayModeFinding') > 0.5"),
    filter = cms.bool(False)
    )
atLeastOneArturMuTau = cms.EDProducer("CandViewShallowCloneCombiner",
                                              decay = cms.string("arturMuon arturTau"),
                                              cut   = cms.string("sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+(daughter(0).phi-daughter(1).phi)*(daughter(0).phi-daughter(1).phi))>0.3"),
                                              checkCharge = cms.bool(False)
                                              )
atLeastOneArturMuTauCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneArturMuTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
atLeastOneArturMuon = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
    cut = cms.string("pt > 15"),
    filter = cms.bool(True)
    )

hlt = cms.EDFilter("HLTHighLevel",
                   TriggerResultsTag = cms.InputTag("TriggerResults","","REDIGI311X"),
                   HLTPaths = cms.vstring('HLT_Mu11_PFTau15_v2'),
                   eventSetupPathsKey = cms.string(''),
                   andOr = cms.bool(True),  
                   throw = cms.bool(False)
                   )


arturPreselection = cms.Sequence(
    hlt + atLeastOneArturMuon + (arturMuon + arturTau)*atLeastOneArturMuTau*atLeastOneArturMuTauCounter
    )
