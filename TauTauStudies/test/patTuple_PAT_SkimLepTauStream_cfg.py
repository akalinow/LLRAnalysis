###############################################################################
## Pat-tuple for l+tau analysis
## 
## Last modificaion: 12/02/2013 Michal
## TODO: synch tests
##
##
###############################################################################

from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.load('Configuration.StandardSequences.Services_cff')
#MB needed?:
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
process.load("JetMETCorrections.Configuration.JetCorrectionServices_cff")

postfix     = "PFlow"
runOnMC     = True
runOnEmbed  = False

#from Configuration.PyReleaseValidation.autoCond import autoCond
#process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

if runOnMC:
    #process.GlobalTag.globaltag = cms.string('START53_V10::All')
    process.GlobalTag.globaltag = cms.string('START53_V15::All')
else:
    #process.GlobalTag.globaltag = cms.string('GR_P_V41_AN1::All')
    process.GlobalTag.globaltag = cms.string('GR_P_V39_AN3::All')


process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source.fileNames = cms.untracked.vstring(
    #'root://polgrid4.in2p3.fr//dpm/in2p3.fr/home/cms/trivcat/store/mc/Summer12_DR53X/DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball/AODSIM/PU_S10_START53_V7A-v1/0000/FE414F4B-F6D2-E111-A4E9-003048674048.root' #12.5k
    'file:/data_CMS/cms/anayak/HTauTauSynchronization/8TeV/53X/VBF_HToTauTau_M-125_8TeV-powheg-pythia6-Summer12_DR53X-PU_S10_START53_V7A-v1-1200542B-D9ED-E111-B708-00237DA1A548.root' #7k
    #'file:/data_CMS/cms/mbluj/Production/test/WJetsToLNu_TuneZ2Star_8TeV-madgraph-tarball-Summer12_DR53X-PU_S10_START53_V7A-v2-AODSIM-FE4F82A9-68F3-E111-8CD3-003048D476AE.root' #13k
    #'root://node12.datagrid.cea.fr//dpm/datagrid.cea.fr/home/cms/trivcat/store/mc/Summer12_DR53X/W3JetsToLNu_TuneZ2Star_8TeV-madgraph/AODSIM/PU_S10_START53_V7A-v1/0000/0088DD9F-B105-E211-BB06-001E67397D5A.root' #no access??
    #'file:/data_CMS/cms/mbluj/Production/test/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola-Summer12_DR53X-PU_S10_START53_V7A-v2-AODSIM-FED775BD-B8E1-E111-8ED5-003048C69036.root' #11k
    #'file:/data_CMS/cms/mbluj/Production/test/W3JetsToLNu_TuneZ2Star_8TeV-madgraph-Summer12_DR53X-PU_S10_START53_V7A-v2-AODSIM-FC3ED802-E606-E211-A0FD-0025B31E330A.root' #9k
    )

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:751063'
#    )
################### event content ##################

process.printEventContent = cms.EDAnalyzer("EventContentAnalyzer")

################### filters log  ####################

process.allEventsFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.primaryVertexFilter = cms.EDFilter(
    "AllEventsFilter"
    )

