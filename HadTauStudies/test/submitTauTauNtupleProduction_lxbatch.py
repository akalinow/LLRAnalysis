#!/usr/bin/env python

from LLRAnalysis.HadTauStudies.recoSampleDefinitionsAHtoTauTau_8TeV_grid_cfi import recoSampleDefinitionsAHtoTauTau_8TeV
from LLRAnalysis.HadTauStudies.tools.submitAnalysisToGrid import submitAnalysisToGrid
import LLRAnalysis.HadTauStudies.tools.eos as eos
from LLRAnalysis.HadTauStudies.tools.jobtools import make_bsub_script
from LLRAnalysis.HadTauStudies.tools.harvestingLXBatch import make_harvest_scripts

import json
import os
import random
import re
import shlex
import subprocess
import sys
import time

configFile = 'runTauTauNtupleProducer_PostMoriond2013_NewTauES_ByPair_cfg.py'
jobId = '2014Jun09'

version = "v2_02"

inputFilePath = "/store/group/phys_higgs/cmshtt/CMSSW_5_3_x/PATTuples/AHtoTauTau/%s/" % jobId

maxEventsPerJob = 10000

##lxbatch_queue = '1nw'
lxbatch_queue = '1nd'
##lxbatch_queue = '8nh'

samplesToAnalyze = [
    # CV: leave empty in order to produce Ntuples for all samples
]

numEventsFileName = 'submitTauTauNtupleProduction.json'

numEvents_dict = {}
if os.path.exists(numEventsFileName):
    if os.path.isfile(numEventsFileName):
        print("Loading #Events from File %s" % numEventsFileName)
        numEventsFile = open(numEventsFileName, "r")
        numEvents_dict = json.load(numEventsFile)
        #print(numEvents_dict)
        numEventsFile.close()
    else:
        raise ValueError("File %s exists, but is a directory !!" % numEventsFileName)
else:
    print("File %s does not yet exist...creating it." % numEventsFileName)

if len(samplesToAnalyze) == 0:
    samplesToAnalyze = recoSampleDefinitionsAHtoTauTau_8TeV['SAMPLES_TO_ANALYZE']
    
skipExistingPATtuples = True

outputFileMachine_and_Path = "/store/user/veelken/CMSSW_5_3_x/Ntuples/AHtoTauTau/%s/%s" % (jobId, version)
##outputFileMachine_and_Path = "/store/group/phys_higgs/cmshtt/CMSSW_5_3_x/Ntuples/AHtoTauTau/%s/%s" % (jobId, version)
##outputFileMachine_and_Path = "/store/group/phys_higgs/cmshtt/CMSSW_5_3_x/Ntuples/AHtoTauTau/2013Dec19f/%s" % (jobId, version)
#outputFileMachine_and_Path = "ucdavis:/data1/veelken/CMSSW_5_3_x/Ntuples/AHtoTauTau/%s/%s" % (jobId, version)

outputFileMachine = None
outputFilePath = None
if outputFileMachine_and_Path.find(":") != -1:
    outputFileMachine = outputFileMachine_and_Path[:outputFileMachine_and_Path.find(":")]
    outputFilePath = outputFileMachine_and_Path[outputFileMachine_and_Path.find(":") + 1:]
else:
    outputFilePath = outputFileMachine_and_Path

executable_bsub = 'bsub'
executable_waitForLXBatchJobs = 'python %s/src/TauAnalysis/Configuration/python/tools/waitForLXBatchJobs.py' % os.environ['CMSSW_BASE']
executable_rfrm = 'eos rm -f' # CV: ignore error code returned by eos in case file does not exist
executable_hadd = 'hadd -f'
executable_shell = '/bin/csh'

# Functions to create output directory on eos
def createFilePath(filePath):
    if not filePath.endswith('/'):
        filePath += '/'
    if filePath.startswith('/store/'):
        try:
            eos.lsl(filePath)
        except IOError:
            print "filePath = %s does not yet exist, creating it." % filePath
            eos.mkdir(filePath)
            time.sleep(3)
            eos.chmod(filePath, 777)
    else:
        if not os.path.exists(filePath):
            print "filePath = %s does not yet exist, creating it." % filePath
            os.mkdir(filePath)

