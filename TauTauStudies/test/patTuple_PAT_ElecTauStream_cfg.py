from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 2000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

process.load("CondCore.DBCommon.CondDBCommon_cfi")
process.jec = cms.ESSource("PoolDBESSource",
      DBParameters = cms.PSet(
        messageLevel = cms.untracked.int32(0)
        ),
      timetype = cms.string('runnumber'),
      toGet = cms.VPSet(
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_Jec11V2_AK5PF'),
            label  = cms.untracked.string('AK5PF')
            )
      ),
      connect = cms.string('sqlite:Jec11V2.db')
)
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')


process.load("RecoTauTag.Configuration.RecoPFTauTag_cff")
process.load('RecoJets.Configuration.RecoPFJets_cff')
process.kt6PFJets.doRhoFastjet = True
process.kt6PFJets.Rho_EtaMax = cms.double(4.4)
process.kt6PFJets.Ghost_EtaMax = cms.double(5.0)
process.ak5PFJets.doAreaFastjet = True
#process.ak5PFJets.Rho_EtaMax = cms.double(4.4)
#process.ak5PFJets.Ghost_EtaMax = cms.double(5.0)

## re-run kt4PFJets within lepton acceptance to compute rho
process.load('RecoJets.JetProducers.kt4PFJets_cfi')
process.kt6PFJetsCentral = process.kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True )
process.kt6PFJetsCentral.Rho_EtaMax = cms.double(2.5)
process.kt6PFJetsNeutral = process.kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True, src="pfAllNeutral" )

#process.ak5PFL1Fastjet.useCondDB = False

process.fjSequence = cms.Sequence(process.kt6PFJets+process.ak5PFJets+process.kt6PFJetsCentral)

process.source.fileNames = cms.untracked.vstring(
    'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Summer11/WH_ZH_TTH_HToWW_M-120_7TeV-pythia6//AODSIM/PU_S3_START42_V11-v1//0000/E430E306-347C-E011-A75A-00261834B5C6.root'
    )

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:1080','1:2003','1:2028','1:6867','1:7016'
#    )

postfix           = "PFlow"
runOnMC           =  True

if runOnMC:
    #process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )
    process.GlobalTag.globaltag = cms.string('START42_V12::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_R_42_V14::All')

process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.printTree1 = cms.EDAnalyzer("ParticleListDrawer",
                                    src = cms.InputTag("genParticles"),
                                    maxEventsToPrint  = cms.untracked.int32(1)
                                    )

process.primaryVertexFilter = cms.EDFilter(
    "GoodVertexFilter",
    vertexCollection = cms.InputTag('offlinePrimaryVertices'),
    minimumNDOF = cms.uint32(4) ,
    maxAbsZ = cms.double(24),
    maxd0 = cms.double(2)
    )

process.scrapping = cms.EDFilter(
    "FilterOutScraping",
    applyfilter = cms.untracked.bool(True),
    debugOn = cms.untracked.bool(False),
    numtrack = cms.untracked.uint32(10),
    thresh = cms.untracked.double(0.25)
    )

process.allEventsFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.vertexScrapingFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.atLeastOneElecTauFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.elecPtEtaFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.elecPtEtaIDFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.tauPtEtaFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.tauPtEtaIDFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.tauPtEtaIDAgMuFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.tauPtEtaIDAgMuAgElecFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.tauPtEtaIDAgMuAgElecCrackRemFilter = cms.EDFilter(
    "AllEventsFilter"
    )




from PhysicsTools.PatAlgos.tools.coreTools import *

if not runOnMC:
    removeMCMatching(process,["All"])
    
removeSpecificPATObjects(process, ['Photons'],
                         outputInProcess=False)
removeCleaning(process,
               outputInProcess=False)

restrictInputToAOD(process, ['All'])

from LLRAnalysis.Utilities.customizePAT  import *
addSelectedPFlowParticle(process)

from PhysicsTools.PatAlgos.tools.metTools import *
addPfMET(process, postfix)

