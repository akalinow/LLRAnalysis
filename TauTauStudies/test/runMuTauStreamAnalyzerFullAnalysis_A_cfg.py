import FWCore.ParameterSet.Config as cms

process = cms.Process("MUTAUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

runOnMC     = True
doSVFitReco = True

if runOnMC:
    print "Running on MC"
else:
    print "Running on Data"
        

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START42_V13::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_R_42_V19::All')
    
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 500
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi//DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/MuTauStream-13Oct2011/d01a4e7ec19203c158c3dddf2fa0ec05/patTuples_MuTauStream_9_1_6gn.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/MuTauStream-13Oct2011/d01a4e7ec19203c158c3dddf2fa0ec05/patTuples_MuTauStream_2_1_qa2.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/VBF_HToTauTau_M-120_7TeV-powheg-pythia6-tauola/MuTauStream-13Oct2011/d01a4e7ec19203c158c3dddf2fa0ec05/patTuples_MuTauStream_1_1_3W2.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/WW_TuneZ2_7TeV_pythia6_tauola/MuTauStream-13Oct2011/d01a4e7ec19203c158c3dddf2fa0ec05/patTuples_MuTauStream_1_1_3Hx.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/WZ_TuneZ2_7TeV_pythia6_tauola/MuTauStream-13Oct2011/d01a4e7ec19203c158c3dddf2fa0ec05/patTuples_MuTauStream_1_1_8ps.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/TTJets_TuneZ2_7TeV-madgraph-tauola/MuTauStream-13Oct2011/d01a4e7ec19203c158c3dddf2fa0ec05/patTuples_MuTauStream_1_1_rCW.root'
    #'file:./root/patTuples_MuTauStream_sync.root'
    )
    )

process.allEventsFilter = cms.EDFilter(
    "AllEventsFilter"
    )

###################################################################################
process.rescaledMET = cms.EDProducer(
    "MEtRescalerProducer",
    metTag = cms.InputTag("patMETsPFlow"),
    jetTag = cms.InputTag("selectedPatJets"),
    electronTag = cms.InputTag(""),
    muonTag = cms.InputTag("muPtEtaIDIso"),
    tauTag = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    unClusterShift = cms.double(0.10),
    tauShift = cms.double(0.03),
    muonShift = cms.double(0.01),
    electronShift = cms.double(0.02),
    jetThreshold  = cms.double(10),
    verbose = cms.bool(False)
    )

process.rescaledMETjet = process.rescaledMET.clone(
    unClusterShift = cms.double(0.10),
    tauShift = cms.double(0.0),
    muonShift = cms.double(0.0),
    electronShift = cms.double(0.0),
    )
process.rescaledMETtau = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift = cms.double(0.03),
    muonShift = cms.double(0.0),
    electronShift = cms.double(0.0),
    )
process.rescaledMETmuon = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift = cms.double(0.0),
    muonShift = cms.double(0.01),
    electronShift = cms.double(0.0),
    )

process.rescaledTaus = cms.EDProducer(
    "TauRescalerProducer",
    inputCollection = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    shift = cms.vdouble(0.03,0.03)
    )
process.rescaledMuons = cms.EDProducer(
    "MuonRescalerProducer",
    inputCollection = cms.InputTag("muPtEtaIDIso"),
    shift = cms.vdouble(0.01,0.01)
    )
process.rescaledMuonsRel = cms.EDProducer(
    "MuonRescalerProducer",
    inputCollection = cms.InputTag("muPtEtaRelID"),
    shift = cms.vdouble(0.01,0.01)
    )

process.rescaledObjects = cms.Sequence(
    process.rescaledMETjet+
    process.rescaledMETtau+
    process.rescaledMETmuon+
    process.rescaledTaus+
    process.rescaledMuons+
    process.rescaledMuonsRel
    )
###################################################################################

process.load("LLRAnalysis.Utilities.diTausReconstruction_cff")
process.diTau = process.allMuTauPairs.clone()
process.diTau.srcLeg1  = cms.InputTag("muPtEtaIDIso")
process.diTau.srcLeg2  = cms.InputTag("tauPtEtaIDAgMuAgElecIso")
process.diTau.srcMET   = cms.InputTag("patMETsPFlow")
process.diTau.dRmin12  = cms.double(0.5)
process.diTau.doSVreco = cms.bool(doSVFitReco)