def createFilePath_recursively(filePath):
    filePath_items = filePath.split('/')
    currentFilePath = "/"
    for filePath_item in filePath_items:
        currentFilePath = os.path.join(currentFilePath, filePath_item)
        if len(currentFilePath) <= 1:
            continue
        createFilePath(currentFilePath)

configFilePath = os.path.join(os.getcwd(), "lxbatch", version)
createFilePath_recursively(configFilePath)

logFilePath = os.path.join(os.getcwd(), "lxbatch_log", version)
createFilePath_recursively(logFilePath)

def runCommand(commandLine):
    sys.stdout.write("%s\n" % commandLine)
    args = shlex.split(commandLine)
    retVal = subprocess.Popen(args, stdout = subprocess.PIPE)
    retVal.wait()
    return retVal.stdout

def getNumEventsInFile(inputFilePath, inputFileName):
    numEventsInFile = None
    key = os.path.join(inputFilePath, inputFileName)
    if numEvents_dict.has_key(key):
        numEventsInFile = numEvents_dict[key]
    else:
        edmFileUtil_output = runCommand('edmFileUtil %s' % os.path.join(inputFilePath, inputFileName))
        edmFileUtil_regex = r"[a-zA-Z0-9_/:.]*\s*\([0-9]+\s*runs,\s*[0-9]+\s*lumis,\s*(?P<events>[0-9]+)\s*events,\s*[0-9]+\s*bytes\)\s*"
        edmFileUtil_matcher = re.compile(edmFileUtil_regex)
        for line in edmFileUtil_output:
            edmFileUtil_match = edmFileUtil_matcher.match(line)
            if edmFileUtil_match:
                numEventsInFile = int(edmFileUtil_match.group('events'))
        if numEventsInFile is None:
            raise ValueError("Failed to read number of events contained in file = %s !!" % inputFileName)
        print " numEventsInFile = %i" % numEventsInFile
        numEvents_dict[key] = numEventsInFile       
    return numEventsInFile

def divideInputFiles_in_chunks(inputFilePath, inputFileNames):

    inputFileNames_in_chunks = []

    inputFileNames_in_currentChunk = []
    numEvents_in_currentChunk = 0

    for inputFileName in inputFileNames:
        numEventsInFile = getNumEventsInFile(inputFilePath, inputFileName)
        if (numEvents_in_currentChunk + numEventsInFile) > maxEventsPerJob:
            inputFileNames_in_chunks.append(inputFileNames_in_currentChunk)
            inputFileNames_in_currentChunk = [ inputFileName ]
            numEvents_in_currentChunk = numEventsInFile
        else:
            inputFileNames_in_currentChunk.append(inputFileName)
            numEvents_in_currentChunk = numEvents_in_currentChunk + numEventsInFile

    if len(inputFileNames_in_currentChunk) > 0:
        inputFileNames_in_chunks.append(inputFileNames_in_currentChunk)

    return inputFileNames_in_chunks

# Function that maps a sample name to its skim file
def input_mapper(inputFileNames, sampleToAnalyze):
    inputFile_regex = r"[a-zA-Z0-9_/:.]*patTuple_HadTauStream_(?P<gridJob>\d*)(_(?P<gridTry>\d*))*_(?P<hash>[a-zA-Z0-9]*).root"
    inputFile_matcher = re.compile(inputFile_regex)
    for input_file in inputFileNames:
        if inputFile_matcher.match(input_file):
            if input_file.find("/store") != -1:
                yield input_file
            else:
                yield "".join(["file:", input_file])

# Define what output ntuple file name a sample will have
def output_mapper(sample, chunkId):
    output_file = 'H2TauTauTreeProducerTauTau_%i.root' % chunkId
    return output_file

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
        raise ValueError("<getStringRep>: Function argument 'value' is of unsupported type = %s !!" % type(value))	
    return retVal

