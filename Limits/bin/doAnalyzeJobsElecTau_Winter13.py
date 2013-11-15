#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def analyze(mH,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY,RUN,stream="ElecTau",version="Moriond",useEmb=1):
    
    release="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_11_p6_NewTauID/src/"

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
    f.write('analyze'+stream+'_Winter13 '+str(mH)+' '+category+' '+variable+' '+xtitle+' '+unity +' '+str(nBins)+' '+str(xMin)+' '+str(xMax)+' '+str(magnify)+' '+str(hltEff)+' '+str(logy)+' '+str(maxY)+' '+stream+'/'+outputDir+' '+RUN+' '+version+' '+str(useEmb)+' '+analysis+' > '+fileLog+'\n')
    f.close()
    os.system('chmod u+x batch/analyze/'+stream+'/'+outputDir+'/job*.sh')
###########################################
###########################################

stream = "ElecTau"

versionList=[
    #'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID',##"Baseline"
    ##AntiEMVA5
##     'AntiMu3Loose_AntiEle5VLoose_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Loose_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Tight_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5VTight_HPSDB3H_TauOldDM_OldEleID',
##     ##AntiMu
##     'AntiMu2Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMuMVALoose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMuMVAMedium_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID',
##     ##TauID
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTLoose_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTMedium_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTVTight_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwoLT_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3newDMwLT_TauNewDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3newDMwoLT_TauNewDM_OldEleID'

        ##TauPT bins
    'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt20_OldEleID',## pt>20
    'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt30_OldEleID',## pt>30
    'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt3045_OldEleID',## pt>30 pt<45
    'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt45_OldEleID',## pt>45
    'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt4560_OldEleID',## pt>45 pt<60
    'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt60_OldEleID',## pt>60
    'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt6090_OldEleID',###pt>60 pt<90
    'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt90_OldEleID',####pt>90

             ]

data = 'ABCD'
#iteration = 'Control'
iteration = 'Datacards'

useEmb=1

for version in versionList:
    
    outputDir = 'Results_'+data+'_'+version+'_'+iteration

##     ### Datacards
##     analyze(125,"inclusive"         ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"bTag"     ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"nobTag"   ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    
##     analyze(125,"inclusive"         ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
##     analyze(125,"inclusive"         ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
##     analyze(125,"inclusive"         ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
##     analyze(125,"inclusive"         ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);

## #VisMass
##     analyze(125,"inclusive"         ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"bTag"     ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"nobTag"   ,"nominal" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    
##     analyze(125,"inclusive"         ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"TauUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
##     analyze(125,"inclusive"         ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"TauDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
##     analyze(125,"inclusive"         ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
    
##     analyze(125,"inclusive"         ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"JetDown" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);


##     ##     ##### Datacards MSSM
    analyze(125,"inclusive"         ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTag"              ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTag"            ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"inclusive"         ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTag"              ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTag"            ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"inclusive"         ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTag"              ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTag"            ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"inclusive"         ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTag"              ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTag"            ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"inclusive"         ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTag"              ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTag"            ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

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

##AntiE Control plots
##     analyze(125,"inclusive"         ,"nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat0","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 0'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat1","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 1'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat2","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 2'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat3","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 3'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat4","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 4'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat5","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 5'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat6","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 6'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat7","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 7'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat8","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 8'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat9","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 9'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat10","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 10'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat11","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 11'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat12","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 12'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat13","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 13'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat14","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 14'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat15","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 15'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat16","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 16'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveAntiECat17","nominal" ,"AntiEMVA3raw" ,"'AntiEMVA3 BDT cat 17'","units",outputDir,40,0.9,1,5.0,1.0,0,1.2,data,stream,version,useEmb);
     
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


###### TauPt
##     analyze(125,"inclusive"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,1,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"         ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,1,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"       ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,1,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"         ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"       ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);

## ## ##     ##  Control plots MSSM
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
##     analyze(125,"inclusive"      ,""   ,"ptAllB","'All b-jets p_{T}'","GeV"            ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"      ,""   ,"csvAllB","'b-discriminator'","units"          ,outputDir,40,0, 1.,5.0,1.0,0,2.,data, stream, version, useEmb); 
##     analyze(125,"inclusive"      ,""   ,"ptAll","'Al jets p_{T}'","GeV"                ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);

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

##     analyze(125,"novbfMedium"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"novbfMedium"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"novbfMedium"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfMediumNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

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

##     analyze(125,"boostMedium"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostMedium"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostMedium"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostMediumNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostMedium"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

##     analyze(125,"boostHighlowhiggs"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostHighlowhiggs"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostHighlowhiggs"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggsNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

##     analyze(125,"boostHighhighhiggs"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostHighhighhiggs"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"boostHighhighhiggs"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggsNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

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

##     analyze(125,"vbfTight"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"vbfTight"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb); 
##     analyze(125,"vbfTight"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbfTightNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"vbfTight"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,1,10   ,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfLow"          ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfMedium"       ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"novbfHigh"         ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostLow"          ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostMedium"       ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighlowhiggs" ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"boostHighhighhiggs","JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbf"               ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"vbfTight"          ,"JetUp" ,"diTauVisMass" ,"'Vis Mass'","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2,data,stream,version,useEmb);

    
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