process.atLeastOneMuTauFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.atLeastOneElecTauFilter = cms.EDFilter(
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
process.elecTauPtEtaFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.elecTauPtEtaIDFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.elecTauPtEtaIDAgMuFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.elecTauPtEtaIDAgMuAgElecFilter = cms.EDFilter(
    "AllEventsFilter"
    )

process.atLeast1selectedDiTauFilter = cms.EDFilter(
    "AllEventsFilter"
    )
process.atLeast1selectedElecTauFilter = cms.EDFilter(
    "AllEventsFilter"
    )

################### HLT trigger  ####################

process.HLTFilterMuTau = cms.EDFilter(
    "HLTHighLevel",
    TriggerResultsTag  = cms.InputTag("TriggerResults","","HLT"),
    HLTPaths           = cms.vstring("HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v*",
                                     "HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v*",
                                     "HLT_IsoMu15_eta2p1_L1ETM20_v*",
                                     "HLT_IsoMu8_eta2p1_LooseIsoPFTau20_L1ETM26_v*",
                                     "HLT_IsoMu8_eta2p1_LooseIsoPFTau20_v*",#monitoring
                                     ),
    eventSetupPathsKey = cms.string(''),
    andOr              = cms.bool(True),   # OR
    #andOr              = cms.bool(False), # AND
    throw              = cms.bool(False)
    )

process.HLTFilterEleTau = process.HLTFilterMuTau.clone()
process.HLTFilterEleTau.HLTPaths = cms.vstring("HLT_Ele20_CaloIdVT_CaloIsoRhoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v*",
                                               "HLT_Ele22_eta2p1_WP90Rho_LooseIsoPFTau20_v*",
                                               "HLT_Ele13_eta2p1_WP90Rho_LooseIsoPFTau20_L1ETM36_v*",
                                               "HLT_Ele13_eta2p1_WP90Rho_LooseIsoPFTau20_v*",#monitoring
                                               )

### to "emulate" e/mu+tau_etm triggers in MC
process.HLTFilterSingleMu = process.HLTFilterMuTau.clone()
process.HLTFilterSingleMu.HLTPaths = cms.vstring("HLT_Mu8_v*") #p'ed in data but should be fine in MC
process.HLTFilterSingleEle = process.HLTFilterMuTau.clone()
process.HLTFilterSingleEle.HLTPaths = cms.vstring("HLT_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v*") #p'ed in data but should be fine in MC

process.goodL1ETM20 = cms.EDFilter("HLTLevel1MET",
    saveTags = cms.bool(False),
    MinPt = cms.double(22.0), #22=20/0.9 scale correction
    MinN = cms.int32(1),
    MaxEta = cms.double(-1.0),
    MinMass = cms.double(-1.0),
    inputTag =  cms.InputTag("l1extraParticles:MET"),
    MinE = cms.double(-1.0),
    triggerType = cms.int32(87)
)
process.goodL1ETM30 = process.goodL1ETM20.clone()
process.goodL1ETM30.MinPt = cms.double(33.0) #33=30/0.9 scale correction

process.goodL1Mu7er = cms.EDFilter("HLTLevel1Muon",
    saveTags = cms.bool(False),
    MinPt = cms.double(7.0),
    MinN = cms.int32(1),
    MaxEta = cms.double(2.1),
    MinMass = cms.double(-1.0),
    inputTag =  cms.InputTag("l1extraParticles"),
    MinE = cms.double(-1.0),
    triggerType = cms.int32(83)
)
process.goodL1IsoEG12er = cms.EDFilter("HLTLevel1EG",
    saveTags = cms.bool(False),
    MinPt = cms.double(12.0),
    MinN = cms.int32(1),
    MaxEta = cms.double(2.17),
    MinMass = cms.double(-1.0),
    inputTag =  cms.InputTag("l1extraParticles:Isolated"),
    MinE = cms.double(-1.0),
    triggerType = cms.int32(82)
)

### cone-based tau to mimic trigger //CUSTOM
process.load("LLRAnalysis.TauTauStudies.emulateHLTPFTau_cff")

process.pseudoMuTauETMtriggerSequence = cms.Sequence(
    #process.HLTFilterSingleMu+
    #process.goodL1Mu7er+
    process.goodL1ETM20+
    process.hltPFTauSequence
    #+process.hltPFTauFilterSeqence
    )
process.pseudoEleTauETMtriggerSequence = cms.Sequence(
    #process.HLTFilterSingleEle+
    #process.goodL1IsoEG12er+
    process.goodL1ETM30+
    process.hltPFTauSequence
    #+process.hltPFTauFilterSeqence
    )

################### gen listing  ####################

process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.printTree1 = cms.EDAnalyzer(
    "ParticleListDrawer",
    src = cms.InputTag("genParticles"),
    maxEventsToPrint  = cms.untracked.int32(1)
    )

################### jet sequence ####################
#MB needed??
process.load('RecoJets.Configuration.RecoPFJets_cff')
'''

process.kt6PFJetsForRhoComputationVoronoi = process.kt6PFJets.clone(
    doRhoFastjet = True,
    voronoiRfact = 0.9
    )

process.kt6PFJets.doRhoFastjet  = True
process.kt6PFJets.doAreaFastjet = True
process.kt6PFJets.Rho_EtaMax    = cms.double(4.4)
process.kt6PFJets.Ghost_EtaMax  = cms.double(5.0)
process.ak5PFJets.doAreaFastjet = True

## re-run kt4PFJets within lepton acceptance to compute rho
process.load('RecoJets.JetProducers.kt4PFJets_cfi')

process.kt6PFJetsCentral = process.kt6PFJets.clone(
    rParam       = 0.6,
    doRhoFastjet = True )
process.kt6PFJetsCentral.Rho_EtaMax   = cms.double(1.9)
process.kt6PFJetsCentral.Ghost_EtaMax = cms.double(2.5)

#process.kt6PFJetsNeutral = process.kt4PFJets.clone(
#    rParam       = 0.6,
#    doRhoFastjet = True,
#    src          = "pfAllNeutral" )
#process.kt6PFJetsNeutral.Rho_EtaMax   = cms.double(1.9)
#process.kt6PFJetsNeutral.Ghost_EtaMax = cms.double(2.5)

process.fjSequence = cms.Sequence(process.kt6PFJets+
                                  process.ak5PFJets+
                                  process.kt6PFJetsCentral
                                  #+process.kt6PFJetsForRhoComputationVoronoi
                                  )
'''
# load the PU JetID sequence
#process.load("CMGTools.External.pujetidsequence_cff")
process.load("RecoJets.JetProducers.pujetidsequence_cff")
process.puJetId.algos.label = 'full_5x'

################### met ################################

#process.load("RecoMET.METProducers.mvaPFMET_cff")
#process.load("RecoMET.METProducers.mvaPFMET_cff_leptons")
process.load("JetMETCorrections.METPUSubtraction.mvaPFMET_leptons_cff")

if runOnMC:
    process.calibratedAK5PFJetsForPFMEtMVA.correctors = cms.vstring("ak5PFL1FastL2L3")
else:
    process.calibratedAK5PFJetsForPFMEtMVA.correctors = cms.vstring("ak5PFL1FastL2L3Residual") 

#process.pfMEtMVA.srcLeptons = cms.VInputTag(cms.InputTag('muPtEtaRelIDRelIso'), cms.InputTag('tauPtEtaIDAgMuAgElecRelIso') )

process.patPFMetByMVA = process.patMETs.clone(
    metSource = cms.InputTag('pfMEtMVA'),
    addMuonCorrections = cms.bool(False),
    genMETSource = cms.InputTag('genMetTrue')
    )

process.patPFMetByMVA.addGenMET = cms.bool(runOnMC)
process.pfMEtMVA.srcVertices = cms.InputTag("offlinePrimaryVertices")


################### bTag ##############################

if runOnEmbed:
    process.load('RecoBTag/Configuration/RecoBTag_cff')
    process.load('RecoJets/JetAssociationProducers/ak5JTA_cff')
    process.ak5JetTracksAssociatorAtVertex.jets   = cms.InputTag("ak5PFJets")
    process.ak5JetTracksAssociatorAtVertex.tracks = cms.InputTag("tmfTracks")

## Plus, add this to your path:
#process.ak5JetTracksAssociatorAtVertex*process.btagging

################### vertex sequence ####################

process.selectedPrimaryVertices = cms.EDFilter(
    "VertexSelector",
    src = cms.InputTag('offlinePrimaryVertices'),
    cut = cms.string("isValid & ndof >= 4 & z > -24 & z < +24 & position.Rho < 2."),
    filter = cms.bool(False)                                          
)

process.primaryVertexCounter = cms.EDFilter(
    "VertexCountFilter",
    src = cms.InputTag('selectedPrimaryVertices'),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

################### pat specific ####################

from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag

from PhysicsTools.PatAlgos.tools.coreTools import *
if not runOnMC:
    removeMCMatching(process,["All"])

removeSpecificPATObjects(process, ['Photons'],
                         outputInProcess=None)
removeCleaning(process,
               outputInProcess=None)

restrictInputToAOD(process, ['All'])

from LLRAnalysis.Utilities.customizePAT  import *
addSelectedPFlowParticle(process)

from PhysicsTools.PatAlgos.tools.metTools import *
addPfMET(process, postfix)
##MB standard corrections for PFMET
##MB FIXME: check with Christian
##MB FIXME: are the following corrections properly stacked??
#type-0 + type-1
process.pfType1CorrectedMet.applyType0Corrections = False #->added by other meas(see below). Is it taken by PatMEt as correction level which can be removed?
process.pfType1CorrectedMet.applyType1Corrections = True
process.pfType1CorrectedMet.applyType2Corrections = False
#define type-1
process.pfJetMETcorr.skipEM=False
process.pfJetMETcorr.skipMuons=False
process.pfJetMETcorr.skipMuonSelection='isGlobalMuon' #or default 'isGlobalMuon | isStandAloneMuon'?
#define type-0
process.load("JetMETCorrections.Type1MET.pfMETCorrectionType0_cfi")
process.pfType1CorrectedMet.srcType1Corrections = cms.VInputTag(
    cms.InputTag('pfMETcorrType0'),
    cms.InputTag('pfJetMETcorr', 'type1')        
)
process.producePFMETCorrections.replace(process.pfType1CorrectedMet,
                                        process.type0PFMEtCorrection+process.pfType1CorrectedMet)
#MET x/y Shift Correction (for phi modulation)
process.load("JetMETCorrections.Type1MET.pfMETsysShiftCorrections_cfi")
if (not runOnMC) or runOnEmbed:
    process.pfMEtSysShiftCorr.parameter = process.pfMEtSysShiftCorrParameters_2012runAvsNvtx_data
process.pfType1CorrectedMet.srcType1Corrections.append(cms.InputTag('pfMEtSysShiftCorr') )
process.producePFMETCorrections.replace(process.pfType1CorrectedMet,
                                        process.pfMEtSysShiftCorrSequence+process.pfType1CorrectedMet)
##

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
process.patJetCorrFactors.rho    = cms.InputTag('kt6PFJets','rho')
process.patJetCorrFactors.useRho = True

process.patJetCorrFactorsL1Offset = process.patJetCorrFactors.clone(
    levels = cms.vstring('L1Offset',
                         'L2Relative',
                         'L3Absolute')
    )
process.patJetCorrFactorsL1Offset.useRho = False #MB
if runOnMC:
    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute']
else:
    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual']

process.patJets.jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactors"),
                                                     cms.InputTag("patJetCorrFactorsL1Offset"))
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


#################### tau sequence #######################

process.load("RecoTauTag.Configuration.RecoPFTauTag_cff")

from PhysicsTools.PatAlgos.tools.tauTools import *
#switchToPFTauHPS(process, 
#                 pfTauLabelOld = 'shrinkingConePFTauProducer',
#                 pfTauLabelNew = 'hpsPFTauProducer'
#                 )

#MB needed??
switchToPFTauHPS(process)

'''
#MB needed? tracks and PFlow kept and gen
getattr(process,"patTaus").embedIsolationTracks             = cms.bool(True)
getattr(process,"patTaus").embedSignalTracks                = cms.bool(True)
getattr(process,"patTaus").embedLeadTrack                   = cms.bool(True)
getattr(process,"patTaus").embedLeadPFCand                  = cms.bool(True)
getattr(process,"patTaus").embedLeadPFChargedHadrCand       = cms.bool(True)
getattr(process,"patTaus").embedLeadPFNeutralCand           = cms.bool(True)
getattr(process,"patTaus").embedSignalPFCands               = cms.bool(True)
getattr(process,"patTaus").embedSignalPFChargedHadrCands    = cms.bool(True)
getattr(process,"patTaus").embedSignalPFNeutralHadrCands    = cms.bool(True)
getattr(process,"patTaus").embedSignalPFGammaCands          = cms.bool(True)
getattr(process,"patTaus").embedIsolationPFCands            = cms.bool(True)
getattr(process,"patTaus").embedIsolationPFChargedHadrCands = cms.bool(True)
getattr(process,"patTaus").embedIsolationPFNeutralHadrCands = cms.bool(True)
getattr(process,"patTaus").embedIsolationPFGammaCands       = cms.bool(True)

getattr(process,"patTaus").embedGenMatch                    = cms.bool(True)
getattr(process,"patTaus").embedGenJetMatch                 = cms.bool(True)
'''

process.tauMatch.maxDeltaR                = 0.15
process.tauMatch.resolveAmbiguities       = cms.bool(False)
process.tauGenJetMatch.resolveAmbiguities = cms.bool(False)
process.tauGenJetMatch.maxDeltaR          = 0.15
process.tauGenJetMatch.maxDPtRel          = 999

#from RecoTauTag.RecoTau.TauDiscriminatorTools import requireLeadTrack
#from RecoTauTag.RecoTau.PFRecoTauDiscriminationByMVAIsolation_cfi import *
#process.hpsPFTauDiscriminationByMVAIsolation = pfRecoTauDiscriminationByMVAIsolation.clone(
#    PFTauProducer = cms.InputTag('hpsPFTauProducer'),
#    Prediscriminants = cms.PSet(
#    BooleanOperator = cms.string('and'),
#    decayMode = cms.PSet(
#    cut = cms.double(0.5),
#    Producer = cms.InputTag("hpsPFTauDiscriminationByDecayModeFinding")
#    )
#    ),
#    )
# add to the PFTau sequence
#process.recoTauClassicHPSSequence.replace( process.hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr,
#                                           process.hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr+process.hpsPFTauDiscriminationByMVAIsolation )

##################################################################

from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFMuonIso, setupPFElectronIso
process.muIsoSequence       = setupPFMuonIso(process,'muons')
process.electronIsoSequence = setupPFElectronIso(process,'gsfElectrons')
from CommonTools.ParticleFlow.pfParticleSelection_cff import pfParticleSelectionSequence
process.pfParticleSelectionSequence = pfParticleSelectionSequence

process.patMuons.isoDeposits = cms.PSet(
    pfAllParticles   = cms.InputTag("muPFIsoDepositPUPFIso"),      # all PU   CH+MU+E
    pfChargedHadrons = cms.InputTag("muPFIsoDepositChargedPFIso"), # all noPU CH
    pfNeutralHadrons = cms.InputTag("muPFIsoDepositNeutralPFIso"), # all NH
    pfPhotons        = cms.InputTag("muPFIsoDepositGammaPFIso"),   # all PH
    user = cms.VInputTag(
    cms.InputTag("muPFIsoDepositChargedAllPFIso"),                 # all noPU CH+MU+E
    )
    )
process.patMuons.isolationValues = cms.PSet(
    pfAllParticles   = cms.InputTag("muPFIsoValuePU04PFIso"),
    pfChargedHadrons = cms.InputTag("muPFIsoValueCharged04PFIso"),
    pfNeutralHadrons = cms.InputTag("muPFIsoValueNeutral04PFIso"),
    pfPhotons        = cms.InputTag("muPFIsoValueGamma04PFIso"),
    user = cms.VInputTag(
    cms.InputTag("muPFIsoValueChargedAll04PFIso"),
    )
    )

process.patElectrons.isoDeposits = cms.PSet(
    pfAllParticles   = cms.InputTag("elPFIsoDepositPUPFIso"),      # all PU   CH+MU+E
    pfChargedHadrons = cms.InputTag("elPFIsoDepositChargedPFIso"), # all noPU CH
    pfNeutralHadrons = cms.InputTag("elPFIsoDepositNeutralPFIso"), # all NH
    pfPhotons        = cms.InputTag("elPFIsoDepositGammaPFIso"),   # all PH
    user = cms.VInputTag(
    cms.InputTag("elPFIsoDepositChargedAllPFIso"),                 # all noPU CH+MU+E
    )
    )
process.patElectrons.isolationValues = cms.PSet(
    pfAllParticles   = cms.InputTag("elPFIsoValuePU04PFIdPFIso"),
    pfChargedHadrons = cms.InputTag("elPFIsoValueCharged04PFIdPFIso"),
    pfNeutralHadrons = cms.InputTag("elPFIsoValueNeutral04PFIdPFIso"),
    pfPhotons        = cms.InputTag("elPFIsoValueGamma04PFIdPFIso"),
    user = cms.VInputTag(
    cms.InputTag("elPFIsoValueChargedAll04PFIdPFIso"),
    cms.InputTag("elPFIsoValueChargedAll04NoPFIdPFIso"),
    cms.InputTag("elPFIsoValuePU04NoPFIdPFIso"),
    cms.InputTag("elPFIsoValueCharged04NoPFIdPFIso"),
    cms.InputTag("elPFIsoValueGamma04NoPFIdPFIso"),
    cms.InputTag("elPFIsoValueNeutral04NoPFIdPFIso")
    )
    )

#process.patElectrons.isoDeposits = cms.PSet(
#    pfAllParticles   = cms.InputTag("elPFIsoDepositPUPFIso"),
#    )
process.patElectrons.isolationValues = cms.PSet(
    #pfAllParticles   = cms.InputTag("elPFIsoValuePU04PFIdPFIso"),
    )

########################  pat::muon  #############################

#addPFMuonIsolation(process,process.patMuons)
#addTriggerMatchingMuon(process,isMC=runOnMC)
#MB: neeed? all tracks are kept
#getattr(process,"patMuons").embedTrack = True


######################## pat::electron ###########################

#addPFElectronIsolation(process,process.patElectrons)
#MB: neeed? all tracks and gsfTracks are kept
#getattr(process,"patElectrons").embedTrack    = True
#getattr(process,"patElectrons").embedGsfTrack = True
#addTriggerMatchingElectron(process,isMC=runOnMC)


######################## pat::tau ################################

#addTriggerMatchingTau(process,isMC=runOnMC,postfix="",XtriggerMu=True)


######################## pat::jet ################################

getattr(process,"selectedPatJets").cut = cms.string('pt>10 && abs(eta)<5.0')


######################## pat::trigger ############################

from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )
process.patTriggerEvent.processName = '*'

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'
    

