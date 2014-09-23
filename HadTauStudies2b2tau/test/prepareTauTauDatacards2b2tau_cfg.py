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
    fileName = cms.string('prepareTauTauDatacards2b2tau.root')
)

process.prepareTauTauDatacards2b2tau = cms.PSet(

    signalRegion = cms.string("OSiso1_iso2"),

    processesToCopy = cms.vstring(
        "data_obs",
        "ZTT",
        "ZL",
        "ZJ",
        "W",
        "ggH_SM[0-9]+",
        "qqH_SM[0-9]+",
	"VH_SM[0-9]+",
        "singleH_SM[0-9]+",
        "TT",
        "VV",
        "QCD",
        "hhTo2b2tau",
        "mssmH[0-9]+tohh",
        "abelianZprime[0-9]+tohh",
        "graviton[0-9]+Tohh",
        "radion[0-9]+Tohh"
    ),

    categories = cms.vstring("inclusive", "2bM", "2bL", "1b1j", "2j"),
    discriminator = cms.string('2tauT'),

    tauPtBins = cms.vstring(
        "tau1PtGtXXtau2PtGtXX"
    ),

    histogramToFit = cms.string("svFitMassS"),
    histogramToFit_fine_binning = cms.string("svFitMassL"),

    sysShifts = cms.vstring(
        "CMS_scale_t_tautau_8TeVUp",
        "CMS_scale_t_tautau_8TeVDown",
        "CMS_eff_t_mssmHigh_tautau_8TeVUp",
        "CMS_eff_t_mssmHigh_tautau_8TeVDown",
        "CMS_eff_b_8TeVUp",
        "CMS_eff_b_8TeVDown",
        "CMS_fake_b_8TeVUp",
        "CMS_fake_b_8TeVDown",
        "CMS_htt_ttbarPtReweight_8TeVUp",
        "CMS_htt_ttbarPtReweight_8TeVDown",        
        "CMS_htt_WShape_tautau_8TeVUp",
        "CMS_htt_WShape_tautau_8TeVDown",
        "CMS_htt_QCDfrNorm_tautau_8TeVUp",
        "CMS_htt_QCDfrNorm_tautau_8TeVDown",
        "CMS_htt_QCDfrShape_tautau_8TeVUp",
        "CMS_htt_QCDfrShape_tautau_8TeVDown"
    )
)
