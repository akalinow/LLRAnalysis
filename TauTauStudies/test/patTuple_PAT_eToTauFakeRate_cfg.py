from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 2000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

process.load("RecoTauTag.Configuration.RecoPFTauTag_cff")

process.load('RecoJets.Configuration.RecoPFJets_cff')
process.kt6PFJets.doRhoFastjet = True
process.kt6PFJets.Rho_EtaMax = cms.double(4.4)
process.ak5PFJets.doAreaFastjet = True
process.ak5PFJets.Rho_EtaMax = cms.double(4.4)
process.fjSequence = cms.Sequence(process.kt6PFJets+process.ak5PFJets)

process.source.fileNames = cms.untracked.vstring(
    'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Summer11/WH_ZH_TTH_HToWW_M-120_7TeV-pythia6//AODSIM/PU_S3_START42_V11-v1//0000/E430E306-347C-E011-A75A-00261834B5C6.root'
    )

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:1080','1:2003','1:2028','1:6867','1:7016'
#    )

postfix           = "PFlow"
runOnMC           = True
doEfficiency      = False

if not doEfficiency:
    FileName = "treeEtoTauTnP.root"
else:
    FileName = "treeEtoTauEff.root"

if runOnMC:
    #process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )
    process.GlobalTag.globaltag = cms.string('START42_V12::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_R_42_V14::All')


process.primaryVertexFilter = cms.EDFilter(
    "GoodVertexFilter",
    vertexCollection = cms.InputTag('offlinePrimaryVertices'),
    minimumNDOF = cms.uint32(4) ,
    maxAbsZ = cms.double(24),
    maxd0 = cms.double(2)
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
    decayModeFinding = cms.InputTag("hpsPFTauDiscriminationByDecayModeFinding"),
    byLooseIsolation = cms.InputTag("hpsPFTauDiscriminationByLooseIsolation"),
    byMediumIsolation = cms.InputTag("hpsPFTauDiscriminationByMediumIsolation"),
    byTightIsolation = cms.InputTag("hpsPFTauDiscriminationByTightIsolation"),
    againstElectronLoose = cms.InputTag("hpsPFTauDiscriminationByLooseElectronRejection"),
    againstElectronMedium = cms.InputTag("hpsPFTauDiscriminationByMediumElectronRejection"),
    againstElectronTight = cms.InputTag("hpsPFTauDiscriminationByTightElectronRejection"),
    againstElectronCrackRem = cms.InputTag("hpsPFTauDiscriminationAgainstElectronCrackRem"),
    againstMuonLoose = cms.InputTag("hpsPFTauDiscriminationByLooseMuonRejection"),
    againstMuonTight = cms.InputTag("hpsPFTauDiscriminationByTightMuonRejection")
    )

process.tauMatch.maxDeltaR = 0.15
process.tauMatch.resolveAmbiguities = cms.bool(False)
process.tauGenJetMatch.resolveAmbiguities = cms.bool(False)
process.tauGenJetMatch.maxDeltaR = 0.15
process.tauGenJetMatch.maxDPtRel = 999

addPFElectronIsolation(process,process.patElectrons)

process.pfPileUp.Vertices = "offlinePrimaryVertices"

getattr(process,"patElectrons").embedTrack = True
getattr(process,"patElectrons").embedGsfTrack = True
addTriggerMatchingElectron(process,isMC=runOnMC)
process.eleTriggerMatchHLTElectrons.matchedCuts =  cms.string('(path("HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v*") || path("HLT_Ele32_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v*") ) && type("TriggerElectron")')


if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'


process.selectedPatElectronsTriggerMatchUserEmbedded = cms.EDProducer(
    "ElectronsUserEmbedded",
    electronTag = cms.InputTag("selectedPatElectronsTriggerMatch"),
    vertexTag = cms.InputTag("offlinePrimaryVertices"),
    isMC = cms.bool(runOnMC)
    )

