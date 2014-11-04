#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def analyze(mH,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY,RUN,stream="ElecTau",version="Moriond",useEmb=1):
    
    #release="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_11_p6_NewTauID/src/"
    release = os.getcwd()+"/../../.."
    
    #os.system('cp -r results/model/ results/'+stream+'/'+outputDir)
    #os.system('mkdir batch/analyze/'+stream+'/'+outputDir)

    os.system('mkdir results/'+stream+'/'+outputDir)
    os.system('cp -rf results/model/* results/'+stream+'/'+outputDir+'/.')
    os.system('mkdir batch/analyze/'+stream+'/'+outputDir)

    if(variable!="diTauVisMass" and variable!="diTauNSVfitMass"):
        nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_control_'+variable+'_'+stream
    if (logy==1):
        nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_'+variable+'_'+stream+'_log'
    else :
        nameJob = 'job_m'+str(mH)+'_'+category+'_'+analysis+'_'+variable+'_'+stream
        
    fileJob = 'batch/analyze/'+stream+'/'+outputDir+'/'+nameJob+'.sh'
    fileLog = 'results/'+stream+'/'+outputDir+'/log/'+nameJob+'.txt'
    ##
    f = open(fileJob,'w')
    f.write('#!/bin/sh\n\n')
    f.write('export WORKINGDIR="'+release+'"\n')
    #f.write('export WORKINGDIR="'+release+'/LLRAnalysis/Limits/bin/"\n')
    f.write('')
    f.write('cd $WORKINGDIR\n')
    f.write('export SCRAM_ARCH=slc5_amd64_gcc462\n')
    f.write('source /cvmfs/cms.cern.ch/cmsset_default.sh\n')
    #f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
    f.write('eval `scram runtime -sh`\n')
    f.write('cd LLRAnalysis/Limits/bin\n')
    
    ##
    f.write('analyze'+stream+'_Summer14 '+str(mH)+' '+category+' '+variable+' '+xtitle+' '+unity +' '+str(nBins)+' '+str(xMin)+' '+str(xMax)+' '+str(magnify)+' '+str(hltEff)+' '+str(logy)+' '+str(maxY)+' '+stream+'/'+outputDir+' '+RUN+' '+version+' '+str(useEmb)+' '+analysis+' > '+fileLog+'\n')
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

        ##TauPT bins datacards
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt20_OldEleID',pt>20
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt30_OldEleID',pt>30
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt3045_OldEleID',pt>30 pt<45
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt45_OldEleID',pt>45
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt4560_OldEleID',pt>45 pt<60
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt60_OldEleID',## pt>60
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt6090_OldEleID',###pt>60 pt<90
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt90_OldEleID',####pt>90

##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt20_OldEleID',####pt>20
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt30_OldEleID',##pt>30
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID',####pt>30 pt<45
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_OldEleID',####pt>45
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID',####pt>45 pt<60
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID',####pt>60
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt6090_OldEleID',###pt>60 pt<90
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt90_OldEleID',####pt>90

## SM datacards check
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID',

###MSSM new TauIDMVa control
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_NoUseEmb',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID',

###MSSM standard Mar14
    #'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID'

###E->Tau FR SF 
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID',
##     ##Datacards
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZeeSel1Prong0Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZeeSel1Prong0Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZeeSel1Prong1Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZeeSel1Prong1Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID_ZeeSel1Prong0Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID_ZeeSel1Prong0Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID_ZeeSel1Prong1Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID_ZeeSel1Prong1Pi0EC',
## ##tauptBins
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_ZeeSel1Prong0Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_ZeeSel1Prong0Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_ZeeSel1Prong1Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_ZeeSel1Prong1Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt3045_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt3045_OldEleID_ZeeSel1Prong0Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt3045_OldEleID_ZeeSel1Prong0Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt3045_OldEleID_ZeeSel1Prong1Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt3045_OldEleID_ZeeSel1Prong1Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID_ZeeSel1Prong0Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID_ZeeSel1Prong0Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID_ZeeSel1Prong1Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID_ZeeSel1Prong1Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt4560_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt4560_OldEleID_ZeeSel1Prong0Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt4560_OldEleID_ZeeSel1Prong0Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt4560_OldEleID_ZeeSel1Prong1Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt4560_OldEleID_ZeeSel1Prong1Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID_ZeeSel1Prong0Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID_ZeeSel1Prong0Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID_ZeeSel1Prong1Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID_ZeeSel1Prong1Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt60_OldEleID_ZeeSel',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt60_OldEleID_ZeeSel1Prong0Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt60_OldEleID_ZeeSel1Prong0Pi0EC',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt60_OldEleID_ZeeSel1Prong1Pi0BL',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt60_OldEleID_ZeeSel1Prong1Pi0EC',
    
