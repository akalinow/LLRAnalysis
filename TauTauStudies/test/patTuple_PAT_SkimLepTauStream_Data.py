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
runOnMC     = False
runOnEmbed  = False
embedType   = "RhEmbedMuTau" #"PfEmbed" or "RhEmbedMuTau" or "RhEmbedElTau"

#from Configuration.PyReleaseValidation.autoCond import autoCond
#process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START53_V23::All')
else:
    process.GlobalTag.globaltag = cms.string('FT_53_V21_AN4::All')


process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source.fileNames = cms.untracked.vstring(
    #'root://polgrid4.in2p3.fr//dpm/in2p3.fr/home/cms/trivcat/store/mc/Summer12_DR53X/DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball/AODSIM/PU_S10_START53_V7A-v1/0000/FE414F4B-F6D2-E111-A4E9-003048674048.root' #12.5k
    #'file:/data_CMS/cms/anayak/HTauTauSynchronization/8TeV/53X/VBF_HToTauTau_M-125_8TeV-powheg-pythia6-Summer12_DR53X-PU_S10_START53_V7A-v1-1200542B-D9ED-E111-B708-00237DA1A548.root' #7k
    #'file:/data_CMS/cms/mbluj/Production/test/WJetsToLNu_TuneZ2Star_8TeV-madgraph-tarball-Summer12_DR53X-PU_S10_START53_V7A-v2-AODSIM-FE4F82A9-68F3-E111-8CD3-003048D476AE.root' #13k
    #'root://node12.datagrid.cea.fr//dpm/datagrid.cea.fr/home/cms/trivcat/store/mc/Summer12_DR53X/W3JetsToLNu_TuneZ2Star_8TeV-madgraph/AODSIM/PU_S10_START53_V7A-v1/0000/0088DD9F-B105-E211-BB06-001E67397D5A.root' #no access??
    #'file:/data_CMS/cms/mbluj/Production/test/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola-Summer12_DR53X-PU_S10_START53_V7A-v2-AODSIM-FED775BD-B8E1-E111-8ED5-003048C69036.root' #11k
    'file:/data_CMS/cms/mbluj/Production/test/W3JetsToLNu_TuneZ2Star_8TeV-madgraph-Summer12_DR53X-PU_S10_START53_V7A-v2-AODSIM-FC3ED802-E606-E211-A0FD-0025B31E330A.root' #9k
    #'file:/data_CMS/cms/mbluj/Production/HToTauTau/simDYmumu_embedded_mutau_2013Feb21_AOD.root'
    #'/store/results/higgs/DoubleMuParked/StoreResults-Run2012B_22Jan2013_v1_RHembedded_trans1_tau116_ptmu1_16had1_18_v1-f456bdbb960236e5c696adfe9b04eaae/DoubleMuParked/USER/StoreResults-Run2012B_22Jan2013_v1_RHembedded_trans1_tau116_ptmu1_16had1_18_v1-f456bdbb960236e5c696adfe9b04eaae/0000/00F466CB-9EB0-E211-B414-0023AEFDE9B8.root' #emb
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

################### New Type-1 MET ###################
process.load("LLRAnalysis.TauTauStudies.sumCaloTowersInEtaSlices_cfi")
#Add process.sumCaloTowersInEtaSlicesNoHF to cms path
################### bTag ##############################

if runOnEmbed:
    process.load('RecoBTag.Configuration.RecoBTag_cff')
    process.load('RecoJets.JetAssociationProducers.ak5JTA_cff')
    process.ak5JetTracksAssociatorAtVertex.jets   = cms.InputTag("ak5PFJets")
    if "PfEmbed" in embedType:
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
'''
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
'''
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

#process.patJetCorrFactorsL1Offset = process.patJetCorrFactors.clone(
#    levels = cms.vstring('L1Offset',
#                         'L2Relative',
#                         'L3Absolute')
#    )
#process.patJetCorrFactorsL1Offset.useRho = False #MB
#if runOnMC:
#    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute']
#else:
#    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual']

process.patJets.jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactors"),
                                                     #cms.InputTag("patJetCorrFactorsL1Offset")
                                                     )
#process.patDefaultSequence.replace(process.patJetCorrFactors,
#                                   process.patJetCorrFactors+process.patJetCorrFactorsL1Offset)

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

##################################################################

