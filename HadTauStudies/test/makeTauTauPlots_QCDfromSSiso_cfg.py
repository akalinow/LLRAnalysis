import FWCore.ParameterSet.Config as cms

import os

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    
    ##maxEvents = cms.int32(100000),
    maxEvents = cms.int32(-1),
    
    outputEvery = cms.uint32(100000)
)

process.makeTauTauPlots = cms.PSet(

    regions = cms.VPSet(
        cms.PSet(
            name = cms.string("OSiso1_iso2_TightBtag"),
            label = cms.string("")
        ),        
        cms.PSet(
            name = cms.string("SSiso1_iso2_TightBtag"),
            label = cms.string("SS")
        ) 
    ),

    processData = cms.string("data_obs"),
    processZTT  = cms.string("ZTT"),
    processZL   = cms.string("ZL"),
    processZJ   = cms.string("ZJ"),    
    processTT   = cms.string("TT"),
    processW    = cms.string("W"),
    processVV   = cms.string("VV"),
    processQCD  = cms.string("QCDalt"),
    
    ##signal1 = cms.PSet(
    ##    process_ggH = cms.string("ggH160"),
    ##    sf_ggH = cms.double(0.18683 + 0.44517),
    ##    process_bbH = cms.string("bbH160"),
    ##    sf_bbH = cms.double(0.59374 + 0.41421),
    ##    legendEntry = cms.string("#Phi #rightarrow #tau#tau (m_{A}=160 GeV, tan#beta=8)"),
    ##),
    ##signal2 = cms.PSet(
    ##    process_ggH = cms.string("ggH500"),
    ##    sf_ggH = cms.double(0.00185 + 0.00127),
    ##    process_bbH = cms.string("bbH500"),
    ##    sf_bbH = cms.double(0.01541 + 0.01479),
    ##    legendEntry = cms.string("#Phi #rightarrow #tau#tau (m_{A}=500 GeV, tan#beta=20)"),
    ##),
    signal1 = cms.PSet(
        process_ggH = cms.string("ggH350"),
        sf_ggH = cms.double(0.03111 + 0.04429),
        process_bbH = cms.string("bbH350"),
        sf_bbH = cms.double(0.29595 + 0.33469),
        legendEntry = cms.string("#Phi #rightarrow #tau#tau (m_{A}=350 GeV, tan#beta=30)"),
    ),

    categories = cms.VPSet(
        cms.PSet(
            name = cms.string("inclusive"),
            label = cms.string("Inclusive")
        ),
        cms.PSet(
            name = cms.string("nobtag"),
            label = cms.string("No B-Tag")
        ),
        cms.PSet(
            name = cms.string("btag"),
            label = cms.string("B-Tag")
        )
    ),

    tauPtBins = cms.VPSet(
        cms.PSet(
            name = cms.string("tau1PtGt45tau2PtGt45"),
            label = cms.string("")
        ),
        cms.PSet(
            name = cms.string("tau1PtGt45tau2Pt45to60"),
            label = cms.string("45 < P_{T}^{#tau_{2}} < 60 GeV")
        ),
        cms.PSet(
            name = cms.string("tau1PtGt45tau2Pt60to80"),
            label = cms.string("60 < P_{T}^{#tau_{2}} < 80 GeV")
        ),
        cms.PSet(
            name = cms.string("tau1PtGt45tau2PtGt60"),
            label = cms.string("P_{T}^{#tau_{2}} > 60 GeV")
        ),
        cms.PSet(
            name = cms.string("tau1PtGt45tau2PtGt80"),
            label = cms.string("P_{T}^{#tau_{2}} > 80 GeV")
        )
    ),
    tauPtBinsToPlot = cms.vstring(
        "tau1PtGt45tau2PtGt45",
        "tau1PtGt45tau2Pt45to60",
        "tau1PtGt45tau2Pt60to80",
        "tau1PtGt45tau2PtGt80",
        "tau1PtGt45tau2PtGt60"
    ),

    distributions = cms.VPSet(
        cms.PSet(
            histogramName = cms.string("tau1IsoDiscrS"),
            xAxisTitle = cms.string("MVA_{#tau_{1}}"),
            yAxisTitle = cms.string("dN/dMVA_{#tau_{1}}")
        ),
        cms.PSet(
            histogramName = cms.string("tau1IsoDiscrL"),
            xAxisTitle = cms.string("I_{#tau_{1}} [GeV]"),
            yAxisTitle = cms.string("dN/dI_{#tau_{1}} [1/GeV]")
        ),
        cms.PSet(
            histogramName = cms.string("tau2IsoDiscrS"),
            xAxisTitle = cms.string("MVA_{#tau_{2}}"),
            yAxisTitle = cms.string("dN/dMVA_{#tau_{2}}")
        ),
        cms.PSet(
            histogramName = cms.string("tau2IsoDiscrL"),
            xAxisTitle = cms.string("I_{#tau_{2}} [GeV]"),
            yAxisTitle = cms.string("dN/dI_{#tau_{2}} [1/GeV]")
        )
    ),

    nuisanceParameters = cms.PSet(
        normalization = cms.PSet(
            ZTT = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.20"),
                category_nobtag = cms.string("1.0 +/- 0.20"),
                category_btag = cms.string("1.0 +/- 0.20")
            ),
            ZL = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.20"),
                category_nobtag = cms.string("1.0 +/- 0.20"),
                category_btag = cms.string("1.0 +/- 0.20")
            ),
            ZJ = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.20"),
                category_nobtag = cms.string("1.0 +/- 0.20"),
                category_btag = cms.string("1.0 +/- 0.20")
            ),
            TT = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.12"),
                category_nobtag = cms.string("1.0 +/- 0.12"),
                category_btag = cms.string("1.0 +/- 0.12")
            ),
            W = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.30"),
                category_nobtag = cms.string("1.0 +/- 0.30"),
                category_btag = cms.string("1.0 +/- 0.30")
            ),
            VV = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.25"),
                category_nobtag = cms.string("1.0 +/- 0.25"),
                category_btag = cms.string("1.0 +/- 0.25")
            ),
            QCDalt = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.35"),
                category_nobtag = cms.string("1.0 +/- 0.35"),
                category_btag = cms.string("1.0 +/- 0.35")
            )            
        ),
        shape = cms.PSet(
            CMS_scale_t_tautau_8TeV = cms.string("0.00 +/- 1.00"),
            CMS_ttbarPtReweight_8TeV = cms.string("0.00 +/- 1.00"),
            CMS_htt_WShape_tautau_8TeV = cms.string("0.00 +/- 1.00"),
            CMS_htt_QCDfrNorm_tautau_8TeV = cms.string("0.00 +/- 1.00"),
            CMS_htt_QCDfrShape_tautau_8TeV = cms.string("0.00 +/- 1.00")
        )
    ),
    showBgrUncertainty = cms.bool(False),

    labelOnTop = cms.string("CMS Preliminary, H#rightarrow #tau#tau, %1.1f fb^{-1} at 8 TeV"),    
    intLumiData = cms.double(18.3), # in units of fb^-1

    outputFileName = cms.string("plots/makeTauTauPlots.png")
)