## ##     ##Jet to tau FR measurement
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_JetTauFR',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTLoose_TauOldDM_OldEleID_JetTauFR',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID_JetTauFR',    
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3HRelaxIso_TauOldDM_OldEleID_JetTauFR',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_NoWCorr_JetTauFR',
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTLoose_TauOldDM_OldEleID_NoWCorr_JetTauFR',
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID_NoWCorr_JetTauFR',    
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3HRelaxIso_TauOldDM_OldEleID_NoWCorr_JetTauFR'

## ##         TTBar contamination
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt3045_OldEleID_TTbarEmb',## pt>30 pt<45
## ##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt4560_OldEleID_TTbarEmb',## pt>45 pt<60
## ##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt60_OldEleID_TTbarEmb',## pt>60
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt45_OldEleID_TTbarEmb',##pt>45
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_TTbarEmb',####pt>30 pt<45
## ##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID_TTbarEmb',####pt>45 pt<60
## ##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID_TTbarEmb',####pt>60
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_OldEleID_TTbarEmb',##pt>45
## 2 btagged jets
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt3045_OldEleID_TTbarEmb_2bTagged',## pt>30 pt<45
##     'AntiMu3Loose_AntiEle5Medium_HPSDB3H_TauOldDM_taupt45_OldEleID_TTbarEmb_2bTagged',##pt>45
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_TTbarEmb_2bTagged',####pt>30 pt<45
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_OldEleID_TTbarEmb_2bTagged',##pt>45
    
##     ## Control plots loose MVA
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTLoose_TauOldDM_taupt30_OldEleID',####pt>30
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTLoose_TauOldDM_taupt3045_OldEleID',####pt>30 pt<45
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTLoose_TauOldDM_taupt4560_OldEleID',####pt>45 pt<60
##     'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTLoose_TauOldDM_taupt60_OldEleID',####pt>60

    ##for QCD shape templates
    #'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut'

    ##for datacards 28/07/14
    #'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut'

    ##for control plots 12/08/14
    #'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut'

    ##for datacards 10/14
    #'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut'

    ##for control plots 21/10/14
    'AntiMu3Loose_AntiEle5Medium_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut'

    ]

data = 'ABCD'
#iteration = 'ControlNewTauID'
#iteration = 'ControlwithoutWG'
#iteration = 'DatacardsSM'
#iteration = 'DatacardsRelax'
#iteration = 'DatacardswithWG'
#iteration = 'ZeeSFDatacards'
#iteration = 'ZeeSFControl'
#iteration = 'JetTauFR'
#iteration = 'Control'
#iteration = 'Datacards'
#iteration = 'Test'
#iteration = 'ControlMVAIso'
#iteration = 'ControlTTbarEmb'
#iteration  = 'QCDShapeTemplateCorrection_010714'
#iteration  = 'QCDShapeTemplateCorrection_010714'
#iteration   = 'Datacards_280714'
#iteration = 'ControlPlots_GoF'
#iteration = 'Datacards_TauSpinner_220914'
#iteration = 'Datacards_ForSignoff_300914'
#iteration = 'Datacards_ForSignoff_031014'
#iteration = 'Datacards_TestEmb_081014'
#iteration = 'Datacards_FinalTest_091014'
#iteration = 'Datacards_QCDCorrUncertainty_101014'
#iteration = 'Datacards_FixedWShape_141014'
#iteration = 'Datacards_ScalePtReweighting_141014'
#iteration = 'Datacards_GoodXS_161014'
#iteration = 'ControlPlotsLOG_TauSpinner_050914'
#iteration = 'Datacards_FixedTrigger_171014'
#iteration = 'ControlPlots_211014'
#iteration = 'ControlPlots_281014'
#iteration = 'Datacards_FixedTTbarEmb'
#iteration = 'ControlPlots_FixedTTbarEmb'
#iteration = 'ControlPlots_WStitching'
iteration = 'Datacards_WStitching'

