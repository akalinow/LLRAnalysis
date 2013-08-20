#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def analyze(mH,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY,RUN,stream="ElecTau",version="Moriond",useEmb=1):
    
    #release="/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534p2_Spring13_Trees/src/"
    #release="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_4_p2_Trees/src/"
    release="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_10_GITanalysis/src/"

    os.system('cp -r results/model/ results/'+stream+'/'+outputDir)
    os.system('mkdir batch/analyze/'+stream+'/'+outputDir)

    if(variable!="diTauVisMass" and variable!="diTauNSVfitMass"):
        nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_control_'+variable+'_'+stream
    elif (logy==1):
        nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_'+variable+'_'+stream+'_log'
    else :
        nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_'+variable+'_'+stream
        
    fileJob = 'batch/analyze/'+stream+'/'+outputDir+'/'+nameJob+'.sh'
    fileLog = 'results/'+stream+'/'+outputDir+'/log/'+nameJob+'.txt'
    ##
    f = open(fileJob,'w')    
    f.write('#!/bin/sh\n\n')
    f.write('export WORKINGDIR="'+release+'/LLRAnalysis/Limits/bin/"\n')
    f.write('')
    f.write('cd $WORKINGDIR\n')
    f.write('export SCRAM_ARCH=slc5_amd64_gcc462\n')
    f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
    f.write('eval `scram runtime -sh`\n')
    f.write('source /opt/exp_soft/cms/cms_ui_env_crab.sh\n')
    ##
    f.write('analyze'+stream+'_Spring13 '+str(mH)+' '+category+' '+variable+' '+xtitle+' '+unity +' '+str(nBins)+' '+str(xMin)+' '+str(xMax)+' '+str(magnify)+' '+str(hltEff)+' '+str(logy)+' '+str(maxY)+' '+stream+'/'+outputDir+' '+RUN+' '+version+' '+str(useEmb)+' '+analysis+' > '+fileLog+'\n')
    f.close()
    os.system('chmod u+x batch/analyze/'+stream+'/'+outputDir+'/job*.sh')
###########################################
###########################################

stream = "ElecTau"
#versionList=['AntiMu1_AntiEle1_TauIso1','AntiMu1_AntiEle2Tight_TauIso1', 'AntiMu1_AntiEle2VeryTight_TauIso1', ## standard analysis checks
#             'AntiMu1_AntiEle1_TauIso2', 'AntiMu1_AntiEle1_HPSDB3H', 'AntiMu2_AntiEle1_TauIso1',              ## standard analysis checks
#             'AntiMu2_AntiEle2Tight_TauIso2', 'AntiMu2_AntiEle2VeryTight_TauIso2']                            ## standard analysis checks
#versionList=['AntiMu1_AntiEle1_TauIso1','AntiMu1_AntiEle2Tight_TauIso1', 'AntiMu1_AntiEle2VeryTight_TauIso1']                            ## standard analysis checks
#versionList=['AntiMu1_AntiEle1_TauIso1','AntiMu1_AntiEle2Tight_TauIso1', 'AntiMu1_AntiEle2VeryTight_TauIso1', ## standard analysis checks
#             'AntiMu1_AntiEle1_TauIso2', 'AntiMu1_AntiEle1_HPSDB3H', 'AntiMu2_AntiEle1_TauIso1'              ## standard analysis checks
#             ]
#versionList=['AntiMu1_AntiEle1_TauIso1','AntiMu1_AntiEle2Tight_TauIso1', 'AntiMu1_AntiEle2VeryTight_TauIso1', ## standard analysis checks
#            ]
#versionList=[## 'AntiMu1_AntiEle1_TauIso1_OldEleID',
##              'AntiMu1_AntiEle1_TauIso1_NewEleID',
##              'AntiMu2_AntiEle2Tight_HPSDB3H_NewEleID'
##              'AntiMu2_AntiEle2Tight_HPSDB3H_OldEleID'
#             ]