from PhysicsTools.PatAlgos.tools.jetTools import *
switchJetCollection(process,cms.InputTag('ak5PFJets'),
                    doJTA        = True,
                    doBTagging   = True,
                    jetCorrLabel = ('AK5PF', ['L2Relative', 'L3Absolute']),
                    doType1MET   = False,
                    genJetCollection=cms.InputTag("ak5GenJets"),
                    doJetID      = True,
                    jetIdLabel   = 'ak5'
                    )

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
process.patDefaultSequence.replace(process.patJetCorrFactors,
                                   process.patJetCorrFactors+process.patJetCorrFactorsL1Offset)



if runOnMC:
    process.load("RecoJets.Configuration.GenJetParticles_cff")
    process.load("RecoJets.Configuration.RecoGenJets_cff")
    process.genJetsNoNu = cms.Sequence(process.genParticlesForJetsNoNu*
                                       process.ak5GenJetsNoNu)
    process.patDefaultSequence.replace(process.patJetGenJetMatch,
                                       process.genJetsNoNu*
                                       process.patJetGenJetMatch)
    process.patJetGenJetMatch.matched = cms.InputTag("ak5GenJetsNoNu")

## <tau part>
from PhysicsTools.PatAlgos.tools.tauTools import *
switchToPFTauHPS(process, 
                 pfTauLabelOld = 'shrinkingConePFTauProducer',
                 pfTauLabelNew = 'hpsPFTauProducer'
                 )

getattr(process,"patTaus").embedIsolationTracks = cms.bool(True)
getattr(process,"patTaus").embedSignalTracks = cms.bool(True)
getattr(process,"patTaus").embedGenMatch = cms.bool(True)
getattr(process,"patTaus").embedLeadTrack = cms.bool(True)
getattr(process,"patTaus").embedLeadPFCand = True
getattr(process,"patTaus").embedLeadPFChargedHadrCand = True
getattr(process,"patTaus").embedLeadPFNeutralCand = True
getattr(process,"patTaus").embedSignalPFCands = True
getattr(process,"patTaus").embedSignalPFChargedHadrCands = True
getattr(process,"patTaus").embedSignalPFNeutralHadrCands = True
getattr(process,"patTaus").embedSignalPFGammaCands = True
getattr(process,"patTaus").embedIsolationPFCands = True
getattr(process,"patTaus").embedIsolationPFChargedHadrCands = True
getattr(process,"patTaus").embedIsolationPFNeutralHadrCands = True
getattr(process,"patTaus").embedIsolationPFGammaCands = True
getattr(process,"patTaus").embedGenJetMatch = cms.bool(True)


from RecoTauTag.RecoTau.PFRecoTauDiscriminationAgainstElectron_cfi import pfRecoTauDiscriminationAgainstElectron
process.hpsPFTauDiscriminationAgainstElectronCrackRem = pfRecoTauDiscriminationAgainstElectron.clone(
    PFTauProducer = cms.InputTag('hpsPFTauProducer'),
    Prediscriminants = cms.PSet(
    BooleanOperator = cms.string("and"),
    leadTrack = cms.PSet(
    Producer = cms.InputTag('hpsPFTauDiscriminationByDecayModeFinding'),
    cut = cms.double(0.5)
    )
    ),
    ApplyCut_EcalCrackCut = cms.bool(True),
    ApplyCut_PFElectronMVA =  cms.bool(False)
    )

# add the crack-rem sequence before running patTau
process.patDefaultSequence.replace(process.patTaus,
                                   process.hpsPFTauDiscriminationAgainstElectronCrackRem+process.patTaus)

getattr(process,"patTaus").tauIDSources = cms.PSet(
    getattr(process,"patTaus").tauIDSources,
    cms.PSet(againstElectronCrackRem = cms.InputTag("hpsPFTauDiscriminationAgainstElectronCrackRem"))
    )

process.tauMatch.maxDeltaR = 0.15
process.tauMatch.resolveAmbiguities = cms.bool(False)
process.tauGenJetMatch.resolveAmbiguities = cms.bool(False)
process.tauGenJetMatch.maxDeltaR = 0.15
process.tauGenJetMatch.maxDPtRel = 999
## <\tau part>

addPFMuonIsolation(process,process.patMuons)

