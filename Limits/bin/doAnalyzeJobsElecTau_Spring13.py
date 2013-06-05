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
    release="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_10_analysis/src/"

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
versionList=[## 'AntiMu1_AntiEle1_TauIso1_OldEleID',
             'AntiMu1_AntiEle1_TauIso1_NewEleID',
             'AntiMu2_AntiEle2Tight_HPSDB3H_NewEleID'
##              'AntiMu2_AntiEle2Tight_HPSDB3H_OldEleID'
             ]
#versionList=['AntiMu1_AntiEle2Medium_TauIso1']

data = 'ABCD'
iteration = 'Tight_Datacards'
#iteration = 'Control'
iterationDYMC = 'ControlDYMC'
iteration0 = 'Met0'
iteration5 = 'Met5'
iteration10 = 'Met10'
iteration15 = 'Met15'
iteration20 = 'Met20'
iteration25 = 'Met25'
iteration30 = 'Met30'
iteration35 = 'Met35'
iteration40 = 'Met40'
iteration45 = 'Met45'

iteration02 = 'AntiZee02'
iteration04 = 'AntiZee04'
iteration06 = 'AntiZee06'
iteration08 = 'AntiZee08'
iteration09 = 'AntiZee09'
iteration099 = 'AntiZee099'
iteration0995 = 'AntiZee0995'
iteration0997 = 'AntiZee0997'
iteration0996 = 'AntiZee0996'
iteration0999 = 'AntiZee0999'
iteration09995 = 'AntiZee09995'

iteration09Met10 = 'AntiZee09Met10'
iteration09Met20 = 'AntiZee09Met20'
iteration09Met30 = 'AntiZee09Met30'
iteration09Met40 = 'AntiZee09Met40'

iteration099Met10 = 'AntiZee099Met10'
iteration099Met20 = 'AntiZee099Met20'
iteration099Met30 = 'AntiZee099Met30'
iteration099Met40 = 'AntiZee099Met40'

useEmb=1

for version in versionList:
    
    outputDir = 'Results_'+data+'_'+version+'_'+iteration
    outputDirDYMC = 'Results_'+data+'_'+version+'_'+iterationDYMC
    outputDir0 = 'Results_'+data+'_'+version+'_'+iteration0
    outputDir5 = 'Results_'+data+'_'+version+'_'+iteration5
    outputDir10 = 'Results_'+data+'_'+version+'_'+iteration10
    outputDir15 = 'Results_'+data+'_'+version+'_'+iteration15
    outputDir20 = 'Results_'+data+'_'+version+'_'+iteration20
    outputDir25 = 'Results_'+data+'_'+version+'_'+iteration25
    outputDir30 = 'Results_'+data+'_'+version+'_'+iteration30
    outputDir35 = 'Results_'+data+'_'+version+'_'+iteration35
    outputDir40 = 'Results_'+data+'_'+version+'_'+iteration40
    outputDir45 = 'Results_'+data+'_'+version+'_'+iteration45

    outputDir02 = 'Results_'+data+'_'+version+'_'+iteration02
    outputDir04 = 'Results_'+data+'_'+version+'_'+iteration04
    outputDir06 = 'Results_'+data+'_'+version+'_'+iteration06
    outputDir08 = 'Results_'+data+'_'+version+'_'+iteration08
    outputDir09 = 'Results_'+data+'_'+version+'_'+iteration09
    outputDir099 = 'Results_'+data+'_'+version+'_'+iteration099
    outputDir0995 = 'Results_'+data+'_'+version+'_'+iteration0995
    outputDir0996 = 'Results_'+data+'_'+version+'_'+iteration0996
    outputDir0997 = 'Results_'+data+'_'+version+'_'+iteration0997
    outputDir0999 = 'Results_'+data+'_'+version+'_'+iteration0999
    outputDir09995 = 'Results_'+data+'_'+version+'_'+iteration09995

    outputDir09Met10 = 'Results_'+data+'_'+version+'_'+iteration09Met10
    outputDir09Met20 = 'Results_'+data+'_'+version+'_'+iteration09Met20
    outputDir09Met30 = 'Results_'+data+'_'+version+'_'+iteration09Met30
    outputDir09Met40 = 'Results_'+data+'_'+version+'_'+iteration09Met40

    outputDir099Met10 = 'Results_'+data+'_'+version+'_'+iteration099Met10
    outputDir099Met20 = 'Results_'+data+'_'+version+'_'+iteration099Met20
    outputDir099Met30 = 'Results_'+data+'_'+version+'_'+iteration099Met30
    outputDir099Met40 = 'Results_'+data+'_'+version+'_'+iteration099Met40


    
