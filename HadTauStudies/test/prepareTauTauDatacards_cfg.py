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

    signalRegion = cms.string("OSiso1_iso2_TightBtag"),

    processesToCopy = cms.vstring(
        "data_obs",
        "ZTT",
        "ZL",
        "ZJ",
        "W",
        "ggH_SM[0-9]+",
        "qqH_SM[0-9]+",
	"VH_SM[0-9]+",
        "TT",
        "VV",
        "QCD",
        "ggH[0-9]+",
        "bbH[0-9]+"
    ),

    categories = cms.vstring("inclusive", "nobtag", "btag"),

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
        "CMS_eff_t_mssmHigh_tautau_8TeVUp",
        "CMS_eff_t_mssmHigh_tautau_8TeVDown",
        "CMS_htt_eff_trig_mssmHigh_tautau_8TeVUp",
        "CMS_htt_eff_trig_mssmHigh_tautau_8TeVDown",
        "CMS_eff_b_8TeVUp",
        "CMS_eff_b_8TeVDown",
        "CMS_fake_b_8TeVUp",
        "CMS_fake_b_8TeVDown",
        "CMS_htt_higgsPtReweight_8TeVUp",
        "CMS_htt_higgsPtReweight_8TeVDown",
        "CMS_htt_higgsPtReweight_scale_8TeVUp",
        "CMS_htt_higgsPtReweight_scale_8TeVDown",
        "CMS_htt_higgsPtReweightSM_8TeVUp",
        "CMS_htt_higgsPtReweightSM_8TeVDown",
        "CMS_htt_WShape_tautau_8TeVUp",
        "CMS_htt_WShape_tautau_8TeVDown",
        "CMS_htt_ttbarPtReweight_8TeVUp",
        "CMS_htt_ttbarPtReweight_8TeVDown",
        "CMS_htt_ttbarJetFake_tautau_8TeVUp",
        "CMS_htt_ttbarJetFake_tautau_8TeVDown",
        "CMS_htt_QCDfrNorm_tautau_8TeVUp",
        "CMS_htt_QCDfrNorm_tautau_8TeVDown",
        "CMS_htt_QCDfrShape_tautau_8TeVUp",
        "CMS_htt_QCDfrShape_tautau_8TeVDown"
    )
)
