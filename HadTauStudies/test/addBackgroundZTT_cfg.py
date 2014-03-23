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
        "OSisoTightBtag",
        "OSisoLooseBtag",
        "SSisoTightBtag",
        "SSisoLooseBtag"
    ),

    processZTT = cms.string("ZTT"),
    processZTTmc = cms.string("ZTTmc"),
    processZTT_Embedded = cms.string("ZTT_Embedded"),
    processTT_Embedded = cms.string("TT_Embedded"),

    categories = cms.vstring("inclusive", "nobtag", "btag"),
    category_inclusive = cms.string("inclusive"),

    tauPtBins = cms.vstring(
        "tau1PtGt45tau2PtGt45",
        "tau1PtGt45tau2Pt45to60",
        "tau1PtGt45tau2Pt60to80",
        "tau1PtGt45tau2PtGt80",
        "tau1PtGt45tau2PtGt60"
    ),

    sysShifts = cms.vstring(
        "CMS_scale_t_tautau_8TeVUp",
        "CMS_scale_t_tautau_8TeVDown",
        "CMS_eff_t_mssmHigh_tautau_8TeVUp",
        "CMS_eff_t_mssmHigh_tautau_8TeVDown"
    )
)