## ##     ##  Control plots MSSM
##     analyze(125,"inclusive"      ,""   ,"MEt","'Uncorr MET'","GeV"                         ,outputDir,20,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"      ,""   ,"MEtMVA","'MET'","GeV"                             ,outputDir,20,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"      ,""   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir,32,-3.2,3.2,5.0,1.0,0,1.5,data, stream, version, useEmb);
##     analyze(125,"inclusiveNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0,160,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"bTag"           ,""   ,"diTauNSVfitMass" ,"'SVfit mass btag'","GeV"       ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"nobTag"         ,""   ,"diTauNSVfitMass" ,"'SVfit mass no btag'","GeV"    ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"diTauVisMass" ,"'Vis mass'","GeV"                 ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"bTag"           ,""   ,"diTauVisMass" ,"'Vis mass btag'","GeV"            ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"nobTag"         ,""   ,"diTauVisMass" ,"'Vis mass no btag'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"etaL1","'e #eta'","units"                         ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"etaL2","'#tau #eta'","units"                      ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"eta1","'Leading jet #eta'","units"                ,outputDir,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, useEmb); 
##     analyze(125,"inclusive"      ,""   ,"ptAll","'Al jets p_{T}'","GeV"                    ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"      ,""   ,"ptAllB","'All b-jets p_{T}'","GeV"                ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"      ,""   ,"csvAllB","'b-discriminator'","units"              ,outputDir,40,0, 1.,5.0,1.0,0,2.,data, stream, version, useEmb); 
##     analyze(125,"inclusive"      ,""   ,"nJets30","'jet multiplicity'","units"             ,outputDir,10,0,10,5.0,1.0,1,10,data, stream, version, useEmb);
##     analyze(125,"inclusive"      ,""   ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,10,0,10,5.0,1.0,1,10,data, stream, version, useEmb);



## ##     ##  Control 
##     analyze(125,"inclusive"      ,""   ,"MEt","'Uncorr MET'","GeV"                         ,outputDirDYMC,20,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"MEtMVA","'MET'","GeV"                             ,outputDirDYMC,20,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDirDYMC,32,-3.2,3.2,5.0,1.0,0,1.5,data, stream, version, 0);
##     analyze(125,"inclusiveNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDirDYMC,40,0,160,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"               ,outputDirDYMC,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
## ##     analyze(125,"inclusive",""      ,"diTauVisMass" ,"'Vis mass'","GeV"                    ,outputDirDYMC,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##    ##analyze(125,"inclusive"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDirDYMC,-1,0,200,5.0,1.0,0,1.2,data, stream, version, 0); 
##     ##analyze(125,"inclusive"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDirDYMC,60,0,360,5.0,1.0,0,1.2,data, stream, version, 0);
##     ##analyze(125,"inclusive"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDirDYMC,30,0,300,5.0,1.0,0,1.2,data, stream, version, 0); 
##     ##analyze(125,"inclusive"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDirDYMC,35,0,350,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"etaL1","'e #eta'","units"                         ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"ptL1","'e p_{T}'","GeV"                           ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDirDYMC,30,0, 120,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"etaL2","'#tau #eta'","units"                      ,outputDirDYMC,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, 0);
## ##     ##
##     analyze(125,"inclusive"      ,""   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"eta1","'Leading jet #eta'","units"                ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDirDYMC,27,20,300,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDirDYMC,25,-4.5, 4.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"numPV","'reconstructed vertexes'","units"         ,outputDirDYMC,30,0,30,5.0,1.0,0,1.5,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"decayMode",     "'#tau_{h} decay mode'","units"   ,outputDirDYMC,3,0,3, 5.0,1.0,0,1.4,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDirDYMC,40,0,2,5.0,1.0,0,1.2,data, stream, version, 0);  
##     analyze(125,"inclusive"      ,""   ,"nJets30","'jet multiplicity'","units"             ,outputDirDYMC,10,0,10,5.0,1.0,1,10,data, stream, version, 0);
##     analyze(125,"inclusive"      ,""   ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDirDYMC,10,0,10,5.0,1.0,1,10,data, stream, version, 0);

