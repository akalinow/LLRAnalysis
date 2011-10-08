from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

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
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Summer11/DYToMuMu_M-120_TuneZ2_7TeV-pythia6-tauola/AODSIM/PU_S3_START42_V11-v2/0000/2EE73A4A-8288-E011-9561-1CC1DE1CEFC8.root'
    'file:pickevents_1.root'
    )

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:751063','1:751088','1:751100','1:751117','1:751141','1:751237','1:751325','1:751392','1:751466','1:977809','1:977870','1:977898',
#    '1:977915','1:978085','1:978214','1:811102','1:811190'
#    )

postfix           = "PFlow"
runOnMC           = False

if runOnMC:
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
process.patJetCorrFactors.useRho = True

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
process.muonTriggerMatchHLTMuons.matchedCuts =  cms.string('(path("HLT_Mu11_v*") || path("HLT_Mu15_v*") || path("HLT_Mu17_v*") || path("HLT_IsoMu11_v*") || path("HLT_IsoMu15_v*") || path("HLT_IsoMu17_v*")) && type("TriggerMuon")')
getattr(process,"patMuons").embedTrack = True

from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )
process.patTriggerEvent.processName = '*'

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'

process.selectedPatMuonsTriggerMatchUserEmbedded = cms.EDProducer(
    "MuonsUserEmbedded",
    muonTag = cms.InputTag("selectedPatMuonsTriggerMatch"),
    vertexTag = cms.InputTag("offlinePrimaryVertices")
    )

process.tightMuons = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>15 && abs(eta)<2.1 && isTrackerMuon && isGlobalMuon && numberOfMatches>=2 && globalTrack.isNonnull && globalTrack.hitPattern.numberOfValidMuonHits>=1 && globalTrack.hitPattern.numberOfValidPixelHits>=1 && globalTrack.hitPattern.numberOfValidTrackerHits>10 && globalTrack.normalizedChi2<10 && globalTrack.ptError/globalTrack.pt<0.1 && abs(userFloat('dxyWrtPV'))<0.02 && abs(userFloat('dzWrtPV'))<0.2"),
    filter = cms.bool(False)
    )

process.looseMuons = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
    cut = cms.string("pt>10 && abs(eta)<2.4 && isGlobalMuon"),
    filter = cms.bool(False)
    )

process.diMuons = cms.EDProducer(
    "CandViewShallowCloneCombiner",
    decay = cms.string("tightMuons@- looseMuons@+"),
    cut   = cms.string("(60<mass && mass<120) && sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+(daughter(0).phi-daughter(1).phi)*(daughter(0).phi-daughter(1).phi))>0.5"),
    )

process.atLeast1diMuon = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("diMuons"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

getattr(process,"selectedPatJets").cut = cms.string('correctedJet("Uncorrected","none","patJetCorrFactors").pt>10 && abs(eta)<4.5')

process.zPlusJetsAnalyzer = cms.EDAnalyzer(
    "ZmumuPlusJetsAnalyzer",
    diMuons =  cms.InputTag("diMuons"),
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
    process.primaryVertexFilter+
    process.fjSequence*
    process.patDefaultSequence*
    process.kt6PFJetsNeutral*
    process.selectedPatMuonsTriggerMatchUserEmbedded*
    (process.looseMuons+process.tightMuons)*
    process.diMuons*
    process.atLeast1diMuon*
    process.zPlusJetsAnalyzer+
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
    'keep *_generalTracks_*_*',
    'keep *_electronGsfTracks_*_*',
    'keep recoTrackExtras_*_*_*',
    'keep recoGsfTrackExtras_*_*_*',
    'keep *_selectedSuperClusters_*_*',
    'keep *_offlineBeamSpot_*_*',
    'keep *_offlinePrimaryVertices*_*_*',
    'keep *_particleFlow_*_*',
    'keep *_selectedPatJetsNoMuons_*_*',
    'keep *_diMuons_*_*',
    )
                                   )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("treeZmumuPlusJets.root")
                                   )

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('p')
    )

process.out.fileName = cms.untracked.string('patTuples_ZmumuPlusJets.root')

process.outpath = cms.EndPath()

