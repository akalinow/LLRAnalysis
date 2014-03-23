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
    fileName = cms.string('addBackgroundQCDfromSSantiiso.root')
)

process.addBackgroundQCD = cms.PSet(

    regions = cms.VPSet(
        cms.PSet(
            name = cms.string("OSisoTightBtag"),
            qcdRegion_norm = cms.PSet(
                inclusive = cms.string("OSrelaxedFRwTightBtag"),
                nobtag = cms.string("OSrelaxedFRwTightBtag"),
                btag = cms.string("OSrelaxedFRwTightBtag")
            ),        
            qcdRegion_shape = cms.PSet(
                inclusive = cms.string("OSrelaxedFRwTightBtag"),
                nobtag = cms.string("OSrelaxedFRwTightBtag"),
                btag = cms.string("OSvrelaxedFRwLooseBtagFRw")
            )
        ),
        cms.PSet(
            name = cms.string("OSisoLooseBtag"),
            qcdRegion_norm = cms.PSet(
                inclusive = cms.string("OSrelaxedFRwLooseBtag"),
                nobtag = cms.string("OSrelaxedFRwLooseBtag"),
                btag = cms.string("OSrelaxedFRwLooseBtag")
            ),        
            qcdRegion_shape = cms.PSet(
                inclusive = cms.string("OSrelaxedFRwLooseBtag"),
                nobtag = cms.string("OSrelaxedFRwLooseBtag"),
                btag = cms.string("OSvrelaxedFRwLooseBtag")
            )
        ),
        cms.PSet(
            name = cms.string("SSisoTightBtag"),
            qcdRegion_norm = cms.PSet(
                inclusive = cms.string("SSrelaxedFRwTightBtag"),
                nobtag = cms.string("SSrelaxedFRwTightBtag"),
                btag = cms.string("SSrelaxedFRwTightBtag")
            ),        
            qcdRegion_shape = cms.PSet(
                inclusive = cms.string("SSrelaxedFRwTightBtag"),
                nobtag = cms.string("SSrelaxedFRwTightBtag"),
                btag = cms.string("SSvrelaxedFRwLooseBtagFRw")
            )
        ),
        cms.PSet(
            name = cms.string("SSisoLooseBtag"),
            qcdRegion_norm = cms.PSet(
                inclusive = cms.string("SSrelaxedFRwLooseBtag"),
                nobtag = cms.string("SSrelaxedFRwLooseBtag"),
                btag = cms.string("SSrelaxedFRwLooseBtag")
            ),        
            qcdRegion_shape = cms.PSet(
                inclusive = cms.string("SSrelaxedFRwLooseBtag"),
                nobtag = cms.string("SSrelaxedFRwLooseBtag"),
                btag = cms.string("SSvrelaxedFRwLooseBtag")
            )
        )
    ),

    processData = cms.string("data_obs"),
    processQCD = cms.string("QCD"),
    processesToSubtract = cms.vstring(
        "ZTTmc",
        "ZL",
        "ZJ",
        "W",
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

    # CV: add 10% extra bin-by-bin uncertainty for mass range 100-150 GeV,
    #     where QCD shape template is subject to "turn-on" effects which are difficult to model;
    #     cf. https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#TAU_TAU_channel
    addBinByBinUncertainties = cms.VPSet(
        cms.PSet(
            #add = cms.double(0.10),
            min = cms.double(0.10),
            range = cms.vstring("100:150")
        )
    )
)
