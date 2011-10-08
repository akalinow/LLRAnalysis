import FWCore.ParameterSet.Config as cms

from Configuration.Generator.PythiaUESettings_cfi import *

process = cms.Process("TEST")
process.load("FWCore.Framework.test.cmsExceptionsFatal_cff")
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
#process.load("SimGeneral.HepPDTESSource.pdt_cfi")

process.load("Configuration.StandardSequences.Services_cff")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
    )


process.RandomNumberGeneratorService = cms.Service(
    "RandomNumberGeneratorService",
    generator = cms.PSet(
    initialSeed = cms.untracked.uint32(123456789),
    engineName = cms.untracked.string('HepJamesRandom')
    )
    )

process.randomEngineStateProducer = cms.EDProducer("RandomEngineStateProducer")

# The following three lines reduce the clutter of repeated printouts
# of the same exception message.
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.destinations = ['cerr']
process.MessageLogger.statistics = []
process.MessageLogger.fwkJobReports = []

#process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(50))

process.source = cms.Source("LHESource",
    fileNames = cms.untracked.vstring('file:/tmp/bianchi/fermi_events.lhe')
)

process.generator = cms.EDFilter(
    "Pythia6HadronizerFilter",
    pythiaHepMCVerbosity = cms.untracked.bool(True),
    maxEventsToPrint = cms.untracked.int32(0),
    pythiaPylistVerbosity = cms.untracked.int32(1),
    comEnergy = cms.double(7000.0),
    PythiaParameters = cms.PSet(
    pythiaUESettingsBlock,
    processParameters = cms.vstring('MSEL=0         ! User defined processes', 
                                    'PMAS(5,1)=4.4   ! b quark mass',
                                    'PMAS(6,1)=172.4 ! t quark mass',
                                    'MSTJ(1)=1       ! Fragmentation/hadronization on or off',
                                    'MSTP(61)=1      ! Parton showering on or off'),
    # This is a vector of ParameterSet names to be read, in this order
    parameterSets = cms.vstring('pythiaUESettings', 
                                'processParameters')
    ),
    jetMatching = cms.untracked.PSet(
    scheme = cms.string("Madgraph"),
    mode = cms.string("auto"),	# soup, or "inclusive" / "exclusive"
    MEMAIN_etaclmax = cms.double(5.0),
    MEMAIN_qcut = cms.double(15.0),
    MEMAIN_minjets = cms.int32(0),
    MEMAIN_maxjets = cms.int32(5),
    MEMAIN_showerkt= cms.double(1),
    MEMAIN_excres = cms.string(""),
    outTree_flag = cms.int32(0)  
    )    
    )

process.GEN = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string('testMyProcess.root'),
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('p'))
)

process.p = cms.Path(process.generator)
process.p1 = cms.Path(process.randomEngineStateProducer)
process.outpath = cms.EndPath(process.GEN)

process.schedule = cms.Schedule(process.p, process.p1, process.outpath)