#####################################################################################
process.genTauDecaysToHadrons = cms.EDFilter("TauGenJetDecayModeSelector",
                                             src = cms.InputTag("tauGenJets"),
                                             select = cms.vstring('oneProng0Pi0', 'oneProng1Pi0', 'oneProng2Pi0', 'oneProngOther','threeProng0Pi0', 'threeProng1Pi0', 'threeProngOther', 'rare'),
                                             filter = cms.bool(False)
                                             )
process.genTauDecaysToElectron = cms.EDFilter("TauGenJetDecayModeSelector",
                                              src = cms.InputTag("tauGenJets"),
                                              select = cms.vstring('electron'),
                                              filter = cms.bool(False)
                                              )
process.genTauDecaysToMuon = cms.EDFilter("TauGenJetDecayModeSelector",
                                          src = cms.InputTag("tauGenJets"),
                                          select = cms.vstring('muon'),
                                          filter = cms.bool(False)
                                          )
process.taus = cms.EDFilter("PATTauSelector",
                            src = cms.InputTag("selectedPatTaus"),
                            cut = cms.string("pt>20.0 && abs(eta)<2.3 && tauID('decayModeFinding')>0.5"),
                            filter = cms.bool(False)
                            )
process.tauFakeRateAnalyzer = cms.EDAnalyzer("TauFakeRateAnalyzer",
                                             matchTo = cms.string("muon"),
                                             tauTag = cms.InputTag("taus"),
                                             electronsTag = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
                                             )
process.electronMvaAnalyzer = cms.EDAnalyzer("ElectronMvaAnalyzer",
                                             electronsTag = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
                                             )

process.makeEtoTauEff = cms.Sequence(
    process.genTauDecaysToHadrons+
    process.taus*
    process.tauFakeRateAnalyzer
    #process.electronMvaAnalyzer
    )


#####################################################################################
if not doEfficiency:
    process.load("LLRAnalysis.TauTauStudies.eToTauFakeRate_cff")

if (not doEfficiency) and (not runOnMC):
    process.sequence70.remove(process.tag70McMatch)
    process.sequence70.remove(process.probeIDLooseMcMatch)
    process.sequence70.remove(process.probeIDMediumMcMatch)
    process.sequence70.remove(process.probeIDTightMcMatch)
    process.etoTau70IDLoose.isMC = cms.bool(False)
    process.etoTau70IDMedium.isMC = cms.bool(False)
    process.etoTau70IDTight.isMC = cms.bool(False)
    process.etoTau70IDLoose.makeMCUnbiasTree = cms.bool(False)
    process.etoTau70IDMedium.makeMCUnbiasTree= cms.bool(False)
    process.etoTau70IDTight.makeMCUnbiasTree = cms.bool(False)
    process.addUserVariables.isMC = cms.bool(False)

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


process.pat = cms.Sequence(
    process.allEventsFilter+
    #process.offlinePrimaryVertices*
    process.primaryVertexFilter*
    process.PFTau*
    process.fjSequence*
    process.patDefaultSequence*
    process.selectedPatElectronsTriggerMatchUserEmbedded
    )

if not doEfficiency:
    process.pat.replace(process.selectedPatElectronsTriggerMatchUserEmbedded,
                        process.selectedPatElectronsTriggerMatchUserEmbedded*process.sequence70)
else:
    process.pat.replace(process.selectedPatElectronsTriggerMatchUserEmbedded,
                        process.selectedPatElectronsTriggerMatchUserEmbedded*process.makeEtoTauEff)

#massSearchReplaceAnyInputTag(process.pat,
#                             "offlinePrimaryVertices",
#                             "offlinePrimaryVerticesDA",verbose=True)

process.p = cms.Path(process.pat)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(FileName)
                                   )

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('p')
    )

process.out.fileName = cms.untracked.string('patTuples_eToTauFakeRate.root')
process.outpath = cms.EndPath()
