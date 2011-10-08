import FWCore.ParameterSet.Config as cms

process = cms.Process("MUTAUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

#process.load("CondCore.DBCommon.CondDBCommon_cfi")
#process.jec = cms.ESSource("PoolDBESSource",
#      DBParameters = cms.PSet(
#        messageLevel = cms.untracked.int32(0)
#        ),
#      timetype = cms.string('runnumber'),
#      toGet = cms.VPSet(
#      cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_Jec11V2_AK5PF'),
#            label  = cms.untracked.string('AK5PF')
#            )
#      ),
#      connect = cms.string('sqlite:Jec11V2.db')
#)
#process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')


runOnMC = False

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START42_V13::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_R_42_V19::All')
    
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 2000
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/VBF_HToTauTau_M-120_7TeV-powheg-pythia6-tauola/MuTauStream/9d70b246dfc2095f4b7f776df63d9adc/patTuples_MuTauStream_9_1_u2S.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/MuTauStream/9d70b246dfc2095f4b7f776df63d9adc//patTuples_MuTauStream_653_1_J0x.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/TauPlusX/MuTauStream-v6/19b210cddfa58b4f9a99f3faa9d787aa/patTuples_MuTauStream_91_1_VIQ.root'
    )
    )

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
    )

process.allEventsFilter = cms.EDFilter(
    "AllEventsFilter"
    )

###################################################################################
process.rescaledMET = cms.EDProducer(
    "MEtRescalerProducer",
    metTag = cms.InputTag("patMETsPFlow"),
    jetTag = cms.InputTag("selectedPatJetsRemake"),
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
    shift = cms.double(0.03)
    )
process.rescaledMuons = cms.EDProducer(
    "MuonRescalerProducer",
    inputCollection = cms.InputTag("muPtEtaIDIso"),
    shift = cms.double(0.01)
    )

process.rescaledObjects = cms.Sequence(
    process.rescaledMETjet+
    process.rescaledMETtau+
    process.rescaledMETmuon+
    process.rescaledTaus+
    process.rescaledMuons
    )
###################################################################################
process.load('RecoJets.Configuration.RecoPFJets_cff')
process.kt6PFJets.doRhoFastjet = True
process.kt6PFJets.Rho_EtaMax = cms.double(4.4)
process.kt6PFJets.Ghost_EtaMax = cms.double(5.0)
process.ak5PFJets.doAreaFastjet = True

process.load("PhysicsTools.PatAlgos.producersLayer1.jetProducer_cff")
process.load("PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi")

from PhysicsTools.PatAlgos.tools.helpers import *
massSearchReplaceAnyInputTag(process.makePatJets,
                             "ak5CaloJets",
                             "ak5PFJets",verbose=False)

process.patJetCorrFactors.payload = cms.string('AK5PF')
JEClevels = cms.vstring(['L2Relative', 'L3Absolute'])
if runOnMC:
    JEClevels = ['L1FastJet', 'L2Relative', 'L3Absolute']
else:
    JEClevels = ['L1FastJet', 'L2Relative', 'L3Absolute', 'L2L3Residual']

process.patJetCorrFactors.levels = JEClevels
process.patJetCorrFactors.rho = cms.InputTag('kt6PFJets','rho')
process.patJetCorrFactors.useRho = True

process.patJetCorrFactorsL1Offset = process.patJetCorrFactors.clone(
    levels = cms.vstring('L1Offset',
                         'L2Relative',
                         'L3Absolute')
    )
if runOnMC:
    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute']
else:
    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual']
process.patJets.jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactors"),cms.InputTag("patJetCorrFactorsL1Offset"))


process.patJets.addBTagInfo = False
process.patJets.embedCaloTowers = False
process.patJets.getJetMCFlavour = False
process.patJets.addDiscriminators = False
process.patJets.addJetID = False
process.patJets.embedGenPartonMatch = False
process.patJets.embedPFCandidates = False
process.patJets.embedGenJetMatch = False
process.patJets.addAssociatedTracks = False
process.patJets.addGenPartonMatch = False
process.patJets.addGenJetMatch = False
process.patJets.addJetCharge = False

process.makePatJets.replace(process.patJetCorrFactors,
                            process.patJetCorrFactors+process.patJetCorrFactorsL1Offset)
process.makePatJets.remove(process.patJetCharge)
process.makePatJets.remove(process.patJetPartonMatch)
process.makePatJets.remove(process.patJetGenJetMatch)
process.makePatJets.remove(process.patJetPartons)
process.makePatJets.remove(process.patJetPartonAssociation)
process.makePatJets.remove(process.patJetFlavourAssociation)

process.selectedPatJetsRemake = process.selectedPatJets.clone()
getattr(process,"selectedPatJetsRemake").cut = cms.string('correctedJet("Uncorrected","none","patJetCorrFactors").pt>10 && abs(eta)<5.0')

process.remakePatJets = cms.Sequence(
    process.kt6PFJets*
    process.ak5PFJets*
    process.makePatJets *
    process.selectedPatJetsRemake
    )

####################################################################################
from LLRAnalysis.Utilities.customizePAT import addSelectedPFlowParticle
addSelectedPFlowParticle(process,verbose=False)

