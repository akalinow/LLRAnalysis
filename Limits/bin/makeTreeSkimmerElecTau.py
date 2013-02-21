#!/usr/bin/env python

import os
import re
import shlex
import string
import subprocess

Workdir  = '/data_CMS/cms/ivo/HTauTauAnalysis/Trees/ElecTauStream_HCP2012/'

options = {
    # e+tau samples
    ##DATA
    'Run2012B-13Jul2012-Data' : {
    'inputFilePath'  : Workdir+'Run2012B-13Jul2012-Data/',
    'outputFileName' : Workdir+'testIvo/ntuples/nTupleRun2012B-13Jul2012-Data_ElecTau.root',
    'sample'         : 'Run2012B-13Jul2012-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012C-24Aug2012-Data' : {
    'inputFilePath'  : Workdir+'Run2012C-24Aug2012-Data/',
    'outputFileName' : Workdir+'testIvo/ntuples/nTupleRun2012C-24Aug2012-Data_ElecTau.root',
    'sample'         : 'Run2012C-24Aug2012-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 2
    },
    'Run2012C-PromptReco-v2-Data' : {
    'inputFilePath'  : Workdir+'Run2012C-PromptReco-v2-Data/',
    'outputFileName' : Workdir+'testIvo/ntuples/nTupleRun2012C-PromptReco-v2-Data_ElecTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 3
    },
    'Run2012C-PromptReco-v2-Residual-Data' : {
    'inputFilePath'  : Workdir+'Run2012C-PromptReco-v2-Residual-Data/',
    'outputFileName' : Workdir+'testIvo/ntuples/nTupleRun2012C-PromptReco-v2-Residual-Data_ElecTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-Residual-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 3
    },
    ##Bkg MC
    'WJets2Jets' : {
    'inputFilePath'  : Workdir+'testIvo/WJets2Jets/',
    'outputFileName' : Workdir+'testIvo/ntuples/nTupleWJets2Jets_ElecTau.root',
    'sample'         : 'WJets2Jets',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.00634753521127,
    'iJson'          : -1

    } 
}

config_template = string.Template('''
import FWCore.ParameterSet.Config as cms

import os

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    maxEvents = cms.int32(-1),
    #outputEvery = cms.uint32(1000)
)

inputFilePath = '$inputFilePath'
inputFiles = os.listdir(inputFilePath)
process.fwliteInput.fileNames = cms.vstring([ os.path.join(inputFilePath, inputFile) for inputFile in inputFiles ])
                            
process.fwliteOutput = cms.PSet(
    fileName = cms.string('$outputFileName')
)

process.treeSkimmerElecTau_HCP = cms.PSet(
    sample = cms.string('$sample'),
    analysis = cms.string('$analysis'),
    xSection = cms.double($xSection),
    skimEff = cms.double($skimEff),
    iJson = cms.int32($iJson)
)
''')

currentDirectory    = os.getcwd()
submissionDirectory = os.path.join(currentDirectory, "Configs")

for sample, option in options.items():
    for analysis in [ 'nominal', 'TauUp', 'TauDown' ]:
        if  re.search("Data",sample)!=None and analysis != 'nominal':
            continue
        configOptions = option.copy()
        configOptions['analysis'] = analysis
        if  re.search("Data",sample)==None :
            configOptions['outputFileName'] = configOptions['outputFileName'].replace('.root', '_%s.root' % analysis) 
        configFileName = "treeSkimmerElecTau_%s_%s_cfg.py" % (sample,analysis)
        configFileName_full = os.path.join(submissionDirectory, configFileName)
        configFile = open(configFileName_full, 'w')
        configConfig = config_template.substitute(configOptions)
        configFile.write(configConfig)
        configFile.close()
