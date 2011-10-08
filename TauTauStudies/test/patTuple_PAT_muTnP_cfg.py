from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 2000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

process.source.fileNames = cms.untracked.vstring(
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Summer11/WH_ZH_TTH_HToWW_M-120_7TeV-pythia6//AODSIM/PU_S3_START42_V11-v1//0000/E430E306-347C-E011-A75A-00261834B5C6.root',
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/mc/Summer11/DYToMuMu_M-120_TuneZ2_7TeV-pythia6-tauola/AODSIM/PU_S3_START42_V11-v2/0000/2EE73A4A-8288-E011-9561-1CC1DE1CEFC8.root',
    'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/data/Run2011A/SingleMu/AOD//May10ReReco-v1/0000/FEC692C5-B67B-E011-A46E-1CC1DE1CED22.root',
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/data/Run2011A/SingleMu/AOD//May10ReReco-v1/0000/FE7A9E7C-AE7B-E011-9FEE-1CC1DE1D208E.root',
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/data/Run2011A/SingleMu/AOD//May10ReReco-v1/0000/FE412F53-627B-E011-9245-0018FEFA56AE.root',
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/data/Run2011A/SingleMu/AOD//May10ReReco-v1/0000/FE1E1891-E47C-E011-9983-001E0B5FA5A8.root',
    )

postfix           = "PFlow"
runOnMC           = True
FileName          = "treeMuTnP.root"

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START42_V13::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_R_42_V19::All')


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

addPFMuonIsolation(process,process.patMuons)

addTriggerMatchingMuon(process,isMC=runOnMC)
process.muonTriggerMatchHLTMuons.matchedCuts = cms.string('(path("HLT_IsoMu17_v*",0,0) || path("HLT_IsoMu24_v*",0,0) || filter("hltSingleMuIsoL3IsoFiltered12") || filter("hltSingleMuIsoL3IsoFiltered15")) && type("TriggerMuon")')

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'

process.selectedPatMuonsTriggerMatchUserEmbedded = cms.EDProducer(
    "MuonsUserEmbedded",
    muonTag = cms.InputTag("selectedPatMuonsTriggerMatch"),
    vertexTag = cms.InputTag("offlinePrimaryVertices"),
    isMC = cms.bool(runOnMC)
    )

#####################################################################################
#####################################################################################
process.load("LLRAnalysis.TauTauStudies.muTnP_cff")

if not runOnMC:
    process.sequence.remove(process.tagMcMatch)
    process.sequence.remove(process.probeMcMatch)
    process.muTnP.isMC = cms.bool(False)
    process.muTnP.makeMCUnbiasTree = cms.bool(False)
    process.addUserVariables.isMC = cms.bool(False)


process.pat = cms.Sequence(
    process.allEventsFilter+
    process.primaryVertexFilter*
    process.patDefaultSequence*
    process.selectedPatMuonsTriggerMatchUserEmbedded*
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