useEmb=1

for version in versionList:
    
    outputDir = 'Results_'+data+'_'+version+'_'+iteration

## Standard analysis Mar14 with consistent implementation of bTag/nobTag categories in had. tau pT
##    analyze(125,"nobTagHigh"       ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);

## QCD shape templates studies
##    analyze(125,"inclusive","nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,50,0   ,500,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
    
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
##     analyze(125,"bTag"     ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"   ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    
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

## ####TauPt
##     analyze(125,"inclusive"         ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"TauUp" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"TauUp" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"TauUp" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"TauDown" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"TauDown" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"TauDown" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"JetUp" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"JetUp" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"JetUp" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusive"         ,"JetDown" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"bTag"              ,"JetDown" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"nobTag"            ,"JetDown" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

## ## ## ###### TauPt for jet->Tau FR
##     analyze(125,"inclusiveSSHighMt"         ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveSSNoMt"         ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveHighMt"         ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveNoMt"         ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"bTagSSHighMt"              ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"nobTagSSHighMt"            ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveSSHighMt"         ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,1.0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveSSNoMt"         ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,1.0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveHighMt"         ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,1.0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveNoMt"         ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,1.0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"bTagSSHighMt"              ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,1.0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"nobTagSSHighMt"            ,"nominal" ,"ptL2" ,"'#tau p_{T}'","GeV" ,outputDir,30,0   ,300,5.0,1.0,1.0,1.2,data,stream,version,useEmb);

##     analyze(125,"inclusiveSSHighMt"         ,"nominal" ,"MtLeg1MVA" ,"'M_{T}(e#nu)'","GeV" ,outputDir,40,0   ,160,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveSSNoMt"         ,"nominal" ,"MtLeg1MVA" ,"'M_{T}(e#nu)'","GeV" ,outputDir,40,0   ,160,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveHighMt"         ,"nominal" ,"MtLeg1MVA" ,"'M_{T}(e#nu)'","GeV" ,outputDir,40,0   ,160,5.0,1.0,0,1.2,data,stream,version,useEmb);
##     analyze(125,"inclusiveNoMt"         ,"nominal" ,"MtLeg1MVA" ,"'M_{T}(e#nu)'","GeV" ,outputDir,40,0   ,160,5.0,1.0,0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"bTagSSHighMt"              ,"nominal" ,"MtLeg1MVA" ,"'M_{T}(e#nu)'","GeV" ,outputDir,40,0   ,160,5.0,1.0,0,1.2,data,stream,version,useEmb);
## ##     analyze(125,"nobTagSSHighMt"            ,"nominal" ,"MtLeg1MVA" ,"'M_{T}(e#nu)'","GeV" ,outputDir,40,0   ,160,5.0,1.0,0,1.2,data,stream,version,useEmb);

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
     
##ZeeSF
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

########ZeeSF Datacards
##     analyze(125,"inclusive"             ,"nominal" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"nominal" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"nominal" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"TauUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"TauUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"TauUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"TauDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"TauDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"TauDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"JetUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"JetUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"JetUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"JetDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"JetDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"JetDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);

##     analyze(125,"inclusive"             ,"nominal" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"nominal" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"nominal" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"TauUp" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"TauUp" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"TauUp" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"TauDown" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"TauDown" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"TauDown" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"JetUp" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"JetUp" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"JetUp" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"JetDown" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"JetDown" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"JetDown" ,"diTauNSVfitMass","'SVfit Mass'","GeV"         ,outputDir,-1,0,100 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);

###### TauPt
##     analyze(125,"inclusive"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,1,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"         ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,1,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"       ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,1,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTag"         ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"       ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);