##     analyze(125,"novbfLow",""   ,"MEt","'Uncorr MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0,   1.2,data, stream, version, 0);
##     analyze(125,"novbfLow",""   ,"MEtMVA","'MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0,   1.2,data, stream, version, 0);
##     analyze(125,"novbfLow",""   ,"MEtMVAPhi","'MET #phi'","units"                    ,outputDir,20,-3.2,3.2,   5.0,1.0,0,1.5,data, stream, version, 0);
##     analyze(125,"novbfLowNoMt",""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"             ,outputDir,16,0,160,5.0,1.0,0,1.2,data, stream, version, 0);
## ##     analyze(125,"novbfLow",""   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,30,0,300,5.0,1.0,0,1.2,data, stream, version, 0);  
## ##     analyze(125,"novbfLow",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfLow",""   ,"etaL1","'e #eta'","units"                       ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data, stream, version, 0);
##     analyze(125,"novbfLow",""   ,"ptL1","'e p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfLow",""   ,"ptL2","'#tau p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"novbfLow",""   ,"etaL2","'#tau #eta'","units"                       ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.,data, stream, version, 0);




##     analyze(125,"novbfHigh",""   ,"MEt","'Uncorr MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""   ,"MEtMVA","'MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5,data, stream, version, useEmb);
##     analyze(125,"novbfHighNoMt",""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"novbfHigh",""   ,"diTauVisMass","'visible mass'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2,data, stream, version, useEmb); 
## ##     analyze(125,"novbfHigh",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""   ,"etaL1","'e #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""   ,"ptL1","'e p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""   ,"etaL2","'#tau #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data, stream, version, useEmb);

##     analyze(125,"boostLow",""   ,"MEt","'Uncorr MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow",""   ,"MEtMVA","'MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow",""   ,"MEtMVAPhi","'MET #phi'","units"                    ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5,data, stream, version, useEmb);
##     analyze(125,"boostLowNoMt",""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"             ,outputDir,16,0,160,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"boostLow",""   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,30,0,300,5.0,1.0,0,1.2,data, stream, version, useEmb);  
## ##     analyze(125,"boostLow",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow",""   ,"etaL1","'e #eta'","units"                       ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data, stream, version, useEmb);
##     analyze(125,"boostLow",""   ,"ptL1","'e p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow",""   ,"ptL2","'#tau p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostLow",""   ,"etaL2","'#tau #eta'","units"                       ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data, stream, version, useEmb);

##     analyze(125,"boostHigh",""   ,"MEt","'Uncorr MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh",""   ,"MEtMVA","'MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh",""   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5,data, stream, version, useEmb);
##     analyze(125,"boostHighNoMt",""   ,"MtLeg1MVA","M_{T}(e#nu)'","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"boostHigh",""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2,data, stream, version, useEmb); 
## ##     analyze(125,"boostHigh",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh",""   ,"etaL1","'e #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data, stream, version, useEmb);
##     analyze(125,"boostHigh",""   ,"ptL1","'e p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh",""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostHigh",""   ,"etaL2","'#tau #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data, stream, version, useEmb);

##     analyze(125,"boostMet15Low",""   ,"MEt","'Uncorr MET'","GeV"                              ,outputDir15,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15Low",""   ,"MEtMVA","'MET'","GeV"                              ,outputDir15,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15Low",""   ,"MEtMVAPhi","'MET #phi'","units"                    ,outputDir15,10,-3.2,3.2,   5.0,1.0,0,1.5,data, stream, version, useEmb);
##     analyze(125,"boostMet15LowNoMt",""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"             ,outputDir15,16,0,160,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"boostMet15High",""   ,"MEt","'Uncorr MET'","GeV"                             ,outputDir15,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15High",""   ,"MEtMVA","'MET'","GeV"                             ,outputDir15,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15High",""   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir15,10,-3.2,3.2,   5.0,1.0,0,1.5,data, stream, version, useEmb);
##     analyze(125,"boostMet15HighNoMt",""   ,"MtLeg1MVA","M_{T}(e#nu)'","GeV"            ,outputDir15,16,0,160,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"vbf",""   ,"MEt","'Uncorr MET'","GeV"                                   ,outputDir,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"vbf",""   ,"MEtMVA","'MET'","GeV"                                   ,outputDir,10,0,100,5.0,1.0,0, 1.2,data, stream, version, useEmb);
##     analyze(125,"vbf",""   ,"MEtMVAPhi","'MET #phi'","units"                         ,outputDir,16,-3.2,3.2,   5.0,1.0,0,1.5,data, stream, version, useEmb);
##     analyze(125,"vbfNoMt",""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,16,0,160,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ##     analyze(125,"vbf",""   ,"diTauVisMass","'Visible mass'","GeV"                    ,outputDir,30,0,300,5.0,1.0,0,1.2,data, stream, version, useEmb);  
## ##     analyze(125,"vbf",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"                   ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf",""   ,"etaL1","'e #eta'","units"                            ,outputDir,50,-2.5, 2.5,5.0,1.0,0, 2.,data, stream, version, useEmb);
##     analyze(125,"vbf",""   ,"ptL1","'e p_{T}'","GeV"                              ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf",""   ,"ptL2","'#tau p_{T}'","GeV"                              ,outputDir,16, 15, 95,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf",""   ,"etaL2","'#tau #eta'","units"                            ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.,data, stream, version, useEmb);