# Function to prepare customized config files specific to Tau(ED)Ntuple production
def customizeConfigFile(sampleName, nom_or_sysShift, inputFileNames, outputFileName, cfgFileName_original, cfgFileName_modified):
    cfgFile_original = open(cfgFileName_original, "r")
    cfg_original = cfgFile_original.read()
    cfgFile_original.close()

    cfg_modified = cfg_original.replace("#__", "")
    for parameterName, parameterValue in recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sampleName]['parameters_to_overwrite'].items():
        cfg_modified = cfg_modified.replace("$%s" % parameterName, getStringRep(parameterValue))

    higgsSample_regex = r"(HiggsGGH|HiggsVBF|HiggsSUSYGluGlu|HiggsSUSYBB)(?P<mass>[0-9]+)"
    higgsSample_matcher = re.compile(higgsSample_regex)
    higgsSample_match = higgsSample_matcher.match(sampleName)
    if higgsSample_match:
        cfg_modified = cfg_modified.replace("$applyHiggsMassCut", getStringRep(True))
        nomHiggsMass = int(higgsSample_match.group('mass'))
        cfg_modified = cfg_modified.replace("$nomHiggsMass", getStringRep(nomHiggsMass))
    else:
        cfg_modified = cfg_modified.replace("$applyHiggsMassCut", getStringRep(False))
        cfg_modified = cfg_modified.replace("$nomHiggsMass", getStringRep(0.))
    runNominal   = False
    runTauEnUp   = False
    runTauEnDown = False
    if nom_or_sysShift == "nom":
        runNominal = True
    elif nom_or_sysShift == "up":
        runTauEnUp = True
    elif nom_or_sysShift == "down":
        runTauEnDown = True
    else:
        raise ValueError("Invalid Parameter 'nom_or_sysShift' = %s !!" % nom_or_sysShift)
    cfg_modified = cfg_modified.replace("$runNominal", getStringRep(runNominal))
    cfg_modified = cfg_modified.replace("$runTauEnUp", getStringRep(runTauEnUp))
    cfg_modified = cfg_modified.replace("$runTauEnDown", getStringRep(runTauEnDown))

    cfg_modified += "\n"
    cfg_modified += "process.source.fileNames = cms.untracked.vstring(%s)\n" % \
                      [ "".join([ "file:", inputFileName ]) for inputFileName in inputFileNames ]
    cfg_modified += "process.TFileService.fileName = cms.string('%s')" % outputFileName

    cfgFile_modified = open(cfgFileName_modified, "w")
    cfgFile_modified.write(cfg_modified)
    cfgFile_modified.close()

    return cfgFileName_modified

