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
    fileName = cms.string('addBackgroundQCDfromSSiso.root')
)

process.addBackgroundQCD = cms.PSet(

    regions = cms.VPSet(
        cms.PSet(
            name = cms.string("OSiso1_iso2_TightBtag"),
            region_norm = cms.PSet(
                category_inclusive = cms.string("OSrelaxed1FRw2ndTauTight_iso2_TightBtag"),
                category_nobtag = cms.string("OSrelaxed1FRw2ndTauTight_iso2_TightBtag"),
                ##category_btag = cms.string("OSvrelaxed1FRw2ndTauTight_iso2_LooseBtagFRw")
                category_btag = cms.string("OSrelaxed1FRw2ndTauTight_iso2_TightBtag")
            ),
            region_shape = cms.PSet(
                category_inclusive = cms.string("SSiso1_iso2_TightBtag"),
                category_nobtag = cms.string("SSiso1_iso2_TightBtag"),
                category_btag = cms.string("SSiso1_iso2_LooseBtagFRw")
            )
        ),
        cms.PSet(
            name = cms.string("SSiso1_iso2_TightBtag"),
            region_norm = cms.PSet(
                category_inclusive = cms.string("SSrelaxed1FRw2ndTauTight_iso2_TightBtag"),
                category_nobtag = cms.string("SSrelaxed1FRw2ndTauTight_iso2_TightBtag"),
                ##category_btag = cms.string("SSvrelaxed1FRw2ndTauTight_iso2_LooseBtagFRw")
                category_btag = cms.string("SSrelaxed1FRw2ndTauTight_iso2_TightBtag")
            ),
            region_shape = cms.PSet(
                category_inclusive = cms.string("SSrelaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight_TightBtag"),
                category_nobtag = cms.string("SSrelaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight_TightBtag"),
                category_btag = cms.string("SSvrelaxed1FRw2ndTauLoose_vrelaxed2FRw1stTauTight_LooseBtagFRw")
            )
        )
    ),

    processData = cms.string("data_obs"),
    processQCD = cms.string("QCDalt"),
    processesToSubtract = cms.vstring(
        "ZTTmc",
        "ZL",
        "ZJ",
        "Wtmp",
        "TT",
        "VV"
    ),

    categories = cms.vstring("inclusive", "nobtag", "btag"),

    tauPtBins = cms.vstring(
        "tau1PtGt45tau2PtGt45",
        "tau1PtGt45tau2Pt45to60",
        "tau1PtGt45tau2Pt60to80",
        "tau1PtGt45tau2PtGt80",
        "tau1PtGt45tau2PtGt60"
    ),

    sysShifts = cms.vstring(
        "CMS_htt_QCDfrNorm_tautau_8TeVUp",
        "CMS_htt_QCDfrNorm_tautau_8TeVDown",
        "CMS_htt_QCDfrShape_tautau_8TeVUp",
        "CMS_htt_QCDfrShape_tautau_8TeVDown"
    ),

    # CV: add 10% extra bin-by-bin uncertainty for mass range 100-250 GeV (depending on tau Pt bin),
    #     where QCD shape template is subject to "turn-on" effects which are difficult to model;
    #     cf. https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#TAU_TAU_channel
    addBinByBinUncertainties = cms.VPSet(
        cms.PSet(
            tauPtBins = cms.vstring("tau1PtGt45tau2PtGt45", "tau1PtGt45tau2Pt45to60"),
            ##add = cms.double(0.10),
            ##addDerr = cms.double(0.),
            min = cms.double(0.10),
            ranges = cms.vstring("50:155") # CV: extend slightly beyond 150 GeV as btag category uses binning {..., 140, 160,...}
        ),
        cms.PSet(
            tauPtBins = cms.vstring("tau1PtGt45tau2Pt60to80"),
            ##add = cms.double(0.10),
            ##addDerr = cms.double(0.),
            min = cms.double(0.10),
            ranges = cms.vstring("50:200")
        ),
        cms.PSet(
            tauPtBins = cms.vstring("tau1PtGt45tau2PtGt80", "tau1PtGt45tau2PtGt60"),
            ##add = cms.double(0.10),
            ##addDerr = cms.double(0.),
            min = cms.double(0.10),
            ranges = cms.vstring("50:250")
        )
    )
)