##     analyze(125,"boostMet0Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet30Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet30High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## #AntiZeeMVA
##     analyze(125,"boostAntiZee02Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir02,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee02High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir02,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee04Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir04,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee04High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir04,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee06Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir06,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee06High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir06,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee08Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir08,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee08High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir08,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"boostAntiZee09Met10Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    

##     # Datacards
    analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

    analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

    analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

    analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

    analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"boostHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    

##     ##AntiZee09Met10
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met10High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     ##AntiZee09Met20
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met20High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     ##AntiZee09Met30
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met30High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     ##AntiZee09Met40
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Met40High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     ##AntiZee099Met10
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met10High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     ##AntiZee099Met20
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met20High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     ##AntiZee099Met30
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met30High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met30,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     ##AntiZee099Met40
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Met40High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099Met40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);


##     ##AntiZee09
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##AntiZee099
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee099High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir099,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##AntiZee0995
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0995High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
    
## ##AntiZee0996
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0996High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0996,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##AntiZee0997
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0997High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0997,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##AntiZee0999
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee0999High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0999,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##AntiZee09995
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostAntiZee09995High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir09995,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##Met0
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet0High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir0,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##Met5
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet5High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir5,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##Met10
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet10High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir10,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##Met15
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet15High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir15,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);


## ##Met20
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet20High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir20,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##Met25
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet25High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir25,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##Met35
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet35High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir35,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##Met40
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet40High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir40,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ##Met45
##     analyze(125,"inclusive",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45Low" ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45High",""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,""      ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45Low" ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45High","TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45Low" ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45High","TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45Low" ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45High","JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"inclusive","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfLow" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"novbfHigh","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45Low" ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"boostMet45High","JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"vbf"      ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir45,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);

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

## ## analyze(125,"bTag",""        ,"ptB1", "'leading b-tagged jet p_{T}'","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,100,data, stream, version, useEmb);
## ## analyze(125,"bTag",""        ,"etaB1","'leading b-tagged jet #eta'","units"      ,outputDir,21,-5, 5,5.0,1.0,0,2.,data, stream, version, useEmb);

###QCDShape uncertainty
## analyze(125,"inclusiveQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"inclusiveQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 
## analyze(125,"inclusiveQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"inclusiveQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 

## analyze(125,"novbfHighQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"novbfHighQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 
## analyze(125,"novbfHighQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"novbfHighQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 

## analyze(125,"novbfLowQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"novbfLowQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 
## analyze(125,"novbfLowQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"novbfLowQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 

## analyze(125,"boostHighQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"boostHighQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 
## analyze(125,"boostHighQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"boostHighQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 

## analyze(125,"boostLowQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"boostLowQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 
## analyze(125,"boostLowQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"boostLowQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 

## analyze(125,"vbfQCDEn"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"vbfQCDEn"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 
## analyze(125,"vbfQCDEnSS"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,36,0,360,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb);
## analyze(125,"vbfQCDEnSS"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,20,0,200,5.0,1.0,0,1.2,"ABC",data, stream, version, useEmb); 

##Decay modes
## ## #analyze(125,"inclusiveDM0BLHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM0BLHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong0Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0BLHigh",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 BLHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM0BLHigh",""   ,"ptL2"         ,"'#tau p_{T} 1Prong0Pi0 BLHigh'" ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM1BLHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM1BLHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong1Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1BLHigh",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 BLHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1BLHigh",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 BLHigh'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM2BLHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM2BLHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 3Prongs'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2BLHigh",""   ,"diTauVisMass" ,"'visible mass 3Prongs BLHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2BLHigh",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs BLHigh'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
 
