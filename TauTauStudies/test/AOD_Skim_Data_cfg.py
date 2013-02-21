## AOD skim for l+tau analysis
## 
## Last modificaion: 16/11/2012 Michal
## TODO: 
##
###############################################################################

## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *
process.setName_("AODSkimm")

runOnMC     = False
runOnEmbed  = False

################### global tag ##################
if runOnMC:
    process.GlobalTag.globaltag = cms.string('START53_V10::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_P_V41_AN1::All')


################### global options ##################
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
#### counting sequences 
#process.load("PFAnalyses.CommonTools.countingSequences_cfi")
# DQM (used for event counters, collect host not needed)
#process.DQM.collectorHost = ''

################### source ##################
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.source.fileNames = cms.untracked.vstring(
    #'root://polgrid4.in2p3.fr//dpm/in2p3.fr/home/cms/trivcat/store/mc/Summer12_DR53X/DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball/AODSIM/PU_S10_START53_V7A-v1/0000/FE414F4B-F6D2-E111-A4E9-003048674048.root' #12.5k
    #'file:/data_CMS/cms/anayak/HTauTauSynchronization/8TeV/53X/VBF_HToTauTau_M-125_8TeV-powheg-pythia6-Summer12_DR53X-PU_S10_START53_V7A-v1-1200542B-D9ED-E111-B708-00237DA1A548.root' #7k
    'file:/data_CMS/cms/mbluj/Production/test/WJetsToLNu_TuneZ2Star_8TeV-madgraph-tarball-Summer12_DR53X-PU_S10_START53_V7A-v2-AODSIM-FE4F82A9-68F3-E111-8CD3-003048D476AE.root' #13k
    #'root://node12.datagrid.cea.fr//dpm/datagrid.cea.fr/home/cms/trivcat/store/mc/Summer12_DR53X/W3JetsToLNu_TuneZ2Star_8TeV-madgraph/AODSIM/PU_S10_START53_V7A-v1/0000/0088DD9F-B105-E211-BB06-001E67397D5A.root' #no access??
    #'file:/data_CMS/cms/mbluj/Production/test/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola-Summer12_DR53X-PU_S10_START53_V7A-v2-AODSIM-FED775BD-B8E1-E111-8ED5-003048C69036.root' #11k
    #'file:/data_CMS/cms/mbluj/Production/test/W3JetsToLNu_TuneZ2Star_8TeV-madgraph-Summer12_DR53X-PU_S10_START53_V7A-v2-AODSIM-FC3ED802-E606-E211-A0FD-0025B31E330A.root' #9k
    )
#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:751063'
#    )

##########################################################################
# Don't modify anything below this line unless you know what you are doing


################### output module ##################
process.out.fileName = cms.untracked.string('tautauSkimmAOD.root')
process.out.dataset = cms.untracked.PSet(filterName = cms.untracked.string('aodSkimm'))
process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring("p")
    )
process.out.outputCommands = cms.untracked.vstring('keep *',
                                                   'drop *_*_*_AODSkimm',
                                                   'keep *_MEtoEDMConverter_*_AODSkimm',
                                                   'keep *_TriggerResults_*_AODSkimm',
                                                   'keep *_selectedHltPatTaus_*_AODSkimm',
                                                   )

################### print event content ##################
process.printEventContent = cms.EDAnalyzer("EventContentAnalyzer")

################### gen listing  ####################

