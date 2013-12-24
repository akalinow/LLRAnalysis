#!/usr/bin/env python

import os
import shlex
import subprocess
import sys
import time

import LLRAnalysis.HadTauStudies.tools.eos as eos

#--------------------------------------------------------------------------------
# Copy directory and all subdirectories within it
# from EOS to local disk and vice versa.
#
# The output directory is created in case it does not yet exist.
#
# Author: Christian Veelken 
# 
# Examples:
# 
# ./copyFilesRecursively.py /store/user/veelken/CMSSW_5_3_x/NTuples/AHtoTauTau/2013Dec07/ /data1/veelken/CMSSW_5_3_x/NTuples/
#
#--------------------------------------------------------------------------------

if not (len(sys.argv) == 3):
    raise ValueError("Usage: copyFilesRecursively.py sourceFilePath targetFilePath")

sourceFilePath = sys.argv[1]
targetFilePath = sys.argv[2]

maxNumConcurrentJobs = 10

def runCommand(commandLine):
    print(commandLine)
    args = shlex.split(commandLine)
    retVal = subprocess.Popen(args, stdout = subprocess.PIPE)
    return retVal.stdout.readlines()

whoami = runCommand('whoami')
if len(whoami) != 1:
    raise ValueError("Failed to identify userName !!")
userName = whoami[0].strip()
print("userName = %s" % userName)

# Functions to create output directory on eos
def createFilePath(filePath):
    if filePath.find("/store") != -1:
        try:
            eos.lsl(filePath)
        except IOError:
            print "filePath = %s does not yet exist, creating it." % filePath
            eos.mkdir(filePath)
            time.sleep(3)
            eos.chmod(filePath, 777)
    else:
        if not os.path.isdir(filePath):
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

def copyFile(sourceFileName, targetFilePath):

    ##print "<copyFile>: sourceFileName = '%s', targetFilePath = '%s'" % (sourceFileName, targetFilePath)

    if not targetFilePath[len(targetFilePath) - 1] == "/":
        targetFilePath = targetFilePath + "/"

    # CV: check that maximum number of jets does not get exceeded
    waitFor = 10 # time to wait in seconds
    waitForJobToFinish = True
    while waitForJobToFinish:
        waitForJobToFinish = False
        args = shlex.split('ps aux')
        running_jobs = subprocess.Popen(args, stdout = subprocess.PIPE)
        running_jobs.wait()
        running_jobs = running_jobs.stdout.readlines()
        numConcurrentJobs = {}
        jobsToWaitFor = [
            "python",
            "eos",
            "xrdcp"
        ]
        for running_job in running_jobs:
            columns = running_job.split()
            if len(columns) < 11:
                continue
            running_job_user = columns[0]
            running_job_executable = columns[10]
            if running_job_user == userName:
                for jobToWaitFor in jobsToWaitFor:
                    if running_job_executable == jobToWaitFor:
                        if jobToWaitFor in numConcurrentJobs.keys():
                            numConcurrentJobs[jobToWaitFor] = numConcurrentJobs[running_job_executable] + 1
                        else:
                            numConcurrentJobs[jobToWaitFor] = 1
        for jobToWaitFor in jobsToWaitFor:
            if jobToWaitFor in numConcurrentJobs.keys() and numConcurrentJobs[jobToWaitFor] >= maxNumConcurrentJobs:
                waitForJobToFinish = True
        if waitForJobToFinish:
            print "Maximum number of %i copy jobs reached --> waiting for %i seconds." % (maxNumConcurrentJobs, waitFor)
            time.sleep(waitFor)        
        
    if sourceFileName.find("/store") != -1 and targetFilePath.find("/store") == -1: # copy from eos to local disk
        eos.cp([ sourceFileName ], targetFilePath)
    elif sourceFileName.find("/store") == -1 and targetFilePath.find("/store") != -1: # copy from local disk to eos
        eos.cp([ sourceFileName ], targetFilePath)
    elif sourceFileName.find("/store") == -1 and targetFilePath.find("/store") == -1: # copy from local disk to local disk
        raise ValueError("Use 'cp -r' to copy files from local disk to local disk !!")
    else:
        raise ValueError("Copying from EOS to EOS not implemented yet !!")

def processFilePath(sourceFilePath, targetFilePath):
    
    ##print "<processFilePath>: sourceFilePath = '%s', targetFilePath = '%s'" % (sourceFilePath, targetFilePath)

    if not sourceFilePath[len(sourceFilePath) - 1] == "/":
        sourceFilePath = sourceFilePath + "/"
    if not targetFilePath[len(targetFilePath) - 1] == "/":
        targetFilePath = targetFilePath + "/"

    # create targetFilePath in case it does not yet exist
    createFilePath_recursively(targetFilePath)
    
    source_file_infos = eos.lsl(sourceFilePath)
    for source_file_info in source_file_infos:
        print source_file_info
        file_size = source_file_info['size']
        if file_size <= 1000: # directory
            source_file_infos_bak = source_file_infos
            processFilePath(source_file_info['path'], os.path.join(targetFilePath, source_file_info['file']))
            source_file_infos = source_file_infos_bak
        else: # actual file
            print "sourceFilePath = '%s'" % sourceFilePath
            copyFile(source_file_info['path'], targetFilePath)

processFilePath(sourceFilePath, targetFilePath)
