from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 10

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'START38_V13::All'

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')


process.source.fileNames = cms.untracked.vstring(
    #'file:/data_CMS/cms/lbianchini/DY_ZTauTau_Fall10_RECO.root'
    #'file:/data_CMS/cms/lbianchini/ZTT_RelVal386_1.root',
    #'file:/data_CMS/cms/lbianchini/ZTT_RelVal386_2.root',
    #'file:/data_CMS/cms/lbianchini/ZTT_RelVal386_3.root',
    'file:/data_CMS/cms/lbianchini/ZTT_RelVal386_4.root',
    #'file:/data_CMS/cms/lbianchini/F41A3437-7AED-DF11-A50D-002618943894.root',
    #'file:/data_CMS/cms/lbianchini/ZElEl_RelVal386_1.root',
    #'file:/data_CMS/cms/lbianchini/ZElEl_RelVal386_2.root',
    #'file:/data_CMS/cms/lbianchini/ZElEl_RelVal386_3.root',
    #'file:/data_CMS/cms/lbianchini/ZElEl_RelVal386_4.root',
    )

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:1080','1:2003','1:2028','1:6867','1:7016'
#    )

postfix           = "PFlow"
sample            = ""
runOnMC           = True


process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.printTree1 = cms.EDAnalyzer("ParticleListDrawer",
                                    src = cms.InputTag("genParticles"),
                                    maxEventsToPrint  = cms.untracked.int32(1)
                                    )

process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                           vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                           minimumNDOF = cms.uint32(4) ,
                                           maxAbsZ = cms.double(24),
                                           maxd0 = cms.double(2)
                                           )

process.scrapping = cms.EDFilter("FilterOutScraping",
                         applyfilter = cms.untracked.bool(True),
                         debugOn = cms.untracked.bool(False),
                         numtrack = cms.untracked.uint32(10),
                         thresh = cms.untracked.double(0.25)
                         )

process.allEventsFilter = cms.EDFilter("AllEventsFilter")

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

process.makePatTaus.remove(process.patPFCandidateIsoDepositSelection)

from PhysicsTools.PatAlgos.tools.metTools import *
addPfMET(process, postfix)

from PhysicsTools.PatAlgos.tools.jetTools import *
switchJetCollection(process,cms.InputTag('ak5PFJets'),
                    doJTA        = True,
                    doBTagging   = True,
                    jetCorrLabel = ('AK5PF', ['L2Relative', 'L3Absolute',]),
                    #jetCorrLabel = ('AK5', 'PF'),
                    doType1MET   = False,
                    genJetCollection=cms.InputTag("ak5GenJets"),
                    doJetID      = True,
                    jetIdLabel   = 'ak5'
                    )

from PhysicsTools.PatAlgos.tools.tauTools import *

switchToPFTauHPS(process, 
                 pfTauLabelOld = 'shrinkingConePFTauProducer',
                 pfTauLabelNew = 'hpsPFTauProducer'
                 )

cloneProcessingSnippet(process, process.makePatTaus, postfix)


switchToPFTauShrinkingCone(process,
                           pfTauLabelOld = 'hpsPFTauProducer',
                           pfTauLabelNew = 'shrinkingConePFTauProducer'
                           )

process.patCandidates.replace(process.makePatTaus,
                              process.makePatTaus+
                              getattr(process,"makePatTaus"+postfix)
                              )
process.patCandidateSummary.candidates.append(cms.InputTag("patTaus"+postfix))

setattr(process,"selectedPatTaus"+postfix,process.selectedPatTaus.clone())
getattr(process,"selectedPatTaus"+postfix).src = 'patTaus'+postfix
process.selectedPatCandidates.replace(process.selectedPatTaus,
                                      process.selectedPatTaus+
                                      getattr(process,"selectedPatTaus"+postfix)
                                      )
process.selectedPatCandidateSummary.candidates.append(cms.InputTag("selectedPatTaus"+postfix))
# and add counter
setattr(process,"countPatTaus"+postfix,process.countPatTaus.clone())
getattr(process,"countPatTaus"+postfix).src = 'selectedPatTaus'+postfix
process.countPatCandidates.replace(process.countPatTaus,
                                   process.countPatTaus+
                                   getattr(process,"countPatTaus"+postfix)
                                   )