######################## embedding ###############################

process.selectedPatMuonsUserEmbedded = cms.EDProducer(
    "MuonsUserEmbedded",
    muonTag            = cms.InputTag("selectedPatMuons"),
    vertexTag          = cms.InputTag("offlinePrimaryVertices"),
    fitUnbiasedVertex  = cms.bool(False)
    )

process.selectedPatElectronsUserEmbedded = cms.EDProducer(
    "ElectronsUserEmbedded",
    electronTag = cms.InputTag("selectedPatElectrons"),
    vertexTag   = cms.InputTag("offlinePrimaryVertices"),
    isMC        = cms.bool(runOnMC),
    doMVAMIT    = cms.bool(True),
    doMVADaniele= cms.bool(True),
    inputFileName0 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet0LowPt_NoIPInfo_BDTG.weights.xml'),
    inputFileName1 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet1LowPt_NoIPInfo_BDTG.weights.xml'),
    inputFileName2 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet2LowPt_NoIPInfo_BDTG.weights.xml'),
    inputFileName3 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet0HighPt_NoIPInfo_BDTG.weights.xml'),
    inputFileName4 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet1HighPt_NoIPInfo_BDTG.weights.xml'),
    inputFileName5 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet2HighPt_NoIPInfo_BDTG.weights.xml'),
    inputFileName0v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat1.weights.xml'),
    inputFileName1v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat2.weights.xml'),
    inputFileName2v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat3.weights.xml'),
    inputFileName3v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat4.weights.xml'),
    inputFileName4v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat5.weights.xml'),
    inputFileName5v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat6.weights.xml'),
    inputFileName0v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat1.weights.xml'),
    inputFileName1v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat2.weights.xml'),
    inputFileName2v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat3.weights.xml'),
    inputFileName3v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat4.weights.xml'),
    inputFileName4v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat5.weights.xml'),
    inputFileName5v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat6.weights.xml'),
    #inputFileNameMVADaniele = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/TMVA_BDTSimpleCat.weights.xml')
    )