if not runOnMC:
    process.diTau.srcGenParticles = ""
        
process.selectedDiTau = cms.EDFilter(
    "MuTauPairSelector",
    src = cms.InputTag("diTau"),
    cut = cms.string("dR12>0.5")
    )
process.selectedDiTauCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("selectedDiTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

#######################################################################

process.diTauJetUp =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("rescaledMETjet",  "UNNNU")
                                          )
process.selectedDiTauJetUp = process.selectedDiTau.clone(src = cms.InputTag("diTauJetUp") )
process.selectedDiTauJetUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauJetUp"))

process.diTauJetDown =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("rescaledMETjet",  "DNNND")
                                            )
process.selectedDiTauJetDown = process.selectedDiTau.clone(src = cms.InputTag("diTauJetDown") )
process.selectedDiTauJetDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauJetDown"))


process.diTauMuUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                        srcLeg1 = cms.InputTag("rescaledMuons","U"),
                                        srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                        srcMET  = cms.InputTag("rescaledMETmuon","NNUNN")
                                        )
process.selectedDiTauMuUp = process.selectedDiTau.clone(src = cms.InputTag("diTauMuUp") )
process.selectedDiTauMuUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauMuUp"))

process.diTauMuDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("rescaledMuons","D"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("rescaledMETmuon","NNDNN")
                                          )
process.selectedDiTauMuDown = process.selectedDiTau.clone(src = cms.InputTag("diTauMuDown") )
process.selectedDiTauMuDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauMuDown"))



process.diTauTauUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                         srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                         srcLeg2 = cms.InputTag("rescaledTaus", "U"),
                                         srcMET  = cms.InputTag("rescaledMETtau","NNNUN")
                                         )
process.selectedDiTauTauUp = process.selectedDiTau.clone(src = cms.InputTag("diTauTauUp") )
process.selectedDiTauTauUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauUp"))

process.diTauTauDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                           srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                           srcLeg2 = cms.InputTag("rescaledTaus", "D"),
                                           srcMET  = cms.InputTag("rescaledMETtau","NNNDN")
                                           )
process.selectedDiTauTauDown = process.selectedDiTau.clone(src = cms.InputTag("diTauTauDown") )
process.selectedDiTauTauDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauDown"))

process.allDiTau = cms.Sequence(
    (process.diTau*process.selectedDiTau*process.selectedDiTauCounter)+
    (process.diTauJetUp*process.selectedDiTauJetUp*process.selectedDiTauJetUpCounter +
     process.diTauJetDown*process.selectedDiTauJetDown*process.selectedDiTauJetDownCounter) +
    (process.diTauMuUp*process.selectedDiTauMuUp*process.selectedDiTauMuUpCounter +
     process.diTauMuDown*process.selectedDiTauMuDown*process.selectedDiTauMuDownCounter) +
    (process.diTauTauUp*process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter +
     process.diTauTauDown*process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter)
    )
#######################################################################

process.tauPtEtaIDAgMuAgElecIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("tauID('byLooseCombinedIsolationDeltaBetaCorr')>0.5 && pt>20 && abs(eta)<2.3"),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.tauPtEtaIDAgMuAgElecIsoTauUp  =  process.tauPtEtaIDAgMuAgElecIso.clone(
    src = cms.InputTag("rescaledTaus", "U")
    )
process.tauPtEtaIDAgMuAgElecIsoTauUpCounter = process.tauPtEtaIDAgMuAgElecIsoCounter.clone(
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIsoTauUp"),
    )
process.tauPtEtaIDAgMuAgElecIsoTauDown  =  process.tauPtEtaIDAgMuAgElecIso.clone(
    src = cms.InputTag("rescaledTaus", "D")
    )
process.tauPtEtaIDAgMuAgElecIsoTauDownCounter = process.tauPtEtaIDAgMuAgElecIsoCounter.clone(
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIsoTauDown"),
    )


