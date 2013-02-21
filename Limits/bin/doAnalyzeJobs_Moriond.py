#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def analyze(mH,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY):
    
    release="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_4_Oct12/"
    stream = "ElecTau"

    nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_'+variable+'_'+outputDir
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
    f.write('analyze'+stream+'_HCP '+str(mH)+' '+category+' '+variable+' '+xtitle+' '+unity +' '+str(nBins)+' '+str(xMin)+' '+str(xMax)+' '+str(magnify)+' '+str(hltEff)+' '+str(logy)+' '+str(maxY)+' '+outputDir+' '+analysis+' > '+fileLog+'\n')
    f.close()
    os.system('chmod u+x batch/analyze/job*.sh')
  
###########################################
###########################################

#outputDir = 'ControlPlots'
#outputDir = 'plotMass'
#outputDir = 'plotDecayModeUncorr'
#outputDir = 'test'
#outputDir = 'jetPlots'
#outputDir = 'plotMass'
#outputDir = 'Combination'
outputDir = 'iter1'

## analyze(125,"inclusive"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfLow",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostLow",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostHigh",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"vbf",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"                   ,outputDir,-1,0,100,5.0,1.0,0,1.2);

analyze(125,"inclusive"      ,""   ,"decayMode",     "'#tau_{h} decay mode'","units"   ,outputDir,3,0,3, 5.0,1.0,0,1.4);
analyze(125,"inclusive"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,40,0,2,5.0,1.0,0,1.2);  
analyze(125,"inclusive"      ,""   ,"MEtMVA","'MET'","GeV"                             ,outputDir,20,0,100,5.0,1.0,0,1.2);
analyze(125,"inclusive"      ,""   ,"MEt","'Uncorr MET'","GeV"                         ,outputDir,20,0,100,5.0,1.0,0,1.2);
analyze(125,"inclusive"      ,""   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir,32,-3.2,3.2,5.0,1.0,0,1.5);
analyze(125,"inclusiveNoMt"  ,""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0,160,5.0,1.0,0,1.2);
#analyze(125,"inclusive"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,-1,0,200,5.0,1.0,0,1.2); 
#analyze(125,"inclusive"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,60,0,360,5.0,1.0,0,1.2);
analyze(125,"inclusive"      ,""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
analyze(125,"inclusive"      ,""   ,"diTauNSVfitMass" ,"'SVfit mass'","GeV"            ,outputDir,35,0,350,5.0,1.0,0,1.2);
analyze(125,"inclusive"      ,""   ,"etaL1","'e #eta'","units"                         ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.);
analyze(125,"inclusive"      ,""   ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,30,0, 120,5.0,1.0,0,1.2);
analyze(125,"inclusive"      ,""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,30,0, 120,5.0,1.0,0,1.2);
analyze(125,"inclusive"      ,""   ,"etaL2","'#tau #eta'","units"                      ,outputDir,36,-2.4, 2.4,5.0,1.0,0,2.);
## analyze(125,"inclusive"      ,""   ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0,30,5.0,1.0,0,1.5);
analyze(125,"inclusive"      ,""   ,"nJets30","'jet multiplicity'","units"             ,outputDir,10,0,10,5.0,1.0,1,10);
analyze(125,"inclusive"      ,""   ,"nJets20BTagged","'bTag jet multiplicity'","units" ,outputDir,10,0,10,5.0,1.0,1,10);
analyze(125,"inclusive"      ,""   ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,27,20,300,5.0,1.0,0,1.2);
analyze(125,"inclusive"      ,""   ,"eta1","'Leading jet #eta'","units"                ,outputDir,25,-4.5, 4.5,5.0,1.0,0,2.);
analyze(125,"inclusive"      ,""   ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,27,20,300,5.0,1.0,0,1.2);
analyze(125,"inclusive"      ,""   ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,25,-4.5, 4.5,5.0,1.0,0,2.);

## ## analyze(125,"bTag",""        ,"ptB1", "'leading b-tagged jet p_{T}'","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,100);
## ## analyze(125,"bTag",""        ,"etaB1","'leading b-tagged jet #eta'","units"      ,outputDir,21,-5, 5,5.0,1.0,0,2.);

## analyze(125,"novbfLow",""   ,"MEt","'Uncorr MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0,   1.2);
## analyze(125,"novbfLow",""   ,"MEtMVA","'MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0,   1.2);
## analyze(125,"novbfLow",""   ,"MEtMVAPhi","'MET #phi'","units"                    ,outputDir,20,-3.2,3.2,   5.0,1.0,0,1.5);
## analyze(125,"novbfLowNoMt",""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"             ,outputDir,16,0,160,5.0,1.0,0,1.2);
## analyze(125,"novbfLow",""   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,30,0,300,5.0,1.0,0,1.2);  
## analyze(125,"novbfLow",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfLow",""   ,"etaL1","'e #eta'","units"                       ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.);
## analyze(125,"novbfLow",""   ,"ptL1","'e p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"novbfLow",""   ,"ptL2","'#tau p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"novbfLow",""   ,"etaL2","'#tau #eta'","units"                       ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);

## analyze(125,"novbfHigh",""   ,"MEt","'Uncorr MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2);
## analyze(125,"novbfHigh",""   ,"MEtMVA","'MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2);
## analyze(125,"novbfHigh",""   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
## analyze(125,"novbfHighNoMt",""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh",""   ,"diTauVisMass","'visible mass'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfHigh",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh",""   ,"etaL1","'e #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.);
## analyze(125,"novbfHigh",""   ,"ptL1","'e p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh",""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh",""   ,"etaL2","'#tau #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.);

## analyze(125,"boostLow",""   ,"MEt","'Uncorr MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0, 1.2);
## analyze(125,"boostLow",""   ,"MEtMVA","'MET'","GeV"                              ,outputDir,10,0,100,5.0,1.0,0, 1.2);
## analyze(125,"boostLow",""   ,"MEtMVAPhi","'MET #phi'","units"                    ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
## analyze(125,"boostLowNoMt",""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"             ,outputDir,16,0,160,5.0,1.0,0,1.2);
## analyze(125,"boostLow",""   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,30,0,300,5.0,1.0,0,1.2);  
## analyze(125,"boostLow",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostLow",""   ,"etaL1","'e #eta'","units"                       ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.);
## analyze(125,"boostLow",""   ,"ptL1","'e p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"boostLow",""   ,"ptL2","'#tau p_{T}'","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"boostLow",""   ,"etaL2","'#tau #eta'","units"                       ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.);

## analyze(125,"boostHigh",""   ,"MEt","'Uncorr MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2);
## analyze(125,"boostHigh",""   ,"MEtMVA","'MET'","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2);
## analyze(125,"boostHigh",""   ,"MEtMVAPhi","'MET #phi'","units"                   ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
## analyze(125,"boostHighNoMt",""   ,"MtLeg1MVA","M_{T}(e#nu)'","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2);
## analyze(125,"boostHigh",""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostHigh",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostHigh",""   ,"etaL1","'e #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.);
## analyze(125,"boostHigh",""   ,"ptL1","'e p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"boostHigh",""   ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"boostHigh",""   ,"etaL2","'#tau #eta'","units"                      ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.);

## analyze(125,"vbf",""   ,"MEt","'Uncorr MET'","GeV"                                   ,outputDir,10,0,100,5.0,1.0,0, 1.2);
## analyze(125,"vbf",""   ,"MEtMVA","'MET'","GeV"                                   ,outputDir,10,0,100,5.0,1.0,0, 1.2);
## analyze(125,"vbf",""   ,"MEtMVAPhi","'MET #phi'","units"                         ,outputDir,16,-3.2,3.2,   5.0,1.0,0,1.5);
## analyze(125,"vbfNoMt",""   ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,16,0,160,5.0,1.0,0,1.2);
## analyze(125,"vbf",""   ,"diTauVisMass","'Visible mass'","GeV"                    ,outputDir,30,0,300,5.0,1.0,0,1.2);  
## analyze(125,"vbf",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"                   ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"vbf",""   ,"etaL1","'e #eta'","units"                            ,outputDir,50,-2.5, 2.5,5.0,1.0,0, 2.);
## analyze(125,"vbf",""   ,"ptL1","'e p_{T}'","GeV"                              ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"vbf",""   ,"ptL2","'#tau p_{T}'","GeV"                              ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
## analyze(125,"vbf",""   ,"etaL2","'#tau #eta'","units"                            ,outputDir,50,-2.5, 2.5,5.0,1.0,0,2.);

#Decay modes
## analyze(125,"inclusive1Prong0Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"inclusive1Prong1Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"inclusive3ProngsBL",""   ,"diTauVisMass","'Visible mass (3prongs in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"inclusive1Prong0Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"inclusive1Prong1Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"inclusive3ProngsBL",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"inclusive1Prong0Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"inclusive1Prong1Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"inclusive3ProngsEC",""   ,"diTauVisMass","'Visible mass (3prongs in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"inclusive1Prong0Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"inclusive1Prong1Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"inclusive3ProngsEC",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);

## analyze(125,"vbf1Prong0Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"vbf1Prong1Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"vbf3ProngsBL",""   ,"diTauVisMass","'Visible mass (3prongs in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"vbf1Prong0Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"vbf1Prong1Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"vbf3ProngsBL",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"vbf1Prong0Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"vbf1Prong1Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"vbf3ProngsEC",""   ,"diTauVisMass","'Visible mass (3prongs in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"vbf1Prong0Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"vbf1Prong1Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"vbf3ProngsEC",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);

## analyze(125,"boostLow1Prong0Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostLow1Prong1Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostLow3ProngsBL",""   ,"diTauVisMass","'Visible mass (3prongs in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostLow1Prong0Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostLow1Prong1Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostLow3ProngsBL",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostLow1Prong0Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostLow1Prong1Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostLow3ProngsEC",""   ,"diTauVisMass","'Visible mass (3prongs in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostLow1Prong0Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostLow1Prong1Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostLow3ProngsEC",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);

## analyze(125,"boostHigh1Prong0Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostHigh1Prong1Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostHigh3ProngsBL",""   ,"diTauVisMass","'Visible mass (3prongs in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostHigh1Prong0Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostHigh1Prong1Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostHigh3ProngsBL",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostHigh1Prong0Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostHigh1Prong1Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostHigh3ProngsEC",""   ,"diTauVisMass","'Visible mass (3prongs in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"boostHigh1Prong0Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostHigh1Prong1Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"boostHigh3ProngsEC",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);

## analyze(125,"novbfLow1Prong0Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfLow1Prong1Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfLow3ProngsBL",""   ,"diTauVisMass","'Visible mass (3prongs in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfLow1Prong0Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfLow1Prong1Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfLow3ProngsBL",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfLow1Prong0Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfLow1Prong1Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfLow3ProngsEC",""   ,"diTauVisMass","'Visible mass (3prongs in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfLow1Prong0Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfLow1Prong1Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfLow3ProngsEC",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);

## analyze(125,"novbfHigh1Prong0Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfHigh1Prong1Pi0BL",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfHigh3ProngsBL",""   ,"diTauVisMass","'Visible mass (3prongs in barrel)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfHigh1Prong0Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh1Prong1Pi0BL",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh3ProngsBL",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in barrel)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh1Prong0Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong0Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfHigh1Prong1Pi0EC",""   ,"diTauVisMass","'Visible mass (1Prong1Pi0 in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfHigh3ProngsEC",""   ,"diTauVisMass","'Visible mass (3prongs in endcap)'","GeV"              ,outputDir,30,0,300,5.0,1.0,0,1.2); 
## analyze(125,"novbfHigh1Prong0Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong0Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh1Prong1Pi0EC",""   ,"diTauNSVfitMass","'SVfit mass (1Prong1Pi0 in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh3ProngsEC",""   ,"diTauNSVfitMass","'SVfit mass (3prongs in endcap)'","GeV"             ,outputDir,35,0,350,5.0,1.0,0,1.2);


## ###For datacard production
## analyze(125,"inclusive",""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,-1,0,200,5.0,1.0,0,1.2); 
## analyze(125,"inclusive",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,60,0,360,5.0,1.0,0,1.2);
## analyze(125,"inclusive","TauUp"   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,-1,0,200,5.0,1.0,0,1.2); 
## analyze(125,"inclusive","TauUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,60,0,360,5.0,1.0,0,1.2);
## analyze(125,"inclusive","TauDown"   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,-1,0,200,5.0,1.0,0,1.2); 
## analyze(125,"inclusive","TauDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,60,0,360,5.0,1.0,0,1.2);
## analyze(125,"inclusive","JetUp"   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,-1,0,200,5.0,1.0,0,1.2); 
## analyze(125,"inclusive","JetUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,60,0,360,5.0,1.0,0,1.2);
## analyze(125,"inclusive","JetDown"   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,-1,0,200,5.0,1.0,0,1.2); 
## analyze(125,"inclusive","JetDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,60,0,360,5.0,1.0,0,1.2);

## analyze(125,"novbfLow",""   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"novbfLow",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfLow","TauUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"novbfLow","TauUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfLow","TauDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"novbfLow","TauDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfLow","JetUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"novbfLow","JetUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfLow","JetDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"novbfLow","JetDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);

## analyze(125,"novbfHigh",""   ,"diTauVisMass","'visible mass'","GeV"              ,outputDir,10,0,200,5.0,1.0,0,1.2); 
## analyze(125,"novbfHigh",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh","TauUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"novbfHigh","TauUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh","TauDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"novbfHigh","TauDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh","JetUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"novbfHigh","JetUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"novbfHigh","JetDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"novbfHigh","JetDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);

## analyze(125,"boostLow",""   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"boostLow",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostLow","TauUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"boostLow","TauUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostLow","TauDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"boostLow","TauDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostLow","JetUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"boostLow","JetUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostLow","JetDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"boostLow","JetDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);

## analyze(125,"boostHigh",""   ,"diTauVisMass","'Visible mass'","GeV"              ,outputDir,10,0,200,5.0,1.0,0,1.2); 
## analyze(125,"boostHigh",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostHigh","TauUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"boostHigh","TauUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostHigh","TauDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"boostHigh","TauDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostHigh","JetUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"boostHigh","JetUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"boostHigh","JetDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"boostHigh","JetDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);

## analyze(125,"vbf",""   ,"diTauVisMass","'Visible mass'","GeV"                    ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"vbf",""   ,"diTauNSVfitMass","'SVfit mass'","GeV"                   ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"vbf","TauUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"vbf","TauUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"vbf","TauDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"vbf","TauDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"vbf","JetUp"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"vbf","JetUp"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
## analyze(125,"vbf","JetDown"   ,"diTauVisMass","'Visible mass'","GeV"               ,outputDir,10,0,200,5.0,1.0,0,1.2);  
## analyze(125,"vbf","JetDown"   ,"diTauNSVfitMass","'SVfit mass'","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);



