#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def treeSkim( sample, dirOut):
    release="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_4_p2_Trees/"

    #stream   = 'MuTau'
    stream   = 'ElecTau'
    nameHadd = 'hadd_'+sample
    fileHadd = 'batch/hadder/'+nameHadd+'.sh'
    fileLog  = 'batch/hadder/log/'+nameHadd+'.txt'

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
    f.write('rm '+dirOut+'/nTuple'+sample+'_'+stream+'_*.root\n')
    f.close()
    ##
    os.system('chmod u+x batch/hadder/*.sh')
  
###########################################
###########################################

#dirOut = '/data_CMS/cms/htautau/PostMoriond/NTUPLES/MuTau/temp'
dirOut = '/data_CMS/cms/htautau/PostMoriond/NTUPLES/EleTau/temp'

## MC Higgs SM
#basenameH = 'HTT_RUN_MuTau_Dec25_MC_'
basenameH = ''
mass =['90','95','100','105','110','115','120','125','130','135','140','145','150','155','160']
prod =['GGFH','VBFH','VH']

for iM in range(0,len(mass)):
    for iP in range(0,len(prod)):
        treeSkim( basenameH+prod[iP]+mass[iM] , dirOut)

## ## Embedded samples
## basenameEmb = 'HTT_RUN_MuTau_Dec25_Emb_'
## samples = ['Run2012A_06Aug', 'Run2012A_13Jul2012_v1', 'Run2012B_13Jul2012_v4', 'Run2012C_24Aug', 'Run2012C_PRV2']
## json    = [3,0,0,1,4]

## for iS in range(0,len(samples)):
##         treeSkim(basenameEmb+samples[iS] , dirOut)

## for iD in range(1,10):
##         treeSkim( 'HTT_RUN_MuTau_Dec30_Emb_Run201D_PromptReco_v1c2_p'+str(iD), dirOut)

## ## Data (from Arun)
## treeSkim("Run2012A-MuTau-13Jul2012-v1-c1_run",          dirOut)
## treeSkim("Run2012B-MuTau-13Jul2012-v1_run",             dirOut)
## treeSkim("Run2012B-MuTau-13Jul2012-v1-Residual_run",    dirOut)
## treeSkim("Run2012C-MuTau-24Aug2012-v1_run",             dirOut)
## treeSkim("Run2012C-MuTau-PromptReco-v2-c1_run",         dirOut)
## treeSkim("Run2012C-MuTau-PromptReco-v2-Residual_run",   dirOut)

## for iD in range(1,11):
##     treeSkim('Run2012D-MuTau-PromptReco-v1c2_07Dec_p'+str(iD)+'_run',   dirOut)

## Backgrounds 
treeSkim("DYJets",         dirOut)
treeSkim("WJets1Jets",             dirOut)
treeSkim("WJets2Jets",             dirOut)
treeSkim("WJets3Jets",             dirOut)
treeSkim("WJets4Jets",             dirOut)
treeSkim("WJets-p1",             dirOut)
treeSkim("WJets-p2",             dirOut)
treeSkim("TTJets",            dirOut)
treeSkim("T-tW",                dirOut)
treeSkim("Tbar-tW",             dirOut)
treeSkim("WWJetsTo2L2Nu",     dirOut)
treeSkim("WZJetsTo2L2Q",      dirOut)
treeSkim("WZJetsTo3LNu",      dirOut)
treeSkim("ZZJetsTo2L2Q",      dirOut)
treeSkim("ZZJetsTo2L2Nu",     dirOut)
treeSkim("ZZJetsTo4L",        dirOut)
