from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 2000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

## temporary JEC
#process.load("CondCore.DBCommon.CondDBCommon_cfi")
#process.jec = cms.ESSource(
#    "PoolDBESSource",
#    DBParameters = cms.PSet(
#    messageLevel = cms.untracked.int32(0)
#    ),
#    timetype = cms.string('runnumber'),
#    toGet = cms.VPSet(
#    cms.PSet(
#    record = cms.string('JetCorrectionsRecord'),
#    tag    = cms.string('JetCorrectorParametersCollection_Jec10V3_AK5PF'),
#    label  = cms.untracked.string('AK5PF')
#    )
#    ),
#    ## here you add as many jet types as you need (AK5Calo, AK5JPT, AK7PF, AK7Calo, KT4PF, KT4Calo, KT6PF, KT6Calo)
#    connect = cms.string('sqlite_file:Jec10V3.db')
#    )
#
#process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')


process.load("RecoTauTag.Configuration.RecoPFTauTag_cff")
process.load('RecoJets.Configuration.RecoPFJets_cff')
process.kt6PFJets.doRhoFastjet = True
process.kt6PFJets.Rho_EtaMax = cms.double(4.4)
#process.kt6PFJets.Ghost_EtaMax = cms.double(5.0)
process.ak5PFJets.doAreaFastjet = True
process.ak5PFJets.Rho_EtaMax = cms.double(4.4)
#process.ak5PFJets.Ghost_EtaMax = cms.double(5.0)

#process.ak5PFL1Fastjet.useCondDB = False

## re-run kt4PFJets within lepton acceptance to compute rho
process.load('RecoJets.JetProducers.kt4PFJets_cfi')
process.kt6PFJetsCentral = process.kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True )
process.kt6PFJetsCentral.Rho_EtaMax = cms.double(2.5)

process.fjSequence = cms.Sequence(process.kt6PFJets+process.ak5PFJets+process.kt6PFJetsCentral)

process.source.fileNames = cms.untracked.vstring(
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Fall10/VBF_HToTauTau_M-115_7TeV-powheg-pythia6-tauola/GEN-SIM-RECO/START38_V12-v1/0000/044E940A-55EC-DF11-89D6-0023AEFDEE60.root',
    'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Spring11/VBF_ToHToZZTo4L_M-500_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0030/3A38172E-9553-E011-BC76-00E0817918C5.root'
    #'file:/data_CMS/cms/lbianchini/F41A3437-7AED-DF11-A50D-002618943894.root',
    #'file:/data_CMS/cms/lbianchini/ZElEl_RelVal386_1.root',
    )

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:751063','1:751088','1:751100','1:751117','1:751141','1:751237','1:751325','1:751392','1:751466','1:977809','1:977870','1:977898',
#    '1:977915','1:978085','1:978214','1:811102','1:811190'
#    )

postfix           = "PFlow"
sample            = ""
runOnMC           = False

if runOnMC:
    #process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )
    process.GlobalTag.globaltag = cms.string('START41_V0::All')

else:
    #process.GlobalTag.globaltag = cms.string(autoCond[ 'com10' ])
    #process.GlobalTag.globaltag = cms.string('GR_R_311_V4::All')
    process.GlobalTag.globaltag = cms.string('GR_R_41_V0::All')


process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.printTree1 = cms.EDAnalyzer("ParticleListDrawer",
                                    src = cms.InputTag("genParticles"),
                                    maxEventsToPrint  = cms.untracked.int32(1)
                                    )

process.offlinePrimaryVerticesDA = cms.EDProducer(
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
    JEClevels = ['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual']

process.patJetCorrFactors.levels = JEClevels
process.patJetCorrFactors.rho = cms.InputTag('kt6PFJets','rho')

process.patJetCorrFactorsL1Offset = process.patJetCorrFactors.clone(
    levels = cms.vstring('L1Offset',
                         'L2Relative',
                         'L3Absolute')
    )

if runOnMC:
    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute']
else:
    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute','L2L3Residual']

process.patJets.jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactors"),cms.InputTag("patJetCorrFactorsL1Offset"))
process.patDefaultSequence.replace(process.patJetCorrFactors,
                                   process.patJetCorrFactors+process.patJetCorrFactorsL1Offset)



addPFMuonIsolation(process,process.patMuons)
addTriggerMatchingMuon(process,isMC=runOnMC)
getattr(process,"patMuons").embedTrack = True

from LLRAnalysis.Utilities.electrons import *
addCutBasedID(process)
addPFElectronIsolation(process,process.patElectrons)
getattr(process,"patElectrons").embedTrack = True
getattr(process,"patElectrons").embedGsfTrack = True

addTriggerMatchingElectron(process,isMC=runOnMC)
process.eleTriggerMatchHLTElectrons.matchedCuts =  cms.string('type("TriggerElectron")')

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

    
#########
## PAT

#simpleCutsWP95 = "(userFloat('nHits')<=1 && userFloat('dist')>-999 && userFloat('dcot')>-999 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.8 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.7 && userFloat('dEta')<0.01 && userFloat('HoE')<0.15) ))"
simpleCutsWP95 = "(userFloat('nHits')<=1 && userFloat('dist')>-999 && userFloat('dcot')>-999 &&  ( (pt>=20 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.8 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.7 && userFloat('dEta')<0.01 && userFloat('HoE')<0.15) )) || (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.8 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.7 && userFloat('dEta')<0.01 && userFloat('HoE')<0.15) ) )  ) )"
#simpleCutsWP80 = "(userFloat('nHits')==0 && userFloat('dist')>0.02 && userFloat('dcot')>0.02 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.06 && userFloat('dEta')<0.004 && userFloat('HoE')<0.04) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) ))"
simpleCutsWP80 = "(userFloat('nHits')==0 && userFloat('dist')>0.02 && userFloat('dcot')>0.02 &&  ( (pt>=20 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.06 && userFloat('dEta')<0.004 && userFloat('HoE')<0.04) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) )) || (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.004 && userFloat('HoE')<0.025) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.02 && userFloat('dEta')<0.005 && userFloat('HoE')<0.15) ) )  ) )"


process.selectedPatElectronsTriggerMatchUserEmbedded = cms.EDProducer(
    "ElectronsUserEmbedded",
    electronTag = cms.InputTag("selectedPatElectronsTriggerMatch"),
    vertexTag = cms.InputTag("offlinePrimaryVertices"),
    isMC = cms.bool(runOnMC)
    )

process.tightElectrons = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>17 && abs(eta)<2.4 && !isEBEEGap &&  abs(userFloat('dxyWrtPV'))<0.02 && abs(userFloat('dzWrtPV'))<0.2 &&"+simpleCutsWP80),
    filter = cms.bool(False)
    )