#--------------------------------------------------------------------------------
# Build config files for producing PAT-tuples on lxbatch
bsubFileNames       = {}
bsubScriptFileNames = {}
bsubJobNames        = {}
for sampleToAnalyze in samplesToAnalyze:

    print "checking sample %s" % sampleToAnalyze

    bsubFileNames[sampleToAnalyze]       = {}
    bsubScriptFileNames[sampleToAnalyze] = {}
    bsubJobNames[sampleToAnalyze]        = {}

    if sampleToAnalyze == "HiggsSUSYBB300": # CV: skip obsolete sample (superseded by HiggsSUSYBB300v2)
        continue    
    if sampleToAnalyze == "data_TauParked_Run2012C_22Jan2013_v1": # CV: skip, because PAT-tuple production jobs have not yet finished
        continue
    
    inputFilePath_sample = os.path.join(inputFilePath, sampleToAnalyze)
    print " inputFilePath = %s" % inputFilePath_sample
        
    inputFileNames = None
    if inputFilePath.find("/store") != -1:
        inputFileNames = [ file_info['path'] for file_info in eos.lsl(inputFilePath_sample) ]
    else:
        inputFileNames = [ file for file in os.listdir(inputFilePath_sample) ]
    #print " inputFileNames = %s" % inputFileNames
    
    inputFileNames_matched = [ os.path.basename(input_file) for input_file in input_mapper(inputFileNames, sampleToAnalyze) ]
    print "inputFileNames_matched = %s" % inputFileNames_matched
    print "--> found %i inputFiles" % len(inputFileNames_matched)

    inputFileNames_matched_in_chunks = divideInputFiles_in_chunks(inputFilePath_sample, inputFileNames_matched)
    print "inputFileNames_matched_in_chunks:"
    print inputFileNames_matched_in_chunks

    for nom_or_sysShift in [ "nom", "up", "down" ]:

        if recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sampleToAnalyze]['type'] == "Data" and nom_or_sysShift != "nom":
            continue

        bsubFileNames[sampleToAnalyze][nom_or_sysShift]       = {}
        bsubScriptFileNames[sampleToAnalyze][nom_or_sysShift] = {}
        bsubJobNames[sampleToAnalyze][nom_or_sysShift]        = {}

        outputFileMachine_and_Path_sample = os.path.join(outputFileMachine_and_Path, nom_or_sysShift, sampleToAnalyze)
        print " outputFileMachine_and_Path = %s" % outputFileMachine_and_Path_sample
        createFilePath_recursively(os.path.join(outputFilePath, nom_or_sysShift, sampleToAnalyze))
        
        for chunkId, inputFileNames_in_currentChunk in enumerate(inputFileNames_matched_in_chunks):

            print "processing chunk #%i: inputFiles = %s" % (chunkId, inputFileNames_in_currentChunk)
            
            # Build script for batch job submission;
            # the None in the tuple indicates that batch job has no dependencies on other batch jobs
            input_files_and_jobs = \
              [ (None, os.path.join(inputFilePath_sample, inputFileName)) for inputFileName in inputFileNames_in_currentChunk ]

            outputFileName = output_mapper(sampleToAnalyze, chunkId)

            configFileName_modified = os.path.join(configFilePath, configFile.replace("_cfg.py", "_%s_%s_%i_cfg.py" % (sampleToAnalyze, nom_or_sysShift, chunkId)))
            #print " configFileName = %s" % configFileName_modified
            customizeConfigFile(sampleToAnalyze, nom_or_sysShift, inputFileNames_in_currentChunk, outputFileName, configFile, configFileName_modified)

            logFileName = os.path.basename(configFileName_modified.replace('_cfg.py', '.log'))
            def log_file_maker(job_hash):
                return os.path.join(logFilePath, logFileName)

            jobName, bsubScript = make_bsub_script(
                os.path.join(outputFileMachine_and_Path_sample, outputFileName),
                input_files_and_jobs,
                log_file_maker,
                "cmsRun %s" % os.path.join(configFilePath, configFileName_modified))
            bsubFileNames[sampleToAnalyze][nom_or_sysShift][chunkId] = [ outputFileName ]
                    
            bsubScriptFileName = os.path.join(configFilePath, logFileName.replace(".log", ".sh"))
            bsubScriptFile = open(bsubScriptFileName, "w")
            bsubScriptFile.write(bsubScript)
            bsubScriptFile.close()

            bsubScriptFileNames[sampleToAnalyze][nom_or_sysShift][chunkId] = bsubScriptFileName
 
            bsubJobName = "TauTauNtuple_%s_%s_%i" % (sampleToAnalyze, nom_or_sysShift, chunkId)
            bsubJobNames[sampleToAnalyze][nom_or_sysShift][chunkId] = bsubJobName

    numEventsFile = open(numEventsFileName, "w")
    json.dump(numEvents_dict, numEventsFile)
    numEventsFile.close()
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to collect all histograms
# for same Data/MC sample into single .root file
#
bsubFileNames_harvesting = {}
bsubJobNames_harvesting  = {}
for sampleToAnalyze in samplesToAnalyze:
    
    bsubFileNames_harvesting[sampleToAnalyze] = {}
    bsubJobNames_harvesting[sampleToAnalyze]  = {}

    for nom_or_sysShift in [ "nom", "up", "down" ]:

        if not nom_or_sysShift in bsubScriptFileNames[sampleToAnalyze].keys():
            continue

        outputFileMachine_and_Path_sample = os.path.join(outputFileMachine_and_Path, nom_or_sysShift, sampleToAnalyze)

        plot_regex = r"H2TauTauTreeProducerTauTau_[0-9]+.root"
        skim_regex = r"dont match anything"
        
        def local_copy_mapper(sample):
            return os.path.join(
                outputFileMachine_and_Path,
                'H2TauTauTreeProducerTauTau_all.root')
    
        inputFileInfos = []
        for chunkId in bsubScriptFileNames[sampleToAnalyze][nom_or_sysShift].keys():
            for inputFileName in bsubFileNames[sampleToAnalyze][nom_or_sysShift][chunkId]:
                inputFileInfo = {
                    'path'        : os.path.join(outputFileMachine_and_Path, inputFileName),
                    'size'        : 1,           # dummy
                    'time'        : time.localtime(),
                    'file'        : inputFileName,
                    'permissions' : 'mrw-r--r--' # "ordinary" file access permissions
                }
                #print "inputFileInfo = %s" % inputFileInfo
                inputFileInfos.append(inputFileInfo)

        retVal_make_harvest_scripts = make_harvest_scripts(
            plot_regex,
            skim_regex,
            channel = "TauTauNtupleHarvesting",
            sampleToAnalyze = sampleToAnalyze,
            job_id = version,
            input_files_info = inputFileInfos,
            harvester_command = executable_hadd,
            abort_on_rfcp_error = False,
            castor_output_directory = outputFileMachine_and_Path_sample,
            script_directory = configFilePath,
            merge_script_name = \
            os.path.join(configFilePath, "_".join(['submit', sampleToAnalyze, nom_or_sysShift, 'merge']) + '.sh'),
            local_copy_mapper = local_copy_mapper,
            chunk_size = 2.e+9, # 2 GB
            run_merging = False,
            check_old_files = False,
            max_bsub_concurrent_file_access = 250,
            verbosity = 0
        )

        bsubFileNames_harvesting[sampleToAnalyze][nom_or_sysShift] = retVal_make_harvest_scripts

        bsubJobName = "TauTauNtupleHarvesting%s%s" % (sampleToAnalyze, nom_or_sysShift)
        bsubJobNames_harvesting[sampleToAnalyze][nom_or_sysShift] = bsubJobName
