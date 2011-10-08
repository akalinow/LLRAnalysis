import FWCore.ParameterSet.Config as cms

process = cms.Process("ANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
process.load('RecoJets.Configuration.RecoPFJets_cff')

process.ak5PFL1Fastjet.useCondDB = False
process.kt6PFJets.doRhoFastjet = True

process.kt6PFJets.doRhoFastjet = True
process.kt6PFJets.Rho_EtaMax = cms.double(4.4)
process.kt6PFJets.Ghost_EtaMax = cms.double(5.0)
process.ak5PFJets.doAreaFastjet = True
process.ak5PFJets.Rho_EtaMax = cms.double(4.4)
process.ak5PFJets.Ghost_EtaMax = cms.double(5.0)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10

process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.printTree1 = cms.EDAnalyzer("ParticleListDrawer",
                                    src = cms.InputTag("genParticles"),
                                    maxEventsToPrint  = cms.untracked.int32(1)
                                    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )


process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Fall10/VBF_HToTauTau_M-115_7TeV-powheg-pythia6-tauola/GEN-SIM-RECO/START38_V12-v1/0000/044E940A-55EC-DF11-89D6-0023AEFDEE60.root',
    'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Fall10/VBF_HToTauTau_M-115_7TeV-powheg-pythia6-tauola/GEN-SIM-RECO/START38_V12-v1/0000/0C277B13-43EC-DF11-BB3E-0021286B81DA.root',
    'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Fall10/VBF_HToTauTau_M-115_7TeV-powheg-pythia6-tauola/GEN-SIM-RECO/START38_V12-v1/0000/1A27547A-4AEC-DF11-9052-0021286B7D72.root',
    )
    )

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
    )

process.vbfJetAnalyzer = cms.EDAnalyzer(
    "VbfJetAnalyzer",
    verbose = cms.untracked.bool(True),
    
    )

process.p = cms.Path(
    (process.kt6PFJets * process.ak5PFJets )*
    process.vbfJetAnalyzer
    #+process.printTree1
    )

process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring( 'keep *'),
    fileName = cms.untracked.string('vbfHTauTau_Fall10.root'),
    )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("vbfTree.root")
                                   )

#process.outpath = cms.EndPath(process.out)
process.outpath = cms.EndPath()