process.looseElectrons = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>15 && abs(eta)<2.4 && !isEBEEGap &&  abs(userFloat('dxyWrtPV'))<0.02 && abs(userFloat('dzWrtPV'))<0.2 &&"+simpleCutsWP95),
    filter = cms.bool(False)
    )

process.diElectrons = cms.EDProducer(
    "CandViewShallowCloneCombiner",
    decay = cms.string("tightElectrons@- looseElectrons@+"),
    cut   = cms.string("(60<mass && mass<120) && sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+(daughter(0).phi-daughter(1).phi)*(daughter(0).phi-daughter(1).phi))>0.5"),
    )

process.atLeast1diElectron = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("diElectrons"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )


getattr(process,"selectedPatJets").cut = cms.string('pt>15 && abs(eta)<5.0')

process.deltaRJetElectrons = cms.EDProducer(
    "DeltaRNearestElectronComputer",
    probes = cms.InputTag("selectedPatJets"),
    objects = cms.InputTag("looseElectrons"),
    )

process.selectedPatJetsNoElectrons = cms.EDProducer(
    "JetsCleaner",
    jets =  cms.InputTag("selectedPatJets"),
    valueMap = cms.InputTag("deltaRJetElectrons"),
    minDeltaR = cms.double(0.5)
    )

process.zPlusJetsAnalyzer = cms.EDAnalyzer(
    "ZelelPlusJetsAnalyzer",
    diElectrons =  cms.InputTag("diElectrons"),
    jets =  cms.InputTag("selectedPatJets"),
    triggerResults = cms.InputTag("patTriggerEvent"),
    isMC = cms.bool(runOnMC),
    deltaRLegJet = cms.untracked.double(0.5),
    minCorrPt = cms.untracked.double(15.),
    minJetID  = cms.untracked.double(0.5), # 1=loose,2=medium,3=tight
    verbose =  cms.untracked.bool(False),
    )

process.pat = cms.Sequence(
    process.allEventsFilter+
    process.offlinePrimaryVerticesDA*
    process.primaryVertexFilter+
    process.scrapping +
    #process.makeSCs +
    process.fjSequence*
    process.patDefaultSequence*
    process.selectedPatElectronsTriggerMatchUserEmbedded*
    (process.looseElectrons+process.tightElectrons)*
    process.diElectrons*
    process.atLeast1diElectron*
    process.deltaRJetElectrons*
    process.selectedPatJetsNoElectrons*
    process.zPlusJetsAnalyzer+
    process.printTree1
    )

if not runOnMC:
    process.pat.remove(process.printTree1)

process.p = cms.Path(process.pat)

massSearchReplaceAnyInputTag(process.pat,
                             "offlinePrimaryVertices",
                             "offlinePrimaryVerticesDA",verbose=True)

from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out.outputCommands = cms.untracked.vstring('drop *',
                                                   *patEventContentNoCleaning ) 
process.out.outputCommands.extend( cms.vstring(
    'keep *_TriggerResults_*_*',
    'keep *_hltTriggerSummaryAOD_*_*',
    'keep recoGenParticles_genParticles*_*_*',
    'keep *_generalTracks_*_*',
    'keep *_electronGsfTracks_*_*',
    'keep recoTrackExtras_*_*_*',
    'keep recoGsfTrackExtras_*_*_*',
    #'keep TrackingRecHitsOwned_*_*_*',
    'keep *_selectedSuperClusters_*_*',
    'keep *_offlineBeamSpot_*_*',
    'keep *_offlinePrimaryVertices*_*_*',
    'keep *_particleFlow_*_*',
    'keep *_selectedPatJetsNoMuons_*_*',
    'keep *_diMuons_*_*',
    )
                                   )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("treeZelelPlusJets.root")
                                   )

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('p')
    )

process.out.fileName = cms.untracked.string('patTuples_'+sample+'.root')

#process.outpath = cms.EndPath(process.out)
process.outpath = cms.EndPath()