## ##     ##  Control plots newTauID
##    #####mva output variables
##     analyze(125,"inclusive"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,10    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     ## mva input variables
##     analyze(125,"inclusive"    ,"" ,"patDecayMode","'#tau decay mode'","units"            ,outputDir,10 ,0    ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"hasSecVtx","'#tau has 2nd vertex'","units"           ,outputDir,2  ,0    ,2     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,50    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,100    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.03,0.03  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.05,0.05  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-10  ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-30  ,30    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"flightLength2","'(#tau flight length)^2 '","units"   ,outputDir,50 ,0    ,0.1   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"flightLengthSig","'#tau flight length Sig '","units" ,outputDir,42 ,-2  ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##    #####mva output variables
##     analyze(125,"inclusiveSS"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,10    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     ## mva input variables
##     analyze(125,"inclusiveSS"    ,"" ,"patDecayMode","'#tau decay mode'","units"            ,outputDir,10 ,0    ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"hasSecVtx","'#tau has 2nd vertex'","units"           ,outputDir,2  ,0    ,2     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,50    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,100    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.03,0.03  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.05,0.05  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-10  ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-30  ,30    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"flightLength2","'(#tau flight length)^2 '","units"   ,outputDir,50 ,0    ,0.1   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusiveSS"    ,"" ,"flightLengthSig","'#tau flight length Sig '","units" ,outputDir,42 ,-2  ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     ## mva useful variables
##     analyze(125,"inclusive"    ,"" ,"flightLengthX","'#tau flight length X '","units"     ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"flightLengthY","'#tau flight length Y '","units"     ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"flightLengthZ","'#tau flight length Z '","units"     ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pvX","'PV X '","units"                               ,outputDir,20 ,-0.5   ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pvY","'PV Y '","units"                               ,outputDir,20 ,-0.5   ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pvZ","'PV Z '","units"                               ,outputDir,20 ,-25    ,25     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"svX","'SV X '","units"                               ,outputDir,20 ,-0.5   ,1.0    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"svY","'SV Y '","units"                               ,outputDir,20 ,-0.5   ,1.0    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"svZ","'SV Z '","units"                               ,outputDir,20 ,-10    ,10     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pvCov00","'PV Cov00 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pvCov01","'PV Cov01 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pvCov02","'PV Cov02 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pvCov11","'PV Cov11 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pvCov12","'PV Cov12 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"pvCov22","'PV Cov22 '","units"                       ,outputDir,20 ,0.     ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"svCov00","'SV Cov00 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"svCov01","'SV Cov01 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"svCov02","'SV Cov02 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"svCov11","'SV Cov11 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"svCov12","'SV Cov12 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"svCov22","'SV Cov22 '","units"                       ,outputDir,20 ,0.     ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##    #####mva output variables
##     analyze(125,"bTag"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,10    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     ## mva input variables
##     analyze(125,"bTag"    ,"" ,"patDecayMode","'#tau decay mode'","units"            ,outputDir,10 ,0    ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"hasSecVtx","'#tau has 2nd vertex'","units"           ,outputDir,2  ,0    ,2     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,50    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,100    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.03,0.03  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.05,0.05  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-10  ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-30  ,30    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"flightLength2","'(#tau flight length)^2 '","units"   ,outputDir,50 ,0    ,0.1   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"flightLengthSig","'#tau flight length Sig '","units" ,outputDir,42 ,-2  ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
   #####mva output variables
    #analyze(125,"bTagSS"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,10    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    ## mva input variables
    #analyze(125,"bTagSS"    ,"" ,"patDecayMode","'#tau decay mode'","units"            ,outputDir,10 ,0    ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"hasSecVtx","'#tau has 2nd vertex'","units"           ,outputDir,2  ,0    ,2     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagSS"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,50    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,100    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagSS"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.03,0.03  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.05,0.05  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagSS"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-10  ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-30  ,30    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"flightLength2","'(#tau flight length)^2 '","units"   ,outputDir,50 ,0    ,0.1   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"bTagSS"    ,"" ,"flightLengthSig","'#tau flight length Sig '","units" ,outputDir,42 ,-2  ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     ## mva useful variables
