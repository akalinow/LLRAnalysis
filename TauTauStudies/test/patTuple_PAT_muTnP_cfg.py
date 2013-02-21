from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 1

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

process.source.fileNames = cms.untracked.vstring(
    'file:/data_CMS/cms/ivo/RootFiles/DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball_START52_V9/4C6532AF-CF96-E111-8563-003048D47752.root'
    #'file:/data_CMS/cms/ivo/RootFiles/SingleMu2012B/CAA8ABDD-72AD-E111-9601-0030486780A8.root'
)

postfix           = "PFlow"
runOnMC           = True
FileName          = "treeMuTnP.root"

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START52_V9::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_R_52_V7::All')
    
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

###### Isolation sequence ############################################################
from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFMuonIso, setupPFElectronIso
process.muIsoSequence       = setupPFMuonIso(process,'muons')
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

######################## Trigger requirements ############################
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.HLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.HLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults", "", "HLT")
process.HLTFilter.HLTPaths = [
    # single electron triggers (Summer'12 MC, produced with CMSSW_5_2_x)
    'HLT_IsoMu24_eta2p1_v11',
    # single electron triggers (all of 2012 Run A)
    'HLT_IsoMu24_eta2p1_v11',
    'HLT_IsoMu24_eta2p1_v12',
    # single electron triggers (all of 2012 Run B)
    'HLT_IsoMu24_eta2p1_v13',
]
process.HLTFilter.throw = cms.bool(False)

######################## pat::trigger ############################
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )
process.patTriggerEvent.processName = '*'

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'

########################  pat::muon  #############################

getattr(process,"patMuons").embedTrack = True


######################## pat::electrons ############################
process.selectedPatMuonsUserEmbedded = cms.EDProducer(
    "MuonsUserEmbedded",
    muonTag = cms.InputTag("selectedPatMuons"),
    vertexTag = cms.InputTag("offlinePrimaryVertices"),
    isMC = cms.bool(runOnMC),
    fitUnbiasedVertex  = cms.bool(False)
    )

#####################################################################################
#####################################################################################
process.load("LLRAnalysis.TauTauStudies.muTnP_cff")

if not runOnMC:
    process.sequence.remove(process.tagMcMatch)
    process.sequence.remove(process.probeMcMatch)
    process.muTnP.isMC = cms.bool(False)
    process.muTnP.makeMCUnbiasTree = cms.bool(False)
    process.addUserVariablesTag.isMC = cms.bool(False)
    process.addUserVariablesProbe.isMC = cms.bool(False)


process.pat = cms.Sequence(
    process.allEventsFilter+
    process.primaryVertexFilter*
    process.pfParticleSelectionSequence*
    process.muIsoSequence*
    process.patDefaultSequence*
    process.HLTFilter*
    process.selectedPatMuonsUserEmbedded*
    process.sequence
    )

process.p = cms.Path(process.pat)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(FileName)
                                   )

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('p')
    )

process.out.fileName = cms.untracked.string('patTuples_muTnP.root')
process.outpath = cms.EndPath()
