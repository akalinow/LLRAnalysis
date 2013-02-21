from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 1

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

process.source.fileNames = cms.untracked.vstring(
    #'file:/data_CMS/cms/ivo/RootFiles/DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball_START52_V9/4C6532AF-CF96-E111-8563-003048D47752.root'
    'file:/data_CMS/cms/ivo/RootFiles/SingleEle2012A/3686680B-0C81-E111-ACE6-E0CB4E4408E7.root'
 )

postfix           = "PFlow"
runOnMC           =  False
FileName          = "treeElecTnP.root"

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
from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFElectronIso
process.electronIsoSequence = setupPFElectronIso(process,'gsfElectrons')
from CommonTools.ParticleFlow.pfParticleSelection_cff import pfParticleSelectionSequence
process.pfParticleSelectionSequence = pfParticleSelectionSequence

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

######################## Trigger requirements ############################
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.HLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.HLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults", "", "HLT")
process.HLTFilter.HLTPaths = [
    # single electron triggers (Summer'12 MC, produced with CMSSW_5_2_x)
    'HLT_Ele27_WP80_v4',
    'HLT_Ele27_WP80_v5',
    # single electron triggers (all of 2012 Run A)
    'HLT_Ele27_WP80_v6',
    'HLT_Ele27_WP80_v7',
    'HLT_Ele27_WP80_v8',
    'HLT_Ele27_WP80_v9',
    'HLT_Ele27_WP80_v10'
]
process.HLTFilter.throw = cms.bool(False)


######################## pat::trigger ############################

from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )
process.patTriggerEvent.processName = '*'

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'
    
getattr(process,"patElectrons").embedTrack = True
getattr(process,"patElectrons").embedGsfTrack = True

######################## pat::electrons ############################
process.selectedPatElectronsUserEmbedded = cms.EDProducer(
    "ElectronsUserEmbedded",
    electronTag = cms.InputTag("selectedPatElectrons"),
    vertexTag = cms.InputTag("offlinePrimaryVertices"),
    isMC = cms.bool(runOnMC),
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
    inputFileName5v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat6.weights.xml')
   
    )


#####################################################################################
#####################################################################################
process.load("LLRAnalysis.TauTauStudies.elecTnP_cff")

if not runOnMC:
    process.sequence.remove(process.tagMcMatch)
    process.sequence.remove(process.probeMcMatch)
    process.elecTnP.isMC = cms.bool(False)
    process.elecTnP.makeMCUnbiasTree = cms.bool(False)
    process.addUserVariablesProbe.isMC = cms.bool(False)
    process.addUserVariablesTag.isMC = cms.bool(False)

process.pat = cms.Sequence(
    process.allEventsFilter+
    process.primaryVertexFilter*
    process.pfParticleSelectionSequence*
    process.electronIsoSequence*
    process.patDefaultSequence*
    process.HLTFilter*
    process.selectedPatElectronsUserEmbedded*
    process.sequence
    )

process.p = cms.Path(process.pat)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(FileName)
                                   )

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('p')
    )

process.out.fileName = cms.untracked.string('patTuples_elecTnP.root')
process.outpath = cms.EndPath()

## processDumpFile = open('patTuple_PAT_electauTnP.dump', 'w')
## print >> processDumpFile, process.dumpPython()

