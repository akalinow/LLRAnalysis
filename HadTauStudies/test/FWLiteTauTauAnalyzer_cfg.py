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

    applyTauTriggerTurnOn = cms.string("tree"),

    applyTightBtag = cms.bool(True),
    
    applyBJetLooseToTightWeight = cms.bool(False), # CV: ratio of probabilities for jets to pass tight/loose b-tag discriminators
    bJetLooseToTightWeight = cms.PSet(
        inputFileName = cms.string(""),
        fitFunctionNormName = cms.string("bJetLooseToTightWeight/btag/$particleEtaBin/fitFunctionNorm_SSvrelaxed1_vrelaxed2_TightBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag"),
        fitFunctionShapeName_bJet1 = cms.string("bJetLooseToTightWeight/btag/$particleEtaBin/fitFunctionShape_bJet1PtL_SSvrelaxed1_vrelaxed2_TightBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag"),
        fitFunctionShapePower_bJet1 = cms.double(1.0),
        fitFunctionShapeName_bJet2 = cms.string(""),
        fitFunctionShapePower_bJet2 = cms.double(0.)
    ),
    bJet1EtaBins = cms.vdouble(-1.0, 1.0, 2.0, 9.9),
    bJet2EtaBins = cms.vdouble(-1.0, 9.9),
    
    applyJetToTauFakeRateLooseToTightWeight = cms.bool(False), # CV: ratio of probabilities for jet -> tau fakes to pass tight/loose tau ID requirements (used for QCD background estimation)
    jetToTauFakeRateLooseToTightWeight = cms.PSet(
        inputFileName = cms.string(""),
        fitFunctionNormName = cms.string("jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSisoLooseBtag_div_SSrelaxedLooseBtag"),
        fitFunctionShapeName_tau1 = cms.string("jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtL_SSisoLooseBtag_div_SSrelaxedLooseBtag"),
        fitFunctionShapePower_tau1 = cms.double(1.0),
        fitFunctionShapeName_tau2 = cms.string(""),
        fitFunctionShapePower_tau2 = cms.double(0.)
    ),
    
    applyJetToTauFakeRateCorrection = cms.bool(False), # CV: data/MC correction for for jet -> tau fake-rate (used for W+jets background estimation)
    jetToTauFakeRateCorrection = cms.string("1.0"),

    lumiScale = cms.double(1.),
    stitchingWeights = cms.vdouble(),
    addWeights = cms.vstring(),

    selEventsFileName = cms.string("")
)