versionList=[##'AntiMu2_AntiEle2Medium_HPSDB3H',
##              'AntiMu1_AntiEleNewMedium_HPSDB3H_OldEleID_ZeeSel',
##              'AntiMu1_AntiEleNewMedium_HPSDB3H_OldEleID_Ztt',
##              'AntiMu1_AntiEleNewMedium_HPSDB3H_OldEleID',
##              'AntiMu1_AntiEleNewMedium_HPSDB3H_OldEleID_NoEleIDSF',
##              'AntiMu1_AntiEleNewMedium_HPSDB3H_OldEleID_MSSM_Ztt',
##              'AntiMu1_AntiEle1_HPSDB3H_OldEleID',
##              'AntiMu1_AntiEle2Tight_HPSDB3H_OldEleID',
##              'AntiMu1_AntiEleNewTight_HPSDB3H_OldEleID',
##              'AntiMu1_AntiEleNewVeryTight_HPSDB3H_OldEleID',
##              'AntiMu2_AntiEle2Tight_HPSDB3H_NewEleIDLooseWP',
##              'AntiMu2_AntiEle2Tight_HPSDB3H_OldEleID'
##              'AntiMu2_AntiEle2VTight_HPSDB3H',
##              'AntiMu2_AntiEle0p92_HPSDB3H'



    #'AntiMu1_AntiEleNewMedium_HPSDB3H_OldEleID_NoWHighStat_MSSM',
    #'AntiMu1_AntiEleNewMedium_HPSDB3H_OldEleID_MSSM',
    'AntiMu1_AntiEleNewMedium_HPSDB3H_OldEleID'
             ]

data = 'ABCD'
#iteration = 'Control'
#iteration = 'ControlZeeSFv2'
#iteration = 'ControlZeeSFv3'
#iteration = 'Shift'
#iteration = 'DatacardsTailFit'
iteration = 'Datacards'


useEmb=1

for version in versionList:
    
    outputDir = 'Results_'+data+'_'+version+'_'+iteration