process.muPtEtaIDIso  = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("muPtEtaID"),
    cut = cms.string("userFloat('PFRelIsoDB04v2')<0.10 && pt>15 && abs(eta)<2.1"),
    filter = cms.bool(False)
    )
process.muPtEtaIDIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("muPtEtaIDIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.muPtEtaIDIsoMuUp = process.muPtEtaIDIso.clone(
    src = cms.InputTag("rescaledMuons","U")
    )
process.muPtEtaIDIsoMuUpCounter = process.muPtEtaIDIsoCounter.clone(
    src = cms.InputTag("muPtEtaIDIsoMuUp"),
    )
process.muPtEtaIDIsoMuDown = process.muPtEtaIDIso.clone(
    src = cms.InputTag("rescaledMuons","D")
    )
process.muPtEtaIDIsoMuDownCounter = process.muPtEtaIDIsoCounter.clone(
    src = cms.InputTag("muPtEtaIDIsoMuDown"),
    )
process.muPtEtaRelID = process.muPtEtaIDIso.clone(
    src = cms.InputTag("muPtEtaRelID"),
    cut = cms.string("pt>15")
    )
process.muPtEtaRelIDMuUp   = process.muPtEtaRelID.clone(
    src = cms.InputTag("rescaledMuonsRel","U")
    )
process.muPtEtaRelIDMuDown = process.muPtEtaRelID.clone(
    src = cms.InputTag("rescaledMuonsRel","D")
    )

process.filterSequence = cms.Sequence(
    (process.tauPtEtaIDAgMuAgElecIso       * process.tauPtEtaIDAgMuAgElecIsoCounter) +
    (process.tauPtEtaIDAgMuAgElecIsoTauUp  * process.tauPtEtaIDAgMuAgElecIsoTauUpCounter) +
    (process.tauPtEtaIDAgMuAgElecIsoTauDown* process.tauPtEtaIDAgMuAgElecIsoTauDownCounter) +
    (process.muPtEtaIDIso      * process.muPtEtaIDIsoCounter) +
    (process.muPtEtaIDIsoMuUp  * process.muPtEtaIDIsoMuUpCounter) +
    (process.muPtEtaIDIsoMuDown* process.muPtEtaIDIsoMuDownCounter) +
    (process.muPtEtaRelID+process.muPtEtaRelIDMuUp+process.muPtEtaRelIDMuDown)
    )

#######################################################################


process.muTauStreamAnalyzer = cms.EDAnalyzer(
    "MuTauStreamAnalyzer",
    diTaus         = cms.InputTag("selectedDiTau"),
    jets           = cms.InputTag("selectedPatJets"),
    newJets        = cms.InputTag(""),
    met            = cms.InputTag("patMETsPFlow"),
    rawMet         = cms.InputTag("patMETsPFlow"),
    muons          = cms.InputTag("muPtEtaIDIso"),
    muonsRel       = cms.InputTag("muPtEtaRelID"),
    vertices       = cms.InputTag("selectedPrimaryVertices"),
    triggerResults = cms.InputTag("patTriggerEvent"),
    isMC           = cms.bool(runOnMC),
    deltaRLegJet   = cms.untracked.double(0.5),
    minCorrPt      = cms.untracked.double(15.),
    minJetID       = cms.untracked.double(0.5), # 1=loose,2=medium,3=tight
    verbose        = cms.untracked.bool( False ),
    )
process.muTauStreamAnalyzerJetUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetUp"),
    met    =  cms.InputTag("rescaledMETjet",  "UNNNU"),
    )
process.muTauStreamAnalyzerJetDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetDown"),
    met    =  cms.InputTag("rescaledMETjet",  "DNNND"),
    )
process.muTauStreamAnalyzerMuUp    = process.muTauStreamAnalyzer.clone(
    diTaus   =  cms.InputTag("selectedDiTauMuUp"),
    met      =  cms.InputTag("rescaledMETmuon","NNUNN"),
    muons    =  cms.InputTag("muPtEtaIDIsoMuUp"),
    muonsRel =  cms.InputTag("muPtEtaRelIDMuUp"),
    )
