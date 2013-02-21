#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def treeSkim( sample, xsection, runInSeries=True):

    os.system( 'mkdir batch/' )

    #stream = "MuTau"
    if(re.search("MuTau",sample)!=None):
        stream = "MuTau"
    else:
        stream = "ElecTau"
    print "Stream ", stream
    
    if runInSeries:
         print "Running in series via the command ..."
         runInSeries   = 'treeSkimmer'+stream+'_MVA '+sample+' '+str(xsection)
         print runInSeries
         os.system(runInSeries)
         mv     = 'mv nTuple'+sample+'_Open_'+stream+'Stream_*.root batch/' 
         hadd   = 'hadd -f batch/nTuple'+sample+'_Open_'+stream+'Stream.root batch/nTuple'+sample+'_Open_'+stream+'Stream_*.root'
         remove = 'rm batch/nTuple'+sample+'_Open_'+stream+'Stream_*.root'
         print 'Now doing hadd...'
         os.system(mv)
         os.system(hadd)
         os.system(remove)

    else:
        print "Creating the shell file for the batch..."
        f = open('batch/job'+'_'+sample+'.sh','w')    
        f.write('#!/bin/sh\n\n')
        f.write('export WORKINGDIR="/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/bin/"\n')
        f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
        f.write('cd $WORKINGDIR\n')
        f.write('eval `scram runtime -sh`\n')
        f.write('source /opt/exp_soft/cms/cms_ui_env_crab.sh\n')
        f.write('treeSkimmer'+stream+'_MVA '+sample+' '+str(xsection)+' \n')
        f.write('mv nTuple'+sample+'_Open_'+stream+'Stream_*.root batch/\n')
        f.write('hadd -f batch/nTuple'+sample+'_Open_'+stream+'Stream.root batch/nTuple'+sample+'_Open_'+stream+'Stream_*.root\n')
        f.write('rm batch/nTuple'+sample+'_Open_'+stream+'Stream_*.root\n')
        f.close()
        #os.system('source /opt/exp_soft/cms/t3/t3setup')
        print "Submitting the job to the cms queue via the command ..."
        submitToQueue = '/opt/exp_soft/cms/t3/t3submit -q llr -k eo -N '+sample+' /home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/bin/batch/job'+'_'+sample+'.sh > batch/'+sample+'.txt'
        print submitToQueue
        os.system(submitToQueue)
  
###########################################
###########################################

#treeSkim("VBFH125-MuTau-8TeV-powheg-DR53X-PUS10_run",      1.578 * 0.0632 * 1.0 * 0.0780138080726)
#treeSkim("VBFH125-MuTau-8TeV-powheg-DR53X-PUS10-v2_run",      1.578 * 0.0632 * 1.0 * 0.0780138080726)
treeSkim("VBFH125-MuTau-8TeV-powheg-DR53X-PUS10-v3_run",      1.578 * 0.0632 * 1.0 * 0.0780138080726)

treeSkim("VBFH125-ElecTau-8TeV-powheg-DR53X-PUS10-v3_run", 1.578 * 0.0632 * 1.0 * 0.0627940923235)
