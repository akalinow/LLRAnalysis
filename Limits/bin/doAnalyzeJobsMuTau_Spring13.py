#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def analyze(mH,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY,RUN,stream="MuTau",version="Moriond",useEmb=1,antiWsgn=20,antiWsdb=70):
    
    release="/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_5_3_10_SyncRecoEleIdJEC/src/"

    os.system('cp -r results/model/ results/'+stream+'/'+outputDir)
    os.system('mkdir batch/analyze/'+stream+'/'+outputDir)

    if(variable!="diTauVisMass" and variable!="diTauNSVfitMass"):
        nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_control_'+variable+'_'+stream
    else:
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
    f.write('analyze'+stream+'_Spring13 '+str(mH)+' '+category+' '+variable+' '+xtitle+' '+unity +' '+str(nBins)+' '+str(xMin)+' '+str(xMax)+' '+str(magnify)+' '+str(hltEff)+' '+str(logy)+' '+str(maxY)+' '+stream+'/'+outputDir+' '+RUN+' '+version+' '+str(useEmb)+' '+str(antiWsgn)+' '+str(antiWsdb)+' '+analysis+' &> '+fileLog+'\n')
    f.close()
    os.system('chmod u+x batch/analyze/'+stream+'/'+outputDir+'/job*.sh')
###########################################
###########################################

stream = "MuTau"

#versionList=['AntiMu1_TauIso1', 'AntiMu1_TauIso2', 'AntiMu1_HPSDB3H', ## standard analysis checks
#             'AntiMu2_TauIso1', 'AntiMu2_TauIso2', 'AntiMu2_HPSDB3H']
#data = 'ABCD'
data = 'ABCD'

versionList=['AntiMu2_HPSDB3H']
iteration = 'v1'

useEmbList=[1]
antiWsgnList=[20]
antiWsdbList=[70]

