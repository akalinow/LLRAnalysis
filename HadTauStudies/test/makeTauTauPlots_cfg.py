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
            name = cms.string("OSisoTightBtag"),
            label = cms.string("")
        ),        
        cms.PSet(
            name = cms.string("SSisoTightBtag"),
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
    processQCD  = cms.string("QCD"),
    
    signal1 = cms.PSet(
        process_ggH = cms.string("ggH160"),
        sf_ggH = cms.double(0.18683 + 0.44517),
        process_bbH = cms.string("bbH160"),
        sf_bbH = cms.double(0.59374 + 0.41421),
        legendEntry = cms.string("#Phi #rightarrow #tau#tau (m_{A}=160 GeV, tan#beta=8)"),
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
            histogramName = cms.string("tau1PtS"),
            xAxisTitle = cms.string("P_{T}^{#tau_{1}} [GeV]"),
            yAxisTitle = cms.string("dN/dP_{T}^{#tau_{1}} [1/GeV]")
        ),
        cms.PSet(
            histogramName = cms.string("tau1PtL"),
            xAxisTitle = cms.string("P_{T}^{#tau_{1}} [GeV]"),
            yAxisTitle = cms.string("dN/dP_{T}^{#tau_{1}} [1/GeV]")
        ),
        cms.PSet(
            histogramName = cms.string("tau1Eta"),
            xAxisTitle = cms.string("#eta_{#tau_{1}}"),
            yAxisTitle = cms.string("dN/d#eta_{#tau_{1}}")
        ),
        cms.PSet(
            histogramName = cms.string("tau1Phi"),
            xAxisTitle = cms.string("#phi_{#tau_{1}}"),
            yAxisTitle = cms.string("dN/d#phi_{#tau_{1}}")
        ),
        cms.PSet(
            histogramName = cms.string("tau1DecayMode"),
            xAxisTitle = cms.string("#tau_{1} Decay mode"),
            yAxisTitle = cms.string("N")
        ),
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
            histogramName = cms.string("tau2PtS"),
            xAxisTitle = cms.string("P_{T}^{#tau_{2}} [GeV]"),
            yAxisTitle = cms.string("dN/dP_{T}^{#tau_{2}} [1/GeV]")
        ),
        cms.PSet(
            histogramName = cms.string("tau2PtL"),
            xAxisTitle = cms.string("P_{T}^{#tau_{2}} [GeV]"),
            yAxisTitle = cms.string("dN/dP_{T}^{#tau_{2}} [1/GeV]")
        ),
        cms.PSet(
            histogramName = cms.string("tau2Eta"),
            xAxisTitle = cms.string("#eta_{#tau_{2}}"),
            yAxisTitle = cms.string("dN/d#eta_{#tau_{2}}")
        ),
        cms.PSet(
            histogramName = cms.string("tau2Phi"),
            xAxisTitle = cms.string("#phi_{#tau_{2}}"),
            yAxisTitle = cms.string("dN/d#phi_{#tau_{2}}")
        ),
        cms.PSet(
            histogramName = cms.string("tau2DecayMode"),
            xAxisTitle = cms.string("#tau_{2} Decay mode"),
            yAxisTitle = cms.string("N")
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
        ),
        cms.PSet(
            histogramName = cms.string("svFitMassS"),
            xAxisTitle = cms.string("m_{#tau#tau} [GeV]"),
            yAxisTitle = cms.string("dN/dm_{#tau#tau} [1/GeV]"),
            keepBlinded = cms.vstring("100:1500")
        ),
        cms.PSet(
            histogramName = cms.string("visMassS"),
            xAxisTitle = cms.string("m_{vis} [GeV]"),
            yAxisTitle = cms.string("dN/dm_{vis} [1/GeV]"),
            keepBlinded = cms.vstring("100:1500")
        ),
        cms.PSet(
            histogramName = cms.string("bJet1PtS"),
            xAxisTitle = cms.string("P_{T}^{b-jet} [GeV]"),
            yAxisTitle = cms.string("dN/dP_{T}^{b-jet} [1/GeV]")
        ),
        cms.PSet(
            histogramName = cms.string("bJet1Eta"),
            xAxisTitle = cms.string("#eta_{b-jet}"),
            yAxisTitle = cms.string("dN/d#eta_{b-jet}")
        ),
        cms.PSet(
            histogramName = cms.string("bJet1Phi"),
            xAxisTitle = cms.string("#phi_{b-jet}"),
            yAxisTitle = cms.string("dN/d#phi_{b-jet}")
        ),
        cms.PSet(
            histogramName = cms.string("metS"),
            xAxisTitle = cms.string("E_{T}^{miss} [GeV]"),
            yAxisTitle = cms.string("dN/dE_{T}^{miss} [1/GeV]")
        ),
        cms.PSet(
            histogramName = cms.string("numVertices"),
            xAxisTitle = cms.string("N_{vtx}"),
            yAxisTitle = cms.string("dN/dN_{vtx}")
        )
    ),
    showBgrUncertainty = cms.bool(False),

    labelOnTop = cms.string("CMS Preliminary, H#rightarrow #tau#tau, %1.1f fb^{-1} at 8 TeV"),    
    intLumiData = cms.double(18.3), # in units of fb^-1

    outputFileName = cms.string("plots/makeTauTauPlots.png")
)
