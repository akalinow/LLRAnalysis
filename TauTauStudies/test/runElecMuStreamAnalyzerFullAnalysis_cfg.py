import FWCore.ParameterSet.Config as cms

process = cms.Process("ELECMUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

runOnMC = False

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START42_V13::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_R_42_V19::All')
    
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    'file:./patTuples_ElecMuStream.root'
    #'rfio:rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/TauPlusX/ElecTauStream-v6/add82882179501750b106d9900e51989/patTuples_ElecTauStream_45_1_UZL.root',
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
    electronTag = cms.InputTag("elecPtEtaIDIso"),
    muonTag = cms.InputTag("muPtEtaIDIso"),
    tauTag = cms.InputTag(""),
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
process.rescaledMETelectron = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift = cms.double(0.0),
    muonShift = cms.double(0.0),
    electronShift = cms.double(0.02),
    )
process.rescaledMETmuon = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift = cms.double(0.0),
    muonShift = cms.double(0.01),
    electronShift = cms.double(0.0),
    )

process.rescaledElectrons = cms.EDProducer(
    "ElectronRescalerProducer",
    inputCollection = cms.InputTag("elecPtEtaIDIso"),
    shift = cms.double(0.02)
    )
process.rescaledMuons = cms.EDProducer(
    "MuonRescalerProducer",
    inputCollection = cms.InputTag("muPtEtaIDIso"),
    shift = cms.double(0.01)
    )

process.rescaledObjects = cms.Sequence(
    process.rescaledMETjet+
    process.rescaledMETelectron+
    process.rescaledMETmuon+
    process.rescaledElectrons+
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
getattr(process,"selectedPatJetsRemake").cut = cms.string('correctedJet("Uncorrected","none","patJetCorrFactors").pt>10 && abs(eta)<4.5')

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

doSVFitReco = False

process.load("LLRAnalysis.Utilities.diTausReconstruction_cff")
process.diTau = process.allElecMuPairs.clone()
process.diTau.srcLeg1 = cms.InputTag("elecPtEtaIDIso")
process.diTau.srcLeg2 = cms.InputTag("muPtEtaIDIso")
process.diTau.srcMET  = cms.InputTag("patMETsPFlow")
process.diTau.dRmin12  = cms.double(0.5)
process.diTau.doSVreco = cms.bool(doSVFitReco)

if not runOnMC:
    process.diTau.srcGenParticles = ""
        
process.selectedDiTau = cms.EDFilter(
    "PATElecMuPairSelector",
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
                                          srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                          srcLeg2 = cms.InputTag("muPtEtaIDIso"),
                                          srcMET  = cms.InputTag("rescaledMETjet",  "UNNNU")
                                          )
process.selectedDiTauJetUp = process.selectedDiTau.clone(src = cms.InputTag("diTauJetUp") )

process.diTauJetDown =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                            srcLeg2 = cms.InputTag("muPtEtaIDIso"),
                                            srcMET  = cms.InputTag("rescaledMETjet",  "DNNND")
                                            )
process.selectedDiTauJetDown = process.selectedDiTau.clone(src = cms.InputTag("diTauJetDown") )


process.diTauElecUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("rescaledElectrons","U"),
                                          srcLeg2 = cms.InputTag("muPtEtaIDIso"),
                                          srcMET  = cms.InputTag("rescaledMETelectron","NUNNN")
                                          )
process.selectedDiTauElecUp = process.selectedDiTau.clone(src = cms.InputTag("diTauElecUp") )

process.diTauElecDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("rescaledElectrons","D"),
                                            srcLeg2 = cms.InputTag("muPtEtaIDIso"),
                                            srcMET  = cms.InputTag("rescaledMETelectron","NDNNN")
                                            )
process.selectedDiTauElecDown = process.selectedDiTau.clone(src = cms.InputTag("diTauElecDown") )

