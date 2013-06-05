#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def analyze(mH,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY,RUN,stream="MuTau",version="Moriond", useEmb=0,antiWsgn=20,antiWsdb=70):
    
    release="/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_5_3_10_SyncRecoEleIdJEC/src/"

    os.system('mkdir -p results/SoftAnalysis/'+stream+'/'+outputDir)
    os.system('cp -r results/model/* results/SoftAnalysis/'+stream+'/'+outputDir)
    os.system('mkdir -p batch/analyze/SoftAnalysis/'+stream+'/'+outputDir)

    if(variable!="diTauVisMass" and variable!="diTauNSVfitMass"):
        nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_control_'+variable+'_'+stream
    else:
        nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_'+variable+'_'+stream
    fileJob = 'batch/analyze/SoftAnalysis/'+stream+'/'+outputDir+'/'+nameJob+'.sh'
    fileLog = 'results/SoftAnalysis/'+stream+'/'+outputDir+'/log/'+nameJob+'.txt'
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
    f.write('analyze'+stream+'_Spring13 '+str(mH)+' '+category+' '+variable+' '+xtitle+' '+unity +' '+str(nBins)+' '+str(xMin)+' '+str(xMax)+' '+str(magnify)+' '+str(hltEff)+' '+str(logy)+' '+str(maxY)+' SoftAnalysis/'+stream+'/'+outputDir+' '+RUN+' '+version+' '+str(useEmb)+' '+str(antiWsgn)+' '+str(antiWsdb)+' '+analysis+' &> '+fileLog+'\n')
    f.close()
    os.system('chmod u+x batch/analyze/SoftAnalysis/'+stream+'/'+outputDir+'/job*.sh')
###########################################
###########################################

stream = "MuTau"

#versionList=['SoftD_HLTmatch_AntiMu1_TauIso2', 'SoftD_HLTmatch_AntiMu1_HPSDB3H', 'SoftD_HLTmatch_AntiMu2_TauIso2', 'SoftD_HLTmatch_AntiMu2_HPSDB3H',
#             'SoftD_L1ETMcut_AntiMu1_TauIso2', 'SoftD_L1ETMcut_AntiMu1_HPSDB3H', 'SoftD_L1ETMcut_AntiMu2_TauIso2', 'SoftD_L1ETMcut_AntiMu2_HPSDB3H',
#             'SoftABC_HLTmatch_AntiMu1_TauIso2', 'SoftABC_HLTmatch_AntiMu1_HPSDB3H', 'SoftABC_HLTmatch_AntiMu2_TauIso2', 'SoftABC_HLTmatch_AntiMu2_HPSDB3H',
#             'SoftABC_L1ETMcut_AntiMu1_TauIso2', 'SoftABC_L1ETMcut_AntiMu1_HPSDB3H', 'SoftABC_L1ETMcut_AntiMu2_TauIso2', 'SoftABC_L1ETMcut_AntiMu2_HPSDB3H',
#             'SoftLTau_HLTmatch_AntiMu2_TauIso2', 'SoftLTau_HLTmatch_AntiMu2_HPSDB3H', 'SoftLTau_HLTmatch_AntiMu2_TauIso2', 'SoftLTau_HLTmatch_AntiMu2_HPSDB3H']

#versionList=['SoftD_L1ETMcut_AntiMu1_TauIso1', 'SoftD_NoMaxPt_L1ETMcut_AntiMu1_TauIso1', 'SoftD_HLTmatch_AntiMu1_TauIso1', 'SoftD_NoMaxPt_HLTmatch_AntiMu1_TauIso1',
#             'SoftABC_L1ETMcut_AntiMu1_TauIso1', 'SoftABC_NoMaxPt_L1ETMcut_AntiMu1_TauIso1', 'SoftABC_HLTmatch_AntiMu1_TauIso1', 'SoftABC_NoMaxPt_HLTmatch_AntiMu1_TauIso1']

