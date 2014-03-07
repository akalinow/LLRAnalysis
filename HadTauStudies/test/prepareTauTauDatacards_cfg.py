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
    fileName = cms.string('prepareTauTauDatacards.root')
)

process.prepareTauTauDatacards = cms.PSet(

    signalRegion = cms.string("OSisoTightBtag"),

    processesToCopy = cms.vstring(
        "data_obs",
        "ZLL",
        "ZJ",
        "W",
        "ggH_SM[0-9]+",
        "qqH_SM[0-9]+",
        "TT",
        "VV",
        "ggH[0-9]+",
        "bbH[0-9]+"
    ),

    processData = cms.string("data_obs"),
    processZTT = cms.string("ZTT"),
    processZTTmc = cms.string("ZTTmc"),
    processZTT_Embedded = cms.string("ZTT_Embedded"),
    processTT_Embedded = cms.string("TT_Embedded"),

    qcdRegion = cms.string("OSantiisoFRwTightBtag"),
    processQCD = cms.string("QCD"),
    processesToSubtract = cms.vstring(
        "ZTTmc",
        "ZLL",
        "ZJ",
        "W",
        "TT",
        "VV"
    ),

    categoriesToCopy = cms.vstring("inclusive", "nobtag", "btag"),
    category_inclusive = cms.string("inclusive"),

    tauPtBins = cms.vstring(
        "tau1PtGt45tau2PtGt45",
        "tau1PtGt45tau2Pt45to60",
        "tau1PtGt45tau2Pt60to80",
        "tau1PtGt45tau2PtGt80",
        "tau1PtGt45tau2PtGt60"
    ),

    histogramToFit = cms.string("svFitMassS"),
    histogramToFit_fine_binning = cms.string("svFitMassL"),

    sysShifts = cms.vstring(
        "CMS_scale_t_tautau_8TeVUp",
        "CMS_scale_t_tautau_8TeVDown",
        "CMS_ttbarPtReweight_8TeVUp",
        "CMS_ttbarPtReweight_8TeVDown"
    )
)
