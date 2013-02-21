import FWCore.ParameterSet.Config as cms


######################################################################################

muIDCutOld = ("isTrackerMuon "+
             "&& isGlobalMuon "+
             "&& numberOfMatches>=2 "+
             "&& globalTrack.isNonnull "+
             "&& globalTrack.hitPattern.numberOfValidMuonHits>=1 "+
             "&& globalTrack.hitPattern.numberOfValidPixelHits>=1 "+
             "&& globalTrack.hitPattern.numberOfValidTrackerHits>10 "+
             "&& globalTrack.normalizedChi2<10 "+
             "&& globalTrack.ptError/globalTrack.pt<0.1")
muIDCutNew = ("isGlobalMuon "+
             "&& isPFMuon "+
             "&& globalTrack.normalizedChi2<10 "+
             "&& globalTrack.isNonnull "+
             "&& globalTrack.hitPattern.numberOfValidMuonHits>=1 "+
             "&& numberOfMatches>=2 "+
             "&& abs(userFloat('dzWrtPV'))<0.2 "+
             "&& abs(userFloat('dxyWrtPV'))<0.5 "+
             "&& globalTrack.hitPattern.numberOfValidPixelHits>=1 "+
             "&& globalTrack.hitPattern.trackerLayersWithMeasurement>8")

muHasTrack = "globalTrack.isNonnull>0.5 "

tag = cms.EDFilter("PATMuonRefSelector",
                   src = cms.InputTag("selectedPatMuonsUserEmbedded"),
                   cut = cms.string('pt>25.0 '+
                                    '&& abs(eta)<2.1 '+
                                    '&& userFloat("PFRelIsoDB04")<0.1 '+
                                    '&& abs(userFloat("dxyWrtPV"))<0.045 '+
                                    '&& abs(userFloat("dzWrtPV"))<0.2 '+
                                    '&& '+muIDCutNew),
                   filter = cms.bool(False)
                   )
######################################################################################
probe = cms.EDFilter("PATMuonRefSelector",
                     src = cms.InputTag("selectedPatMuonsUserEmbedded"),
                     cut = cms.string('pt>15.0 && abs(eta)<2.1'),
                     filter = cms.bool(False)
                     )
######################################################################################
tnp = cms.EDProducer("CandViewShallowCloneCombiner",
                     decay = cms.string("tag probe"),
                     cut   = cms.string("30 < mass < 150"),
                     checkCharge = cms.bool(False),   
                     )
######################################################################################
oneTp = cms.EDFilter("CandViewCountFilter",
                     src = cms.InputTag("tnp"),
                     minNumber = cms.uint32(1)
                     )
######################################################################################
tagMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                            pdgId = cms.vint32(13,-13),
                            src = cms.InputTag("tag"),
                            distMin = cms.double(0.15),
                            matched = cms.InputTag("genParticles")
                            )

probeMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                              pdgId = cms.vint32(13,-13),
                              src = cms.InputTag("probe"),
                              distMin = cms.double(0.15),
                              matched = cms.InputTag("genParticles")
                              )