process.selectedPatElectronsUserEmbeddedIso = cms.EDProducer(
    "ElectronsUserEmbeddedIso",
    electronTag = cms.InputTag("selectedPatElectronsUserEmbedded")
    )

process.selectedPatTausUserEmbedded = cms.EDProducer(
    "TausUserEmbedded",
    tauTag    = cms.InputTag("selectedPatTaus"),
    vertexTag = cms.InputTag("offlinePrimaryVertices"),
    )

####################### mu+tau pairing ##################################

process.atLeastOneMuTau = cms.EDProducer(
    "CandViewShallowCloneCombiner",
    decay = cms.string("selectedPatMuonsUserEmbedded selectedPatTausUserEmbedded"),
    cut = cms.string("sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  ) *  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  )  )>0.5"),
    checkCharge = cms.bool(False)
    )

process.atLeastOneMuTauCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneMuTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.muPtEta = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string("pt>7 && abs(eta)<2.4"),
    filter = cms.bool(False)
    )
process.atLeastOneMuTaumuPtEta = process.atLeastOneMuTau.clone(
    decay=cms.string("muPtEta selectedPatTausUserEmbedded")
    )
process.muPtEtaCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneMuTaumuPtEta"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.muPtEtaRelID = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string("pt>7 && abs(eta)<2.4 && isGlobalMuon && isPFMuon && isTrackerMuon"+
                     #"&& abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2"
                     "&& abs(userFloat('dzWrtPV'))<0.2"
                     ),
    filter = cms.bool(False)
    )

process.muPtEtaRelIDRelIso = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string(process.muPtEtaRelID.cut.value()+
                     " && userFloat('PFRelIsoDB04v2')<0.20"),
    filter = cms.bool(False)
    )

process.muPtEtaID = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string(process.muPtEta.cut.value()+
                     " && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2"+
                     " && ("+
                     " isGlobalMuon && isPFMuon"+
                     " && globalTrack.isNonnull "+
                     " && globalTrack.normalizedChi2<10"+
                     " && globalTrack.hitPattern.numberOfValidMuonHits>0"+                     
                     " && numberOfMatchedStations>1"+                     
                     " && innerTrack.hitPattern.numberOfValidPixelHits>0"+
                     " && track.hitPattern.trackerLayersWithMeasurement > 5)"
                     ),
    filter = cms.bool(False)
    )

process.muonsForVeto =  cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string("pt>10 && abs(eta)<2.4"+
                     " && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2"+
                     " && ("+
                     " isGlobalMuon && isPFMuon"+
                     " && globalTrack.isNonnull "+
                     " && globalTrack.normalizedChi2<10"+
                     " && globalTrack.hitPattern.numberOfValidMuonHits>0"+                     
                     " && numberOfMatchedStations>1"+                     
                     " && innerTrack.hitPattern.numberOfValidPixelHits>0"+
                     " && track.hitPattern.trackerLayersWithMeasurement > 5)"+
                     " && userFloat('PFRelIsoDB04v2')<0.30"),
    filter = cms.bool(False)
    )


