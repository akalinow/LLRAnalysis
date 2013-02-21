#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def treeSkim( ana, sample, xsection, dirIn, dirOut, iJson, lepveto=1):
    
    release="/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534_TopUp/"
    
    stream = "MuTau"
        
    nameJob = 'job_'+sample+'_'+ana
    fileJob = 'batch/Moriond/treeSkim/'+nameJob+'.sh'
    fileLog = 'batch/Moriond/treeSkim/log/'+nameJob+'.txt'
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
    if lepveto==0:
        f.write('treeSkimmer'+stream+'_Moriond '+ana+' '+sample+' '+str(xsection)+' '+str(dirIn)+' '+str(dirOut)+' '+str(iJson)+' &> '+fileLog+'\n')
    elif lepveto==1:
        f.write('treeSkimmer'+stream+'_Moriond '+ana+' '+sample+' '+str(xsection)+' '+str(dirIn)+' '+str(dirOut)+' '+str(iJson)+' '+str(lepveto)+' &> '+fileLog+'\n')
    f.close()
    os.system('chmod u+x batch/Moriond/treeSkim/*.sh')
    
###########################################
###########################################

dirInArun = '/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/'
#dirInTest = '/data_CMS/cms/ndaci/ndaci_2012/HTauTau/AnalysisHCP/trees/TestTrileptonVeto/'
#dirIn     = '/data_CMS/cms/ndaci/ndaci_2012/HTauTau/AnalysisHCP/trees/'
dirOut    = '/data_CMS/cms/htautau/Moriond/ntuples/MuTau/round3/'

## MC Higgs SM
basenameH = 'HTT_RUN_MuTau_Dec25_MC_'
mass =['110','115','120','125','130','135','140','145','150','155','160']
prod =['GluGluToHToTauTau_M-','VBF_HToTauTau_M-','WH_ZH_TTH_HToTauTau_M-']
anaMC=['nominal','TauUp','TauDown','JetUp','JetDown']

xsec= [[ 25.04 , 22.96 , 21.13 ,19.52 , 18.07 , 16.79 , 15.63 , 14.59 , 13.65 , 12.79 , 11.95 ],  ## GG
       [ 1.809 , 1.729 , 1.649 , 1.578 , 1.511 , 1.448 , 1.389 , 1.333 , 1.280 , 1.231 , 1.185 ],  ## VBF
       [ 1.060+0.5869+0.1887 , 0.9165+0.5117+0.1663 , 0.7966+0.4483+0.1470 , 0.6966+0.3943+0.1302 , 0.6095+0.3473+0.1157 , 0.5351+0.3074+0.1031 ,
         0.4713+0.2728+0.09207 , 0.4164+0.2424+0.08246 , 0.3681+0.2159+0.07403 , 0.3252+0.1923+0.06664 , 0.2817+0.1687+0.06013 ]]  ## VH = WH+ZH+ttH

BR  = [ 7.95E-02 , 7.58E-02 , 7.04E-02 , 6.32E-02 , 5.45E-02 , 4.49E-02 , 3.52E-02 , 2.61E-02 , 1.79E-02 , 1.05E-02 , 3.96E-03 ]

eff = [[ 0.0382199 , 0.0414543 , 0.0441801 , 0.0478489 , 0.0500014 , 0.0523548 , 0.0564325 , 0.0589897 , 0.0611469 , 0.0633131 , 0.0662302],
       [ 0.0699099 , 0.0722955 , 0.0751282 , 0.0779075 , 0.0808326 , 0.0830556 , 0.085988 , 0.0886707 , 0.0913129 , 0.0933543 , 0.0953452],
       [ 0.104487 , 0.109097 , 0.114171 , 0.117655 , 0.121498 , 0.124363 , 0.128446 , 0.130612 , 0.136261 , 0.13828 , 0.139825]]

for iM in range(0,len(mass)):
    for iP in range(0,len(prod)):
        for iAn in range(0,len(anaMC)):
            
            treeSkim( anaMC[iAn] ,
                      basenameH+prod[iP]+mass[iM] ,
                      xsec[iP][iM] * BR[iM] * eff[iP][iM] ,
                      dirInArun,
                      dirOut,
                      -1)