getattr(process,"patTaus"+postfix).embedIsolationTracks = cms.bool(True)
getattr(process,"patTaus"+postfix).embedSignalTracks = cms.bool(True)
getattr(process,"patTaus"+postfix).embedGenMatch = cms.bool(True)
getattr(process,"patTaus"+postfix).embedLeadTrack = cms.bool(True)
getattr(process,"patTaus"+postfix).embedLeadPFCand = True
getattr(process,"patTaus"+postfix).embedLeadPFChargedHadrCand = True
getattr(process,"patTaus"+postfix).embedLeadPFNeutralCand = True
getattr(process,"patTaus"+postfix).embedSignalPFCands = True
getattr(process,"patTaus"+postfix).embedSignalPFChargedHadrCands = True
getattr(process,"patTaus"+postfix).embedSignalPFNeutralHadrCands = True
getattr(process,"patTaus"+postfix).embedSignalPFGammaCands = True
getattr(process,"patTaus"+postfix).embedIsolationPFCands = True
getattr(process,"patTaus"+postfix).embedIsolationPFChargedHadrCands = True
getattr(process,"patTaus"+postfix).embedIsolationPFNeutralHadrCands = True
getattr(process,"patTaus"+postfix).embedIsolationPFGammaCands = True
getattr(process,"patTaus"+postfix).embedGenJetMatch = cms.bool(True)
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


setattr(process,"hpsPFTauDiscriminationAgainstElectronCrackRem",
        getattr(process,"hpsPFTauDiscriminationAgainstElectron").clone(
    ApplyCut_EcalCrackCut = cms.bool(True),
    ApplyCut_PFElectronMVA =  cms.bool(False)
    )
        )

setattr(process,"shrinkingConePFTauDiscriminationAgainstElectronCrackRem",
        getattr(process,"shrinkingConePFTauDiscriminationAgainstElectron").clone(
    ApplyCut_EcalCrackCut = cms.bool(True),
    ApplyCut_PFElectronMVA =  cms.bool(False)
    )
        )

process.patHPSPFTauDiscrimination += process.hpsPFTauDiscriminationAgainstElectronCrackRem
process.patShrinkingConePFTauDiscrimination += process.shrinkingConePFTauDiscriminationAgainstElectronCrackRem

getattr(process,"makePatTaus"+postfix).replace(
    getattr(process,"patTaus"+postfix),
    process.patHPSPFTauDiscrimination + getattr(process,"patTaus"+postfix)
    )
getattr(process,"makePatTaus").replace(
    getattr(process,"patTaus"),
    process.patShrinkingConePFTauDiscrimination + getattr(process,"patTaus")
    )


getattr(process,"patTaus"+postfix).tauIDSources = cms.PSet(
    leadingTrackFinding = cms.InputTag("hpsPFTauDiscriminationByDecayModeFinding"),
    byLooseIsolation = cms.InputTag("hpsPFTauDiscriminationByLooseIsolation"),
    byMediumIsolation = cms.InputTag("hpsPFTauDiscriminationByMediumIsolation"),
    byTightIsolation = cms.InputTag("hpsPFTauDiscriminationByTightIsolation"),
    againstElectron = cms.InputTag("hpsPFTauDiscriminationAgainstElectron"),
    againstElectronCrackRem = cms.InputTag("hpsPFTauDiscriminationAgainstElectronCrackRem"),
    againstMuon = cms.InputTag("hpsPFTauDiscriminationAgainstMuon")
    )

getattr(process,"patTaus").tauIDSources = cms.PSet(
    leadingTrackFinding = cms.InputTag("shrinkingConePFTauDiscriminationByLeadingTrackFinding"),
    leadingTrackPtCut = cms.InputTag("shrinkingConePFTauDiscriminationByLeadingTrackPtCut"),
    leadingPionPtCut = cms.InputTag("shrinkingConePFTauDiscriminationByLeadingPionPtCut"),
    trackIsolation = cms.InputTag("shrinkingConePFTauDiscriminationByTrackIsolation"),
    trackIsolationUsingLeadingPion = cms.InputTag("shrinkingConePFTauDiscriminationByTrackIsolationUsingLeadingPion"),
    ecalIsolation = cms.InputTag("shrinkingConePFTauDiscriminationByECALIsolation"),
    ecalIsolationUsingLeadingPion = cms.InputTag("shrinkingConePFTauDiscriminationByECALIsolationUsingLeadingPion"),
    byIsolation = cms.InputTag("shrinkingConePFTauDiscriminationByIsolation"),
    byIsolationUsingLeadingPion = cms.InputTag("shrinkingConePFTauDiscriminationByIsolationUsingLeadingPion"),
    againstElectron = cms.InputTag("shrinkingConePFTauDiscriminationAgainstElectron"),
    againstElectronCrackRem = cms.InputTag("shrinkingConePFTauDiscriminationAgainstElectronCrackRem"),
    againstMuon = cms.InputTag("shrinkingConePFTauDiscriminationAgainstMuon"),
    byTaNC = cms.InputTag("shrinkingConePFTauDiscriminationByTaNC"),
    byTaNCfrOnePercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrOnePercent"),
    byTaNCfrHalfPercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrHalfPercent"),
    byTaNCfrQuarterPercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrQuarterPercent"),
    byTaNCfrTenthPercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrTenthPercent")
    )

