import FWCore.ParameterSet.Config as cms

process = cms.Process("COPY")


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.printTree1 = cms.EDAnalyzer("ParticleListDrawer",
                                    src = cms.InputTag("genParticles"),
                                    maxEventsToPrint  = cms.untracked.int32(5)
                                    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )


process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    'rfio:/dpm/in2p3.fr/home/cms/trivcat//store/mc/Fall10/VBF_HToTauTau_M-115_7TeV-powheg-pythia6-tauola/GEN-SIM-RECO/START38_V12-v1/0000/044E940A-55EC-DF11-89D6-0023AEFDEE60.root',
    )
    )

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
    )

process.p = cms.Path( process.printTree1 )

process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring( 'keep *'),
    fileName = cms.untracked.string('vbfHTauTau_Fall10.root'),
)

#process.outpath = cms.EndPath(process.out)
