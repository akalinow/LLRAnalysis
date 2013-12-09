#!/usr/bin/env python

from LLRAnalysis.HadTauStudies.recoSampleDefinitionsAHtoTauTau_8TeV_grid_cfi import recoSampleDefinitionsAHtoTauTau_8TeV
from LLRAnalysis.HadTauStudies.tools.submitAnalysisToGrid import submitAnalysisToGrid
import LLRAnalysis.HadTauStudies.tools.eos as eos

import getpass
import os
import subprocess
import time

configFile = 'patTuple_PAT_SkimHadTauStream_cfg.py'
jobId = '2013Dec07'

outputFilePath = 'CMSSW_5_3_x/PATTuples/AHtoTauTau/%s/' % jobId
##outputFilePath = ''

samplesToAnalyze = [
    "HiggsSUSYGluGlu130",
    "HiggsSUSYBB130"
]

if len(samplesToAnalyze) == 0:
    samplesToAnalyze = recoSampleDefinitionsAHtoTauTau_8TeV['SAMPLES_TO_ANALYZE']

def getStringRep(value):
    retVal = None
    if type(value) is str:
        retVal = "'%s'" % value
    elif type(value) is float:
        retVal = "%f" % value
    elif type(value) is int:
        retVal = "%i" % value
    elif type(value) is bool:
        if value:
            retVal = "True"
        else:
            retVal = "False"
    else:
        raise ValueError("<getStringRep>: Function argument 'value' is of unsupported type !!")
    return retVal

# Function to prepare customized config files specific to TauIdEff. skim 
def customizeConfigFile(sampleName, cfgFileName_original, cfgFileName_modified = None):
    cfgFile_original = open(cfgFileName_original, "r")
    cfg_original = cfgFile_original.read()
    cfgFile_original.close()

    cfg_modified = cfg_original.replace("#__", "")
    for parameterName, parameterValue in recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sampleName]['parameters_to_overwrite'].items():
        cfg_modified = cfg_modified.replace("$%s" % parameterName, getStringRep(parameterValue))
    
    if cfgFileName_modified is None:
        cfgFileName_modified = "crab/%s" % cfgFileName_original
        cfgFileName_modified = cfgFileName_modified.replace("_cfg.py", "_customized_%s_cfg.py" % sampleName)
    cfgFile_modified = open(cfgFileName_modified, "w")
    cfgFile_modified.write(cfg_modified)
    cfgFile_modified.close()

    return cfgFileName_modified

# Functions to create output directory on eos
def createFilePath(filePath):
    try:
        eos.lsl(filePath)
    except IOError:
        print "filePath = %s does not yet exist, creating it." % filePath
        eos.mkdir(filePath)
        time.sleep(3)
        eos.chmod(filePath, 777)

def createFilePath_recursively(filePath):
    filePath_items = filePath.split('/')
    currentFilePath = "/"
    for filePath_item in filePath_items:
        currentFilePath = os.path.join(currentFilePath, filePath_item)
        if len(currentFilePath) <= 1:
            continue
        createFilePath(currentFilePath)

for sampleToAnalyze in samplesToAnalyze:

    # prepare customized config file as basis for further modifications by "TauAnalysis machinery"...
    configFile_customized = customizeConfigFile(sampleToAnalyze, configFile)

    # create output directory
    outputFilePath_sample = os.path.join(outputFilePath, sampleToAnalyze)
    eos_base = os.path.join("/store/user/", getpass.getuser())
    createFilePath_recursively(os.path.join(eos_base, outputFilePath_sample))

    # apply further modifications and submit job to grid
    submitAnalysisToGrid(configFile = configFile_customized,
                         channel = "AHtoTauTau", sample = sampleToAnalyze, jobId = jobId,
                         sample_info = recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sampleToAnalyze],
                         outputFilePath = outputFilePath_sample, outputFiles = [ "patTuple_HadTauStream.root", "skimHadTauStream.root" ])