process.atLeastOneMuTaumuPtEtaID = process.atLeastOneMuTau.clone(
    decay=cms.string("muPtEtaID selectedPatTausUserEmbedded")
    )
process.muPtEtaIDCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneMuTaumuPtEtaID"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.tauPtEta  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string("pt>19 && abs(eta)<2.3"),
    filter = cms.bool(False)
    )
process.atLeastOneMuTautauPtEta = process.atLeastOneMuTau.clone(
    decay=cms.string("muPtEtaID tauPtEta")
    )
process.tauPtEtaCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneMuTautauPtEta"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.tauPtEtaID  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEta.cut.value()+
                     " && tauID('decayModeFinding')>0.5"+
                     " && abs(userFloat('dzWrtPV'))<0.2"),
    filter = cms.bool(False)
    )
process.tausForVeto  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaID.cut.value()+
                     " && pt>20 && tauID('byLooseIsolationMVA')>0.5"),
    filter = cms.bool(False)
    )

process.atLeastOneMuTautauPtEtaID = process.atLeastOneMuTau.clone(
    decay=cms.string("muPtEtaID tauPtEtaID")
    )
process.tauPtEtaIDCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneMuTautauPtEtaID"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.tauPtEtaIDAgMu  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaID.cut.value()+
                     #" && tauID('againstMuonTight')>0.5"),
                     " && ( tauID('againstMuonTight')>0.5 || tauID('againstMuonTight2')>0.5 )"),
    filter = cms.bool(False)
    )

process.atLeastOneMuTautauPtEtaIDAgMu = process.atLeastOneMuTau.clone(
    decay=cms.string("muPtEtaID tauPtEtaIDAgMu")
    )
process.tauPtEtaIDAgMuCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneMuTautauPtEtaIDAgMu"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.tauPtEtaIDAgMuAgElec  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaIDAgMu.cut.value()+
                     " && tauID('againstElectronLoose')>0.5"),
    filter = cms.bool(False)
    )

process.atLeastOneMuTautauPtEtaIDAgMuAgElec = process.atLeastOneMuTau.clone(
    decay=cms.string("muPtEtaID tauPtEtaIDAgMuAgElec")
    )
process.tauPtEtaIDAgMuAgElecCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneMuTautauPtEtaIDAgMuAgElec"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.tauPtEtaIDAgMuAgElecRelIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaIDAgMuAgElec.cut.value()+
                     " && tauID('byVLooseCombinedIsolationDeltaBetaCorr')>0.5"),
    filter = cms.bool(False)
    )

####################### e+tau pairing ##################################

simpleCutsVeto = "(userFloat('nHits')<=999"+ \
                 " && (" + \
                 " (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.80 && "+ \
                 "          userFloat('dEta') <0.007 && userFloat('HoE') <0.15)"   + \
                 " || "  + \
                 " (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.70 && "+ \
                 "          userFloat('dEta') <0.010 && userFloat('HoE') <999)"   + \
                 "     )"+ \
                 ")"
simpleCutsWP95 = "(userFloat('nHits')<=1"+ \
                 " && (" + \
                 " (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.80 && "+ \
                 "          userFloat('dEta') <0.007 && userFloat('HoE') <0.15)"   + \
                 " || "  + \
                 " (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.70 && "+ \
                 "          userFloat('dEta') <0.010 && userFloat('HoE') <0.07)"   + \
                 "     )"+ \
                 ")"
simpleCutsWP80 = "(userFloat('nHits')==0 && userInt('antiConv')>0.5 "+ \
                 " && ("   + \
                 " (pt>=20 && ("    + \
                 "               (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.06 && "  + \
                 "                        userFloat('dEta')< 0.004 && userFloat('HoE') <0.04)"     + \
                 "               ||"+ \
                 "               (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.030 && " + \
                 "                        userFloat('dEta') <0.007 && userFloat('HoE') <0.025) )) "+ \
                 "     || "+ \
                 " (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) && ( "+ \
                 "               (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.030 && " + \
                 "                        userFloat('dEta') <0.004 && userFloat('HoE') <0.025) "   + \
                 "               ||"+ \
                 "               (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.020 &&"  + \
                 "                        userFloat('dEta') <0.005 && userFloat('HoE') <0.025) ))" + \
                 "    )"   + \
                 ")"

MVA = "((pt<=20 && abs(superClusterPosition.Eta)>=0.0 && abs(superClusterPosition.Eta)<1.0 && userFloat('mva')>0.133) ||" + \
      "(pt<=20 && abs(superClusterPosition.Eta)>=1.0 && abs(superClusterPosition.Eta)<1.5 && userFloat('mva')>0.465) ||" + \
      "(pt<=20 && abs(superClusterPosition.Eta)>=1.5 && abs(superClusterPosition.Eta)<2.5 && userFloat('mva')>0.518) ||" + \
      "(pt>20  && abs(superClusterPosition.Eta)>=0.0 && abs(superClusterPosition.Eta)<1.0 && userFloat('mva')>0.942) ||" + \
      "(pt>20  && abs(superClusterPosition.Eta)>=1.0 && abs(superClusterPosition.Eta)<1.5 && userFloat('mva')>0.947) ||" + \
      "(pt>20  && abs(superClusterPosition.Eta)>=1.5 && abs(superClusterPosition.Eta)<2.5 && userFloat('mva')>0.878))"

process.atLeastOneElecTau = cms.EDProducer(
    "CandViewShallowCloneCombiner",
    decay = cms.string("selectedPatElectronsUserEmbeddedIso selectedPatTausUserEmbedded"),
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
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string("pt>12 && abs(eta)<2.5"),
    filter = cms.bool(False)
    )
process.atLeastOneElecTauelecPtEta = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEta selectedPatTausUserEmbedded")
    )
process.elecPtEtaCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTauelecPtEta"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.elecPtEtaID = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string(process.elecPtEta.cut.value()+
                     " && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2"#+
                     #" && dr03TkSumPt/pt<0.30 &&"+
                     #simpleCutsVeto
                     #" && userInt('isTriggerElectron')>0.5"
                     #+"("+simpleCutsWP80+" || "+CiCTight+" || "+MVA+")"),
                     ),
    filter = cms.bool(False)
    )
process.atLeastOneElecTauelecPtEtaID = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID selectedPatTausUserEmbedded")
    )
process.elecPtEtaIDCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTauelecPtEtaID"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.elecPtEtaRelID = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string("pt>12 && abs(eta)<2.5 &&"+
                     #"abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2 &&"+
                     "abs(userFloat('dzWrtPV'))<0.2 &&"+
                     simpleCutsVeto
                     ),
    filter = cms.bool(False)
    )