#--------------------------------------------------------------------------------

def make_MakeFile_vstring(list_of_strings):
    retVal = ""
    for i, string_i in enumerate(list_of_strings):
        if i > 0:
            retVal += " "
        retVal += string_i
    return retVal

# done building config files, now build Makefile...
makeFileName = "Makefile_TauTauNtupleProduction"
makeFile = open(makeFileName, "w")
makeFile.write("\n")
jobsTauTauNtupleProduction = []
existingOutputFiles = None
for sampleToAnalyze in samplesToAnalyze:
    for nom_or_sysShift in bsubScriptFileNames[sampleToAnalyze].keys():
        outputFilePath_sample = os.path.join(outputFilePath, nom_or_sysShift, sampleToAnalyze)
        if outputFilePath_sample.find("/store") != -1:
            existingOutputFiles = [ file_info['file'] for file_info in eos.lsl(outputFilePath_sample) ]
        else:
            existingOutputFiles = [ file for file in os.listdir(outputFilePath_sample) ]
        for chunkId in bsubScriptFileNames[sampleToAnalyze][nom_or_sysShift].keys():
            outputFilesExist = True
            for outputFileName in bsubFileNames[sampleToAnalyze][nom_or_sysShift][chunkId]:
                if not outputFileName in existingOutputFiles:
                    outputFilesExist = False
            if skipExistingPATtuples and outputFilesExist:
                print "Output files for sample = %s, nom_or_sysShift = %s, chunkId = %i exist --> skipping !!" % (sampleToAnalyze, nom_or_sysShift, chunkId)
            else:
                jobsTauTauNtupleProduction.append(bsubJobNames[sampleToAnalyze][nom_or_sysShift][chunkId])