##     analyze(125,"bTag"    ,"" ,"flightLengthX","'#tau flight length X '","units"     ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"flightLengthY","'#tau flight length Y '","units"     ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"flightLengthZ","'#tau flight length Z '","units"     ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pvX","'PV X '","units"                               ,outputDir,20 ,-0.5   ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pvY","'PV Y '","units"                               ,outputDir,20 ,-0.5   ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pvZ","'PV Z '","units"                               ,outputDir,20 ,-25    ,25     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"svX","'SV X '","units"                               ,outputDir,20 ,-0.5   ,1.0    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"svY","'SV Y '","units"                               ,outputDir,20 ,-0.5   ,1.0    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"svZ","'SV Z '","units"                               ,outputDir,20 ,-10    ,10     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pvCov00","'PV Cov00 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pvCov01","'PV Cov01 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pvCov02","'PV Cov02 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pvCov11","'PV Cov11 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pvCov12","'PV Cov12 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"pvCov22","'PV Cov22 '","units"                       ,outputDir,20 ,0.     ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"svCov00","'SV Cov00 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"svCov01","'SV Cov01 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"svCov02","'SV Cov02 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"svCov11","'SV Cov11 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"svCov12","'SV Cov12 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"    ,"" ,"svCov22","'SV Cov22 '","units"                       ,outputDir,20 ,0.     ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##    ###mva output variables
##     analyze(125,"nobTag"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,10    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
## ## ##     mva input variables
##     analyze(125,"nobTag"    ,"" ,"patDecayMode","'#tau decay mode'","units"            ,outputDir,10 ,0    ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"hasSecVtx","'#tau has 2nd vertex'","units"           ,outputDir,2  ,0    ,2     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,50    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,100    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.03,0.03  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.05,0.05  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-10  ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-30  ,30    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"flightLength2","'(#tau flight length)^2 '","units"   ,outputDir,50 ,0    ,0.1   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"flightLengthSig","'#tau flight length Sig '","units" ,outputDir,42 ,-2  ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
   #####mva output variables
    #analyze(125,"nobTagSS"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"hpsMVA3oldDMwLT","'MVA3 oldDMwLT output'","units"    ,outputDir,30 ,-1   ,1     ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"hpsDB3H","'hpsDB3H'","GeV"                           ,outputDir,30 ,0    ,10    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    ## mva input variables
    #analyze(125,"nobTagSS"    ,"" ,"patDecayMode","'#tau decay mode'","units"            ,outputDir,10 ,0    ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"hasSecVtx","'#tau has 2nd vertex'","units"           ,outputDir,2  ,0    ,2     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"chargedIsoPtSum","'Sum P_{T}(charged) '","GeV"       ,outputDir,35 ,0.5  ,7.5   ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"neutralIsoPtSum","'Sum P_{T}(neutral) '","GeV"       ,outputDir,25 ,0    ,25    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagSS"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,50    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"puCorrPtSum","'Sum P_{T}(pu corrected) '","GeV"      ,outputDir,25 ,0    ,100    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagSS"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.03,0.03  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"dxyTau","'#tau dxy '","units"                        ,outputDir,30 ,-0.05,0.05  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"dxyErrTau","'#tau dxy error '","units"               ,outputDir,100 ,0    ,0.02  ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagSS"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-10  ,10    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"dxySigTau","'#tau dxy Sig '","units"                 ,outputDir,20 ,-30  ,30    ,5.0,1.0,1.0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"flightLength2","'(#tau flight length)^2 '","units"   ,outputDir,50 ,0    ,0.1   ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
    #analyze(125,"nobTagSS"    ,"" ,"flightLengthSig","'#tau flight length Sig '","units" ,outputDir,42 ,-2  ,25    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     ## mva useful variables
##     analyze(125,"nobTag"    ,"" ,"flightLengthX","'#tau flight length X '","units"     ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"flightLengthY","'#tau flight length Y '","units"     ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"flightLengthZ","'#tau flight length Z '","units"     ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pvX","'PV X '","units"                               ,outputDir,20 ,-0.5   ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pvY","'PV Y '","units"                               ,outputDir,20 ,-0.5   ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pvZ","'PV Z '","units"                               ,outputDir,20 ,-25    ,25     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"svX","'SV X '","units"                               ,outputDir,20 ,-0.5   ,1.0    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"svY","'SV Y '","units"                               ,outputDir,20 ,-0.5   ,1.0    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"svZ","'SV Z '","units"                               ,outputDir,20 ,-10    ,10     ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pvCov00","'PV Cov00 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pvCov01","'PV Cov01 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pvCov02","'PV Cov02 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pvCov11","'PV Cov11 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pvCov12","'PV Cov12 '","units"                       ,outputDir,20 ,-0.2   ,0.2    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"pvCov22","'PV Cov22 '","units"                       ,outputDir,20 ,0.     ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"svCov00","'SV Cov00 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"svCov01","'SV Cov01 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"svCov02","'SV Cov02 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"svCov11","'SV Cov11 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"svCov12","'SV Cov12 '","units"                       ,outputDir,20 ,-0.1   ,0.1    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"    ,"" ,"svCov22","'SV Cov22 '","units"                       ,outputDir,20 ,0.     ,0.5    ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);

