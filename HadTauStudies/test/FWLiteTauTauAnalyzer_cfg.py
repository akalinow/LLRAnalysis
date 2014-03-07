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
    fileName = cms.string('FWLiteTauTauAnalyzer.root')
)

process.FWLiteTauTauAnalyzer = cms.PSet(

    treeName = cms.string('tauTauNtupleProducer/H2TauTauTreeProducerTauTau'),

    process = cms.string("Data"),
    massPoint = cms.double(0.),

    region = cms.string("OSiso"),

    central_or_shift = cms.string("central"),

    tau1Selection = cms.string(""),
    tau1PtMin = cms.double(45.),
    tau1PtMax = cms.double(-1.),
    tau2Selection = cms.string(""),
    tau2PtMin = cms.double(45.),
    tau2PtMax = cms.double(-1.),

    applyTightBtag = cms.bool(True),
    applyJetToTauFakeRateLooseToTightWeight = cms.bool(False), # CV: ratio of probabilities for jet -> tau fakes to pass tight/loose tau ID requirements
    jetToTauFakeRateLooseToTightWeight = cms.PSet(
        inputFileName = cms.string(""),
        fitFunctionNormName = cms.string("jetToTauFakeRate/inclusive/fitFunctionNorm_SSisoLooseBtag_div_SSantiisoLooseBtag"),
        fitFunctionShapeName_tau1 = cms.string("jetToTauFakeRate/inclusive/fitFunctionShape_tau1PtL_SSisoLooseBtag_div_SSantiisoLooseBtag"),
        fitFunctionShapePower_tau1 = cms.double(0.5),
        fitFunctionShapeName_tau2 = cms.string("jetToTauFakeRate/inclusive/fitFunctionShape_tau2PtL_SSisoLooseBtag_div_SSantiisoLooseBtag"),
        fitFunctionShapePower_tau2 = cms.double(0.5)
    ),
    applyJetToTauFakeRateCorrection = cms.bool(False), # CV: data/MC correction for for jet -> tau fake-rate
    jetToTauFakeRateCorrection = cms.string("1.0"),

    lumiScale = cms.double(1.),
    stitchingWeights = cms.vdouble(),
    addWeights = cms.vstring()
)
