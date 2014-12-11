import FWCore.ParameterSet.Config as cms

import os

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring("ntuple_HadTauStream.root"),
    
    ##maxEvents = cms.int32(100000),
    maxEvents = cms.int32(-1),
    
    outputEvery = cms.uint32(100000)
)

process.fwliteOutput = cms.PSet(
    fileName = cms.string('FWLiteTauTauAnalyzer2b2tau.root')
)

process.FWLiteTauTauAnalyzer2b2tau = cms.PSet(

    treeName = cms.string('tauTauNtupleProducer/H2TauTauTreeProducerTauTau'),

    process = cms.string("ggH130"),

    region = cms.string("OSiso"),

    central_or_shift = cms.string("central"),

    tau1Selection = cms.string(""),
    tau1PtMin = cms.double(45.),
    tau1PtMax = cms.double(-1.),
    tau2Selection = cms.string(""),
    tau2PtMin = cms.double(45.),
    tau2PtMax = cms.double(-1.),
    discriminator = cms.string("2tauT"),

    trigger = cms.string("TauPlusJet"),
    tauPtForSwitchingTriggers = cms.double(350.),

    applyTauTriggerTurnOn = cms.string("tree"),

    applyJetToTauFakeRateLooseToTightWeight = cms.bool(False), # CV: ratio of probabilities for jet -> tau fakes to pass tight/loose tau ID requirements (used for QCD background estimation)
    jetToTauFakeRateLooseToTightWeight = cms.PSet(
        inputFileName = cms.string(""),
        fitFunctionNormName = cms.string("jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSisoLooseBtag_div_SSrelaxedLooseBtag"),
        graphShapeName_tau1 = cms.string(""),
        fitFunctionShapeName_tau1_central = cms.string("jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtL_SSisoLooseBtag_div_SSrelaxedLooseBtag"),
        fitFunctionShapeName_tau1_shift = cms.string(""),
        fitFunctionShapePower_tau1 = cms.double(1.0),
        applyFitFunction_or_graph_tau1 = cms.string("fitFunction"),
        graphShapeName_tau2 = cms.string(""),
        fitFunctionShapeName_tau2_central = cms.string(""),
        fitFunctionShapeName_tau2_shift = cms.string(""),
        fitFunctionShapePower_tau2 = cms.double(0.),
        applyFitFunction_or_graph_tau2 = cms.string("notApplied")
    ),
    tau1EtaBins = cms.vdouble(-1., 1.2, 1.7, 9.9),
    tau2EtaBins = cms.vdouble(-1., 9.9),
    
    applyJetToTauFakeRateCorrection = cms.bool(False), # CV: data/MC correction for for jet -> tau fake-rate (used for W+jets background estimation)
    jetToTauFakeRateCorrection = cms.string("1.0"),

    jetCorrInputFileName = cms.FileInPath('PhysicsTools/PatUtils/data/Summer13_V1_DATA_UncertaintySources_AK5PF.txt'),
    jetCorrUncertaintyTag = cms.string('SubTotalMC'),
    
    lumiScale = cms.double(1.),
    stitchingWeights = cms.vdouble(),
    addWeights = cms.vstring(),

    selEventsFileName_input = cms.string(""),
    selEventsFileName_output = cms.string("")
)