#process.pfPileUp.Vertices = "offlinePrimaryVerticesDA"

addTriggerMatchingMuon(process,isMC=runOnMC)
getattr(process,"patMuons").embedTrack = True

from LLRAnalysis.Utilities.electrons import *
addCutBasedID(process)
addPFElectronIsolation(process,process.patElectrons)

getattr(process,"patElectrons").embedTrack = True
getattr(process,"patElectrons").embedGsfTrack = True
addTriggerMatchingElectron(process,isMC=runOnMC)

addTriggerMatchingTau(process,isMC=runOnMC,postfix="",XtriggerMu=False)

from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )
process.patTriggerEvent.processName = '*'

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'

# SC sequence
process.mergedSuperClusters = cms.EDProducer(
    "SuperClusterMerger",
    src = cms.VInputTag(cms.InputTag("correctedHybridSuperClusters"),
                        cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"))
    )
process.selectedSuperClusters = cms.EDFilter(
    "SuperClusterSelector",
    src = cms.InputTag("mergedSuperClusters"),
    cut = cms.string("energy()*sin(position().theta()) > 4"),
    filter = cms.bool(False)
    )
process.makeSCs = cms.Sequence(process.mergedSuperClusters*process.selectedSuperClusters)

    
########### PAT

simpleCutsWP95 = "(userFloat('nHits')<=1  && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.8 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.7 && userFloat('dEta')<0.01 && userFloat('HoE')<0.07) ))"

simpleCutsWP80 = "(userFloat('nHits')==0 && userInt('antiConv')>0.5 &&  ( (pt>=20 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.06 && userFloat('dEta')<0.004 && userFloat('HoE')<0.04) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.007 && userFloat('HoE')<0.025) )) || (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.004 && userFloat('HoE')<0.025) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.02 && userFloat('dEta')<0.005 && userFloat('HoE')<0.025) ) )  ) )"

#likelihoodWP95 = "(userFloat('nHits')==0 && ( (isEB && ((numberOfBrems==0 && electronID('electronIDLH')>-4.274) || (numberOfBrems>0 && electronID('electronIDLH')>-3.773 ) )  )  || (isEE && ((numberOfBrems==0 && electronID('electronIDLH')>-5.092) || (numberOfBrems>0 && electronID('electronIDLH')>-2.796 ) )) ) )"
#likelihoodWP80 = "(userFloat('nHits')==0 && userFloat('dist')>0.02 && userFloat('dcot')>0.02 && ( (isEB && ((numberOfBrems==0 && electronID('electronIDLH')>1.193) || (numberOfBrems>0 && electronID('electronIDLH')>1.345 ) ) )  || (isEE && ((numberOfBrems==0 && electronID('electronIDLH')>0.810) || (numberOfBrems>0 && electronID('electronIDLH')>3.021) )) ) )"

process.selectedPatElectronsTriggerMatchUserEmbedded = cms.EDProducer(
    "ElectronsUserEmbedded",
    electronTag = cms.InputTag("selectedPatElectronsTriggerMatch"),
    vertexTag = cms.InputTag("offlinePrimaryVertices"),
    isMC = cms.bool(runOnMC)
    )

process.selectedPatTausTriggerMatchUserEmbedded = cms.EDProducer(
    "TausUserEmbedded",
    tauTag = cms.InputTag("selectedPatTausTriggerMatch"),
    vertexTag = cms.InputTag("offlinePrimaryVertices"),
    )

process.atLeastOneElecTau = cms.EDProducer(
    "CandViewShallowCloneCombiner",
    decay = cms.string("selectedPatElectronsTriggerMatchUserEmbedded selectedPatTausTriggerMatchUserEmbedded"),
    cut = cms.string("sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  ) *  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  )  )>0.5"),
    checkCharge = cms.bool(False)
    )

process.atLeastOneElecTauCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.elecPtEta = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>15 && abs(eta)<2.1 && !isEBEEGap"),
    filter = cms.bool(False)
    )
process.atLeastOneElecTauelecPtEta = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEta selectedPatTausTriggerMatchUserEmbedded")
    )