from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFMuonIso, setupPFElectronIso
process.muIsoSequence       = setupPFMuonIso(process,'muons')
process.electronIsoSequence = setupPFElectronIso(process,'gsfElectrons')
from CommonTools.ParticleFlow.pfParticleSelection_cff import pfParticleSelectionSequence
process.pfParticleSelectionSequence = pfParticleSelectionSequence
process.pfIsolationSequence = cms.Sequence(
    process.pfParticleSelectionSequence*
    process.muIsoSequence* process.electronIsoSequence
    )
        
#Custom cone size for Electron isolation
process.elPFIsoValueChargedAll04PFIdPFIso.deposits[0].vetos = cms.vstring(
    'EcalBarrel:ConeVeto(0.01)','EcalEndcaps:ConeVeto(0.015)',
    )
process.elPFIsoValueGamma04PFIdPFIso.deposits[0].vetos = cms.vstring(
    'EcalBarrel:ConeVeto(0.08)','EcalEndcaps:ConeVeto(0.08)',
    )
process.elPFIsoValuePU04PFIdPFIso.deposits[0].vetos = cms.vstring()
process.elPFIsoValueNeutral04PFIdPFIso.deposits[0].vetos = cms.vstring()

process.elPFIsoValueChargedAll04NoPFIdPFIso.deposits[0].vetos = cms.vstring(
    'EcalBarrel:ConeVeto(0.01)','EcalEndcaps:ConeVeto(0.015)',
    )
process.elPFIsoValueGamma04NoPFIdPFIso.deposits[0].vetos = cms.vstring(
    'EcalBarrel:ConeVeto(0.08)','EcalEndcaps:ConeVeto(0.08)',
    )
process.elPFIsoValuePU04NoPFIdPFIso.deposits[0].vetos = cms.vstring()
process.elPFIsoValueNeutral04PFIdPFIso.deposits[0].vetos = cms.vstring()

#Custom cone size for Muon isolation
process.muPFIsoValueChargedAll04PFIso.deposits[0].vetos = cms.vstring(
    '0.0001','Threshold(0.0)'
    )

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
        )
    )