######################################################################################
muTnP = cms.EDAnalyzer("TagProbeFitTreeProducer",
                       tagProbePairs = cms.InputTag("tnp"),
                       arbitration   = cms.string("OneProbe"),
                       #massForArbitration = cms.double(91),
                       variables = cms.PSet(
    Pt = cms.string("pt"),
    PfRelIso= cms.string('userFloat("PFRelIsoDB04")'),
    absEta = cms.string("abs(eta)"),
    Eta =  cms.string("eta"),
    Phi = cms.string("phi"),
    GenDecay   =  cms.InputTag("addUserVariablesProbe","genDecay"),
    HLTIsoMu24Bit     =  cms.InputTag("addUserVariablesProbe","HLTIsoMu24Bit"),
    HLTIsoMu24Match   =  cms.InputTag("addUserVariablesProbe","HLTIsoMu24Match"),
    hltIsoMu20Match   =  cms.InputTag("addUserVariablesProbe","hltIsoMu20Match"),
    muMatches         =  cms.string('numberOfMatches'),
    muDz              =  cms.string('abs(userFloat("dzWrtPV"))'),
    muDxy             =  cms.string('abs(userFloat("dxyWrtPV"))'),
    muTrackChi2       =  cms.string("? (%s) ? globalTrack.normalizedChi2:99" % muHasTrack ),
    muValidHits       =  cms.string("? (%s) ? globalTrack.hitPattern.numberOfValidMuonHits:99.0" % muHasTrack ),
    muPixelHits       =  cms.string("? (%s) ? globalTrack.hitPattern.numberOfValidPixelHits:99.0" % muHasTrack ),
    muTrackerHits     =  cms.string("? (%s) ? globalTrack.hitPattern.numberOfValidTrackerHits:99.0" % muHasTrack ),
    muTrackLayers     =  cms.string("? (%s) ? globalTrack.hitPattern.trackerLayersWithMeasurement:99.0" % muHasTrack ),
    muTrackResol      =  cms.string("? (%s) ? globalTrack.ptError/globalTrack.pt:99.0" % muHasTrack ),
    muIsGlobal        =  cms.string('isGlobalMuon'),
    muIsTracker       =  cms.string('isTrackerMuon'),
    muIsStandalone    =  cms.string('isStandAloneMuon'),
    muIsPF            =  cms.string("userInt('isPFMuon')")
    ),
                       flags = cms.PSet(
    muID             =  cms.string(muIDCutNew),
    muIDOld          =  cms.string(muIDCutOld),
    muHasGlobalTrack =  cms.string(muHasTrack),
    muIso            =  cms.string('userFloat("PFRelIsoDB04")<0.1'),
    ),
                         tagVariables = cms.PSet(
    Pt              = cms.string("pt"), 
    Eta             = cms.string("eta"),
    Phi             = cms.string("phi"),
    PfRelIso        = cms.string("userFloat('PFRelIsoDB04')"),
    GenDecay        = cms.InputTag("addUserVariablesTag","genDecay"),
    HLTIsoMu24Bit   = cms.InputTag("addUserVariablesTag","HLTIsoMu24Bit"),
    HLTIsoMu24Match = cms.InputTag("addUserVariablesTag","HLTIsoMu24Match"),
    NumGenPU        = cms.InputTag("addUserVariablesTag","numGenPU"),
    Mt              = cms.InputTag("addUserVariablesTag","Mt"),
    PUMCWeight2011A = cms.InputTag("addUserVariablesTag","puMCWeightRun2011A"),
    PUMCWeight2011B = cms.InputTag("addUserVariablesTag","puMCWeightRun2011B"),
    PUMCWeight2012A = cms.InputTag("addUserVariablesTag","puMCWeightRun2012A")
    ),
                         tagFlags = cms.PSet(),
                         pairVariables = cms.PSet(
    tnpCharge = cms.string('charge'),
    deltaR = cms.string('sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+  min( abs(daughter(0).phi-daughter(1).phi), 2*3.141 - abs(daughter(0).phi-daughter(1).phi)  ) *  min( abs(daughter(0).phi-daughter(1).phi), 2*3.141 - abs(daughter(0).phi-daughter(1).phi)  )  )')
    ),
                       pairFlags = cms.PSet(),
                       isMC = cms.bool( True ),
                       tagMatches = cms.InputTag("tagMcMatch") ,
                       probeMatches  = cms.InputTag("probeMcMatch"),
                       motherPdgId = cms.vint32(23),
                       makeMCUnbiasTree = cms.bool(True),
                       checkMotherInUnbiasEff = cms.bool(True),
                       allProbes = cms.InputTag("probe"),
                       addRunLumiInfo = cms.bool(True),
                       addEventVariablesInfo = cms.bool(True),
                       )