process.elecPtEtaRelIDRelIso = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string(process.elecPtEtaRelID.cut.value()+
                     " && userFloat('PFRelIsoDB04v3')<0.20"
                     ),
    filter = cms.bool(False)
    )

process.electronsForVeto = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string("pt>10 && abs(eta)<2.5" +
                     " && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2"+
                     " && ( ( pt > 20 && ( (abs(eta)<0.80 && userFloat('mvaPOGNonTrig')>0.905) || (abs(eta)<1.479 && abs(eta)>0.80 && userFloat('mvaPOGNonTrig')>0.955) || (abs(eta)>1.479 && userFloat('mvaPOGNonTrig')>0.975) )) || (pt < 20 && ( (abs(eta)<0.80 && userFloat('mvaPOGNonTrig')>0.925) || (abs(eta)<1.479 && abs(eta)>0.80 && userFloat('mvaPOGNonTrig')>0.915) || (abs(eta)>1.479 && userFloat('mvaPOGNonTrig')>0.965) )))"+
                     " && userFloat('PFRelIsoDB04v3')<0.30"),
    filter = cms.bool(False)
    )

'''
#MB already defined
process.tauPtEta  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string("pt>19 && abs(eta)<2.3"),
    filter = cms.bool(False)
    )
'''
process.atLeastOneElecTautauPtEta = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEta")
    )
process.elecTauPtEtaCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEta"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )


'''
#MB already defined
process.tauPtEtaID  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEta.cut.value()+
                     " && tauID('decayModeFinding')>0.5"+
                     " && abs(userFloat('dzWrtPV'))<0.2"),
    filter = cms.bool(False)
    )
'''
    
'''
#MB already defined
process.tausForVeto  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaID.cut.value()+
                     " && pt>20 && tauID('byLooseIsolationMVA')>0.5"),
    filter = cms.bool(False)
    )
'''
process.atLeastOneElecTautauPtEtaID = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEtaID")
    )
process.elecTauPtEtaIDCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEtaID"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

'''
#MB already defined -> use different name
#MBprocess.tauPtEtaIDAgMu  = cms.EDFilter(
'''
process.tauPtEtaIDAgMuL  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaID.cut.value()+
                     " && tauID('againstMuonLoose')>0.5"),
    filter = cms.bool(False)
    )

process.atLeastOneElecTautauPtEtaIDAgMu = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEtaIDAgMuL")
    )
process.elecTauPtEtaIDAgMuCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEtaIDAgMu"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

'''
#MB already defined -> use different name
#MBprocess.tauPtEtaIDAgMuAgElec  = cms.EDFilter(
'''
process.tauPtEtaIDAgMuLAgElec  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaIDAgMuL.cut.value()+
                     #" && tauID('againstElectronLoose')>0.5"),
                     " && ( tauID('againstElectronLoose')>0.5 || tauID('againstElectronLooseMVA3')>0.5 )"),
    filter = cms.bool(False)
    )
    
process.atLeastOneElecTautauPtEtaIDAgMuAgElec = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEtaIDAgMuLAgElec")
    )
process.elecTauPtEtaIDAgMuAgElecCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEtaIDAgMuAgElec"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

'''
#MB already defined -> use different name
#MBprocess.tauPtEtaIDAgMuAgElecRelIso  = cms.EDFilter(
'''
process.tauPtEtaIDAgMuLAgElecRelIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaIDAgMuLAgElec.cut.value()+
                     " && tauID('byVLooseCombinedIsolationDeltaBetaCorr')>0.5"),
    filter = cms.bool(False)
    )

'''
#MB: needed?
###################### electrons ####################################

#https://twiki.cern.ch/twiki/bin/view/CMS/EgammaCutBasedIdentification

simpleCutsVeto = "(userFloat('nHits')<=999"+ \
                 " && (" + \
                 " (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.80 && "+ \
                 "          userFloat('dEta') <0.007 && userFloat('HoE') <0.15)"   + \
                 " || "  + \
                 " (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.70 && "+ \
                 "          userFloat('dEta') <0.010 && userFloat('HoE') <999)"   + \
                 "     )"+ \
                 ")"
simpleCutsWP95 = "(userFloat('nHits')<=1"+ \
                 " && (" + \
                 " (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.80 && "+ \
                 "          userFloat('dEta') <0.007 && userFloat('HoE') <0.15)"   + \
                 " || "  + \
                 " (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.70 && "+ \
                 "          userFloat('dEta') <0.010 && userFloat('HoE') <0.07)"   + \
                 "     )"+ \
                 ")"


process.elecPtEtaRelID = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string("pt>12 && abs(eta)<2.5" +
                     " && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2 &&"+
                     simpleCutsVeto
                     ),
    filter = cms.bool(False)
    )
process.elecPtEtaRelIDRelIso = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string(process.elecPtEtaRelID.cut.value()+
                     " && userFloat('PFRelIsoDB04v3')<0.20"
                     ),
    filter = cms.bool(False)
    )
process.electronsForVeto = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string("pt>10 && abs(eta)<2.5" +
                     " && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2"+
                     " && ( ( pt > 20 && ( (abs(eta)<0.80 && userFloat('mvaPOGNonTrig')>0.905) || (abs(eta)<1.479 && abs(eta)>0.80 && userFloat('mvaPOGNonTrig')>0.955) || (abs(eta)>1.479 && userFloat('mvaPOGNonTrig')>0.975) )) || (pt < 20 && ( (abs(eta)<0.80 && userFloat('mvaPOGNonTrig')>0.925) || (abs(eta)<1.479 && abs(eta)>0.80 && userFloat('mvaPOGNonTrig')>0.915) || (abs(eta)>1.479 && userFloat('mvaPOGNonTrig')>0.965) )))"+
                     " && userFloat('PFRelIsoDB04v3')<0.30"),
    filter = cms.bool(False)
    )

'''

###################### final sequences ##############################

process.atLeastOneGoodVertexSequence = cms.Sequence(
    process.selectedPrimaryVertices*
    process.primaryVertexCounter*
    process.primaryVertexFilter
    )

process.patLeptonsUserEmbeddedSequnce = cms.Sequence(
    process.selectedPatMuonsUserEmbedded*
    process.selectedPatElectronsUserEmbedded*
    process.selectedPatElectronsUserEmbeddedIso*
    process.selectedPatTausUserEmbedded
    )

process.atLeastOneMuTauSequence = cms.Sequence(
    process.atLeastOneMuTau*
    process.atLeastOneMuTauCounter*
    process.atLeastOneMuTauFilter
    )

