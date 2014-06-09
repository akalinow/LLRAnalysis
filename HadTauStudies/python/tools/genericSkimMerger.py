#!/usr/bin/env cmsRun

'''

Merge EDM files together.  Based on copyPickMerge_cfg by Charles Plager

'''

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

process = cms.Process('MERGE')

process.load('FWCore/MessageService/MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

def fix_file_name(file):
    " Add an file: or rfio: if needed "
    if file.startswith('rfio:') or file.startswith('file:') \
       or file.startswith('/store/'):
        # Already formatted
        return file
    if file.startswith('/castor/cern.ch'):
        return 'rfio:' + file
    return 'file:' + file

process.source = cms.Source ("PoolSource",
    fileNames = cms.untracked.vstring (map(fix_file_name, inputFileNames)),
    noEventSort = cms.untracked.bool(True),
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    skipBadFiles = cms.untracked.bool(False)
)

process.Out = cms.OutputModule("PoolOutputModule",
    cms.PSet(
        outputCommands = cms.untracked.vstring(
            'keep *',
            'drop GenFilterInfo_*_*_*' # CV: drop GenFilterInfo objects,
                                       #     because they cause segmentation violation of merge jobs run in CMSSW_4_2_x (2011/07/10)
        )
    ),
    fileName = cms.untracked.string (outputFileName)                           
)

process.end = cms.EndPath(process.Out)