## ## #analyze(125,"inclusiveDM0ECHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM0ECHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong0Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0ECHigh",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 ECHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM0ECHigh",""   ,"ptL2"         ,"'#tau p_{T} 1Prong0Pi0 ECHigh'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM1ECHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM1ECHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong1Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1ECHigh",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 ECHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1ECHigh",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 ECHigh'" ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM2ECHigh",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM2ECHigh",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 3Prongs'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2ECHigh",""   ,"diTauVisMass" ,"'visible mass 3Prongs ECHigh'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2ECHigh",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs ECHigh'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ## #analyze(125,"inclusiveDM0BLLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM0BLLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong0Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0BLLow",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 BLLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM0BLLow",""   ,"ptL2"         ,"'#tau p_{T} 1Prong0Pi0 BLLow'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM1BLLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM1BLLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong1Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1BLLow",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 BLLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1BLLow",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 BLLow'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM2BLLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM2BLLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 3Prongs'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2BLLow",""   ,"diTauVisMass" ,"'visible mass 3Prongs BLLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2BLLow",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs BLLow'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);

## ## #analyze(125,"inclusiveDM0ECLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM0ECLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong0Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0ECLow",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 ECLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM0ECLow",""   ,"ptL2"         ,"'#tau p_{T}  1Prong0Pi0 ECLow'" ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM1ECLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM1ECLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 1Prong1Pi0'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1ECLow",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 ECLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1ECLow",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 ECLow'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM2ECLow",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## #analyze(125,"inclusiveDM2ECLow",""   ,"visibleTauMass" ,"'visible \#tau_{h} mass 3Prongs'","GeV"  ,outputDir,20,0,2,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2ECLow",""   ,"diTauVisMass" ,"'visible mass 3Prongs ECLow'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2ECLow",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs ECLow'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);

## analyze(125,"inclusiveDM0BLLow",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 BL Low'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM0BLHigh",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 BL High'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM0BL",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 BL'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0BL",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0 BL'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0BL",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 BL'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0BL",""   ,"ptL2"         ,"'#tau p_{T}  1Prong0Pi0 BL'" ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1BLLow",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 BL Low'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1BLHigh",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 BL High'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1BL",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 BL'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1BL",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0 BL'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1BL",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 BL'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1BL",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 BL'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2BLLow",""   ,"etaL2"           ,"'#tau #eta 3Prongs BL Low'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2BLHigh",""   ,"etaL2"           ,"'#tau #eta 3Prongs BL High'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2BL",""   ,"etaL2"           ,"'#tau #eta 3Prongs BL'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2BL",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs BL'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2BL",""   ,"diTauVisMass" ,"'visible mass 3Prongs BL'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2BL",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs BL'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);

## analyze(125,"inclusiveDM0ECLow",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 EC Low'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM0ECHigh",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 EC High'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM0EC",""   ,"etaL2"           ,"'#tau #eta 1Prong0Pi0 EC'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0EC",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong0Pi0 EC'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0EC",""   ,"diTauVisMass" ,"'visible mass 1Prong0Pi0 EC'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM0EC",""   ,"ptL2"         ,"'#tau p_{T}  1Prong0Pi0 EC'" ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1ECLow",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 EC Low'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1ECHigh",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 EC High'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM1EC",""   ,"etaL2"           ,"'#tau #eta 1Prong1Pi0 EC'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1EC",""   ,"diTauNSVfitMass" ,"'SVfit mass 1Prong1Pi0 EC'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1EC",""   ,"diTauVisMass" ,"'visible mass 1Prong1Pi0 EC'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM1EC",""   ,"ptL2"         ,"'#tau p_{T} 1Prong1Pi0 EC'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2ECLow",""   ,"etaL2"           ,"'#tau #eta 3Prongs EC Low'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2ECHigh",""   ,"etaL2"           ,"'#tau #eta 3Prongs EC High'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## analyze(125,"inclusiveDM2EC",""   ,"etaL2"           ,"'#tau #eta 3Prongs EC'","units"                       ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2EC",""   ,"diTauNSVfitMass" ,"'SVfit mass 3Prongs EC'","GeV"             ,outputDir,36,0,360,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2EC",""   ,"diTauVisMass" ,"'visible mass 3Prongs EC'","GeV"              ,outputDir,8,80,120,5.0,1.0,0,1.2,data, stream, version, useEmb);
## ## analyze(125,"inclusiveDM2EC",""   ,"ptL2"         ,"'#tau p_{T} 3Prongs EC'"  ,"GeV"              ,outputDir,30,0, 120,5.0,1.0,0,1.2,data, stream, version, useEmb);

#Mt plots
## analyze(125,"inclusiveNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"inclusiveSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"novbfLowNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"novbfLowSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"novbfHighNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"novbfHighSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"boostLowNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"boostLowSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"boostHighNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"boostHighSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"vbfHighNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);
## analyze(125,"vbfHighSSNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV",outputDir,40,0,160,5.0,1.0,0,1.2,data,data, stream, version, useEmb);

