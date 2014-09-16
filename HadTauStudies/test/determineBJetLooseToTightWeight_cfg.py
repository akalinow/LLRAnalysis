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
    fileName = cms.string('determineBJetLooseToTightWeight.root')
)

process.determineJetToTauFakeRate = cms.PSet(

    type = cms.string("bJetLooseToTightWeight"), 

    looseRegion = cms.string("SSvrelaxedLooseBtag"),
    tightRegion = cms.string("SSvrelaxedTightBtag"),

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
        
    category = cms.string("btag"),

    tauPtBin = cms.string("tau1PtGt45tau2PtGt45"),
    
    histogramsToFit = cms.vstring(
        "bJet1PtS"
    ),

    fitFunction = cms.string("[0]*[0]*TMath::Erf([1]*(x - [2]))"),
    initialParameters = cms.PSet(
        p0 = cms.double(1.5),
        p1 = cms.double(0.01),
        p2 = cms.double(-20.)
    ),
    xMin = cms.double(20.),
    xMax = cms.double(200.)
)