## ## ##     ##  Control plots VisMass/SVFitMass

##     analyze(125,"inclusive"             ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTag"                  ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTag"                ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);

##     analyze(125,"inclusive"             ,"" ,"genVPt","'Gen higgs Pt'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);

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
## ##     analyze(125,"inclusive"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
## ##     analyze(125,"inclusive"      ,""   ,"diTauVisMass" ,"'Vis mass'","GeV"             ,outputDir,-1,0,100,5.0,1.0,0,1.2,data, stream, version, useEmb);
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
## ##     analyze(125,"bTag"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
## ##     analyze(125,"bTag"      ,""   ,"diTauVisMass" ,"'Vis mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data, stream, version, useEmb);

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
## ##     analyze(125,"nobTag"    ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
## ##     analyze(125,"nobTag"      ,""   ,"diTauVisMass" ,"'Vis mass'","GeV"             ,outputDir,-1,0   ,100,5.0,1.0,0,1.2  ,data, stream, version, useEmb);

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


    ##### Test for No B-Tag low GoF
##    analyze(125,"nobTagLow"             ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,60,0,350 ,5.0,1.0,1,1.5  ,data,stream,version,useEmb);

    ##### Datacards MSSM new categories (August/September 2014)
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

    analyze(125,"bTagLow"              ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"             ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"            ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"         ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"           ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"bTagLow"              ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"             ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"            ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"         ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"           ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"bTagLow"              ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"             ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"            ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"         ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"           ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"bTagLow"              ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"             ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"            ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"         ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"           ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"bTagLow"              ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"             ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"            ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"         ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"           ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);


## ##     ##  Control plots MSSM new categories
   
    ## ## ##     ##  Control plots VisMass/SVFitMass

##     analyze(125,"inclusive"             ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"               ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"              ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"             ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"          ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"            ,"" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"             ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"               ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"              ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"             ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"          ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"            ,"" ,"diTauNSVfitMass","'SVfit mass'","GeV"         ,outputDir,40,0,200 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
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
##     analyze(125,"inclusive"    ,""   ,"ptAllB","'All b-jets p_{T}'","GeV"              ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"    ,""   ,"csvAllB","'b-discriminator'","units"            ,outputDir,40,0, 1.,5.0,1.0,0,2.,data, stream, version, useEmb);
##     analyze(125,"inclusive"    ,""   ,"ptAll","'Al jets p_{T}'","GeV"                  ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);

##     analyze(125,"bTagLow"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"nJets20BTagged","'btag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLowNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"nJets20BTagged","'btag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHighNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);

##     analyze(125,"nobTagLow"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"nJets20BTagged","'btag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLowNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"nJets20BTagged","'btag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMediumNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);

##     analyze(125,"nobTagHigh"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"ptL1","'e p_{T}'","GeV"                           ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"etaL1","'e #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,38,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"etaL2","'#tau #eta'","units"                      ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"nJets30","'jet multiplicity'","units"             ,outputDir,9 ,0   ,9  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"nJets20BTagged","'btag jet multiplicity'","units" ,outputDir,4 ,0   ,4  ,5.0,1.0,1,10   ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"pt1","'Leading jet p_{T}'","GeV"                  ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"eta1","'Leading jet #eta'","units"                ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"pt2","'Sub-Leading jet p_{T}'","GeV"              ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"eta2","'Sub-Leading jet #eta'","units"            ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"ptB1","'Leading btag jet p_{T}'","GeV"            ,outputDir,20,0   ,200,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"etaB1","'Leading btag jet #eta'","units"          ,outputDir,50,-5.0,5.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"csvAll","'CSV b-discriminator'","units"           ,outputDir,25,0   ,1.0,5.0,1.0,0,2.0  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"MEtMVA","'MET'","GeV"                             ,outputDir,26,0   ,104,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHighNoMt","" ,"MtLeg1MVA","'M_{T}(e#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
