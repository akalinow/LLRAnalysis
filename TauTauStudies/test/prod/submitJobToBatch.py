#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('../')
from runMuTauStreamAnalyzerFullAnalysis_Recoil_MC_cfg  import *

###########################################
###########################################

def createEventList( stream, sample, directory, firstFile, lastFile):

    print "Analyzing process ", sample
    os.system( 'mkdir batch/'+stream )
    os.system( 'mkdir batch/'+stream+'/'+sample )
    process.source.fileNames = cms.untracked.vstring()

    output = commands.getoutput("dpns-ls /dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+directory)
    outFiles = re.split(r'\n',output)

    totalFiles = 0
    for name in outFiles:
        splitname = re.split(r'/',name)
        if re.search("pat", splitname[len(splitname)-1])!=None :
            totalFiles  = totalFiles + 1
    print "Total number of files in dpm: ", totalFiles
    if totalFiles < lastFile :
        print "Files terminated... exit"
        return

    counter = 0
    for name in outFiles:
        splitname = re.split(r'/',name)
        if re.search("pat", splitname[len(splitname)-1])!=None :
            counter = counter + 1
            if counter>=firstFile and counter<=lastFile :
                print "Adding file "+splitname[ len(splitname) -1 ]
                process.source.fileNames.append('rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/'+directory+'/'+splitname[ len(splitname) -1 ]) 
    process.TFileService.fileName = "treeMuTauStream_"+str(firstFile)+'-'+str(lastFile)+".root"
    out = open('batch/'+stream+'/'+sample+'/'+'runMuTauStreamAnalyzerFullAnalysis_Recoil_MC_'+sample+'_'+str(firstFile)+'-'+str(lastFile)+'_cfg.py','w')
    out.write(process.dumpPython())
    out.close()

    print "Creating the shell file for the batch..."
    f = open('batch/'+stream+'/'+sample+'/job'+'_'+str(firstFile)+'-'+str(lastFile)+'.sh','w')    
    f.write('#!/bin/sh\n\n')
    f.write('export WORKINGDIR="/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7/src/LLRAnalysis/TauTauStudies/test/prod/batch/'+stream+'/'+sample+'"\n')
    f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
    f.write('cd $WORKINGDIR\n')
    f.write('eval `scram runtime -sh`\n')
    f.write('source /opt/exp_soft/cms/cms_ui_env_crab.sh\n')
    f.write('cmsRun '+'runMuTauStreamAnalyzerFullAnalysis_Recoil_MC_'+sample+'_'+str(firstFile)+'-'+str(lastFile)+'_cfg.py\n')
    f.close()

    #os.system('source /opt/exp_soft/cms/t3/t3setup')
    print "Submitting the job to the cms queue via the command ..."
    submitToQueue = '/opt/exp_soft/cms/t3/t3submit -q cms -k eo -N '+sample+'_'+str(firstFile)+'-'+str(lastFile)+' /home/llr/cms/lbianchini/CMSSW_4_2_8_patch7/src/LLRAnalysis/TauTauStudies/test/prod/batch/'+stream+'/'+sample+'/job'+'_'+str(firstFile)+'-'+str(lastFile)+'.sh > batch/'+stream+'/'+sample+'/jobID'+'_'+str(firstFile)+'-'+str(lastFile)+'.txt'
    print submitToQueue
    os.system(submitToQueue)

    print "Creating the shell file to kill the job..."
    jobID   = open('batch/'+stream+'/'+sample+'/jobID'+'_'+str(firstFile)+'-'+str(lastFile)+'.txt','r')
    killJob = open('batch/'+stream+'/'+sample+'/killJob'+'_'+str(firstFile)+'-'+str(lastFile)+'.sh','w')
    for line in jobID:
        killJob.write('#!/bin/sh\n\n')
        killJob.write('qdel '+line)
    jobID.close()
    killJob.close()
    os.system('chmod +x '+'batch/'+stream+'/'+sample+'/killJob'+'_'+str(firstFile)+'-'+str(lastFile)+'.sh')
    
    
###########################################
###########################################
    
    
#createEventList( "MuTau", "QCDmu-MuTau-pythia-20-15-PUS6_run", "/QCD_Pt-20_MuEnrichedPt-15_TuneZ2_7TeV-pythia6/MuTauStream-16Nov2011/5a6ae5cf32db35e729a2aa5976e92294/",  1, 10)

for i in range(10):
    createEventList( "MuTau","DYJets-MuTau-50-madgraph-PUS6_run",
                     "DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/MuTauStream-16Nov2011/700226eee9a93cb10580e91b3d6e5c18",
                     2*i+1,2*i+2)