process.muTauStreamAnalyzerMuDown  = process.muTauStreamAnalyzer.clone(
    diTaus   =  cms.InputTag("selectedDiTauMuDown"),
    met      =  cms.InputTag("rescaledMETmuon","NNDNN"),
    muons    =  cms.InputTag("muPtEtaIDIsoMuDown"),
    muonsRel =  cms.InputTag("muPtEtaRelIDMuDown"),
    )
process.muTauStreamAnalyzerTauUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauUp"),
    met    =  cms.InputTag("rescaledMETtau","NNNUN"),
    )
process.muTauStreamAnalyzerTauDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauDown"),
    met    =  cms.InputTag("rescaledMETtau","NNNDN")
    )

#######################################################################

process.allAnalyzers = cms.Sequence(
    process.muTauStreamAnalyzer+
    process.muTauStreamAnalyzerJetUp+
    process.muTauStreamAnalyzerJetDown+
    process.muTauStreamAnalyzerMuUp+
    process.muTauStreamAnalyzerMuDown+
    process.muTauStreamAnalyzerTauUp+
    process.muTauStreamAnalyzerTauDown
    )

#######################################################################


process.analysis = cms.Sequence(
    process.allEventsFilter*
    process.filterSequence*
    process.rescaledObjects*
    process.allDiTau*
    process.allAnalyzers
    )

process.pNominal = cms.Path(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.rescaledObjects*
    process.diTau*process.selectedDiTau*process.selectedDiTauCounter*
    process.muTauStreamAnalyzer
    )
'''
process.pJetUp = cms.Path(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.rescaledObjects*
    process.diTauJetUp*process.selectedDiTauJetUp*process.selectedDiTauJetUpCounter*
    process.muTauStreamAnalyzerJetUp
    )
process.pJetDown = cms.Path(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.rescaledObjects*
    process.diTauJetDown*process.selectedDiTauJetDown*process.selectedDiTauJetDownCounter*
    process.muTauStreamAnalyzerJetDown
    )
process.pMuUp = cms.Path(
    process.allEventsFilter*
    process.muPtEtaIDIso *
    process.muPtEtaRelID *
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    process.rescaledObjects*
    (process.muPtEtaIDIsoMuUp*process.muPtEtaIDIsoMuUpCounter) *
    process.muPtEtaRelIDMuUp *
    process.diTauMuUp*process.selectedDiTauMuUp*process.selectedDiTauMuUpCounter*
    process.muTauStreamAnalyzerMuUp
    )
process.pMuDown = cms.Path(
    process.allEventsFilter*
    process.muPtEtaIDIso *
    process.muPtEtaRelID *
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    process.rescaledObjects*
    (process.muPtEtaIDIsoMuDown*process.muPtEtaIDIsoMuDownCounter) *
    process.muPtEtaRelIDMuDown *
    process.diTauMuDown*process.selectedDiTauMuDown*process.selectedDiTauMuDownCounter*
    process.muTauStreamAnalyzerMuDown
    )
process.pTauUp = cms.Path(
    process.allEventsFilter*
    (process.muPtEtaIDIso*process.muPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuAgElecIso*
    process.muPtEtaRelID *
    process.rescaledObjects*
    (process.tauPtEtaIDAgMuAgElecIsoTauUp*process.tauPtEtaIDAgMuAgElecIsoTauUpCounter)*
    process.diTauTauUp*process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter*
    process.muTauStreamAnalyzerTauUp
    )
process.pTauDown = cms.Path(
    process.allEventsFilter*
    (process.muPtEtaIDIso*process.muPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuAgElecIso*
    process.muPtEtaRelID *
    process.rescaledObjects*
    (process.tauPtEtaIDAgMuAgElecIsoTauDown*process.tauPtEtaIDAgMuAgElecIsoTauDownCounter)*
    process.diTauTauDown*process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter*
    process.muTauStreamAnalyzerTauDown
    )
'''

process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring( 'drop *'),
    fileName = cms.untracked.string('patTuplesSkimmed_MuTauStream.root'),
    )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("treeMuTauStream.root")
                                   )

process.outpath = cms.EndPath()