#############################################################################
vecFall11MC = (0.0020804,0.0109929,0.0228113,0.0429041,0.0566695,0.0721029,0.0729181,0.0711529,0.0713194,0.0605273,0.0531286,0.0434069,0.0446853,0.0439974,0.0422232,0.0381212,0.0286534,0.0336355,0.0270326,0.0256726,0.0236528,0.0217785,0.0180944,0.0171807,0.0145082,0.0112167,0.00852673,0.00395336,0.00458083,0.00435781,0.00354159,0.00167699,0.00103091,0.000206458,0.000618599,0.000206121,0.000412579,8.83568e-06,0,0.000412545,0,0,0,0,0,0,0,0,0,0)
vecSummer12MC = (2.344E-05,2.344E-05,2.344E-05,2.344E-05,4.687E-04,4.687E-04,7.032E-04,9.414E-04,1.234E-03,1.603E-03,2.464E-03,3.250E-03,5.021E-03,6.644E-03,8.502E-03,1.121E-02,1.518E-02,2.033E-02,2.608E-02,3.171E-02,3.667E-02,4.060E-02,4.338E-02,4.520E-02,4.641E-02,4.735E-02,4.816E-02,4.881E-02,4.917E-02,4.909E-02,4.842E-02,4.707E-02,4.501E-02,4.228E-02,3.896E-02,3.521E-02,3.118E-02,2.702E-02,2.287E-02,1.885E-02,1.508E-02,1.166E-02,8.673E-03,6.190E-03,4.222E-03,2.746E-03,1.698E-03,9.971E-04,5.549E-04,2.924E-04,1.457E-04,6.864E-05,3.054E-05,1.282E-05,5.081E-06,1.898E-06,6.688E-07,2.221E-07,6.947E-08,2.047E-08)
vecData2011A = (0,0.000115513,0.00262453,0.0231253,0.122659,0.234608,0.23849,0.171807,0.110434,0.0654693,0.0246198,0.00540239,0.000590312,5.10257e-05,2.9901e-06,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
vecData2011B = (0,2.0743e-05,1.69768e-05,4.20265e-05,0.000413519,0.00233585,0.0219252,0.0625536,0.0892844,0.112663,0.130604,0.135136,0.130953,0.116884,0.0911061,0.0578308,0.0295598,0.0122804,0.00433753,0.00146337,0.000449148,0.000114735,2.54458e-05,9.34371e-07,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
vecData2012A = (167025,671763,1.39495e+06,2.04506e+06,2.52639e+06,3.09467e+06,4.20416e+06,6.25511e+06,9.424e+06,1.36176e+07,1.85145e+07,2.36502e+07,2.8515e+07, 3.26439e+07,3.56836e+07,3.74294e+07,3.78322e+07,3.6981e+07,3.50694e+07,3.23539e+07,2.91133e+07,2.56141e+07,2.2085e+07,1.87038e+07,1.55924e+07,1.28217e+07,1.04196e+07,8.38235e+06,6.68523e+06,5.29173e+06,4.16073e+06,3.2513e+06,2.52558e+06,1.95021e+06,1.49674e+06,1.14137e+06,864513,650174,485349,359517,264197,192577,139220,99816.3,70974, 50050.9,35008.2,24289.3,16718.5,11417.5,7737.46,5203.99,3474.16,2302.49,1515.1,990.016,642.47,414.121,265.166,168.683,0)

addUserVariablesTag = cms.EDProducer(
    "UserDefinedVariablesMuTnP",
    srcMuons = cms.InputTag("selectedPatMuonsUserEmbedded"),
    triggerResults = cms.InputTag("patTriggerEvent"),
    met = cms.InputTag("patMETsPFlow"),
    isMC = cms.bool(True),
    TrueDist2011A = cms.vdouble(vecData2011A),
    TrueDist2011B = cms.vdouble(vecData2011B),
    TrueDist2012A = cms.vdouble(vecData2012A),
    MCDist = cms.vdouble(vecFall11MC),
    debug = cms.bool(False) 
    )

addUserVariablesProbe = cms.EDProducer(
    "UserDefinedVariablesMuTnP",
    srcMuons = cms.InputTag("selectedPatMuonsUserEmbedded"),
    triggerResults = cms.InputTag("patTriggerEvent"),
    met = cms.InputTag("patMETsPFlow"),
    isMC = cms.bool(True),
    TrueDist2011A = cms.vdouble(vecData2011A),
    TrueDist2011B = cms.vdouble(vecData2011B),
    TrueDist2012A = cms.vdouble(vecData2012A),
    MCDist = cms.vdouble(vecFall11MC),
    debug = cms.bool(False) 
    )

#############################################################################

sequence = cms.Sequence(
    (tag+probe)*
    tnp*
    oneTp+
    addUserVariablesTag*
    addUserVariablesProbe*
    (tagMcMatch+probeMcMatch)*
    muTnP                          
    )
