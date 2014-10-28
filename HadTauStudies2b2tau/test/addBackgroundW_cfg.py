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
            name = cms.string("OSiso1_iso2"),
            region_norm = cms.PSet(
                category_inclusive = cms.string("OSiso1_iso2"),
                category_2bM = cms.string("OSiso1_iso2"),
                category_2bM_nonresonant = cms.string("OSiso1_iso2"),
                category_2bM_resonant = cms.string("OSiso1_iso2"),
                category_2bL = cms.string("OSiso1_iso2"),
                category_2bL_nonresonant = cms.string("OSiso1_iso2"),
                category_2bL_resonant = cms.string("OSiso1_iso2"),
                category_1b1j = cms.string("OSiso1_iso2"),
                category_1b1j_nonresonant = cms.string("OSiso1_iso2"),
                category_1b1j_resonant = cms.string("OSiso1_iso2"),
                category_2j = cms.string("OSiso1_iso2"),
                category_2j_nonresonant = cms.string("OSiso1_iso2"),
                category_2j_resonant = cms.string("OSiso1_iso2")
            ),        
            region_shape = cms.PSet(
                category_inclusive = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_2bM = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_2bM_nonresonant = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_2bM_resonant = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_2bL = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_2bL_nonresonant = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_2bL_resonant = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_1b1j = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_1b1j_nonresonant = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_1b1j_resonant = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_2j = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_2j_nonresonant = cms.string("OSrelaxed1FRw2ndTauTight_iso2"),
                category_2j_resonant = cms.string("OSrelaxed1FRw2ndTauTight_iso2")
            )
        ),
        cms.PSet(
            name = cms.string("SSiso1_iso2"),
            region_norm = cms.PSet(
                category_inclusive = cms.string("SSiso1_iso2"),
                category_2bM = cms.string("SSiso1_iso2"),
                category_2bM_nonresonant = cms.string("SSiso1_iso2"),
                category_2bM_resonant = cms.string("SSiso1_iso2"),
                category_2bL = cms.string("SSiso1_iso2"),
                category_2bL_nonresonant = cms.string("SSiso1_iso2"),
                category_2bL_resonant = cms.string("SSiso1_iso2"),
                category_1b1j = cms.string("SSiso1_iso2"),
                category_1b1j_nonresonant = cms.string("SSiso1_iso2"),
                category_1b1j_resonant = cms.string("SSiso1_iso2"),
                category_2j = cms.string("SSiso1_iso2"),
                category_2j_nonresonant = cms.string("SSiso1_iso2"),
                category_2j_resonant = cms.string("SSiso1_iso2")
            ),        
            region_shape = cms.PSet(
                category_inclusive = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_2bM = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_2bM_nonresonant = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_2bM_resonant = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_2bL = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_2bL_nonresonant = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_2bL_resonant = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_1b1j = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_1b1j_nonresonant = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_1b1j_resonant = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_2j = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_2j_nonresonant = cms.string("SSrelaxed1FRw2ndTauTight_iso2"),
                category_2j_resonant = cms.string("SSrelaxed1FRw2ndTauTight_iso2")
            )
        )
    ),

    processW_tmp = cms.string("Wtmp"),
    processW = cms.string("W"),

    categories = cms.vstring(
        "inclusive",
        "2bM",
        "2bM_nonresonant",
        "2bM_resonant",
        "2bL",
        "2bL_nonresonant",
        "2bL_resonant",
        "1b1j",
        "1b1j_nonresonant",
        "1b1j_resonant",
        "2j",
        "2j_nonresonant",
        "2j_resonant"
    ),
        
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
        "CMS_htt_WShape_tautau_8TeVDown"
    )
)
