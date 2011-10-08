import FWCore.ParameterSet.Config as cms

process = cms.Process('readLHE')

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.MixingNoPileUp_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContent_cff')

process.GlobalTag.globaltag = 'MC_38Y_V13::All'

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.MessageLogger.cerr.FwkReport.reportEvery = 10

# Input source
process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring('file:test.root')
    )

process.readLHE = cms.EDAnalyzer(
    "LHEAnalyzer",
    src = cms.InputTag("generator"),
    nBins = cms.uint32(20),
    )

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("ttbar_plots.root")
    )

process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *'),
    fileName = cms.untracked.string('ttbar_plots.root'),
    )

process.seq = cms.Sequence( process.readLHE )
process.p = cms.Path( process.seq )