process.patMuons.embedTrack = True

addPFMuonIsolation(process,process.patMuons)
addPFMuon(process,postfix)
addTriggerMatchingMuon(process)
getattr(process,"muonTriggerMatchHLTMuons").pathNames=cms.vstring('*')
addTriggerMatchingMuon(process,postfix)
getattr(process,"muonTriggerMatchHLTMuons"+postfix).pathNames=cms.vstring('*')

process.patElectrons.embedGsfTrack = True
from LLRAnalysis.Utilities.electrons import *
addCutBasedID(process)
addPFElectronIsolation(process,process.patElectrons)
addPFElectron(process,postfix)
process.patElectronsPFlow.embedGsfTrack = True
addTriggerMatchingElectron(process)
addTriggerMatchingElectron(process,postfix)

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'

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

getattr(process,"selectedPatJets").cut = cms.string('pt>10')
getattr(process,"selectedPatTaus").cut = cms.string('pt>15 && (eta<2.4&&eta>-2.4) && tauID("leadingTrackFinding") > 0.5 && tauID("leadingPionPtCut") > 0.5 && tauID("byIsolation")>0.5 && tauID("againstElectronCrackRem") > 0.5')
getattr(process,"selectedPatTaus"+postfix).cut = cms.string('pt>15 && (eta<2.4&&eta>-2.4) && tauID("leadingTrackFinding") > 0.5 && tauID("byLooseIsolation")>0.5 && tauID("againstElectronCrackRem") > 0.5')
getattr(process,"selectedPatMuonsTriggerMatch"+postfix).cut = cms.string('pt>10 && (eta<2.1&&eta>-2.1) && isTrackerMuon && numberOfMatches>=2 && globalTrack.isNonnull  && globalTrack.hitPattern.numberOfValidMuonHits>=1 && globalTrack.hitPattern.numberOfValidPixelHits>=1 && globalTrack.normalizedChi2<=10')
getattr(process,"selectedPatMuonsTriggerMatch").cut = cms.string('pt>10 && (eta<2.1&&eta>-2.1) && isTrackerMuon && numberOfMatches>=2 && globalTrack.isNonnull && globalTrack.hitPattern.numberOfValidMuonHits>=1 && globalTrack.hitPattern.numberOfValidPixelHits>=1 && globalTrack.normalizedChi2<=10')

getattr(process,"patElectrons").embedTrack = cms.bool(True)
getattr(process,"patElectrons"+postfix).embedTrack = cms.bool(True)

getattr(process,"selectedPatElectronsTriggerMatch"+postfix).cut = cms.string('(eta<2.4&&eta>-2.4 && !isEBEEGap) && et>10')
getattr(process,"selectedPatElectronsTriggerMatch").cut = cms.string('(eta<2.4&&eta>-2.4 && !isEBEEGap) && et>10')


#from PFAnalyses.VBFHTauTau.recoDiTauReconstruction_cff import recoDiTau
from LLRAnalysis.Utilities.diTausReconstruction_cff import allDiTauPairs as recoDiTau

process.selectedDiTauPairsHadEl = recoDiTau.clone(
    srcLeg1 = cms.InputTag("patElectrons"),
    srcLeg2 = cms.InputTag("patTaus")
    )




process.pat = cms.Sequence(
    process.allEventsFilter+
    process.primaryVertexFilter+
    process.scrapping +
    process.makeSCs +
    process.patDefaultSequence*
    process.selectedDiTauPairsHadEl
    #process.printTree1
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
    #'keep TrackingRecHitsOwned_*_*_*',
    'keep *_selectedSuperClusters_*_*',
    'keep *_offlineBeamSpot_*_*',
    'keep *_offlinePrimaryVertices*_*_*',
    'keep *_particleFlow_*_*',
    'keep *_pfAllMuons_*_*',
    'keep *_pfAllElectrons_*_*',
    'keep *_tauGenJets_*_*',
    #'keep *_genTauDecaysToElectron_*_*',
    #'keep *_genTauDecaysToMuon_*_*',
    #'keep *_genTauDecaysToHadrons_*_*',
    'keep *_selectedSuperClusters_*_*',
    'keep *_selectedDiTauPairsHadEl_*_*'
    )
                                   )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("outfile.root")
                                   )

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('p')
    )

process.out.fileName = cms.untracked.string('patTuples_'+sample+'.root')

process.outpath = cms.EndPath(process.out)

