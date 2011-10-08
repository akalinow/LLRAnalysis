from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 2000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
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

process.load('RecoJets.Configuration.RecoPFJets_cff')

process.kt6PFJets.doRhoFastjet = True
process.kt6PFJets.Rho_EtaMax = cms.double(4.4)
process.kt6PFJets.Ghost_EtaMax = cms.double(5.0)
process.ak5PFJets.doAreaFastjet = True

## re-run kt4PFJets within lepton acceptance to compute rho
process.load('RecoJets.JetProducers.kt4PFJets_cfi')
process.kt6PFJetsCentral = process.kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True )
process.kt6PFJetsCentral.Rho_EtaMax = cms.double(2.5)
process.kt6PFJetsNeutral = process.kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True, src="pfAllNeutral" )

process.fjSequence = cms.Sequence(process.kt6PFJets+process.ak5PFJets+process.kt6PFJetsCentral)

process.source.fileNames = cms.untracked.vstring(
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Summer11/VBF_HToTauTau_M-120_7TeV-powheg-pythia6-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0E47FBF8-0295-E011-818F-0030487E3026.root'
    'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/data/Run2011A/MuEG/AOD/05Aug2011-v1/0000/EA148FB5-ABBF-E011-8C96-E0CB4E19F9A4.root'
    )


postfix           = "PFlow"
runOnMC           = True

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START42_V13::All')

else:
    process.GlobalTag.globaltag = cms.string('GR_R_42_V19::All')

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
process.atLeastElecMuFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.muPtEtaFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.muPtEtaIDFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.elecPtEtaFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.elecPtEtaIDFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.atLeastOneDiTauFilter = cms.EDFilter(
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
                    jetCorrLabel = ('AK5PF', ['L2Relative', 'L3Absolute',]),
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


addPFMuonIsolation(process,process.patMuons)

addTriggerMatchingMuon(process,isMC=runOnMC)
getattr(process,"patMuons").embedTrack = True

from LLRAnalysis.Utilities.electrons import *
addCutBasedID(process)
addPFElectronIsolation(process,process.patElectrons)

getattr(process,"patElectrons").embedTrack = True
getattr(process,"patElectrons").embedGsfTrack = True
addTriggerMatchingElectron(process,isMC=runOnMC)


from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )
process.patTriggerEvent.processName = '*'

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'

###########################################################################################
simpleCutsWP95 = "(userFloat('nHits')<=1  && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.8 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.7 && userFloat('dEta')<0.01 && userFloat('HoE')<0.07) ))"
simpleCutsWP80 = "(userFloat('nHits')==0 && userInt('antiConv')>0.5 &&  ( (pt>=20 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.06 && userFloat('dEta')<0.004 && userFloat('HoE')<0.04) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.007 && userFloat('HoE')<0.025) )) || (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.004 && userFloat('HoE')<0.025) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.02 && userFloat('dEta')<0.005 && userFloat('HoE')<0.025) ) )  ) )"


process.selectedPatMuonsTriggerMatchUserEmbedded = cms.EDProducer(
    "MuonsUserEmbedded",
    muonTag = cms.InputTag("selectedPatMuonsTriggerMatch"),
    vertexTag = cms.InputTag("offlinePrimaryVertices")
    )

process.selectedPatElectronsTriggerMatchUserEmbedded = cms.EDProducer(
    "ElectronsUserEmbedded",
    electronTag = cms.InputTag("selectedPatElectronsTriggerMatch"),
    vertexTag = cms.InputTag("offlinePrimaryVertices"),
    isMC = cms.bool(runOnMC)
    )

process.atLeastOneElecMu = cms.EDProducer(
    "CandViewShallowCloneCombiner",
    decay = cms.string("selectedPatElectronsTriggerMatchUserEmbedded selectedPatMuonsTriggerMatchUserEmbedded"),
    cut = cms.string("sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  ) *  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  )  )>0.5"),
    checkCharge = cms.bool(False)
    )

process.atLeastOneElecMuCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecMu"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.muPtEta = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>9 && abs(eta)<2.1"),
    filter = cms.bool(False)
    )
process.atLeastOneElecMumuPtEta = process.atLeastOneElecMu.clone(
    decay=cms.string("selectedPatElectronsTriggerMatchUserEmbedded muPtEta")
    )
process.muPtEtaCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecMumuPtEta"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.muPtEtaRelID = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>9 && abs(eta)<2.4 && isGlobalMuon"),
    filter = cms.bool(False)
    )

process.muPtEtaID = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
    cut = cms.string(process.muPtEta.cut.value()+" &&  isTrackerMuon && isGlobalMuon && numberOfMatches>=2 && globalTrack.isNonnull && globalTrack.hitPattern.numberOfValidMuonHits>=1 && globalTrack.hitPattern.numberOfValidPixelHits>=1 && globalTrack.hitPattern.numberOfValidTrackerHits>10 && globalTrack.normalizedChi2<10 && globalTrack.ptError/globalTrack.pt<0.1 && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2"),
    filter = cms.bool(False)
    )
process.atLeastOneElecMumuPtEtaID = process.atLeastOneElecMu.clone(
    decay=cms.string("selectedPatElectronsTriggerMatchUserEmbedded muPtEtaID")
    )