process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.printTree1 = cms.EDAnalyzer(
    "ParticleListDrawer",
    src = cms.InputTag("genParticles"),
    maxEventsToPrint  = cms.untracked.int32(1)
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
#process.load("LLRAnalysis.TauTauStudies.emulateHLTPFTau_cff")

process.pseudoMuTauETMtriggerSequence = cms.Sequence(
    #process.HLTFilterSingleMu+
    #process.goodL1Mu7er+
    process.goodL1ETM20
    #+process.hltPFTauSequence
    #+process.hltPFTauFilterSeqence
    )
process.pseudoEleTauETMtriggerSequence = cms.Sequence(
    #process.HLTFilterSingleEle+
    #process.goodL1IsoEG12er+
    process.goodL1ETM30
    #+process.hltPFTauSequence
    #+process.hltPFTauFilterSeqence
    )

################### Offline filtering  ####################

#### Muons
## selection
muonSelectionString = cms.string('pt > 7 & abs(eta) < 2.1 & isPFMuon & isGlobalMuon & isTrackerMuon')
#process.selectedPatMuons.cut = muonSelectionString
process.filterRecoMuons = cms.EDFilter("MuonRefSelector",
                                       src = cms.InputTag("muons"),
                                       cut = muonSelectionString,
                                       filter = cms.bool(True)
                                       )

#### Electrons
## selection
simpleEleCuts = "(gsfTrack.trackerExpectedHitsInner.numberOfHits<1)"
electronSelectionString = cms.string("pt > 12 & abs(eta) < 2.1 & "+simpleEleCuts)

#process.selectedPatElectrons.cut = electronSelectionString
process.filterRecoElectrons = cms.EDFilter("GsfElectronRefSelector",
                                           src = cms.InputTag("gsfElectrons"),
                                           cut = electronSelectionString,
                                           filter = cms.bool(True)
                                           )

#### Tau-jets
## selection
#custom selector in LLRAnalysis/TauTauStudies/plugins/MyPFJetRefSelector.cc
#clone of CommonTools/RecoAlgos/plugins/CaloJetRefSelector.cc
process.filterRecoPFJets = cms.EDFilter("MyPFJetRefSelector",
                                        src = cms.InputTag("ak5PFJets"),
                                        cut = cms.string("pt > 18 &  abs(eta) < 2.5"), ## raw pfjet in tau acceptance
                                        filter = cms.bool(True)
                                        )

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

################### l+tau-jet pairing ####################

process.muTauCandidate = cms.EDProducer(
    "CandViewShallowCloneCombiner",
    decay = cms.string("filterRecoMuons filterRecoPFJets"),
    cut = cms.string("sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  ) *  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  )  )>0.5"),
    checkCharge = cms.bool(False)
    )

process.atLeastOneMuTauCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("muTauCandidate"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

process.eleTauCandidate = cms.EDProducer(
    "CandViewShallowCloneCombiner",
    decay = cms.string("filterRecoElectrons filterRecoPFJets"),
    cut = cms.string("sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  ) *  min( abs(daughter(0).phi-daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi-daughter(1).phi)  )  )>0.5"),
    checkCharge = cms.bool(False)
    )

process.atLeastOneEleTauCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("eleTauCandidate"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

########################## offline sequences ###############################

process.offlineMuTauSequence = cms.Sequence(
    # vertex
    process.selectedPrimaryVertices+
    process.primaryVertexCounter+
    # ele
    process.filterRecoMuons+
    # tau-jet
    process.filterRecoPFJets+
    # ele+tau-jet
    process.muTauCandidate+
    process.atLeastOneMuTauCounter
    )

process.offlineEleTauSequence = cms.Sequence(
    # vertex
    process.selectedPrimaryVertices+
    process.primaryVertexCounter+
    # ele
    process.filterRecoElectrons+
    # tau-jet
    process.filterRecoPFJets+
    # ele+tau-jet
    process.eleTauCandidate+
    process.atLeastOneEleTauCounter
    )


########################## paths ###############################

process.pEleTau1 = cms.Path(
    process.printTree1+
    process.HLTFilterEleTau+
    process.offlineEleTauSequence
    )

process.pMuTau1 = cms.Path(
    #process.printTree1+
    process.HLTFilterMuTau+
    process.offlineMuTauSequence
    )

# emulated trigger bits only for MC
if runOnMC:
    process.pEleTau2 = cms.Path(
        #process.printTree1+
        process.pseudoEleTauETMtriggerSequence+
        process.offlineEleTauSequence
        )
    process.pMuTau2 = cms.Path(
        #process.printTree1+
        process.pseudoMuTauETMtriggerSequence+
        process.offlineMuTauSequence
        )

if not runOnMC:
    process.pEleTau1.remove(process.printTree1)

################################################
#### define which events will be stored 
process.out.SelectEvents.SelectEvents = cms.vstring("pEleTau1",
                                                    "pEleTau2",
                                                    "pMuTau1",
                                                    "pMuTau2")
## do not use emulated triggers for data
if not runOnMC:
    process.out.SelectEvents.SelectEvents = cms.vstring("pEleTau1",
                                                        "pMuTau1")