process.diTauMuUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                        srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                        srcLeg2 = cms.InputTag("rescaledMuons","U"),
                                        srcMET  = cms.InputTag("rescaledMETmuon","NNUNN")
                                        )
process.selectedDiTauMuUp = process.selectedDiTau.clone(src = cms.InputTag("diTauMuUp") )

process.diTauMuDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                          srcLeg2 = cms.InputTag("rescaledMuons","D"),
                                          srcMET  = cms.InputTag("rescaledMETmuon","NNDNN")
                                          )
process.selectedDiTauMuDown = process.selectedDiTau.clone(src = cms.InputTag("diTauMuDown") )


process.allDiTau = cms.Sequence(
    (process.diTau*process.selectedDiTau*process.selectedDiTauCounter)+
    (process.diTauJetUp*process.selectedDiTauJetUp + process.diTauJetDown*process.selectedDiTauJetDown) +
    (process.diTauElecUp*process.selectedDiTauElecUp + process.diTauElecDown*process.selectedDiTauElecDown) +
    (process.diTauMuUp*process.selectedDiTauMuUp + process.diTauMuDown*process.selectedDiTauMuDown)
    )
#######################################################################

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
process.elecPtEtaIDIso  = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("elecPtEtaID"),
    cut = cms.string("userFloat('PFRelIsoDB04v2')<0.1"),
    filter = cms.bool(False)
    )
process.elecPtEtaIDIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("elecPtEtaIDIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.filterSequence = cms.Sequence(
    (process.muPtEtaIDIso*process.muPtEtaIDIsoCounter) +
    (process.elecPtEtaIDIso*process.elecPtEtaIDIsoCounter)
    )

#######################################################################


process.elecMuStreamAnalyzer = cms.EDAnalyzer(
    "ElecMuStreamAnalyzer",
    diTaus         = cms.InputTag("selectedDiTau"),
    jets           = cms.InputTag("selectedPatJets"),
    newJets        = cms.InputTag("patJets"),
    met            = cms.InputTag("patMETsPFlow"),
    rawMet         = cms.InputTag("patMETsPFlow"),
    electrons      = cms.InputTag("elecPtEtaID"),
    electronsRel   = cms.InputTag("elecPtEtaRelID"),
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
process.elecMuStreamAnalyzerJetUp     = process.elecMuStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetUp"),
    met    =  cms.InputTag("rescaledMETjet",  "UNNNU"),
    )
process.elecMuStreamAnalyzerJetDown   = process.elecMuStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetDown"),
    met    =  cms.InputTag("rescaledMETjet",  "DNNND"),
    )
process.elecMuStreamAnalyzerElecUp    = process.elecMuStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauElecUp"),
    met    =  cms.InputTag("rescaledMETelectron","NUNNN")
    )
process.elecMuStreamAnalyzerElecDown  = process.elecMuStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauElecDown"),
    met    =  cms.InputTag("rescaledMETelectron","NDNNN")
    )
process.elecMuStreamAnalyzerMuUp    = process.elecMuStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMuUp"),
    met    =  cms.InputTag("rescaledMETmuon","NNUNN")
    )
process.elecMuStreamAnalyzerMuDown  = process.elecMuStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMuDown"),
    met    =  cms.InputTag("rescaledMETmuon","NNDNN")
    )

process.allAnalyzers = cms.Sequence(
    process.elecMuStreamAnalyzer+
    process.elecMuStreamAnalyzerJetUp+
    process.elecMuStreamAnalyzerJetDown+
    process.elecMuStreamAnalyzerElecUp+
    process.elecMuStreamAnalyzerElecDown+
    process.elecMuStreamAnalyzerMuUp+
    process.elecMuStreamAnalyzerMuDown
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
    fileName = cms.untracked.string('patTuplesSkimmed_ElecMuStream.root'),
    )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("treeElecMuStream.root")
                                   )

process.outpath = cms.EndPath()