##     ##### Datacards
    analyze(125,"inclusive"         ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"     ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"   ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    
    analyze(125,"inclusive"         ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
    analyze(125,"inclusive"         ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
    analyze(125,"inclusive"         ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
    analyze(125,"inclusive"         ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);

###VisMass
    analyze(125,"inclusive"         ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"     ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"   ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    
    analyze(125,"inclusive"         ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
    analyze(125,"inclusive"         ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
    analyze(125,"inclusive"         ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
    analyze(125,"inclusive"         ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfLow"          ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfMedium"       ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"novbfHigh"         ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostLow"          ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostMedium"       ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighlowhiggs" ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"boostHighhighhiggs","JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbf"               ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"vbfTight"          ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);


##     ##     ##### Datacards MSSM
##     analyze(125,"inclusive"         ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

## ##ZeeSF
##     analyze(125,"inclusive"             ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong0Pi0BL"    ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong0Pi0EC"    ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong1Pi0BL"    ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong1Pi0EC"    ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong0Pi0BL"    ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong0Pi0EC"    ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong1Pi0BL"    ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong1Pi0EC"    ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);

##     analyze(125,"inclusive"             ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,200,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong0Pi0BL"    ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,200,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong0Pi0EC"    ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,200,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong1Pi0BL"    ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,200,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong1Pi0EC"    ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,200,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong0Pi0BL"    ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong0Pi0EC"    ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong1Pi0BL"    ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"ZeeSel1Prong1Pi0EC"    ,"" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);


## ##     ##  Control plots MSSM
##     analyze(125,"inclusive"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"inclusive"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"inclusive"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"      ,""   ,"diTauVisMass" ,"'Vis mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"      ,""   ,"ptAllB","'All b-jets p_{T}'","GeV"                ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"      ,""   ,"csvAllB","'b-discriminator'","units"              ,outputDir,40,0, 1.,5.0,1.0,0,2.,data, stream, version, useEmb); 
##     analyze(125,"inclusive"      ,""   ,"ptAll","'Al jets p_{T}'","GeV"                    ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"bTag"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"nJets20BTagged","'btag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"bTag"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"bTag"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"      ,""   ,"diTauVisMass" ,"'Vis mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data, stream, version, useEmb);

##     analyze(125,"nobTag"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"nJets20BTagged","'btag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"nobTag"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"nobTag"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"      ,""   ,"diTauVisMass" ,"'Vis mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data, stream, version, useEmb);

##     analyze(125,"vbf"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"vbf"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"vbf"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbfNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbf"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

##     analyze(125,"novbf"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"novbf"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"novbf"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbf"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

##     analyze(125,"boost"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boost"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boost"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boost"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);


##     analyze(125,"novbfLow"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"novbfLow"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"novbfLow"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfLowNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfLow"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

##     analyze(125,"novbfHigh"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"novbfHigh"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"novbfHigh"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfHighNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

##     analyze(125,"boostLow"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostLow"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostLow"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostLowNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostLow"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

##     analyze(125,"boostHigh"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostHigh"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostHigh"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHigh"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);


## ##     ##  Control DY MC
##     analyze(125,"inclusive"      ,""   ,"MEt","'Uncorr MET'","GeV"                         ,outputDirDYMC,20,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"MEtMVA","'MET'","GeV"                             ,outputDirDYMC,20,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDirDYMC,32,-3.2,3.2,5.0,1.0,0,1.5,data, stream, version, 0);
##     analyze(125,"inclusiveNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDirDYMC,40,0,160,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"               ,outputDirDYMC,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive",""      ,"diTauVisMass" ,"'Vis mass'","GeV"                    ,outputDirDYMC,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
   ##analyze(125,"inclusive"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDirDYMC,-1,0,200,5.0,1.0,0,1.2,data, stream, version, 0); 
    ##analyze(125,"inclusive"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDirDYMC,60,0,360,5.0,1.0,0,1.2,data, stream, version, 0);
    ##analyze(125,"inclusive"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDirDYMC,30,0,300,5.0,1.0,0,1.2,data, stream, version, 0); 
    ##analyze(125,"inclusive"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDirDYMC,35,0,350,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"decayMode",     "'#tau_{h} decay mode'","units"   ,outputDirDYMC,3,0,3, 5.0,1.0,0,1.4,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDirDYMC,40,0,2,5.0,1.0,0,1.2,data, stream, version, 0);  
##     analyze(125,"inclusive"      ,""   ,"etaL1","'e #eta'","units"                         ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"ptL1","'e p_{T}'","GeV"                           ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"etaL2","'#tau #eta'","units"                      ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"eta1","'Leading jet #eta'","units"                ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"nJets30","'jet multiplicity'","units"             ,outputDirDYMC,10,0,10,5.0,1.0,1,10,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDirDYMC,10,0,10,5.0,1.0,1,10,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"numPV","'reconstructed vertexes'","units"         ,outputDirDYMC,30,0,30,5.0,1.0,0,1.5,data, stream, version, 0);

##     analyze(125,"vbf",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"               ,outputDirDYMC,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"vbf"      ,""   ,"etaL1","'e #eta'","units"                         ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"vbf"      ,""   ,"ptL1","'e p_{T}'","GeV"                           ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"vbf"      ,""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"vbf"      ,""   ,"etaL2","'#tau #eta'","units"                      ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"vbf"      ,""   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"vbf"      ,""   ,"eta1","'Leading jet #eta'","units"                ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"vbf"      ,""   ,"ptAll","'Al jets p_{T}'","GeV"                    ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"vbf"      ,""   ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version,0 );
##     analyze(125,"vbf"      ,""   ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version,0 );
##     analyze(125,"vbf"      ,""   ,"numPV","'reconstructed vertexes'","units"         ,outputDirDYMC,30,0,30,5.0,1.0,0,1.5,data, stream, version,0);

##     analyze(125,"novbfLow",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"               ,outputDirDYMC,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfLow"      ,""   ,"etaL1","'e #eta'","units"                         ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"novbfLow"      ,""   ,"ptL1","'e p_{T}'","GeV"                           ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfLow"      ,""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfLow"      ,""   ,"etaL2","'#tau #eta'","units"                      ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"novbfLow"      ,""   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfLow"      ,""   ,"eta1","'Leading jet #eta'","units"                ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"novbfLow"      ,""   ,"ptAll","'Al jets p_{T}'","GeV"                    ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfLow"      ,""   ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version,0 );
##     analyze(125,"novbfLow"      ,""   ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version,0 );
##     analyze(125,"novbfLow"      ,""   ,"numPV","'reconstructed vertexes'","units"         ,outputDirDYMC,30,0,30,5.0,1.0,0,1.5,data, stream, version,0);

##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"               ,outputDirDYMC,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfHigh"      ,""   ,"etaL1","'e #eta'","units"                         ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"novbfHigh"      ,""   ,"ptL1","'e p_{T}'","GeV"                           ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfHigh"      ,""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfHigh"      ,""   ,"etaL2","'#tau #eta'","units"                      ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"novbfHigh"      ,""   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfHigh"      ,""   ,"eta1","'Leading jet #eta'","units"                ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"novbfHigh"      ,""   ,"ptAll","'Al jets p_{T}'","GeV"                    ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfHigh"      ,""   ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version,0 );
##     analyze(125,"novbfHigh"      ,""   ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version,0 );
##     analyze(125,"novbfHigh"      ,""   ,"numPV","'reconstructed vertexes'","units"         ,outputDirDYMC,30,0,30,5.0,1.0,0,1.5,data, stream, version,0);

##     analyze(125,"boostLow",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"               ,outputDirDYMC,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostLow"      ,""   ,"etaL1","'e #eta'","units"                         ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"boostLow"      ,""   ,"ptL1","'e p_{T}'","GeV"                           ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostLow"      ,""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostLow"      ,""   ,"etaL2","'#tau #eta'","units"                      ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"boostLow"      ,""   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostLow"      ,""   ,"eta1","'Leading jet #eta'","units"                ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"boostLow"      ,""   ,"ptAll","'Al jets p_{T}'","GeV"                    ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostLow"      ,""   ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version,0 );
##     analyze(125,"boostLow"      ,""   ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version,0 );
##     analyze(125,"boostLow"      ,""   ,"numPV","'reconstructed vertexes'","units"         ,outputDirDYMC,30,0,30,5.0,1.0,0,1.5,data, stream, version,0);

##     analyze(125,"boostHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"               ,outputDirDYMC,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostHigh"      ,""   ,"etaL1","'e #eta'","units"                         ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"boostHigh"      ,""   ,"ptL1","'e p_{T}'","GeV"                           ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostHigh"      ,""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostHigh"      ,""   ,"etaL2","'#tau #eta'","units"                      ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"boostHigh"      ,""   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostHigh"      ,""   ,"eta1","'Leading jet #eta'","units"                ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"boostHigh"      ,""   ,"ptAll","'Al jets p_{T}'","GeV"                    ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"boostHigh"      ,""   ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version,0 );
##     analyze(125,"boostHigh"      ,""   ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version,0 );
##     analyze(125,"boostHigh"      ,""   ,"numPV","'reconstructed vertexes'","units"         ,outputDirDYMC,30,0,30,5.0,1.0,0,1.5,data, stream, version,0);




##     # Datacards
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    
##     # Visible mass
##     analyze(125,"inclusive",""      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,""      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh",""      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,"TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh","TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,"TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh","TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,"JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh","JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow" ,"JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh","JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
