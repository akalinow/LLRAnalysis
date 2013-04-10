import FWCore.ParameterSet.Config as cms

pickedPatElectron = cms.EDProducer(
    "SinglePatElectronPicker",
    src = cms.InputTag("selectedPatElectrons"),
    itemNumber = cms.uint32(0),
    verbose = cms.untracked.bool(False)
    )

pickedPatMuon = cms.EDProducer(
    "SinglePatMuonPicker",
    src = cms.InputTag("selectedPatMuons"),
    itemNumber = cms.uint32(0),
    verbose = cms.untracked.bool(False)
    )

pickedPatTau = cms.EDProducer(
    "SinglePatTauPicker",
    src = cms.InputTag("selectedPatTaus"),
    itemNumber = cms.uint32(0),
    verbose = cms.untracked.bool(False)
    )