process.elecPtEtaCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTauelecPtEta"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.elecPtEtaID = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
    cut = cms.string(process.elecPtEta.cut.value()+" && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2 &&"+simpleCutsWP80),
    filter = cms.bool(False)
    )
process.atLeastOneElecTauelecPtEtaID = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID selectedPatTausTriggerMatchUserEmbedded")
    )
process.elecPtEtaIDCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTauelecPtEtaID"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.elecPtEtaRelID = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>15 && abs(eta)<2.4 && !isEBEEGap && "+simpleCutsWP95),
    filter = cms.bool(False)
    )

process.tauPtEta  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausTriggerMatchUserEmbedded"),
    cut = cms.string("pt>20 && (eta<2.3&&eta>-2.3)"),
    filter = cms.bool(False)
    )
process.atLeastOneElecTautauPtEta = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEta")
    )
process.tauPtEtaCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEta"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )




process.tauPtEtaID  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausTriggerMatchUserEmbedded"),
    cut = cms.string(process.tauPtEta.cut.value()+" && tauID('decayModeFinding')>0.5 && userFloat('dzWrtPV')<0.2"),
    filter = cms.bool(False)
    )
process.atLeastOneElecTautauPtEtaID = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEtaID")
    )
process.tauPtEtaIDCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEtaID"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.tauPtEtaIDAgMu  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausTriggerMatchUserEmbedded"),
    cut = cms.string(process.tauPtEtaID.cut.value()+" && tauID('againstMuonLoose')>0.5"),
    filter = cms.bool(False)
    )
process.atLeastOneElecTautauPtEtaIDAgMu = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEtaIDAgMu")
    )
process.tauPtEtaIDAgMuCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEtaIDAgMu"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.tauPtEtaIDAgMuAgElec  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausTriggerMatchUserEmbedded"),
    cut = cms.string(process.tauPtEtaIDAgMu.cut.value()+" && tauID('againstElectronTight')>0.5"),
    filter = cms.bool(False)
    )
process.atLeastOneElecTautauPtEtaIDAgMuAgElec = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEtaIDAgMuAgElec")
    )
process.tauPtEtaIDAgMuAgElecCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEtaIDAgMuAgElec"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.tauPtEtaIDAgMuAgElecCrackRem  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausTriggerMatchUserEmbedded"),
    cut = cms.string(process.tauPtEtaIDAgMuAgElec.cut.value()+" && tauID('againstElectronCrackRem')>0.5"),
    filter = cms.bool(False)
    )
process.atLeastOneElecTautauPtEtaIDAgMuAgElecCrackRem = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEtaIDAgMuAgElecCrackRem")
    )
process.tauPtEtaIDAgMuAgElecCrackRemCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEtaIDAgMuAgElecCrackRem"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.atLeastOneGoodVertexSequence = cms.Sequence(
    process.primaryVertexFilter*process.vertexScrapingFilter
    )
#process.PFTau.replace(process.offlinePrimaryVerticesDA,
#                      process.offlinePrimaryVerticesDA*process.atLeastOneGoodVertexSequence)

process.alLeastOneElecTauSequence = cms.Sequence(
    process.atLeastOneElecTau*process.atLeastOneElecTauCounter*process.atLeastOneElecTauFilter
    )

process.elecLegSequence = cms.Sequence(
    (process.elecPtEta*process.atLeastOneElecTauelecPtEta*process.elecPtEtaCounter*process.elecPtEtaFilter) *
    (process.elecPtEtaID*process.atLeastOneElecTauelecPtEtaID*process.elecPtEtaIDCounter*process.elecPtEtaIDFilter) *
    process.elecPtEtaRelID
    )
