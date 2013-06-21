
import FWCore.ParameterSet.Config as cms

convertedL1Jets = cms.EDProducer(
    "L1ExtraJetToCaloJetConverterForTauTauStudies",
    srcL1Jets = cms.VInputTag("l1extraParticles:Tau",
                              "l1extraParticles:Central")
    )