process.patElectrons.isolationValuesNoPFId = cms.PSet(
    pfAllParticles   = cms.InputTag("elPFIsoValuePU04NoPFIdPFIso"),
    pfChargedHadrons = cms.InputTag("elPFIsoValueCharged04NoPFIdPFIso"),
    pfNeutralHadrons = cms.InputTag("elPFIsoValueNeutral04NoPFIdPFIso"),
    pfPhotons        = cms.InputTag("elPFIsoValueGamma04NoPFIdPFIso"),
    user = cms.VInputTag(
        cms.InputTag("elPFIsoValueChargedAll04NoPFIdPFIso")
        )
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
if runOnEmbed:
    process.patTriggerEvent.processName = 'HLT'

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'
    if runOnEmbed:
        process.patTrigger.processName = 'HLT'
        
######################## embedding ###############################

process.selectedPatMuonsUserEmbedded = cms.EDProducer(
    "MuonsUserEmbedded",
    muonTag            = cms.InputTag("selectedPatMuons"),
    vertexTag          = cms.InputTag("offlinePrimaryVertices"),
    fitUnbiasedVertex  = cms.bool(False)
    )

process.selectedPatElectronsUserEmbedded = cms.EDProducer( #MIT MVA Ids to be removed 
    "ElectronsUserEmbedded",
    electronTag = cms.InputTag("selectedPatElectrons"),
    vertexTag   = cms.InputTag("offlinePrimaryVertices"),
    isMC        = cms.bool(runOnMC),
    #doMVAMIT    = cms.bool(True),
    doMVADaniele= cms.bool(True),
    #inputFileName0 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet0LowPt_NoIPInfo_BDTG.weights.xml'),
    #inputFileName1 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet1LowPt_NoIPInfo_BDTG.weights.xml'),
    #inputFileName2 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet2LowPt_NoIPInfo_BDTG.weights.xml'),
    #inputFileName3 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet0HighPt_NoIPInfo_BDTG.weights.xml'),
    #inputFileName4 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet1HighPt_NoIPInfo_BDTG.weights.xml'),
    #inputFileName5 = cms.FileInPath('UserCode/MitPhysics/data/ElectronMVAWeights/Subdet2HighPt_NoIPInfo_BDTG.weights.xml'),
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
    inputFileName0v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat1.weights.xml'),
    inputFileName1v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat2.weights.xml'),
    inputFileName2v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat3.weights.xml'),
    inputFileName3v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat4.weights.xml'),
    inputFileName4v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat5.weights.xml'),
    inputFileName5v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat6.weights.xml'),
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
                     "&& abs(userFloat('dzWrtPV'))<0.2 && abs(userFloat('dxyWrtPV'))<0.045"
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
    cut = cms.string("pt>8 && abs(eta)<2.4"+
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
    cut = cms.string("pt>18 && abs(eta)<2.3"),  #Arun, relax Pt cut, Tau ES correction at tree level
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
                     " && pt>20 && (tauID('byLooseIsolationMVA2')>0.5||tauID('byLooseCombinedIsolationDeltaBetaCorr3Hits')>0.5)"),
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
                     " && ( tauID('againstElectronLoose')>0.5 || tauID('againstElectronLooseMVA3')>0.5 )"),
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
##Loose MVA ID used for e-mu and 3d lepton veto selection
MVALoose = "((pt<=20 && abs(superCluster.eta)>=0.0 && abs(superCluster.eta)<0.8 && userFloat('mvaPOGNonTrig')>0.925) ||" + \
           "(pt<=20 && abs(superCluster.eta)>=0.8 && abs(superCluster.eta)<1.479 && userFloat('mvaPOGNonTrig')>0.915) ||" + \
           "(pt<=20 && abs(superCluster.eta)>=1.479 && abs(superCluster.eta)<2.5 && userFloat('mvaPOGNonTrig')>0.965) ||" + \
           "(pt>20  && abs(superCluster.eta)>=0.0 && abs(superCluster.eta)<0.8 && userFloat('mvaPOGNonTrig')>0.905) ||" + \
           "(pt>20  && abs(superCluster.eta)>=0.8 && abs(superCluster.eta)<1.479 && userFloat('mvaPOGNonTrig')>0.955) ||" + \
           "(pt>20  && abs(superCluster.eta)>=1.479 && abs(superCluster.eta)<2.5 && userFloat('mvaPOGNonTrig')>0.975))"
##Loose MVA ID: new e-Id, same FR as old one
MVALooseNew = "((pt<=20 && abs(superCluster.eta)>=0.0 && abs(superCluster.eta)<0.8 && userFloat('mvaPOGTrigNoIP')>-0.5375) ||" + \
              "(pt<=20 && abs(superCluster.eta)>=0.8 && abs(superCluster.eta)<1.479 && userFloat('mvaPOGTrigNoIP')>-0.375) ||" + \
              "(pt<=20 && abs(superCluster.eta)>=1.479 && abs(superCluster.eta)<2.5 && userFloat('mvaPOGTrigNoIP')>-0.025) ||" + \
              "(pt>20  && abs(superCluster.eta)>=0.0 && abs(superCluster.eta)<0.8 && userFloat('mvaPOGTrigNoIP')>0.325) ||" + \
              "(pt>20  && abs(superCluster.eta)>=0.8 && abs(superCluster.eta)<1.479 && userFloat('mvaPOGTrigNoIP')>0.775) ||" + \
              "(pt>20  && abs(superCluster.eta)>=1.479 && abs(superCluster.eta)<2.5 && userFloat('mvaPOGTrigNoIP')>0.775))"

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
                     " && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2"+
                     " && userFloat('nHits')==0 && userInt('antiConv')>0.5 &&"+ 
                     "("+MVALoose+"||"+MVALooseNew+")"
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
                     "abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2 &&"+
                     #"abs(userFloat('dzWrtPV'))<0.2 &&"+
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
                     " && abs(userFloat('dxyWrtPV'))<0.045 && abs(userFloat('dzWrtPV'))<0.2 && "+
                     #" && ( ( pt > 20 && ( (abs(superCluster.eta)<0.80 && userFloat('mvaPOGNonTrig')>0.905) || (abs(superCluster.eta)<1.479 && abs(superCluster.eta)>0.80 && userFloat('mvaPOGNonTrig')>0.955) || (abs(superCluster.eta)>1.479 && userFloat('mvaPOGNonTrig')>0.975) )) || (pt < 20 && ( (abs(superCluster.eta)<0.80 && userFloat('mvaPOGNonTrig')>0.925) || (abs(superCluster.eta)<1.479 && abs(superCluster.eta)>0.80 && userFloat('mvaPOGNonTrig')>0.915) || (abs(superCluster.eta)>1.479 && userFloat('mvaPOGNonTrig')>0.965) )))"+
                     "("+MVALoose+"||"+MVALooseNew+")"
                     " && userFloat('nHits')==0 && userInt('antiConv')>0.5"+
                     " && userFloat('PFRelIsoDB04v3')<0.30"),
    filter = cms.bool(False)
    )