process.tauLegSequence = cms.Sequence(
    (process.tauPtEta*process.atLeastOneElecTautauPtEta*process.tauPtEtaCounter*process.tauPtEtaFilter) *
    (process.tauPtEtaID*process.atLeastOneElecTautauPtEtaID*process.tauPtEtaIDCounter*process.tauPtEtaIDFilter) *
    (process.tauPtEtaIDAgMu*process.atLeastOneElecTautauPtEtaIDAgMu*process.tauPtEtaIDAgMuCounter*process.tauPtEtaIDAgMuFilter)*
    (process.tauPtEtaIDAgMuAgElec*process.atLeastOneElecTautauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecCounter*process.tauPtEtaIDAgMuAgElecFilter)*
    (process.tauPtEtaIDAgMuAgElecCrackRem*process.atLeastOneElecTautauPtEtaIDAgMuAgElecCrackRem*process.tauPtEtaIDAgMuAgElecCrackRemCounter*process.tauPtEtaIDAgMuAgElecCrackRemFilter)
    )

process.load("LLRAnalysis.Utilities.diTausReconstruction_cff")
process.diTau = process.allElecTauPairs.clone()
process.diTau.srcLeg1 = cms.InputTag("elecPtEtaID")
process.diTau.srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecCrackRem")
process.diTau.srcMET  = cms.InputTag("patMETsPFlow")
process.diTau.dRmin12  = cms.double(0.5)
process.diTau.doSVreco = cms.bool(False)

if not runOnMC:
    process.diTau.srcGenParticles = ""


process.selectedDiTau = cms.EDFilter(
    "ElecTauPairSelector",
    src = cms.InputTag("diTau"),
    #cut = cms.string("charge==0 && mt1MET<40")
    cut = cms.string("dR12>0.5")
    )

process.atLeast1selectedDiTau = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("selectedDiTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.diTauSequence = cms.Sequence(
    process.diTau*
    process.selectedDiTau*
    process.atLeast1selectedDiTau
    )


getattr(process,"selectedPatJets").cut = cms.string('pt>15 && abs(eta)<5.0')

process.deltaRJetElectrons = cms.EDProducer(
    "DeltaRNearestElectronComputer",
    probes = cms.InputTag("selectedPatJets"),
    #objects = cms.InputTag("electronLeg"),
    objects = cms.InputTag("elecPtEtaID"),
    )
process.selectedPatJetsNoElectrons = cms.EDProducer(
    "JetsCleaner",
    jets =  cms.InputTag("selectedPatJets"),
    valueMap = cms.InputTag("deltaRJetElectrons"),
    minDeltaR = cms.double(0.3)
    )

process.deltaRJetTaus = cms.EDProducer(
    "DeltaRNearestTauComputer",
    probes = cms.InputTag("selectedPatJetsNoElectrons"),
    #objects = cms.InputTag("tauLeg"),
    objects = cms.InputTag("tauPtEtaIDAgMuAgElecCrackRem"),
    )
process.selectedPatJetsNoElectronsNoTaus = cms.EDProducer(
    "JetsCleaner",
    jets =  cms.InputTag("selectedPatJetsNoElectrons"),
    valueMap = cms.InputTag("deltaRJetTaus"),
    minDeltaR = cms.double(0.3)
    )

process.jetCleaningSequence = cms.Sequence(
    process.deltaRJetElectrons*process.selectedPatJetsNoElectrons*process.deltaRJetTaus*process.selectedPatJetsNoElectronsNoTaus
    )

process.offlinePrimaryVertices = cms.EDProducer(
    "PrimaryVertexProducer",
    PVSelParameters = cms.PSet(
    maxDistanceToBeam = cms.double(1.0)
    ),
    verbose = cms.untracked.bool(False),
    algorithm = cms.string('AdaptiveVertexFitter'),
    TkFilterParameters = cms.PSet(
    maxNormalizedChi2 = cms.double(20.0),
    minPt = cms.double(0.0),
    algorithm = cms.string('filter'),
    maxD0Significance = cms.double(5.0),
    trackQuality = cms.string('any'),
    minPixelLayersWithHits = cms.int32(2),
    minSiliconLayersWithHits = cms.int32(5)
    ),
    beamSpotLabel = cms.InputTag("offlineBeamSpot"),
    TrackLabel = cms.InputTag("generalTracks"),
    useBeamConstraint = cms.bool(False),
    minNdof = cms.double(0.0),
    TkClusParameters = cms.PSet(
    TkDAClusParameters = cms.PSet(
    dzCutOff = cms.double(4.0),
    d0CutOff = cms.double(3.0),
    Tmin = cms.double(9.0),
    coolingFactor = cms.double(0.8),
    vertexSize = cms.double(0.05)
    ),
    algorithm = cms.string('DA')
    )
    )