process.muLegSequence = cms.Sequence(
    (process.muPtEta*process.atLeastOneMuTaumuPtEta*process.muPtEtaCounter*process.muPtEtaFilter) *
    (process.muPtEtaID*process.atLeastOneMuTaumuPtEtaID*process.muPtEtaIDCounter*process.muPtEtaIDFilter) +
    process.muPtEtaRelID*process.muPtEtaRelIDRelIso#MB*process.muonsForVeto
    )

process.tauLegForMuTauSequence = cms.Sequence(
    (process.tauPtEta*process.atLeastOneMuTautauPtEta*process.tauPtEtaCounter*process.tauPtEtaFilter) *
    (process.tauPtEtaID*process.atLeastOneMuTautauPtEtaID*process.tauPtEtaIDCounter*process.tauPtEtaIDFilter) *
    (process.tauPtEtaIDAgMu*process.tauPtEtaIDAgMuL*process.atLeastOneMuTautauPtEtaIDAgMu*process.tauPtEtaIDAgMuCounter*process.tauPtEtaIDAgMuFilter)*
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuLAgElec*process.atLeastOneMuTautauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecCounter*process.tauPtEtaIDAgMuAgElecFilter)*
    process.tauPtEtaIDAgMuAgElecRelIso*process.tauPtEtaIDAgMuLAgElecRelIso#MB*process.tausForVeto 
    )

process.atLeastOneElecTauSequence = cms.Sequence(
    process.atLeastOneElecTau*
    process.atLeastOneElecTauCounter*
    process.atLeastOneElecTauFilter
    )

process.elecLegSequence = cms.Sequence(
    (process.elecPtEta*process.atLeastOneElecTauelecPtEta*process.elecPtEtaCounter*process.elecPtEtaFilter) *
    (process.elecPtEtaID*process.atLeastOneElecTauelecPtEtaID*process.elecPtEtaIDCounter*process.elecPtEtaIDFilter) *
    process.elecPtEtaRelID*process.elecPtEtaRelIDRelIso#MB*process.electronsForVeto
    )
process.tauLegForElecTauSequence = cms.Sequence(
    (process.tauPtEta*process.atLeastOneElecTautauPtEta*process.elecTauPtEtaCounter*process.elecTauPtEtaFilter) *
    (process.tauPtEtaID*process.atLeastOneElecTautauPtEtaID*process.elecTauPtEtaIDCounter*process.elecTauPtEtaIDFilter) *
    (process.tauPtEtaIDAgMu*process.tauPtEtaIDAgMuL*process.atLeastOneElecTautauPtEtaIDAgMu*process.elecTauPtEtaIDAgMuCounter*process.elecTauPtEtaIDAgMuFilter)*
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuLAgElec*process.atLeastOneElecTautauPtEtaIDAgMuAgElec*process.elecTauPtEtaIDAgMuAgElecCounter*process.elecTauPtEtaIDAgMuAgElecFilter)*
    process.tauPtEtaIDAgMuAgElecRelIso*process.tauPtEtaIDAgMuLAgElecRelIso#MB*process.tausForVeto
    )
process.looseLeptonsSequence = cms.Sequence(
    process.muPtEtaID+process.muPtEtaRelID
    +process.elecPtEtaID+process.elecPtEtaRelID
    )
process.leptonsForVetoSequence = cms.Sequence(
    process.muonsForVeto+process.electronsForVeto+process.tausForVeto
    )

####################### x-cleaning of jets #########################
##MB FIXME: it is correct clean like this? higher lep/tau pT threshold? loose iso?
##MB: needed??
'''
process.deltaRJetMuons = cms.EDProducer(
    "DeltaRNearestMuonComputer",
    probes = cms.InputTag("selectedPatJets"),
    objects = cms.InputTag("muPtEtaID"),
    )
process.selectedPatJetsNoMuons = cms.EDProducer(
    "JetsCleaner",
    jets =  cms.InputTag("selectedPatJets"),
    valueMap = cms.InputTag("deltaRJetMuons"),
    minDeltaR = cms.double(0.5)
    )

process.deltaRJetTaus = cms.EDProducer(
    "DeltaRNearestTauComputer",
    probes = cms.InputTag("selectedPatJetsNoMuons"),
    objects = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    )
process.selectedPatJetsNoMuonsNoTaus = cms.EDProducer(
    "JetsCleaner",
    jets =  cms.InputTag("selectedPatJetsNoMuons"),
    valueMap = cms.InputTag("deltaRJetTaus"),
    minDeltaR = cms.double(0.3)
    )

process.jetCleaningSequence = cms.Sequence(
    process.deltaRJetMuons*process.selectedPatJetsNoMuons*
    process.deltaRJetTaus*process.selectedPatJetsNoMuonsNoTaus
    )
'''
########################## paths ###############################
process.commonOfflineSequence = cms.Sequence(
    process.atLeastOneGoodVertexSequence*
    process.PFTau*
    process.pfParticleSelectionSequence*
    process.muIsoSequence*
    process.electronIsoSequence*
    (process.ak5JetTracksAssociatorAtVertex*process.btagging)*
    process.patDefaultSequence*
    #MB->to tree producer: process.puJetIdSqeuence *
    ##process.kt6PFJetsNeutral*
    process.patLeptonsUserEmbeddedSequnce*
    process.looseLeptonsSequence*
    process.leptonsForVetoSequence
    +process.hltPFTauSequence
    )

process.skimMuTau1 = cms.Sequence(
    process.allEventsFilter+
    process.HLTFilterMuTau*
    process.commonOfflineSequence*
    process.atLeastOneMuTauSequence*
    process.muLegSequence*
    process.tauLegForMuTauSequence
    #MB->to tree producer *(process.pfMEtMVAsequence*process.patPFMetByMVA)
    ##+process.jetCleaningSequence
    +process.printTree1
    )

process.skimMuTau2 = cms.Sequence(
    process.allEventsFilter+
    process.pseudoMuTauETMtriggerSequence*
    process.commonOfflineSequence*
    process.atLeastOneMuTauSequence*
    process.muLegSequence*
    process.tauLegForMuTauSequence
    #MB->to tree producer *(process.pfMEtMVAsequence*process.patPFMetByMVA)
    ##+process.jetCleaningSequence
    ##+process.printTree1
    )

process.skimElecTau1 = cms.Sequence(
    process.allEventsFilter+
    process.HLTFilterEleTau*
    process.commonOfflineSequence*
    process.atLeastOneElecTauSequence*
    process.elecLegSequence*
    process.tauLegForElecTauSequence
    #MB->to tree producer *(process.pfMEtMVAsequence*process.patPFMetByMVA)
    ##+process.jetCleaningSequence
    ##+process.printTree1
    )

