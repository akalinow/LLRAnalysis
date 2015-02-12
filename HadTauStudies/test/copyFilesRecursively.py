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

tmpFilePath = "/tmp/veelken"

##maxNumConcurrentJobs = 100
maxNumConcurrentJobs = 50

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

def readFileInfos(filePath):
    ##print "<readFiles>:"
    fileInfos = None
    if filePath.find("/store") != -1:
        fileInfos = eos.lsl(filePath)
    else:
        fileNames = [ os.path.join(filePath, fileName) for fileName in os.listdir(filePath) ]        
        fileInfos = []
        for fileName in fileNames:
            ##print "fileName = %s" % fileName
            fileStat = os.lstat(fileName)
            permissions = ""
            bit = 9
            while bit >= 0:
                if fileStat.st_mode & (1 << bit):
                    if bit in [ 9 ]:
                        permissions += "d"
                    elif bit in [ 8, 5, 2 ]:
                        permissions += "r"
                    elif bit in [ 7, 4, 1 ]:
                        permissions += "w"
                    elif bit in [ 6, 3, 0 ]:
                        permissions += "x"    
                else:
                    permissions += "-"
                bit = bit - 1
            fileInfo = {
                'file'        : os.path.basename(fileName),
                'path'        : fileName,
                'size'        : fileStat.st_size,
                'time'        : time.localtime(fileStat.st_mtime),
                'permissions' : permissions
            }
            fileInfos.append(fileInfo)
    return fileInfos

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
        print 'eos.cp([ %s ], %s)' % (sourceFileName, targetFilePath)
        eos.cp([ sourceFileName ], targetFilePath)
    elif sourceFileName.find("/store") == -1 and targetFilePath.find("/store") != -1: # copy from local disk to eos
        eos.cp([ sourceFileName ], targetFilePath)
    elif sourceFileName.find("/store") == -1 and targetFilePath.find("/store") == -1: # copy from local disk to local disk
        raise ValueError("Use 'cp -r' to copy files from local disk to local disk !!")
    else:
        tmpFileName = os.path.join(tmpFilePath, os.path.basename(sourceFileName))
        runCommand('rm -f %s' % tmpFileName)
        print 'eos.cp([ %s ], %s)' % (sourceFileName, tmpFilePath)
        eos.cp([ sourceFileName ], tmpFilePath)
        time.sleep(10)
        print 'eos.cp([ %s ], %s)' % (tmpFileName, targetFilePath)
        eos.cp([ tmpFileName ], targetFilePath)
        time.sleep(10)
        runCommand('rm %s' % tmpFileName)

def processFilePath(sourceFilePath, targetFilePath):
    
    print "<processFilePath>: sourceFilePath = '%s', targetFilePath = '%s'" % (sourceFilePath, targetFilePath)

    if not sourceFilePath[len(sourceFilePath) - 1] == "/":
        sourceFilePath = sourceFilePath + "/"
    if not targetFilePath[len(targetFilePath) - 1] == "/":
        targetFilePath = targetFilePath + "/"

    # create targetFilePath in case it does not yet exist
    createFilePath_recursively(targetFilePath)

    targetFileInfos = readFileInfos(targetFilePath)
    ##print "targetFileInfos:"
    ##print targetFileInfos
    
    sourceFileInfos = readFileInfos(sourceFilePath)
    ##print "sourceFileInfos:"
    ##print sourceFileInfos
    
    for sourceFileInfo in sourceFileInfos:
        print sourceFileInfo
        if sourceFileInfo['permissions'][0] in [ "d", "l" ]: # directory
            sourceFileInfos_bak = sourceFileInfos
            processFilePath(sourceFileInfo['path'], os.path.join(targetFilePath, sourceFileInfo['file']))
            sourceFileInfos = sourceFileInfos_bak
        else: # actual file
            print "sourceFilePath = '%s'" % sourceFilePath
            targetFileName = os.path.join(targetFilePath, sourceFileInfo['file'])            
            existsAndIsUpToDate = False
            for targetFileInfo in targetFileInfos:
                if targetFileInfo['path'] == targetFileName and time.mktime(targetFileInfo['time']) > time.mktime(sourceFileInfo['time']):
                    existsAndIsUpToDate = True
            if existsAndIsUpToDate:
                print "sourceFile = '%s' exists and is up-to-date --> skipping !!" % sourceFileInfo['file']
                continue
            copyFile(sourceFileInfo['path'], targetFilePath)

processFilePath(sourceFilePath, targetFilePath)