process.elecTauStreamAnalyzer = cms.EDAnalyzer(
    "ElecTauStreamAnalyzer",
    diTaus =  cms.InputTag("selectedDiTau"),
    #jets =  cms.InputTag("selectedPatJetsNoElectrons"),
    jets =  cms.InputTag("selectedPatJets"),
    triggerResults = cms.InputTag("patTriggerEvent"),
    isMC = cms.bool(runOnMC),
    deltaRLegJet  = cms.untracked.double(0.5),
    minCorrPt = cms.untracked.double(15.),
    minJetID  = cms.untracked.double(0.5), # 1=loose,2=medium,3=tight
    applyTauSignalSel =  cms.bool( True ),
    verbose =  cms.untracked.bool( False ),
    )

process.pat = cms.Sequence(
    process.allEventsFilter+
    #process.offlinePrimaryVertices*
    process.atLeastOneGoodVertexSequence*
    #process.makeSCs +
    process.PFTau*
    process.fjSequence*
    process.patDefaultSequence*
    process.kt6PFJetsNeutral*
    process.selectedPatElectronsTriggerMatchUserEmbedded*
    process.selectedPatTausTriggerMatchUserEmbedded*
    process.alLeastOneElecTauSequence*
    process.elecLegSequence*
    process.tauLegSequence*
    process.diTauSequence*
    process.jetCleaningSequence*
    process.elecTauStreamAnalyzer+
    process.printTree1
    )

#massSearchReplaceAnyInputTag(process.pat,
#                             "offlinePrimaryVertices",
#                             "offlinePrimaryVerticesDA",verbose=True)

if not runOnMC:
    process.pat.remove(process.printTree1)

process.p = cms.Path(process.pat)

from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out.outputCommands = cms.untracked.vstring('drop *',
                                                   *patEventContentNoCleaning ) 
process.out.outputCommands.extend( cms.vstring(
    'keep *_TriggerResults_*_*',
    'keep *_hltTriggerSummaryAOD_*_*',
    'keep recoGenParticles_genParticles*_*_*',
    'keep *_patTriggerEvent_*_*',
    'keep *_patTrigger_*_*',
    'keep *_selectedPatJets_*_*',
    'keep *_ak5PFJets_*_*',
    'keep *_particleFlow__*',
    'keep *_offlinePrimaryVerticesDA_*_*',
    'keep *_offlinePrimaryVertices_*_*',
    'keep *_offlinePrimaryVerticesWithBS_*_*',
    'keep *_offlineBeamSpot_*_*',
    'keep *_patMETsPFlow_*_*',
    'keep *_tauGenJetsSelectorAllHadrons_*_*',
    'keep *_kt6PFJetsCentral_rho_*',
    'keep *_kt6PFJetsNeutral_rho_*',
    'keep *_elecPtEtaID_*_*',
    'keep *_elecPtEtaRelID_*_*',
    'keep *_addPileupInfo_*_*',
    'keep *_generalTracks_*_*',
    'keep *_electronGsfTracks_*_*',
    'keep recoTrackExtras_*_*_*',
    'keep recoGsfTrackExtras_*_*_*',
    'drop *_TriggerResults_*_HLT',
    'drop *_TriggerResults_*_RECO',
    'drop *_selectedPatElectrons_*_*',
    'drop *_selectedPatMuons_*_*',
    'drop *_selectedPatTaus_*_*',
    'drop *_selectedPatElectronsTriggerMatch_*_*',
    'drop *_selectedPatMuonsTriggerMatch_*_*',
    'drop *_selectedPatTausTriggerMatch_*_*',
    )
                                   )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("treeElecTauStream.root")
                                   )

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('p')
    )

process.out.fileName = cms.untracked.string('patTuples_ElecTauStream.root')

#process.outpath = cms.EndPath(process.out)
process.outpath = cms.EndPath()