process.muPtEtaIDCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecMumuPtEtaID"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.elecPtEta = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>9 && abs(eta)<2.5 && !isEBEEGap"),
    filter = cms.bool(False)
    )
process.atLeastOneElecMuelecPtEta = process.atLeastOneElecMu.clone(
    decay=cms.string("elecPtEta muPtEtaID")
    )
process.elecPtEtaCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecMuelecPtEta"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.elecPtEtaID = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
    cut = cms.string(process.elecPtEta.cut.value()+" && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2 &&"+simpleCutsWP80),
    filter = cms.bool(False)
    )
process.atLeastOneElecMuelecPtEtaID = process.atLeastOneElecMu.clone(
    decay=cms.string("elecPtEtaID muPtEtaID")
    )
process.elecPtEtaIDCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecMuelecPtEtaID"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.elecPtEtaRelID = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>9 && abs(eta)<2.5 && !isEBEEGap && "+simpleCutsWP95),
    filter = cms.bool(False)
    )

process.atLeastOneGoodVertexSequence = cms.Sequence(
    process.primaryVertexFilter*process.vertexScrapingFilter
    )

process.alLeastOneElecMuSequence = cms.Sequence(
    process.atLeastOneElecMu*process.atLeastOneElecMuCounter*process.atLeastOneDiTauFilter
    )

process.muLegSequence = cms.Sequence(
    (process.muPtEta*process.atLeastOneElecMumuPtEta*process.muPtEtaCounter*process.muPtEtaFilter) *
    (process.muPtEtaID*process.atLeastOneElecMumuPtEtaID*process.muPtEtaIDCounter*process.muPtEtaIDFilter) *
    process.muPtEtaRelID
    )
process.elecLegSequence = cms.Sequence(
    (process.elecPtEta*process.atLeastOneElecMuelecPtEta*process.elecPtEtaCounter*process.elecPtEtaFilter) *
    (process.elecPtEtaID*process.atLeastOneElecMuelecPtEtaID*process.elecPtEtaIDCounter*process.elecPtEtaIDFilter) *
    process.elecPtEtaRelID
    )

getattr(process,"selectedPatJets").cut = cms.string('pt>10 && abs(eta)<5.0')

process.deltaRJetMuons = cms.EDProducer(
    "DeltaRNearestMuonComputer",
    probes = cms.InputTag("selectedPatJets"),
    objects = cms.InputTag("muPtEtaID"),
    )
process.selectedPatJetsNoMuons = cms.EDProducer(
    "JetsCleaner",
    jets =  cms.InputTag("selectedPatJets"),
    valueMap = cms.InputTag("deltaRJetMuons"),
    minDeltaR = cms.double(0.3)
    )

process.deltaRJetElectrons = cms.EDProducer(
    "DeltaRNearestElectronComputer",
    probes = cms.InputTag("selectedPatJetsNoMuons"),
    objects = cms.InputTag("elecPtEtaID"),
    )
process.selectedPatJetsNoMuonsNoElectrons = cms.EDProducer(
    "JetsCleaner",
    jets =  cms.InputTag("selectedPatJetsNoMuons"),
    valueMap = cms.InputTag("deltaRJetElectrons"),
    minDeltaR = cms.double(0.3)
    )

process.jetCleaningSequence = cms.Sequence(
    process.deltaRJetMuons*process.selectedPatJetsNoMuons*process.deltaRJetElectrons*process.selectedPatJetsNoMuonsNoElectrons
    )


process.pat = cms.Sequence(
    process.allEventsFilter+
    process.atLeastOneGoodVertexSequence*
    process.fjSequence*
    process.patDefaultSequence*
    process.kt6PFJetsNeutral*
    process.selectedPatMuonsTriggerMatchUserEmbedded*
    process.selectedPatElectronsTriggerMatchUserEmbedded*
    process.alLeastOneElecMuSequence*
    process.muLegSequence*
    process.elecLegSequence*
    process.jetCleaningSequence*
    process.printTree1
    )


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
    'keep *_genParticles_*_*',
    'keep *_particleFlow__*',
    'keep *_offlinePrimaryVertices_*_*',
    'keep *_offlinePrimaryVerticesWithBS_*_*',
    'keep *_offlineBeamSpot_*_*',
    'keep *_patMETsPFlow_*_*',
    'keep *_kt6PFJetsCentral_rho_*',
    'keep *_kt6PFJetsNeutral_rho_*',
    'keep *_muPtEtaID_*_*',
    'keep *_muPtEtaRelID_*_*',
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
    'drop *_selectedPatElectrons_*_*',
    'drop *_patMETs_*_*',
    'drop *_selectedPatMuonsTriggerMatch_*_*',
    'drop *_selectedPatElectronsTriggerMatch_*_*',
    'drop *_selectedPatMuonsTriggerMatchUserEmbedded_*_*',
    'drop *_selectedPatElectronsTriggerMatchUserEmbedded_*_*',
    )
                                   )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("skimElecMuStream.root")
                                   )

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('p')
    )

process.out.fileName = cms.untracked.string('patTuples_ElecMuStream.root')

process.outpath = cms.EndPath(process.out)