for antiWsdb in antiWsdbList :
    for antiWsgn in antiWsgnList :
        for useEmb in useEmbList :
            for version in versionList:

                outputDir = 'res_'+data+'_'+version+'_'+iteration

                # Mass
                analyze(125,"inclusive","nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                # Visible mass
                analyze(125,"inclusive","nominal" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"nominal" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","nominal" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"nominal" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","nominal" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"nominal" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"nominal" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"nominal" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"inclusive","TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"TauUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"TauUp" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"TauUp" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"inclusive","TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"TauDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"TauDown" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"TauDown" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"inclusive","JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"JetUp" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"JetUp" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"JetUp" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"inclusive","JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow" ,"JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow" ,"JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf"      ,"JetDown" ,"diTauVisMass" ,"'Vis mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"bTag"     ,"JetDown" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"nobTag"   ,"JetDown" ,"diTauVisMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

                ##  Control
                analyze(125,"inclusive"      ,"nominal"   ,"MEt","'Uncorr MET'","GeV"        ,outputDir,20,0,100,5.0,1.0,0,1.2,    data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive"      ,"nominal"   ,"MEtMVA","'MET'","GeV"            ,outputDir,20,0,100,5.0,1.0,0,1.2,    data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive"      ,"nominal"   ,"MEtMVAPhi","'MET #phi'","units"  ,outputDir,32,-3.2,3.2,5.0,1.0,0,1.5, data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusiveNoMt"  ,"nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV" ,outputDir,40,0,160,5.0,1.0,0,1.2,    data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive"      ,"nominal"   ,"etaL1","'e #eta'","units"        ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,  data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive"      ,"nominal"   ,"ptL1","'e p_{T}'","GeV"          ,outputDir,30,0,120,5.0,1.0,0,1.2,    data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive"      ,"nominal"   ,"ptL2","'#tau p_{T}'","GeV"       ,outputDir,30,0,120,5.0,1.0,0,1.2,    data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive"      ,"nominal"   ,"etaL2","'#tau #eta'","units"     ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,  data,stream,version,useEmb,antiWsgn,antiWsdb);
                ##
                analyze(125,"inclusive","nominal","pt1",           "'Leading jet p_{T}'","GeV"        ,outputDir,27,20,300,5.0,1.0,0,1.2, data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive","nominal","eta1",          "'Leading jet #eta'","units"       ,outputDir,25,-4.5,4.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive","nominal","pt2",           "'Sub-Leading jet p_{T}'","GeV"    ,outputDir,27,20,300,5.0,1.0,0,1.2, data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive","nominal","eta2",          "'Sub-Leading jet #eta'","units"   ,outputDir,25,-4.5,4.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive","nominal","numPV",         "'reconstructed vertexes'","units" ,outputDir,30,0,30,5.0,1.0,0,1.5,   data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive","nominal","decayMode",     "'#tau_{h} decay mode'","units"    ,outputDir,3,0,3,5.0,1.0,0,1.4,     data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive","nominal","visibleTauMass","'visible #tau_{h} mass'","GeV"    ,outputDir,40,0,2,5.0,1.0,0,1.2,    data,stream,version,useEmb,antiWsgn,antiWsdb);  
                analyze(125,"inclusive","nominal","nJets30",       "'jet multiplicity'","units"       ,outputDir,10,0,10,5.0,1.0,1,10,    data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive","nominal","nJets20BTagged","'bTag jet multiplicity'","units"  ,outputDir,10,0,10,5.0,1.0,1,10,    data,stream,version,useEmb,antiWsgn,antiWsdb);
                ##
                analyze(125,"inclusive","nominal","ptAll",  "'All jets p_{T}'","GeV"                  ,outputDir,27,20,300,5.0,1.0,0,1.2, data, stream, version, useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive","nominal","ptAllB", "'All b-jets p_{T}'","GeV"                ,outputDir,27,20,300,5.0,1.0,0,1.2, data, stream, version, useEmb,antiWsgn,antiWsdb);
                analyze(125,"inclusive","nominal","csvAll", "'All jets b-discriminator'","units"      ,outputDir,40,0, 1.,5.0,1.0,0,2.,   data, stream, version, useEmb,antiWsgn,antiWsdb); 
                analyze(125,"inclusive","nominal","csvAllB","'All b-jets b-discriminator'","units"    ,outputDir,40,0, 1.,5.0,1.0,0,2.,   data, stream, version, useEmb,antiWsgn,antiWsdb); 

                analyze(125,"novbfLow","nominal"   ,"MEt","'Uncorr MET'","GeV"                       ,outputDir,10,0,100,5.0,1.0,0,  1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow","nominal"   ,"MEtMVA","'MET'","GeV"                           ,outputDir,10,0,100,5.0,1.0,0,  1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                 ,outputDir,20,-3.2,3.2,  5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLowNoMt","nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow","nominal"   ,"etaL1","'e #eta'","units"                       ,outputDir,50,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow","nominal"   ,"ptL1","'e p_{T}'","GeV"                         ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                      ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfLow","nominal"   ,"etaL2","'#tau #eta'","units"                    ,outputDir,10,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"novbfHigh","nominal"   ,"MEt","'Uncorr MET'","GeV"                      ,outputDir,10,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","nominal"   ,"MEtMVA","'MET'","GeV"                          ,outputDir,10,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                ,outputDir,10,-3.2,3.2,  5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHighNoMt","nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"           ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","nominal"   ,"etaL1","'e #eta'","units"                      ,outputDir,50,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","nominal"   ,"ptL1","'e p_{T}'","GeV"                        ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                     ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"novbfHigh","nominal"   ,"etaL2","'#tau #eta'","units"                   ,outputDir,50,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"boostLow","nominal"   ,"MEt","'Uncorr MET'","GeV"                       ,outputDir,10,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow","nominal"   ,"MEtMVA","'MET'","GeV"                           ,outputDir,10,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                 ,outputDir,10,-3.2,3.2,  5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLowNoMt","nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow","nominal"   ,"etaL1","'e #eta'","units"                       ,outputDir,50,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow","nominal"   ,"ptL1","'e p_{T}'","GeV"                         ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                      ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostLow","nominal"   ,"etaL2","'#tau #eta'","units"                    ,outputDir,50,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"boostHigh","nominal"   ,"MEt","'Uncorr MET'","GeV"                      ,outputDir,10,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","nominal"   ,"MEtMVA","'MET'","GeV"                          ,outputDir,10,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                ,outputDir,10,-3.2,3.2,  5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHighNoMt","nominal"   ,"MtLeg1MVA","M_{T}(e#nu)'","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","nominal"   ,"etaL1","'e #eta'","units"                      ,outputDir,50,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","nominal"   ,"ptL1","'e p_{T}'","GeV"                        ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                     ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"boostHigh","nominal"   ,"etaL2","'#tau #eta'","units"                   ,outputDir,50,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);

                analyze(125,"vbf","nominal"   ,"MEt","'Uncorr MET'","GeV"                            ,outputDir,10,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf","nominal"   ,"MEtMVA","'MET'","GeV"                                ,outputDir,10,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf","nominal"   ,"MEtMVAPhi","'MET #phi'","units"                      ,outputDir,16,-3.2,3.2,  5.0,1.0,0,1.5,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbfNoMt","nominal"   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                 ,outputDir,16,0,160,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf","nominal"   ,"etaL1","'e #eta'","units"                            ,outputDir,50,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf","nominal"   ,"ptL1","'e p_{T}'","GeV"                              ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf","nominal"   ,"ptL2","'#tau p_{T}'","GeV"                           ,outputDir,16,15,95,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
                analyze(125,"vbf","nominal"   ,"etaL2","'#tau #eta'","units"                         ,outputDir,50,-2.5,2.5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);


        ## ## analyze(125,"bTag",""        ,"ptB1","'leading b-tagged jet p_{T}'","GeV"       ,outputDir,50,30,330,5.0,1.0,1,100,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"bTag",""        ,"etaB1","'leading b-tagged jet #eta'","units"      ,outputDir,21,-5,5,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);

        ###QCDShape uncertainty
        ## analyze(125,"inclusiveQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 
        ## analyze(125,"inclusiveQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 

        ## analyze(125,"novbfHighQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"novbfHighQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 
        ## analyze(125,"novbfHighQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"novbfHighQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 

        ## analyze(125,"novbfLowQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"novbfLowQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 
        ## analyze(125,"novbfLowQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"novbfLowQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 

        ## analyze(125,"boostHighQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"boostHighQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 
        ## analyze(125,"boostHighQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"boostHighQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 

        ## analyze(125,"boostLowQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"boostLowQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 
        ## analyze(125,"boostLowQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"boostLowQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 

        ## analyze(125,"vbfQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"vbfQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 
        ## analyze(125,"vbfQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"vbfQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data,stream,version,useEmb,antiWsgn,antiWsdb); 

        ##Decay modes
        ## ## #analyze(125,"inclusiveDM0BLHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM0BLHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong0Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0BLHigh",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 BLHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM0BLHigh",""   ,"ptL2"         ,"'#tau p_{T} 1Prong0Pi0 BLHigh'" ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM1BLHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM1BLHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong1Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1BLHigh",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 BLHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1BLHigh",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 BLHigh'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM2BLHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM2BLHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 3Prongs'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2BLHigh",""   ,"diTauVisMass" ,"'visible mass 3Prongs BLHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2BLHigh",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs BLHigh'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

        ## ## #analyze(125,"inclusiveDM0ECHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM0ECHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong0Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0ECHigh",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 ECHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM0ECHigh",""   ,"ptL2"         ,"'#tau p_{T} 1Prong0Pi0 ECHigh'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM1ECHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM1ECHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong1Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1ECHigh",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 ECHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1ECHigh",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 ECHigh'" ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM2ECHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM2ECHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 3Prongs'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2ECHigh",""   ,"diTauVisMass" ,"'visible mass 3Prongs ECHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2ECHigh",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs ECHigh'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

        ## ## #analyze(125,"inclusiveDM0BLLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM0BLLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong0Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0BLLow",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 BLLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM0BLLow",""   ,"ptL2"         ,"'#tau p_{T} 1Prong0Pi0 BLLow'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM1BLLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM1BLLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong1Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1BLLow",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 BLLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1BLLow",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 BLLow'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM2BLLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM2BLLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 3Prongs'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2BLLow",""   ,"diTauVisMass" ,"'visible mass 3Prongs BLLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2BLLow",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs BLLow'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

        ## ## #analyze(125,"inclusiveDM0ECLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM0ECLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong0Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0ECLow",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 ECLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM0ECLow",""   ,"ptL2"         ,"'#tau p_{T}  1Prong0Pi0 ECLow'" ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM1ECLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM1ECLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong1Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1ECLow",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 ECLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1ECLow",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 ECLow'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM2ECLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## #analyze(125,"inclusiveDM2ECLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 3Prongs'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2ECLow",""   ,"diTauVisMass" ,"'visible mass 3Prongs ECLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2ECLow",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs ECLow'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

        ## analyze(125,"inclusiveDM0BLLow",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 BL Low'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM0BLHigh",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 BL High'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM0BL",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 BL'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0BL",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0 BL'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0BL",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 BL'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0BL",""   ,"ptL2"         ,"'#tau p_{T}  1Prong0Pi0 BL'" ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1BLLow",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 BL Low'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1BLHigh",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 BL High'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1BL",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 BL'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1BL",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0 BL'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1BL",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 BL'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1BL",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 BL'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2BLLow",""   ,"etaL2"           ,"'#tau #eta 3Prongs BL Low'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2BLHigh",""   ,"etaL2"           ,"'#tau #eta 3Prongs BL High'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2BL",""   ,"etaL2"           ,"'#tau #eta 3Prongs BL'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2BL",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs BL'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2BL",""   ,"diTauVisMass" ,"'visible mass 3Prongs BL'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2BL",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs BL'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

        ## analyze(125,"inclusiveDM0ECLow",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 EC Low'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM0ECHigh",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 EC High'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM0EC",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 EC'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0EC",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0 EC'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0EC",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 EC'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM0EC",""   ,"ptL2"         ,"'#tau p_{T}  1Prong0Pi0 EC'" ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1ECLow",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 EC Low'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1ECHigh",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 EC High'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM1EC",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 EC'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1EC",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0 EC'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1EC",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 EC'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM1EC",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 EC'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2ECLow",""   ,"etaL2"           ,"'#tau #eta 3Prongs EC Low'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2ECHigh",""   ,"etaL2"           ,"'#tau #eta 3Prongs EC High'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveDM2EC",""   ,"etaL2"           ,"'#tau #eta 3Prongs EC'","units"                       ,outputDir,36,-2.4,2.4,5.0,1.0,0,2.,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2EC",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs EC'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2EC",""   ,"diTauVisMass" ,"'visible mass 3Prongs EC'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## ## analyze(125,"inclusiveDM2EC",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs EC'"  ,"GeV"              ,outputDir,30,0,120,5.0,1.0,0,1.2,data,stream,version,useEmb,antiWsgn,antiWsdb);

        #Mt plots
        ## analyze(125,"inclusiveNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"inclusiveSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"novbfLowNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"novbfLowSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"novbfHighNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"novbfHighSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"boostLowNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"boostLowSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"boostHighNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"boostHighSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"vbfHighNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);
        ## analyze(125,"vbfHighSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data,stream,version,useEmb,antiWsgn,antiWsdb);

