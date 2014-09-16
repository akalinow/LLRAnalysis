import FWCore.ParameterSet.Config as cms

import os

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    
    ##maxEvents = cms.int32(100000),
    maxEvents = cms.int32(-1),
    
    outputEvery = cms.uint32(100000)
)

process.makeTauTauPlots2b2tau = cms.PSet(

    regions = cms.VPSet(
        cms.PSet(
            name = cms.string("OSiso1_iso2"),
            label = cms.string("")
        ),        
        cms.PSet(
            name = cms.string("SSiso1_iso2"),
            label = cms.string("SS")
        ) 
    ),

    processData    = cms.string("data_obs"),
    processZTT     = cms.string("ZTT"),
    processZL      = cms.string("ZL"),
    processZJ      = cms.string("ZJ"),    
    processTT      = cms.string("TT"),
    processW       = cms.string("W"),
    processVV      = cms.string("VV"),
    processQCD     = cms.string("QCDalt"),
    processSingleH = cms.string("singleH_SM125"),
    
    signal1 = cms.PSet(
        process_ggH = cms.string("hhTo2b2tau"),
        # CV: scale signal to 500x Standard Model cross-section:
        #      sigma(hh) = 9.76 fb, cf. arXiv:1309.6594
        #      BR(h -> bb) = 57.7% @ 125 GeV, cf. https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR3
        #      BR(h -> tautau) = 6.32% @ 125 GeV 
        sf_ggH = cms.double(9.76e-3*0.577*0.0632*2.*100.),
        process_bbH = cms.string(""),
        sf_bbH = cms.double(0.),
        legendEntry = cms.string("hh #rightarrow 2b 2#tau (100x SM)"),
    ),
    signal2 = cms.PSet(
        process_ggH = cms.string("radion700Tohh"),
        # CV: scale resonant signal to 1pb cross-section
        sf_ggH = cms.double(1.*0.577*0.0632*2.),
        process_bbH = cms.string(""),
        sf_bbH = cms.double(0.),
        legendEntry = cms.string("R #rightarrow hh #rightarrow 2b 2#tau (m=700 GeV)"),
    ),

    categories = cms.VPSet(
        cms.PSet(
            name = cms.string("inclusive"),
            label = cms.string("Inclusive")
        ),
        cms.PSet(
            name = cms.string("2bM"),
            label = cms.string("2 B-Tags (Medium)")
        ),
        cms.PSet(
            name = cms.string("2bL"),
            label = cms.string("2 B-Tags (Loose)")
        ),
        cms.PSet(
            name = cms.string("1b1j"),
            label = cms.string("1 B-Tag (Medium)")
        ),
        cms.PSet(
            name = cms.string("2j"),
            label = cms.string("0 B-Tag (Medium)")
        )
    ),

    tauPtBins = cms.VPSet(
        cms.PSet(
            name = cms.string("tau1PtGtXXtau2PtGtXX"),
            label = cms.string("")
        )
    ),
    tauPtBinsToPlot = cms.vstring(
        "tau1PtGtXXtau2PtGtXX"
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
                category_2bM = cms.string("1.0 +/- 0.20"),
                category_2bL = cms.string("1.0 +/- 0.20"),
                category_1b1j = cms.string("1.0 +/- 0.20"),                
                category_2j = cms.string("1.0 +/- 0.20")
            ),
            ZL = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.20"),
                category_2bM = cms.string("1.0 +/- 0.20"),
                category_2bL = cms.string("1.0 +/- 0.20"),
                category_1b1j = cms.string("1.0 +/- 0.20"),                
                category_2j = cms.string("1.0 +/- 0.20")
            ),
            ZJ = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.20"),
                category_2bM = cms.string("1.0 +/- 0.20"),
                category_2bL = cms.string("1.0 +/- 0.20"),
                category_1b1j = cms.string("1.0 +/- 0.20"),                
                category_2j = cms.string("1.0 +/- 0.20")
            ),
            TT = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.12"),
                category_2bM = cms.string("1.0 +/- 0.12"),
                category_2bL = cms.string("1.0 +/- 0.12"),
                category_1b1j = cms.string("1.0 +/- 0.12"),
                category_2j = cms.string("1.0 +/- 0.12")
            ),
            W = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.30"),
                category_2bM = cms.string("1.0 +/- 0.30"),
                category_2bL = cms.string("1.0 +/- 0.30"),
                category_1b1j = cms.string("1.0 +/- 0.30"),                
                category_2j = cms.string("1.0 +/- 0.30")
            ),
            VV = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.25"),
                category_2bM = cms.string("1.0 +/- 0.25"),
                category_2bL = cms.string("1.0 +/- 0.25"),                
                category_1b1j = cms.string("1.0 +/- 0.25"),
                category_2j = cms.string("1.0 +/- 0.25")
            ),
            QCDalt = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.35"),
                category_2bM = cms.string("1.0 +/- 0.35"),
                category_2bL = cms.string("1.0 +/- 0.35"),
                category_1b1j = cms.string("1.0 +/- 0.35"),                
                category_2j = cms.string("1.0 +/- 0.35")
            ),
            singleH_SM125  = cms.PSet(
                category_inclusive = cms.string("1.0 +/- 0.20"),
                category_2bM = cms.string("1.0 +/- 0.20"),
                category_2bL = cms.string("1.0 +/- 0.20"),
                category_1b1j = cms.string("1.0 +/- 0.20"),
                category_2j = cms.string("1.0 +/- 0.20")
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

    labelOnTop = cms.string("CMS Preliminary, hh #rightarrow 2b2#tau, %1.1f fb^{-1} at 8 TeV"),    
    intLumiData = cms.double(18.3), # in units of fb^-1

    outputFileName = cms.string("plots/makeTauTauPlots2b2tau.png")
)