process.atLeastOneElecTautauPtEta = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEta")
    )
process.elecTauPtEtaCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEta"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.atLeastOneElecTautauPtEtaID = process.atLeastOneElecTau.clone(
    decay=cms.string("elecPtEtaID tauPtEtaID")
    )
process.elecTauPtEtaIDCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("atLeastOneElecTautauPtEtaID"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.tauPtEtaIDAgMuL  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaID.cut.value()+
                     " && ( tauID('againstMuonLoose')>0.5 || tauID('againstMuonLoose2')>0.5 )"),
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

process.tauPtEtaIDAgMuLAgElecRelIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(process.tauPtEtaIDAgMuLAgElec.cut.value()+
                     " && tauID('byVLooseCombinedIsolationDeltaBetaCorr')>0.5"),
    filter = cms.bool(False)
    )

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
    #process.pfParticleSelectionSequence*
    #process.muIsoSequence*
    #process.electronIsoSequence*
    process.pfIsolationSequence*
    (process.ak5JetTracksAssociatorAtVertex*process.btagging)*
    process.sumCaloTowersInEtaSlicesNoHF* #Arun
    process.patDefaultSequence*
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
    ##+process.jetCleaningSequence
    ##+process.printTree1
    )

process.skimMuTau2 = cms.Sequence(
    process.allEventsFilter+
    process.pseudoMuTauETMtriggerSequence*
    process.commonOfflineSequence*
    process.atLeastOneMuTauSequence*
    process.muLegSequence*
    process.tauLegForMuTauSequence
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

##Arun:  change vertex for the PFLow isolation?
massSearchReplaceAnyInputTag(process.pfIsolationSequence,
                             "offlinePrimaryVertices",
                             "selectedPrimaryVertices",
                             verbose=False)

process.load("LLRAnalysis.Utilities.recoTauMatchedCaloJet_cff")
process.selectedPatTausForCaloJetMatching.src="tausForVeto"
process.selectedPatTausForCaloJetMatching.cut="tauID('byLooseCombinedIsolationDeltaBetaCorr3Hits')>0.5"
process.commonOfflineSequence += process.makeRecoTauMatchedCaloJets
if runOnMC or runOnEmbed:
    process.load("LLRAnalysis.Utilities.genTauMatchedCaloJet_cff")
    process.commonOfflineSequence += process.makeTauMatchedCaloJets

if not runOnMC:
    ##process.skimMuTau1.remove(process.printTree1)
    process.skimMuTau1.remove(process.HLTFilterMuTau)
    process.skimElecTau1.remove(process.HLTFilterEleTau)
if not runOnEmbed:
    process.commonOfflineSequence.remove(process.ak5JetTracksAssociatorAtVertex)
    process.commonOfflineSequence.remove(process.btagging)
if runOnMC and runOnEmbed:
    process.skimMuTau1.remove(process.HLTFilterMuTau)
    process.skimElecTau1.remove(process.HLTFilterEleTau)
if runOnEmbed:
    if "RhEmbed" in embedType:
        process.load("TauAnalysis.MCEmbeddingTools.embeddingKineReweight_cff")
        if not runOnMC:
            process.commonOfflineSequence += process.embeddingKineReweightSequenceRECembedding
        else:
            process.commonOfflineSequence += process.embeddingKineReweightSequenceGENembedding
        if "MuTau" in embedType:
            process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_recEmbedding_mutau.root')
            process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_genEmbedding_mutau.root")
        else:
            process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_recEmbedding_etau.root')
            process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_genEmbedding_etau.root")

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
    'keep *_muons_*_*',
    'keep *_globalMuons_*_*', #AN: needed for noPU MET
    'keep *_elecPtEtaID_*_*',
    'keep *_elecPtEtaRelID_*_*',
    'keep *_electronsForVeto_*_*',
    'keep *_muonsForVeto_*_*',
    'keep *_tausForVeto_*_*',
    'keep *_addPileupInfo_*_*',
    'keep *_generalTracks_*_*',
    'keep *_tmfTracks_*_*',
    'keep *_generalTracksORG_*_*', #AN: needed for rechit Embed
    'keep *_electronGsfTracks_*_*',
    'keep recoTrackExtras_*_*_*',
    'keep recoGsfTrackExtras_*_*_*',
    'keep *_tauPtEtaIDAgMuAgElec_*_*',
    'keep *_tauPtEtaIDAgMuLAgElec_*_*',#MB+
    'keep *_generator_*_*',
    'keep *_source_*_*',
    'keep *_l1extraParticles_*_*',#MB+
    'keep *_selectedHltPatTaus_*_*',#MB+
    'keep *_reducedEcalRecHitsEB_*_*',
    'keep *_reducedEcalRecHitsEE_*_*',
    #'drop *_TriggerResults_*_HLT',#MB
    #'drop *_TriggerResults_*_RECO',#MB
    'drop *_selectedPatElectrons_*_*',
    'drop *_selectedPatMuons_*_*',
    'drop *_selectedPatTaus_*_*',
    #MB'drop *_patMETs_*_*',
    'keep *_selectedPatMuonsUserEmbedded_*_*', #AN+
    'drop *_selectedPatElectronsUserEmbedded_*_*',
    'keep *_selectedPatElectronsUserEmbeddedIso_*_*', #AN+
    'drop *_selectedPatTausUserEmbedded_*_*',
    'keep *_puJetId_*_*',
    'keep *_puJetMva_*_*',
    #MB'keep *_hfEMClusters_*_*',
    #MB'keep *_hybridSuperClusters_*_*',
    #MB'keep *_multi5x5BasicClusters_*_*',
    #MB'keep *_pfElectronTranslator_*_*',
    #MB'keep *_pfPhotonTranslator_*_*',
    #MB'keep *_hybridSuperClusters_*_*',
    #CV
    'keep *_particleFlowDisplacedVertex_*_*',
    'keep *_pfMet_*_*',
    'keep *_sumCaloTowersInEtaSlicesNoHF_*_*',
    'keep *_generalV0Candidates_Kshort_*',
    'keep *_generalV0Candidates_Lambda_*',
    'keep *_gsfElectrons_*_*',
    'keep *_allConversions_*_*',
    'keep *_genMetTrue_*_*',
    'keep *_genTauMatchedCaloJet*_*_*',
    'keep *_genTauMatchedCaloJetElec_*_*',
    'keep *_genTauMatchedCaloJetMu_*_*',    
    'keep *_patTauMatchedCaloJet_*_*',
    #To re-make patJets
    'keep *_jetTracksAssociatorAtVertex_*_*',
    'keep *_ak5GenJets_*_*',
    'keep *_*JetTagsAOD_*_*',
    'keep *_TauSpinnerReco_TauSpinnerWT*_*',
    'keep *_ZmumuEvtSelEffCorrWeightProducer_weight_*',
    'keep *_muonRadiationCorrWeightProducer_weight_*',
    'keep *_muonRadiationCorrWeightProducer_weightUp_*',
    'keep *_muonRadiationCorrWeightProducer_weightDown_*',
    'keep *_embeddingKineReweightRECembedding_genTau2PtVsGenTau1Pt_*',
    'keep *_embeddingKineReweightRECembedding_genTau2EtaVsGenTau1Eta_*',
    'keep *_embeddingKineReweightRECembedding_genDiTauMassVsGenDiTauPt_*',
    'keep *_embeddingKineReweightGENembedding_genTau2PtVsGenTau1Pt_*',
    'keep *_embeddingKineReweightGENembedding_genTau2EtaVsGenTau1Eta_*',
    'keep *_embeddingKineReweightGENembedding_genDiTauMassVsGenDiTauPt_*',
    'keep *_generator_minVisPtFilter_*',
    'keep *_goldenZmumuCandidatesGe2IsoMuons_*_*',
    )
)
#MB
'''
if runOnEmbed:
    process.out.outputCommands.extend( cms.vstring(
        'keep *_towerMaker_*_*',
        )
                                   )
'''          
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