process.skimElecTau2 = cms.Sequence(
    process.allEventsFilter+
    process.pseudoEleTauETMtriggerSequence*
    process.commonOfflineSequence*
    process.atLeastOneElecTauSequence*
    process.elecLegSequence*
    process.tauLegForElecTauSequence
    #MB->to tree producer *(process.pfMEtMVAsequence*process.patPFMetByMVA)
    ##+process.jetCleaningSequence
    ##+process.printTree1
    )



'''
#MB it is usually incorrect replace offlinePrimaryVertices by selectedPrimaryVertices
massSearchReplaceAnyInputTag(process.skimMuTau1,
                             "offlinePrimaryVertices",
                             "selectedPrimaryVertices",
                             verbose=False)
massSearchReplaceAnyInputTag(process.skimMuTau2,
                             "offlinePrimaryVertices",
                             "selectedPrimaryVertices",
                             verbose=False)
massSearchReplaceAnyInputTag(process.skimElecTau1,
                             "offlinePrimaryVertices",
                             "selectedPrimaryVertices",
                             verbose=False)
massSearchReplaceAnyInputTag(process.skimElecTau2,
                             "offlinePrimaryVertices",
                             "selectedPrimaryVertices",
                             verbose=False)

process.selectedPrimaryVertices.src = cms.InputTag('offlinePrimaryVertices')
process.hltAK5PFJetTracksAssociatorAtVertex.pvSrc = "offlinePrimaryVertices"
process.hltPFTauTagInfo.PVProducer = "offlinePrimaryVertices"
process.hltPFTaus.PVProducer = "offlinePrimaryVertices"
process.hltPFTauLooseIsolationDiscriminator.qualityCuts.primaryVertexSrc = "offlinePrimaryVertices"
'''
#MB use selectedPrimaryVertices only in pat-object embedding moduces
process.selectedPatMuonsUserEmbedded.vertexTag = "selectedPrimaryVertices"
process.selectedPatElectronsUserEmbedded.vertexTag = "selectedPrimaryVertices"
process.selectedPatTausUserEmbedded.vertexTag = "selectedPrimaryVertices"


if not runOnMC:
    process.skimMuTau1.remove(process.printTree1)
    process.skimMuTau1.remove(process.HLTFilterMuTau)
    process.skimElecTau1.remove(process.HLTFilterEleTau)
if not runOnEmbed:
    process.commonOfflineSequence.remove(process.ak5JetTracksAssociatorAtVertex)
    process.commonOfflineSequence.remove(process.btagging)
if runOnMC and runOnEmbed:
    process.skimMuTau1.remove(process.HLTFilterMuTau)
    process.skimElecTau1.remove(process.HLTFilterEleTau)
        
#process.p = cms.Path(process.printEventContent+process.skim)
process.pMuTau1 = cms.Path(process.skimMuTau1)
process.pElecTau1 = cms.Path(process.skimElecTau1)
if runOnMC and not runOnEmbed:
    process.pMuTau2 = cms.Path(process.skimMuTau2)
    process.pElecTau2 = cms.Path(process.skimElecTau2)

########################## output ###############################

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
    'keep *_ak5PFJets_*_*',#MB??
    'keep *_genParticles_*_*',
    'keep *_particleFlow__*',
    'keep *_offlinePrimaryVertices_*_*',
    'keep *_selectedPrimaryVertices_*_*',
    #MB'keep *_offlinePrimaryVerticesWithBS_*_*',
    'keep *_offlineBeamSpot_*_*',
    'keep *_patMETsPFlow_*_*',
    'keep *_patPFMetByMVA_*_*',
    'keep *_metNoHF_*_*',
    'keep *_tauGenJetsSelectorAllHadrons_*_*',
    'keep *_kt6PFJets_rho_*',
    'keep *_kt6PFJetsCentral_rho_*',
    'keep *_kt6PFJetsNeutral_rho_*',
    'keep *_kt6PFJetsForRhoComputationVoronoi_rho_*',
    'keep *_muPtEtaID_*_*',
    'keep *_muPtEtaRelID_*_*',
    #MB 'keep *_muons_*_*',
    'keep *_elecPtEtaID_*_*',
    'keep *_elecPtEtaRelID_*_*',
    'keep *_electronsForVeto_*_*',
    'keep *_muonsForVeto_*_*',
    'keep *_tausForVeto_*_*',
    'keep *_addPileupInfo_*_*',
    'keep *_generalTracks_*_*',
    'keep *_tmfTracks_*_*',
    'keep *_electronGsfTracks_*_*',
    'keep recoTrackExtras_*_*_*',
    'keep recoGsfTrackExtras_*_*_*',
    'keep *_tauPtEtaIDAgMuAgElec_*_*',
    'keep *_tauPtEtaIDAgMuLAgElec_*_*',#MB+
    'keep *_generator_*_*',
    'keep *_source_*_*',
    'keep *_l1extraParticles_*_*',#MB+
    'keep *_selectedHltPatTaus_*_*',#MB+
    #MB'keep *_reducedEcalRecHitsEB_*_*',
    #MB'keep *_reducedEcalRecHitsEE_*_*',
    'drop *_TriggerResults_*_HLT',
    'drop *_TriggerResults_*_RECO',
    'drop *_selectedPatElectrons_*_*',
    'drop *_selectedPatMuons_*_*',
    'drop *_selectedPatTaus_*_*',
    #MB'drop *_patMETs_*_*',
    'drop *_selectedPatMuons_*_*',
    'drop *_selectedPatElectrons_*_*',
    'drop *_selectedPatTaus_*_*',
    'drop *_selectedPatMuonsUserEmbedded_*_*',
    'drop *_selectedPatElectronsUserEmbedded_*_*',
    'drop *_selectedPatElectronsUserEmbeddedIso_*_*',
    'drop *_selectedPatTausUserEmbedded_*_*',
    'keep *_puJetId_*_*',
    'keep *_puJetMva_*_*',
    #MB'keep *_hfEMClusters_*_*',
    #MB'keep *_hybridSuperClusters_*_*',
    #MB'keep *_multi5x5BasicClusters_*_*',
    #MB'keep *_pfElectronTranslator_*_*',
    #MB'keep *_pfPhotonTranslator_*_*',
    #MB'keep *_hybridSuperClusters_*_*',
    )
                                   )
#MB
if runOnEmbed:
    process.out.outputCommands.extend( cms.vstring(
        'keep *_towerMaker_*_*',
        )
                                       )
          
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("skimLepTauStream.root")
    )

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('pMuTau1','pElecTau1')
    )
if runOnMC and not runOnEmbed:
    process.out.SelectEvents.SelectEvents += ['pMuTau2','pElecTau2']

process.out.fileName = cms.untracked.string('patTuples_LepTauStream.root')

process.outpath = cms.EndPath(process.out)

###MB FIXME: do we really need the following??
#processDumpFile = open('patTuplePATSkimLepTauStream.dump', 'w')
#print >> processDumpFile, process.dumpPython()

