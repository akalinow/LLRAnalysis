#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import sys

# Check if someone used cmsRun instead of running as a script
argument_offset = 0
if sys.argv[0] == 'cmsRun':
    argument_offset = 1

if not len(sys.argv) > 2+argument_offset:
    sys.exit("Usage: %s [outputFile] [inputFile1] [inputFile2] ..." % sys.argv[0])

outputFileName = sys.argv[1+argument_offset]
inputFileNames = sys.argv[(2+argument_offset):]

process = cms.Process('genericHarvester')
process.DQMStore = cms.Service("DQMStore")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(0))

process.source = cms.Source("EmptySource")

def fix_file_name(file):
    " Add an file: or rfio: if needed "
    if file.startswith('rfio:') or file.startswith('file:') \
       or file.startswith('/store/'):
        # Already formatted
        return file
    if file.startswith('/castor/cern.ch'):
        return 'rfio:' + file
    return 'file:' + file

process.load = cms.EDAnalyzer("DQMFileLoader",
    toMerge = cms.PSet(
        inputFileNames = cms.vstring(map(fix_file_name, inputFileNames)),
        dqmDirectory_store = cms.string('/')
    )
)

process.save = cms.EDAnalyzer("DQMSimpleFileSaver",
    outputFileName = cms.string(outputFileName),
    outputCommands = cms.vstring(
        'keep *',
        # Dont' keep events lists
        # no, drop them
        "drop */FilterStatistics/evtSelCentralJetEt20bTag/events_passed_cumulative",
        "drop */FilterStatistics/evtSelNonCentralJetEt20bTag/events_passed_cumulative",
        "drop */FilterStatistics/evtSelDiTauCandidateForAHtoMuTauZeroCharge/events_passed_cumulative",
        "drop */FilterStatistics/evtSelDiTauCandidateForAHtoMuTauNonZeroCharge/events_passed_cumulative",
    )
)

process.harvest = cms.Sequence(
    process.load
   + process.save
)

process.p = cms.Path(process.harvest)
