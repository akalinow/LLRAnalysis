#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def analyze(mH,category,analysis,variable,xtitle,unity,outputDir,magnify):
    
    release="/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534_CVS/"
    stream = "MuTau"

    nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_'+variable
    fileJob = 'batch/analyze/'+nameJob+'.sh'
    fileLog = 'batch/analyze/log/'+nameJob+'.txt'
    ##
    f = open(fileJob,'w')    
    f.write('#!/bin/sh\n\n')
    f.write('export WORKINGDIR="'+release+'/src/LLRAnalysis/Limits/bin/"\n')
    f.write('')
    f.write('cd $WORKINGDIR\n')
    f.write('export SCRAM_ARCH=slc5_amd64_gcc462\n')
    f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
    f.write('eval `scram runtime -sh`\n')
    f.write('source /opt/exp_soft/cms/cms_ui_env_crab.sh\n')
    ##
    f.write('analyze'+stream+'_HCP '+str(mH)+' '+category+' '+variable+' '+xtitle+' '+unity +' '+outputDir +' '+str(magnify)+' '+analysis+' > '+fileLog+'\n')
    f.close()
    os.system('chmod u+x batch/analyze/job*.sh')
  
###########################################
###########################################

outputDir = 'test3'

mH = [125]
category = ['novbf','vbf','boost','inclusive']
#category = ['vbf']
mode     = ['','Low','High']
analysis = ['','TauUp','TauDown','JetUp','JetDown']
variable = ['diTauNSVfitMass']
xtitle   = ['mass']
unity    = ['GeV']
magnify  = 1.0

for iM in range(0,len(mH)):
    for iC in range(0,len(category)):
        for iMode in range(0,len(mode)):
            for iA in range(0,len(analysis)):
                for iV in range(0,len(variable)):
                    analyze( mH[iM] , category[iC]+mode[iMode] , analysis[iA] , variable[iV] , xtitle[iV] , unity[iV] , outputDir , magnify )
