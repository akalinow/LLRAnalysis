import FWCore.ParameterSet.Config as cms

correctedCaloMEtNoHF = cms.EDProducer("CorrectedCaloMETforEmbeddedProducer",
    srcMEt = cms.InputTag('metNoHF'),
    srcTauJets = cms.VInputTag('genTauMatchedCaloJet'),
    residualCorrLabel = cms.string("ak5CaloResidual"), # apply data/MC residual correction
    ##residualCorrLabel = cms.string(""), # do not apply data/MC residual correction                                    
    residualCorrEtaMax = cms.double(3.0),
    extraCorrFactor = cms.double(1.05),
    isMC = cms.bool(True), # CV: use 'MC' setting to calibrate simulated tau decay products in Embedded samples
)

uncorrectedL1ETM = cms.EDProducer("L1ExtraMEtToCaloMEtConverterForTauTauStudies",
    src = cms.InputTag('l1extraParticles', 'MET')                  
)

correctedL1ETM = cms.EDProducer("CorrectedCaloMETforEmbeddedProducer",
    srcMEt = cms.InputTag('uncorrectedL1ETM'),
    srcTauJets = cms.VInputTag('genTauMatchedCaloJet'),
    residualCorrLabel = cms.string("ak5CaloResidual"), # apply data/MC residual correction
    ##residualCorrLabel = cms.string(""), # do not apply data/MC residual correction                                    
    residualCorrEtaMax = cms.double(3.0),
    extraCorrFactor = cms.double(1.125),
    isMC = cms.bool(True), # CV: use 'MC' setting to calibrate simulated tau decay products in Embedded samples
)

calibrateCaloMETandL1ETMforEmbedded = cms.Sequence(correctedCaloMEtNoHF * uncorrectedL1ETM * correctedL1ETM)