versionList=['SoftD_L1ETMcut_AntiMu2_HPSDB3H']

#data = 'ABCD'

iteration = 'v1'
useEmb=0

antiWsgnList=[20,25,30,35,40]
antiWsdbList=[45,50,55,60,70]
#antiWsgnList=[20,30]
#antiWsdbList=[50,70]

for version in versionList:
    if 'SoftD' in version:
        data='D'
    elif 'SoftABC' in version:
        data='ABC'
    else:
        data='ABCD'

    for antiWsdb in antiWsdbList :
        for antiWsgn in antiWsgnList :

            outputDir = 'res_'+data+'_'+version+'_'+iteration+'/sgn'+str(antiWsgn)+'_sdb'+str(antiWsdb)

            # Mass
            analyze(125,"inclusive","nominal"      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"nominal"      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","nominal"      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"nominal"      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","nominal"      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"nominal"      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            # Visible mass
            analyze(125,"inclusive","nominal"      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"nominal"      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","nominal"      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"nominal"      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","nominal"      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"nominal"      ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            analyze(125,"inclusive","TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            analyze(125,"inclusive","TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            analyze(125,"inclusive","JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            analyze(125,"inclusive","JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow" ,"JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow" ,"JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

            ##  Control
            analyze(125,"inclusive"      ,"nominal"   ,"caloMEtNoHFUncorr","'Uncorr caloMEtNoHF'","GeV"        ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"caloMEtNoHF","'Corr caloMEtNoHF'","GeV"                ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"L1etm","'Uncorr L1ETM'","GeV"                          ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"L1etmCorr","'Corr L1ETM'","GeV"                        ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"caloMEtNoHFUncorrPhi","'Uncorr caloMEtNoHF Phi'","rad" ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"caloMEtNoHFPhi","'Corr caloMEtNoHF Phi'","rad"         ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"L1etmPhi","'Uncorr L1ETM Phi'","rad"                   ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            ##
            analyze(125,"inclusive"      ,"nominal"   ,"MEt","'Uncorr MET'","GeV"                         ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"MEtMVA","'MET'","GeV"                             ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir,32,-3.2,3.2,5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusiveNoMt"  ,"nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"etaL1","'#mu #eta'","units"                         ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"etaL2","'#tau #eta'","units"                      ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,30,0, 120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"genVPt","'gen #tau p_{T}'","GeV"                        ,outputDir,30,0, 120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            if 'NoMaxPt' in version:
                analyze(125,"inclusive"      ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,30,0,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
            else:
                analyze(125,"inclusive"      ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);
                analyze(125,"inclusive"      ,"nominal"   ,"genMuPt","'gen #mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);
            ##
            analyze(125,"inclusive"      ,"nominal"   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,27,20,300,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"eta1","'Leading jet #eta'","units"                ,outputDir,25,-4.5, 4.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,27,20,300,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,25,-4.5, 4.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0,30,5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"decayMode",     "'#tau_{h} decay mode'","units"   ,outputDir,3,0,3, 5.0,1.0,0,1.4,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,40,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);  
            analyze(125,"inclusive"      ,"nominal"   ,"nJets30","'jet multiplicity'","units"             ,outputDir,10,0,10,5.0,1.0,1,10,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"inclusive"      ,"nominal"   ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,10,0,10,5.0,1.0,1,10,data,stream,version,useEmb,antiWsgn,antiWsdb);

            ## novbfLow
            analyze(125,"novbfLow"      ,"nominal"   ,"caloMEtNoHFUncorr","'Uncorr caloMEtNoHF'","GeV"        ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow"      ,"nominal"   ,"caloMEtNoHF","'Corr caloMEtNoHF'","GeV"                ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow"      ,"nominal"   ,"L1etm","'Uncorr L1ETM'","GeV"                          ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow"      ,"nominal"   ,"L1etmCorr","'Corr L1ETM'","GeV"                        ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow"      ,"nominal"   ,"caloMEtNoHFUncorrPhi","'Uncorr caloMEtNoHF Phi'","rad" ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow"      ,"nominal"   ,"caloMEtNoHFPhi","'Corr caloMEtNoHF Phi'","rad"         ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow"      ,"nominal"   ,"L1etmPhi","'Uncorr L1ETM Phi'","rad"                   ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            ##
            analyze(125,"novbfLow","nominal"   ,"MEt","'Uncorr MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0,   1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow","nominal"   ,"MEtMVA","'MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0,   1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                    ,outputDir,20,-3.2,3.2,   5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLowNoMt","nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"             ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow","nominal"   ,"etaL1","'#mu #eta'","units"                       ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow","nominal"   ,"etaL2","'#tau #eta'","units"                       ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfLow","nominal"   ,"genVPt","'gen #tau p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            if 'NoMaxPt' in version:
                analyze(125,"novbfLow"      ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,30,0,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
            else:
                analyze(125,"novbfLow"      ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);
                analyze(125,"novbfLow"      ,"nominal"   ,"genMuPt","'gen #mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);

            ## novbfHigh
            analyze(125,"novbfHigh"      ,"nominal"   ,"caloMEtNoHFUncorr","'Uncorr caloMEtNoHF'","GeV"        ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh"      ,"nominal"   ,"caloMEtNoHF","'Corr caloMEtNoHF'","GeV"                ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh"      ,"nominal"   ,"L1etm","'Uncorr L1ETM'","GeV"                          ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh"      ,"nominal"   ,"L1etmCorr","'Corr L1ETM'","GeV"                        ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh"      ,"nominal"   ,"caloMEtNoHFUncorrPhi","'Uncorr caloMEtNoHF Phi'","rad" ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh"      ,"nominal"   ,"caloMEtNoHFPhi","'Corr caloMEtNoHF Phi'","rad"         ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh"      ,"nominal"   ,"L1etmPhi","'Uncorr L1ETM Phi'","rad"                   ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            ##
            analyze(125,"novbfHigh","nominal"   ,"MEt","'Uncorr MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","nominal"   ,"MEtMVA","'MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHighNoMt","nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","nominal"   ,"etaL1","'#mu #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","nominal"   ,"etaL2","'#tau #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"novbfHigh","nominal"   ,"genVPt","'gen #tau p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            if 'NoMaxPt' in version:
                analyze(125,"novbfHigh"  ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,30,0,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
            else:
                analyze(125,"novbfHigh"  ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);
                analyze(125,"novbfHigh"  ,"nominal"   ,"genMuPt","'gen #mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);

            ## boostLow
            analyze(125,"boostLow"      ,"nominal"   ,"caloMEtNoHFUncorr","'Uncorr caloMEtNoHF'","GeV"        ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow"      ,"nominal"   ,"caloMEtNoHF","'Corr caloMEtNoHF'","GeV"                ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow"      ,"nominal"   ,"L1etm","'Uncorr L1ETM'","GeV"                          ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow"      ,"nominal"   ,"L1etmCorr","'Corr L1ETM'","GeV"                        ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow"      ,"nominal"   ,"caloMEtNoHFUncorrPhi","'Uncorr caloMEtNoHF Phi'","rad" ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow"      ,"nominal"   ,"caloMEtNoHFPhi","'Corr caloMEtNoHF Phi'","rad"         ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow"      ,"nominal"   ,"L1etmPhi","'Uncorr L1ETM Phi'","rad"                   ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            ##
            analyze(125,"boostLow","nominal"   ,"MEt","'Uncorr MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0, 1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow","nominal"   ,"MEtMVA","'MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0, 1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                    ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLowNoMt","nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"             ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow","nominal"   ,"etaL1","'#mu #eta'","units"                       ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow","nominal"   ,"etaL2","'#tau #eta'","units"                       ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostLow","nominal"   ,"genVPt","'gen #tau p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            if 'NoMaxPt' in version:
                analyze(125,"boostLow"  ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,30,0,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
            else:
                analyze(125,"boostLow"  ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);
                analyze(125,"boostLow"  ,"nominal"   ,"genMuPt","'gen #mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);

            ## boostHigh
            analyze(125,"boostHigh"      ,"nominal"   ,"caloMEtNoHFUncorr","'Uncorr caloMEtNoHF'","GeV"        ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh"      ,"nominal"   ,"caloMEtNoHF","'Corr caloMEtNoHF'","GeV"                ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh"      ,"nominal"   ,"L1etm","'Uncorr L1ETM'","GeV"                          ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh"      ,"nominal"   ,"L1etmCorr","'Corr L1ETM'","GeV"                        ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh"      ,"nominal"   ,"caloMEtNoHFUncorrPhi","'Uncorr caloMEtNoHF Phi'","rad" ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh"      ,"nominal"   ,"caloMEtNoHFPhi","'Corr caloMEtNoHF Phi'","rad"         ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh"      ,"nominal"   ,"L1etmPhi","'Uncorr L1ETM Phi'","rad"                   ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            ##
            analyze(125,"boostHigh","nominal"   ,"MEt","'Uncorr MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","nominal"   ,"MEtMVA","'MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHighNoMt","nominal"   ,"MtLeg1MVA","M_{T}(e#nu)'","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","nominal"   ,"etaL1","'#mu #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","nominal"   ,"etaL2","'#tau #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"boostHigh","nominal"   ,"genVPt","'gen #tau p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            if 'NoMaxPt' in version:
                analyze(125,"boostHigh" ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,30,0,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
            else:
                analyze(125,"boostHigh" ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);
                analyze(125,"boostHigh" ,"nominal"   ,"genMuPt","'gen #mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);

            ## vbf
            analyze(125,"vbf"      ,"nominal"   ,"caloMEtNoHFUncorr","'Uncorr caloMEtNoHF'","GeV"        ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"nominal"   ,"caloMEtNoHF","'Corr caloMEtNoHF'","GeV"                ,outputDir,20,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"nominal"   ,"L1etm","'Uncorr L1ETM'","GeV"                          ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"nominal"   ,"L1etmCorr","'Corr L1ETM'","GeV"                        ,outputDir,24,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"nominal"   ,"caloMEtNoHFUncorrPhi","'Uncorr caloMEtNoHF Phi'","rad" ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"nominal"   ,"caloMEtNoHFPhi","'Corr caloMEtNoHF Phi'","rad"         ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf"      ,"nominal"   ,"L1etmPhi","'Uncorr L1ETM Phi'","rad"                   ,outputDir,20,-3.2,3.2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            ##
            analyze(125,"vbf","nominal"   ,"MEt","'Uncorr MET'","GeV"                                   ,outputDir,10,0,100,5.0,1.0,0, 1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf","nominal"   ,"MEtMVA","'MET'","GeV"                                   ,outputDir,10,0,100,5.0,1.0,0, 1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                         ,outputDir,16,-3.2,3.2,   5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbfNoMt","nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf","nominal"   ,"etaL1","'#mu #eta'","units"                            ,outputDir,50,-2.5, 2.5,5.0,1.0,0, 2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf","nominal"   ,"etaL2","'#tau #eta'","units"                            ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                              ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            analyze(125,"vbf","nominal"   ,"genVPt","'gen #tau p_{T}'","GeV"                              ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
            if 'NoMaxPt' in version:
                analyze(125,"vbf"  ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,30,0,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
            else:
                analyze(125,"vbf"  ,"nominal"   ,"ptL1","'#mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);
                analyze(125,"vbf"  ,"nominal"   ,"genMuPt","'gen #mu p_{T}'","GeV"                       ,outputDir,25,0,25,5.0,1.0,0,1.2,data, stream, version, useEmb);

