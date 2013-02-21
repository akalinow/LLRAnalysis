#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def treeSkim( sample, dirOut):

    release="/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534_CVS/"

    stream   = 'MuTau'
    nameHadd = 'hadd_'+sample
    fileHadd = 'batch/Moriond/hadd/'+nameHadd+'.sh'
    fileLog  = 'batch/Moriond/hadd/log/'+nameHadd+'.txt'

    f = open(fileHadd,'w')    
    f.write('#!/bin/sh\n\n')
    f.write('export WORKINGDIR="'+release+'/src/LLRAnalysis/Limits/bin/"\n')
    f.write('')
    f.write('cd $WORKINGDIR\n')
    f.write('export SCRAM_ARCH=slc5_amd64_gcc462\n')
    f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
    f.write('eval `scram runtime -sh`\n')
    f.write('source /opt/exp_soft/cms/cms_ui_env_crab.sh\n')
    ##
    f.write('hadd -f  '+dirOut+'/nTuple_'+sample+'_'+stream+'.root  '+dirOut+'/nTuple'+sample+'_'+stream+'_*.root\n')
    f.write('rm '+dirOut+'/nTuple'+sample+'_'+stream+'Stream_*.root\n')
    f.close()
    ##
    os.system('chmod u+x batch/Moriond/hadd/*.sh')
  
###########################################
###########################################

dirOut = '/data_CMS/cms/htautau/Moriond/ntuples/MuTau/'

## MC Higgs SM
basenameH = 'HTT_RUN_MuTau_Dec25_MC_'
mass =['110','115','120','125','130','135','140','145','150','155','160']
prod =['GluGluToHToTauTau_M-','VBF_HToTauTau_M-','WH_ZH_TTH_HToTauTau_M-']

for iM in range(0,len(mass)):
    for iP in range(0,len(prod)):
        treeSkim( basenameH+prod[iP]+mass[iM] , dirOut)

## Embedded samples
basenameEmb = 'HTT_RUN_MuTau_Dec25_Emb_'
samples = ['Run2012A_06Aug', 'Run2012A_13Jul2012_v1', 'Run2012B_13Jul2012_v4', 'Run2012C_24Aug', 'Run2012C_PRV2']
json    = [3,0,0,1,4]

for iS in range(0,len(samples)):
        treeSkim(basenameEmb+samples[iS] , dirOut)

for iD in range(1,10):
        treeSkim( 'HTT_RUN_MuTau_Dec30_Emb_Run201D_PromptReco_v1c2_p'+str(iD), dirOut)

## Data (from Arun)
treeSkim("Run2012A-MuTau-13Jul2012-v1-c1_run",          dirOut)
treeSkim("Run2012B-MuTau-13Jul2012-v1_run",             dirOut)
treeSkim("Run2012B-MuTau-13Jul2012-v1-Residual_run",    dirOut)
treeSkim("Run2012C-MuTau-24Aug2012-v1_run",             dirOut)
treeSkim("Run2012C-MuTau-PromptReco-v2-c1_run",         dirOut)
treeSkim("Run2012C-MuTau-PromptReco-v2-Residual_run",   dirOut)

for iD in range(1,11):
    treeSkim('Run2012D-MuTau-PromptReco-v1c2_07Dec_p'+str(iD)+'_run',   dirOut)

## Backgrounds (from Arun)
treeSkim("DYJets-MuTau-50-madgraph-PUS10_run",         dirOut)
treeSkim("TTJets-MuTau-madgraph-PUS10_run",            dirOut)
treeSkim("WJets-MuTau-madgraph-PUS10_run",             dirOut)
treeSkim("T-tW-MuTau-powheg-PUS10_run",                dirOut)
treeSkim("Tbar-tW-MuTau-powheg-PUS10_run",             dirOut)
treeSkim("W1Jets-MuTau-madgraph-PUS10-v2_run",         dirOut)
treeSkim("W2Jets-MuTau-madgraph-PUS10-v2_run",         dirOut)
treeSkim("W3Jets-MuTau-madgraph-PUS10-v2_run",         dirOut)
treeSkim("W4Jets-MuTau-madgraph-PUS10_run",            dirOut)
treeSkim("WZJetsTo2L2Q-MuTau-madgraph-PUS10_run",      dirOut)
treeSkim("ZZJetsTo2L2Q-MuTau-madgraph-PUS10_run",      dirOut)
treeSkim("ZZJetsTo2L2Nu-MuTau-madgraph-PUS10_run",     dirOut)
treeSkim("ZZJetsTo4L-MuTau-madgraph-PUS10_run",        dirOut)
treeSkim("WZJetsTo3LNu-MuTau-madgraph-PUS10_run",      dirOut)
treeSkim("WWJetsTo2L2Nu-MuTau-madgraph-PUS10_run",     dirOut)