#--------------------------------------------------------------------------------    
# CV: randomize jobs in order for ucdavis not to get overloaded with file transfers
#     initiated by too many jobs finishing all at the same time
random.shuffle(jobsTauTauNtupleProduction)
jobDependencyDict = {}
for jobIdx in range(len(jobsTauTauNtupleProduction)):
    if jobIdx >= 1:
        jobDependencyDict[jobsTauTauNtupleProduction[jobIdx]] = jobsTauTauNtupleProduction[jobIdx - 1]
    else:
        jobDependencyDict[jobsTauTauNtupleProduction[jobIdx]] = ""
#--------------------------------------------------------------------------------
makeFile.write("all: %s\n" % make_MakeFile_vstring(jobsTauTauNtupleProduction))
makeFile.write("\techo 'Finished running TauTauNtupleProduction.'\n")
makeFile.write("\n")
for sampleToAnalyze in samplesToAnalyze:
    for nom_or_sysShift in bsubScriptFileNames[sampleToAnalyze].keys():
        for chunkId in bsubScriptFileNames[sampleToAnalyze][nom_or_sysShift].keys():
            if bsubJobNames[sampleToAnalyze][nom_or_sysShift][chunkId] in jobsTauTauNtupleProduction:
                makeFile.write("%s: %s\n" %
                  (bsubJobNames[sampleToAnalyze][nom_or_sysShift][chunkId],
                   jobDependencyDict[bsubJobNames[sampleToAnalyze][nom_or_sysShift][chunkId]]))
                makeFile.write("\t%s -q %s -J %s < %s\n" %
                  (executable_bsub,
                   lxbatch_queue,
                   bsubJobNames[sampleToAnalyze][nom_or_sysShift][chunkId],
                   bsubScriptFileNames[sampleToAnalyze][nom_or_sysShift][chunkId]))
                makeFile.write("\tsleep 1\n")
        makeFile.write("\n")
jobsTauTauNtupleHarvesting = []
for sampleToAnalyze in samplesToAnalyze:
    for nom_or_sysShift in bsubFileNames_harvesting[sampleToAnalyze].keys():
        jobsTauTauNtupleHarvesting.append(bsubJobNames_harvesting[sampleToAnalyze][nom_or_sysShift])        
makeFile.write("harvesting: %s\n" % make_MakeFile_vstring(jobsTauTauNtupleHarvesting))
for sampleToAnalyze in samplesToAnalyze:
    for nom_or_sysShift in bsubFileNames_harvesting[sampleToAnalyze].keys():
        makeFile.write("%s:\n" %
          (bsubJobNames_harvesting[sampleToAnalyze][nom_or_sysShift]))
        makeFile.write("\t%s %s\n" %
          (executable_shell,
           bsubFileNames_harvesting[sampleToAnalyze][nom_or_sysShift]['harvest_script_name']))
        makeFile.write("\n")        
makeFile.write(".PHONY: clean\n")
makeFile.write("clean:\n")
for sampleToAnalyze in samplesToAnalyze:
    for nom_or_sysShift in bsubScriptFileNames[sampleToAnalyze].keys():
        for chunkId in bsubScriptFileNames[sampleToAnalyze][nom_or_sysShift].keys():
            for outputFileName in bsubFileNames[sampleToAnalyze][nom_or_sysShift][chunkId]:
                makeFile.write("\t%s %s\n" %
                  (executable_rfrm,                   
                   os.path.join(outputFilePath, outputFileName)))
for sampleToAnalyze in samplesToAnalyze:
    for nom_or_sysShift in bsubFileNames_harvesting[sampleToAnalyze].keys():
        makeFile.write("\t%s %s\n" %
          (executable_rfrm,
           bsubFileNames_harvesting[sampleToAnalyze][nom_or_sysShift]))                
makeFile.write("\techo 'Finished deleting old files.'\n")
makeFile.write("\n")
makeFile.close()

numEventsFile = open(numEventsFileName, "w")
json.dump(numEvents_dict, numEventsFile)
numEventsFile.close()

print("Finished building Makefile. Now execute 'make -f %s'." % makeFileName)
