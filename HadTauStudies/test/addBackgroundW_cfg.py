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
    fileName = cms.string('addBackgroundW.root')
)

process.addBackgroundW = cms.PSet(

    regions = cms.VPSet(
        cms.PSet(
            name = cms.string("OSiso1_iso2_TightBtag"),
            region_norm = cms.PSet(
                category_inclusive = cms.string("OSiso1_iso2_TightBtag"),
                category_nobtag = cms.string("OSiso1_iso2_TightBtag"),
                category_btag = cms.string("OSiso1_iso2_TightBtag")
            ),        
            region_shape = cms.PSet(
                category_inclusive = cms.string("OSrelaxed1FRw2ndTauTight_iso2_TightBtag"),
                category_nobtag = cms.string("OSrelaxed1FRw2ndTauTight_iso2_TightBtag"),
                category_btag = cms.string("OSvrelaxed1FRw2ndTauTight_iso2_LooseBtagFRw")
            )
        ),
        cms.PSet(
            name = cms.string("OSiso1_iso2_LooseBtag"),
            region_norm = cms.PSet(
                category_inclusive = cms.string("OSiso1_iso2_LooseBtag"),
                category_nobtag = cms.string("OSiso1_iso2_LooseBtag"),
                category_btag = cms.string("OSiso1_iso2_LooseBtag")
            ),        
            region_shape = cms.PSet(
                category_inclusive = cms.string("OSrelaxed1FRw2ndTauTight_iso2_LooseBtag"),
                category_nobtag = cms.string("OSrelaxed1FRw2ndTauTight_iso2_LooseBtag"),
                category_btag = cms.string("OSvrelaxed1FRw2ndTauTight_iso2_LooseBtag")
            )
        ),
        cms.PSet(
            name = cms.string("SSiso1_iso2_TightBtag"),
            region_norm = cms.PSet(
                category_inclusive = cms.string("SSiso1_iso2_TightBtag"),
                category_nobtag = cms.string("SSiso1_iso2_TightBtag"),
                category_btag = cms.string("SSiso1_iso2_TightBtag")
            ),        
            region_shape = cms.PSet(
                category_inclusive = cms.string("SSrelaxed1FRw2ndTauTight_iso2_TightBtag"),
                category_nobtag = cms.string("SSrelaxed1FRw2ndTauTight_iso2_TightBtag"),
                category_btag = cms.string("SSvrelaxed1FRw2ndTauTight_iso2_LooseBtagFRw")
            )
        ),
        cms.PSet(
            name = cms.string("SSiso1_iso2_LooseBtag"),
            region_norm = cms.PSet(
                category_inclusive = cms.string("SSiso1_iso2_LooseBtag"),
                category_nobtag = cms.string("SSiso1_iso2_LooseBtag"),
                category_btag = cms.string("SSiso1_iso2_LooseBtag")
            ),        
            region_shape = cms.PSet(
                category_inclusive = cms.string("SSrelaxed1FRw2ndTauTight_iso2_LooseBtag"),
                category_nobtag = cms.string("SSrelaxed1FRw2ndTauTight_iso2_LooseBtag"),
                category_btag = cms.string("SSvrelaxed1FRw2ndTauTight_iso2_LooseBtag")
            )
        )
    ),

    processW_tmp = cms.string("Wtmp"),
    processW = cms.string("W"),

    categories = cms.vstring("inclusive", "nobtag", "btag"),

    tauPtBins = cms.vstring(
        "tau1PtGt45tau2PtGt45",
        "tau1PtGt45tau2Pt45to60",
        "tau1PtGt45tau2Pt60to80",
        "tau1PtGt45tau2PtGt80",
        "tau1PtGt45tau2PtGt60"
    ),

    sysShifts = cms.vstring(
        "CMS_scale_t_tautau_8TeVUp",
        "CMS_scale_t_tautau_8TeVDown",
        "CMS_eff_b_8TeVUp",
        "CMS_eff_b_8TeVDown",
        "CMS_fake_b_8TeVUp",
        "CMS_fake_b_8TeVDown",
        "CMS_htt_WShape_tautau_8TeVUp",
        "CMS_htt_WShape_tautau_8TeVDown",
        "CMS_htt_eff_trig_mssmHigh_tautau_8TeVUp",
        "CMS_htt_eff_trig_mssmHigh_tautau_8TeVDown"
    )
)
