import FWCore.ParameterSet.Config as cms

import os

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    
    ##maxEvents = cms.int32(100000),
    maxEvents = cms.int32(-1),
    
    outputEvery = cms.uint32(100000)
)

process.fwliteOutput = cms.PSet(
    fileName = cms.string('addBackgroundZTT.root')
)

process.addBackgroundZTT = cms.PSet(

    regions = cms.vstring(
        "OSiso1_iso2",
        "SSiso1_iso2"
    ),

    processZTT = cms.string("ZTT"),
    processZTTmc = cms.string("ZTTmc"),
    processZTT_Embedded = cms.string("ZTT_Embedded"),
    processTT_Embedded = cms.string("TT_Embedded"),

    categories = cms.vstring(
        "inclusive",
        "2bM",
        "2bM_nonresonant",
        "2bM_resonant",
        "2bL",
        "2bL_nonresonant",
        "2bL_resonant",
        "1b1j",
        "1b1j_nonresonant",
        "1b1j_resonant",
        "2j",
        "2j_nonresonant",
        "2j_resonant"
    ),
    category_inclusive = cms.string("inclusive"),

    tauPtBins = cms.vstring(
        "tau1PtGt45tau2PtGt45",
        "tau1PtGt45tau2Pt45to60",
        "tau1PtGt45tau2Pt60to80",
        "tau1PtGt45tau2PtGt80",
        "tau1PtGt45tau2PtGt60"
    ),
    tauPtBin_inclusive = cms.string("tau1PtGt45tau2PtGt45"),

    sysShifts = cms.vstring(
        "CMS_scale_t_tautau_8TeVUp",
        "CMS_scale_t_tautau_8TeVDown",
        "CMS_eff_t_mssmHigh_tautau_8TeVUp",
        "CMS_eff_t_mssmHigh_tautau_8TeVDown"
    )
)