process.load('RecoJets.JetProducers.kt4PFJets_cfi')
process.kt6PFJetsCentral = process.kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True )
process.kt6PFJetsCentral.Rho_EtaMax = cms.double(2.5)
process.kt6PFJetsNeutral = process.kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True, src="pfAllNeutral" )

process.pfAllNeutral = cms.EDFilter(
    "PdgIdPFCandidateSelector",
    src = cms.InputTag("particleFlow"),
    pdgId = cms.vint32(111, 130, 310, 2112, 22)
    )

process.computeRhoNeutral = cms.Sequence(
    process.pfAllNeutral*
    process.kt6PFJetsNeutral
    )
####################################################################################

doSVFitReco = True

process.load("LLRAnalysis.Utilities.diTausReconstruction_cff")
process.diTau = process.allMuTauPairs.clone()
process.diTau.srcLeg1 = cms.InputTag("muPtEtaIDIso")
process.diTau.srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso")
process.diTau.srcMET  = cms.InputTag("patMETsPFlow")
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

process.diTauJetDown =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("rescaledMETjet",  "DNNND")
                                            )
process.selectedDiTauJetDown = process.selectedDiTau.clone(src = cms.InputTag("diTauJetDown") )


process.diTauMuUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                        srcLeg1 = cms.InputTag("rescaledMuons","U"),
                                        srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                        srcMET  = cms.InputTag("rescaledMETmuon","NNUNN")
                                        )
process.selectedDiTauMuUp = process.selectedDiTau.clone(src = cms.InputTag("diTauMuUp") )

process.diTauMuDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("rescaledMuons","D"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("rescaledMETmuon","NNDNN")
                                          )
process.selectedDiTauMuDown = process.selectedDiTau.clone(src = cms.InputTag("diTauMuDown") )



process.diTauTauUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                         srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                         srcLeg2 = cms.InputTag("rescaledTaus", "U"),
                                         srcMET  = cms.InputTag("rescaledMETtau","NNNUN")
                                         )
process.selectedDiTauTauUp = process.selectedDiTau.clone(src = cms.InputTag("diTauTauUp") )

process.diTauTauDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                           srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                           srcLeg2 = cms.InputTag("rescaledTaus", "D"),
                                           srcMET  = cms.InputTag("rescaledMETtau","NNNDN")
                                           )
process.selectedDiTauTauDown = process.selectedDiTau.clone(src = cms.InputTag("diTauTauDown") )


process.allDiTau = cms.Sequence(
    (process.diTau*process.selectedDiTau*process.selectedDiTauCounter)+
    (process.diTauJetUp*process.selectedDiTauJetUp + process.diTauJetDown*process.selectedDiTauJetDown) +
    (process.diTauMuUp*process.selectedDiTauMuUp + process.diTauMuDown*process.selectedDiTauMuDown) +
    (process.diTauTauUp*process.selectedDiTauTauUp + process.diTauTauDown*process.selectedDiTauTauDown)
    )
#######################################################################

process.tauPtEtaIDAgMuAgElecIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("tauID('byLooseCombinedIsolationDeltaBetaCorr')>0.5"),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.muPtEtaIDIso  = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("muPtEtaID"),
    cut = cms.string("userFloat('PFRelIsoDB04v2')<0.1"),
    filter = cms.bool(False)
    )
process.muPtEtaIDIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("muPtEtaIDIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.filterSequence = cms.Sequence(
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter) +
    (process.muPtEtaIDIso*process.muPtEtaIDIsoCounter)
    )

#######################################################################


process.muTauStreamAnalyzer = cms.EDAnalyzer(
    "MuTauStreamAnalyzer",
    diTaus         = cms.InputTag("selectedDiTau"),
    jets           = cms.InputTag("selectedPatJets"),
    newJets        = cms.InputTag("patJets"),
    met            = cms.InputTag("patMETsPFlow"),
    rawMet         = cms.InputTag("patMETsPFlow"),
    muons          = cms.InputTag("muPtEtaID"),
    muonsRel       = cms.InputTag("muPtEtaRelID"),
    vertices       = cms.InputTag("offlinePrimaryVertices"),
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
    diTaus =  cms.InputTag("selectedDiTauMuUp"),
    met    =  cms.InputTag("rescaledMETmuon","NNUNN")
    )
process.muTauStreamAnalyzerMuDown  = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMuDown"),
    met    =  cms.InputTag("rescaledMETmuon","NNDNN")
    )
process.muTauStreamAnalyzerTauUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauUp"),
    met    =  cms.InputTag("rescaledMETtau","NNNUN")
    )
process.muTauStreamAnalyzerTauDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauDown"),
    met    =  cms.InputTag("rescaledMETtau","NNNDN")
    )

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
    process.computeRhoNeutral*
    process.remakePatJets*
    process.rescaledObjects*
    process.allDiTau*
    process.allAnalyzers
    )

process.p = cms.Path(
    process.analysis
    )

process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring( 'drop *'),
    fileName = cms.untracked.string('patTuplesSkimmed_MuTauStream.root'),
    )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("treeMuTauStream.root")
                                   )

process.outpath = cms.EndPath()
