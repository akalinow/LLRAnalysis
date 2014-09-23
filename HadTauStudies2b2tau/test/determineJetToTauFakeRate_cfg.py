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
    fileName = cms.string('determineJetToTauFakeRate.root')
)

process.determineJetToTauFakeRate = cms.PSet(

    type = cms.string("jetToTauFakeRate"), 

    looseRegion = cms.string("SSantiisoLooseBtag"),
    tightRegion = cms.string("SSisoLooseBtag"),

    processData = cms.string("data_obs"),
    processQCD = cms.string("QCD"),
    processesToSubtract = cms.vstring(
        "ZTTmc",
        "ZL",
        "ZJ",
        "Wtmp",
        "TT",
        "VV"
    ),
    
    category = cms.string("inclusive"),

    tauPtBin = cms.string("tau1PtGt45tau2PtGt45"),

    histogramsToFit = cms.vstring(
        "tau1PtS",
        "tau2PtS"
    ),

    fitFunction = cms.string("[0] + [1]*x"),
    xMin = cms.double(0.),
    xMax = cms.double(500.)
)
