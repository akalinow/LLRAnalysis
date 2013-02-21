#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def analyze(mH,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY):
    
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
    f.write('analyze'+stream+'_HCP '+str(mH)+' '+category+' '+variable+' '+xtitle+' '+unity +' '+str(nBins)+' '+str(xMin)+' '+str(xMax)+' '+str(magnify)+' '+str(hltEff)+' '+str(logy)+' '+str(maxY)+' '+outputDir+' '+analysis+' > '+fileLog+'\n')
    f.close()
    os.system('chmod u+x batch/analyze/job*.sh')
  
###########################################
###########################################

outputDir = 'plotAll'
                    
analyze(125,"inclusive",""   ,"decayMode",     "\#tau_{h} decay mode XX","units"   ,outputDir,3,0,3, 5.0,1.0,0,1.4);
analyze(125,"inclusive",""   ,"visibleTauMass","visible \#tau_{h} mass XX","GeV"   ,outputDir,40,0,2,5.0,1.0,0,1.2);  
analyze(125,"inclusive",""   ,"MEtMVA","MET XX","GeV"                             ,outputDir,40,0,100,5.0,1.0,0,1.2);
analyze(125,"inclusive",""   ,"MEtMVAPhi","MET \#phi XX","units"                   ,outputDir,32,-3.2,3.2,   5.0,1.0,0,1.5);
analyze(125,"inclusiveNoMt",""   ,"MtLeg1MVA","M_{T}\(\#mu\#nu\) XX","GeV"            ,outputDir,40,0,160,5.0,1.0,0,1.2);
analyze(125,"inclusive",""   ,"diTauVisMass","visible mass XX","GeV"              ,outputDir,50,0,200,5.0,1.0,0,1.2); 
analyze(125,"inclusive",""   ,"diTauNSVfitMass","SVfit mass XX","GeV"             ,outputDir,60,0,360,5.0,1.0,0,1.2);
analyze(125,"inclusive",""   ,"etaL1","\#mu \#eta XX","units"                      ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
analyze(125,"inclusive",""   ,"ptL1","\#mu p_{T} XX","GeV"                        ,outputDir,27,11, 92,5.0,1.0,0,1.2);
analyze(125,"inclusive",""   ,"ptL2","\#tau p_{T} XX","GeV"                        ,outputDir,27,11, 92,5.0,1.0,0,1.2);
analyze(125,"inclusive",""   ,"etaL2","\#tau \#eta XX","units"                      ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
analyze(125,"inclusive",""   ,"numPV","reconstructed vertexes XX","units"         ,outputDir,30,0,30,5.0,1.0,0,1.5);
analyze(125,"inclusive",""   ,"nJets30","jet multiplicity XX","units"             ,outputDir,10,0, 10,5.0,1.0,1,10);

analyze(125,"bTag",""        ,"ptB1", "leading b-tagged jet p_{T} XX","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,100);
analyze(125,"bTag",""        ,"etaB1","leading b-tagged jet \#eta XX","units"      ,outputDir,21,-5, 5,5.0,1.0,0,2.);

analyze(125,"novbfLow",""   ,"MEtMVA","MET XX","GeV"                              ,outputDir,10,0,100,5.0,1.0,0,   1.2);
analyze(125,"novbfLow",""   ,"MEtMVAPhi","MET \#phi XX","units"                    ,outputDir,20,-3.2,3.2,   5.0,1.0,0,1.5);
analyze(125,"novbfLowNoMt",""   ,"MtLeg1MVA","M_{T}\(\#mu\#nu\) XX","GeV"             ,outputDir,16,0,160,5.0,1.0,0,1.2);
analyze(125,"novbfLow",""   ,"diTauVisMass","visible mass XX","GeV"               ,outputDir,20,0,200,5.0,1.0,0,1.2);  
analyze(125,"novbfLow",""   ,"diTauNSVfitMass","SVfit mass XX","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
analyze(125,"novbfLow",""   ,"etaL1","\#mu \#eta XX","units"                       ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
analyze(125,"novbfLow",""   ,"ptL1","\#mu p_{T} XX","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"novbfLow",""   ,"ptL2","\#tau p_{T} XX","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"novbfLow",""   ,"etaL2","\#tau \#eta XX","units"                       ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);

analyze(125,"novbfHigh",""   ,"MEtMVA","MET XX","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2);
analyze(125,"novbfHigh",""   ,"MEtMVAPhi","MET \#phi XX","units"                   ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
analyze(125,"novbfHighNoMt",""   ,"MtLeg1MVA","M_{T}\(\#mu\#nu\) XX","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2);
analyze(125,"novbfHigh",""   ,"diTauVisMass","visible mass XX","GeV"              ,outputDir,10,0,200,5.0,1.0,0,1.2); 
analyze(125,"novbfHigh",""   ,"diTauNSVfitMass","SVfit mass XX","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2);
analyze(125,"novbfHigh",""   ,"etaL1","\#mu \#eta XX","units"                      ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
analyze(125,"novbfHigh",""   ,"ptL1","\#mu p_{T} XX","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"novbfHigh",""   ,"ptL2","\#tau p_{T} XX","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"novbfHigh",""   ,"etaL2","\#tau \#eta XX","units"                      ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);

analyze(125,"boostLow",""   ,"MEtMVA","MET XX","GeV"                              ,outputDir,10,0,100,5.0,1.0,0, 1.2);
analyze(125,"boostLow",""   ,"MEtMVAPhi","MET \#phi XX","units"                    ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
analyze(125,"boostLowNoMt",""   ,"MtLeg1MVA","M_{T}\(\#mu\#nu\) XX","GeV"             ,outputDir,16,0,160,5.0,1.0,0,1.2);
analyze(125,"boostLow",""   ,"diTauVisMass","visible mass XX","GeV"               ,outputDir,20,0,200,5.0,1.0,0,1.2);  
analyze(125,"boostLow",""   ,"diTauNSVfitMass","SVfit mass XX","GeV"              ,outputDir,-1,0,100,5.0,1.0,0,1.2);
analyze(125,"boostLow",""   ,"etaL1","\#mu \#eta XX","units"                       ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
analyze(125,"boostLow",""   ,"ptL1","\#mu p_{T} XX","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"boostLow",""   ,"ptL2","\#tau p_{T} XX","GeV"                         ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"boostLow",""   ,"etaL2","\#tau \#eta XX","units"                       ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);

analyze(125,"boostHigh",""   ,"MEtMVA","MET XX","GeV"                             ,outputDir,10,0,100,5.0,1.0,0, 1.2);
analyze(125,"boostHigh",""   ,"MEtMVAPhi","MET \#phi XX","units"                   ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
analyze(125,"boostHighNoMt",""   ,"MtLeg1MVA","M_{T}\(\#mu\#nu\) XX","GeV"            ,outputDir,16,0,160,5.0,1.0,0,1.2);
analyze(125,"boostHigh",""   ,"diTauVisMass","visible mass XX","GeV"              ,outputDir,10,0,200,5.0,1.0,0,1.2); 
analyze(125,"boostHigh",""   ,"diTauNSVfitMass","SVfit mass XX","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2);
analyze(125,"boostHigh",""   ,"etaL1","\#mu \#eta XX","units"                      ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
analyze(125,"boostHigh",""   ,"ptL1","\#mu p_{T} XX","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"boostHigh",""   ,"ptL2","\#tau p_{T} XX","GeV"                        ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"boostHigh",""   ,"etaL2","\#tau \#eta XX","units"                      ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);

analyze(125,"vbf",""   ,"MEtMVA","MET XX","GeV"                                   ,outputDir,10,0,100,5.0,1.0,0, 1.2);
analyze(125,"vbf",""   ,"MEtMVAPhi","MET \#phi XX","units"                         ,outputDir,16,-3.2,3.2,   5.0,1.0,0,1.5);
analyze(125,"vbfNoMt",""   ,"MtLeg1MVA","M_{T}\(\#mu\#nu\) XX","GeV"                  ,outputDir,16,0,160,5.0,1.0,0,1.2);
analyze(125,"vbf",""   ,"diTauVisMass","visible mass XX","GeV"                    ,outputDir,10,0,200,5.0,1.0,0,1.2);  
analyze(125,"vbf",""   ,"diTauNSVfitMass","SVfit mass XX","GeV"                   ,outputDir,-1,0,100,5.0,1.0,0,1.2);
analyze(125,"vbf",""   ,"etaL1","\#mu \#eta XX","units"                            ,outputDir,10,-2.5, 2.5,5.0,1.0,0, 2.);
analyze(125,"vbf",""   ,"ptL1","\#mu p_{T} XX","GeV"                              ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"vbf",""   ,"ptL2","\#tau p_{T} XX","GeV"                              ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
analyze(125,"vbf",""   ,"etaL2","\#tau \#eta XX","units"                            ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