## JSONS
## jsonFile[0] = dirJson+"/Cert_190456-196531_8TeV_13Jul2012ReReco_Collisions12_JSON_v2.txt"; // ReReco 13Jul 
## jsonFile[1] = dirJson+"/Cert_198022-198523_8TeV_24Aug2012ReReco_Collisions12_JSON.txt";    // ReReco 24 Aug 
## jsonFile[2] = dirJson+"/Cert_190456-203002_8TeV_PromptReco_Collisions12_JSON_v2.txt";      // PromptReco 
## jsonFile[3] = dirJson+"/Cert_190782-190949_8TeV_06Aug2012ReReco_Collisions12_JSON.txt";    // ReReco 06Aug
## jsonFile[4] = dirJson+"/Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON.txt";         // PromptReco updated

## Embedded samples
anaEmb = ['nominal','TauUp','TauDown']
#ana = ['nominal']

basenameEmb = 'HTT_RUN_MuTau_Dec25_Emb_'
samples = ['Run2012A_06Aug', 'Run2012A_13Jul2012_v1', 'Run2012B_13Jul2012_v4', 'Run2012C_24Aug', 'Run2012C_PRV2']
json    = [3,0,0,1,4]

for iS in range(0,len(samples)):
    for iAn in range(0,len(anaEmb)):
        treeSkim( anaEmb[iAn] , basenameEmb+samples[iS] , 0 , dirInArun, dirOut, json[iS])

for iD in range(1,10):
    for iAn in range(0,len(anaEmb)):
        treeSkim( anaEmb[iAn] , 'HTT_RUN_MuTau_Dec30_Emb_Run201D_PromptReco_v1c2_p'+str(iD),  0 , dirInArun, dirOut, 4)

## Data (from Arun)
treeSkim("nominal","Run2012A-MuTau-13Jul2012-v1-c1_run",        0, dirInArun, dirOut, 0)
treeSkim("nominal","Run2012B-MuTau-13Jul2012-v1_run",           0, dirInArun, dirOut, 0)
treeSkim("nominal","Run2012B-MuTau-13Jul2012-v1-Residual_run",  0, dirInArun, dirOut, 0)
treeSkim("nominal","Run2012C-MuTau-24Aug2012-v1_run",           0, dirInArun, dirOut, 1)
treeSkim("nominal","Run2012C-MuTau-PromptReco-v2-c1_run",       0, dirInArun, dirOut, 4)
treeSkim("nominal","Run2012C-MuTau-PromptReco-v2-Residual_run", 0, dirInArun, dirOut, 4)

for iD in range(1,11):
    treeSkim("nominal",'Run2012D-MuTau-PromptReco-v1c2_07Dec_p'+str(iD)+'_run', 0, dirInArun, dirOut, 4)

## Backgrounds (from Arun)
for iAn in range(0,len(anaMC)):
    treeSkim(anaMC[iAn],"DYJets-MuTau-50-madgraph-PUS10_run",         3504  * 1.0 * 0.0197277263248, dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"TTJets-MuTau-madgraph-PUS10_run",            225.2 * 1.0 * 0.111411640979,  dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"WJets-MuTau-madgraph-PUS10_run",             36257 * 1.0 * 0.007788005454,  dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"T-tW-MuTau-powheg-PUS10_run",                11.1  * 1.0 * 0.0948342592236, dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"Tbar-tW-MuTau-powheg-PUS10_run",             11.1  * 1.0 * 0.0954733978504, dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"W1Jets-MuTau-madgraph-PUS10-v2_run",         36257 * 1.0 * 0.0191415142175, dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"W2Jets-MuTau-madgraph-PUS10-v2_run",         36257 * 1.0 * 0.0390006921894, dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"W3Jets-MuTau-madgraph-PUS10-v2_run",         36257 * 1.0 * 0.0674073416202, dirInArun, dirOut, -1) 
    treeSkim(anaMC[iAn],"W4Jets-MuTau-madgraph-PUS10_run",            36257 * 1.0 * 0.0931306903978, dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"WZJetsTo2L2Q-MuTau-madgraph-PUS10_run",      2.207 * 1.0 * 0.126736184757,  dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"ZZJetsTo2L2Q-MuTau-madgraph-PUS10_run",      2.502 * 1.0 * 0.140152916478,  dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"ZZJetsTo2L2Nu-MuTau-madgraph-PUS10_run",     0.716 * 1.0 * 0.0414729749683, dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"ZZJetsTo4L-MuTau-madgraph-PUS10_run",        0.181 * 1.0 * 0.121016188426,  dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"WZJetsTo3LNu-MuTau-madgraph-PUS10_run",      1.058 * 1.0 * 0.130024930764,  dirInArun, dirOut, -1)
    treeSkim(anaMC[iAn],"WWJetsTo2L2Nu-MuTau-madgraph-PUS10_run",     5.824 * 1.0 * 0.0921184764829, dirInArun, dirOut, -1)
    
