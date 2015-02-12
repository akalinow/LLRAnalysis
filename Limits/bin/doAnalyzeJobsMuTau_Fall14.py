#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def analyze(mH,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY,RUN,stream="ElecTau",version="Moriond",useEmb=1):
    
    #release=os.getcwd()
    
    #release="/home/llr/cms/davignon//TAU_ID/Release_OlivierTES/CMSSW_5_3_11_p6_analysis/src"
    release = os.getcwd()+"/../../.."

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
    f.write('export WORKINGDIR="'+release+'/LLRAnalysis/Limits/bin/"\n')
    f.write('')
    f.write('cd $WORKINGDIR\n')
    f.write('export SCRAM_ARCH=slc5_amd64_gcc462\n')
    f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
    f.write('eval `scram runtime -sh`\n')
    f.write('source /opt/exp_soft/cms/cms_ui_env_crab.sh\n')
    ##
    f.write('analyze'+stream+'_Fall14 '+str(mH)+' '+category+' '+variable+' '+xtitle+' '+unity +' '+str(nBins)+' '+str(xMin)+' '+str(xMax)+' '+str(magnify)+' '+str(hltEff)+' '+str(logy)+' '+str(maxY)+' '+stream+'/'+outputDir+' '+RUN+' '+version+' '+str(useEmb)+' '+analysis+' > '+fileLog+'\n')
    f.close()
    os.system('chmod u+x batch/analyze/'+stream+'/'+outputDir+'/job*.sh')
###########################################
###########################################

stream = "MuTau"

versionList=[
##     'AntiMu3Tight_AntiEle5Loose_HPSDB3H_TauOldDM_OldEleID',##"Baseline"
##     'AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID',##"Baseline"
    ##AntiEMVA5
##     'AntiMu3Tight_AntiEle5VLoose_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMu3Tight_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMu3Tight_AntiEle5Tight_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMu3Tight_AntiEle5VTight_HPSDB3H_TauOldDM_OldEleID',
##     ##AntiMu
##     'AntiMu2Tight_AntiEle5Loose_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMuMVAMedium_AntiEle5Loose_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMuMVATight_AntiEle5Loose_HPSDB3H_TauOldDM_OldEleID',
##     ##TauID
##     'AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwLTLoose_TauOldDM_OldEleID',
##     'AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwLTMedium_TauOldDM_OldEleID',
##     'AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID',
##     'AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwLTVTight_TauOldDM_OldEleID',
    
##     'AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwoLT_TauOldDM_OldEleID',
##     'AntiMu3Tight_AntiEle5Loose_HPSMVA3newDMwLT_TauNewDM_OldEleID',
##     'AntiMu3Tight_AntiEle5Loose_HPSMVA3newDMwoLT_TauNewDM_OldEleID'

    ##TauPT bins
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt20_OldEleID',## pt>20
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt2030_OldEleID',## pt>30
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt30_OldEleID',## pt>30
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt3045_OldEleID',## pt>30 pt<45
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt45_OldEleID',## pt>45
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt4560_OldEleID',## pt>45 pt<60
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt60_OldEleID',## pt>60
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt6090_OldEleID',###pt>60 pt<90
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt90_OldEleID',####pt>90

##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt20_OldEleID',## pt>20
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt2030_OldEleID',## pt>30
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt30_OldEleID',## pt>30
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID',## pt>30 pt<45
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_OldEleID',## pt>45
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID',## pt>45 pt<60
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID',## pt>60
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt6090_OldEleID',#pt>60 pt<90
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt90_OldEleID',####pt>90

## SM datacards check
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID',
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID'

###MSSM new TauIDMVa control
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_NoUseEmb',
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID'

###MSSM
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt30_OldEleID',## pt>30
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt30_OldEleID',## pt>30

## ##     ##Jet to tau FR measurement
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_JetTauFR',
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTLoose_TauOldDM_OldEleID_JetTauFR',
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3HRelaxIso_TauOldDM_OldEleID_JetTauFR',
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID_JetTauFR',
##good    'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_NoWCorr_JetTauFR',
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTLoose_TauOldDM_OldEleID_NoWCorr_JetTauFR',
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3HRelaxIso_TauOldDM_OldEleID_NoWCorr_JetTauFR',
##good    'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID_NoWCorr_JetTauFR',

##         TTBar contamination
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt3045_OldEleID_TTbarEmb',## pt>30 pt<45
## ##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt4560_OldEleID_TTbarEmb',## pt>45 pt<60
## ##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt60_OldEleID_TTbarEmb',## pt>60
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt45_OldEleID_TTbarEmb',## pt>45
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_TTbarEmb',## pt>30 pt<45
## ##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID_TTbarEmb',## pt>45 pt<60
## ##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID_TTbarEmb',## pt>60
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_OldEleID_TTbarEmb',## pt>45
## ## 2 btagged jets
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt3045_OldEleID_TTbarEmb_2bTagged',## pt>30 pt<45
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt45_OldEleID_TTbarEmb_2bTagged',## pt>45
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_TTbarEmb_2bTagged',## pt>30 pt<45
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_OldEleID_TTbarEmb_2bTagged',## pt>45
    
## TauES
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID'

##     ## Control plots loose MVA
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTLoose_TauOldDM_taupt30_OldEleID',pt>30
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTLoose_TauOldDM_taupt3045_OldEleID',## pt>30 pt<45
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTLoose_TauOldDM_taupt4560_OldEleID',## pt>45 pt<60
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTLoose_TauOldDM_taupt60_OldEleID',## pt>60

####Decay modes check (scale factors for 1Prong 0Pi0?)
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_DecayModeNoCorr',
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTVTight_TauOldDM_OldEleID_DecayModeNoCorr',
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID_DecayModeNoCorr',

##            ##barrel
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_Barrel_DecayModeNoCorr',
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTVTight_TauOldDM_OldEleID_Barrel_DecayModeNoCorr',
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID_Barrel_DecayModeNoCorr',

##            ##endcap
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_EndCap_DecayModeNoCorr',
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTVTight_TauOldDM_OldEleID_EndCap_DecayModeNoCorr',
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID_EndCap_DecayModeNoCorr',

####Jet to tau FR measurement for mutau/etau
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_NoWCorr_JetTauFR',
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID_NoWCorr_JetTauFR',    

####WJets SF measurement for thth
##    'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_mupt30_OldEleID_WJetsNorm',
##    'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt45_mupt30_OldEleID_WJetsNorm',

##Standard analysis Mar14
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt20_OldEleID',## pt>20
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt2030_OldEleID',## pt>30
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt30_OldEleID',## pt>30
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt3045_OldEleID',## pt>30 pt<45
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt45_OldEleID',## pt>45
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt4560_OldEleID',## pt>45 pt<60
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt60_OldEleID',## pt>60
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt6090_OldEleID',###pt>60 pt<90
##     'AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt90_OldEleID',####pt>90

##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt20_OldEleID',## pt>20
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt2030_OldEleID',## pt>30
##    'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt30_OldEleID',## pt>30 good
##    'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID',## pt>30 pt<45 good
##    'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_OldEleID',## pt>45 good
##    'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID',## pt>45 pt<60 good
##    'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID',## pt>60 good
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt6090_OldEleID',#pt>60 pt<90
##     'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt90_OldEleID',####pt>90

    ##for QCD shape templates Olivier July 2014
    ##'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut'

    ##for datacards 28/07/14
    'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut'

    ##no W/QCD Corr 07/11/14
    #'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut_NoQCDCorr_NoLooseTightWCorr'

    #DecayModeNoCorr 21/10/14
    #'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_DecayModeNoCorr_SVfitMassCut'
    #'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_DecayModeNoCorr_SVfitMassCut'

    #Control plots 21/10/14
    #'AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut'


    ]

data = 'ABCD'
#iteration = 'ControlNewTauID'
#iteration = 'ControlwithoutWG'
#iteration = 'DatacardsSM'
#iteration = 'DatacardsRelax'
#iteration = 'Datacards'
#iteration = 'MvaOutput'
#iteration = 'ControlTTbarEmb'
#iteration = 'TauESControl'
#iteration = 'JetTauFR'
#iteration = 'DecayModesCheck'
#iteration = 'JetTauFR_thth'
#iteration  = 'CheckHqTSUSY'
#iteration  = 'PlotsHiggspT'
#iteration   = 'Datacards_280714'
#iteration = 'Datacards_TauSpinner_220914'
#iteration = 'Datacards_ForSignoff_300914'
#iteration = 'Datacards_ForSignoff_031014'
#iteration = 'TestTTbarEmb_031014'
#iteration = 'Datacards_ForSignoff_041014'
#iteration = 'Datacards_Test_081014'
#iteration = 'Datacards_FinalTest_091014'
#iteration = 'Datacards_QCDCorrUncertainty_101014'
#iteration = 'Datacards_FixedWShape_141014'
#iteration = 'Datacards_ScalePtReweighting_141014'
#iteration = 'Datacards_FixedTrigger_171014'
#iteration = 'ControlPlots_281014'
#iteration = 'Datacards_FixedTTbarEmb'
#iteration = 'Datacards_WStitching'
#iteration = 'ControlPlots_091114'
#iteration = 'visibleTauMass_091114'
#iteration = 'Datacards_091114'
#iteration = 'Datacards_QCDCorr_WCorr_071114'
#iteration = 'Datacards_DecayMode_051114'
#iteration = 'visibleTauMass_WStitching'
#iteration = 'ControlPlots_WStitching'
#iteration = 'ControlPlots_FixedTTbarEmb'
#iteration = 'visibleTauMass_261014'
#iteration = 'Datacards_TauRecoCorrected_151114'
#iteration = 'ControlPlots_TauRecoCorrected_151114'
#iteration = 'ControlPlots_TauRecoCorrected_181114'
#iteration = 'Datacards_TestTTbarChanges_191114'
#iteration = 'Datacards_TTbarCorrected_211114'
#iteration = 'ControlPlots_TTbarCorrected_211114'
#iteration = 'ControlPlots_261114'
#iteration = 'Datacards_261114'
#iteration = 'Datacards_031214'
#iteration = 'Datacards_TestTriggerBug_081214'
#iteration = 'ControlPlots_TestTriggerBug_081214'
#iteration = 'Datacards_ZttMC_160115'
#iteration = 'Datacards_Sync_210115'
#iteration = 'Datacards_NoTriggerEmb_220115'
#iteration = 'Datacards_NoTriggerEmb_230115'
#iteration = 'Datacards_ZttMC_230115'
#iteration = 'Datacards_Shalhout_250115'
#iteration = 'Datacards_Shalhout_020215'
#iteration = 'Datacards_Shalhout_040215'
#iteration = 'Datacards_CombinationGroup_060215'
#iteration = 'ControlPlots_CombinationGroup_060215'
#iteration = 'ControlPlots_ZttMC_060215'
iteration = 'ControlPlots_ZttMC_tauPolarOn_110215'
#iteration = 'ControlPlots_CombinationGroup_ptL_hi_060215'

useEmb=1

for version in versionList:
    
    outputDir = 'Results_'+data+'_'+version+'_'+iteration
    
#QCD correction for QCD shape template -- Olivier
    ##analyze(125,"inclusive","" ,"ptL2","'#tau p_{T}'","GeV",outputDir,50,0   ,500,5.0,1.0,1,1.2  ,data,stream,version,useEmb);

## ##     ##diTauNSVfitMass
###### Datacards new categories
##     #nominal
    analyze(125,"inclusive"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
       
    analyze(125,"nobTag"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"bTag"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagLow"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"       ,"nominal" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    ##JetUp
    analyze(125,"inclusive"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
       
    analyze(125,"nobTag"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"bTag"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagLow"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"       ,"JetUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    ##JetDown
    analyze(125,"inclusive"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
       
    analyze(125,"nobTag"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"bTag"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagLow"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"       ,"JetDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    ##TauUp
    analyze(125,"inclusive"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
       
    analyze(125,"nobTag"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"bTag"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagLow"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"       ,"TauUp" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    ##TauDown
    analyze(125,"inclusive"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
       
    analyze(125,"nobTag"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagLow"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagMedium"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"nobTagHigh"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

    analyze(125,"bTag"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagLow"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
    analyze(125,"bTagHigh"       ,"TauDown" ,"diTauNSVfitMass" ,"'SVfit mass'","GeV" ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);


## ##     ##  Control plots MSSM new categories
   
    ## ## ##     ##  Control plots VisMass/SVFitMass

    ##  Visible tau mass
   
##     analyze(125,"inclusive"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTag"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTag"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagLow"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagMedium"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagHigh"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagLow"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagHigh"      ,""   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);

##     analyze(125,"inclusive"      ,"TauUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTag"      ,"TauUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTag"      ,"TauUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagLow"      ,"TauUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagMedium"      ,"TauUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagHigh"      ,"TauUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagLow"      ,"TauUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagHigh"      ,"TauUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);

##     analyze(125,"inclusive"      ,"TauDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTag"      ,"TauDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTag"      ,"TauDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagLow"      ,"TauDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagMedium"      ,"TauDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagHigh"      ,"TauDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagLow"      ,"TauDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagHigh"      ,"TauDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);

##     analyze(125,"inclusive"      ,"JetUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTag"      ,"JetUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTag"      ,"JetUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagLow"      ,"JetUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagMedium"      ,"JetUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagHigh"      ,"JetUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagLow"      ,"JetUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagHigh"      ,"JetUp"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);

##     analyze(125,"inclusive"      ,"JetDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTag"      ,"JetDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTag"      ,"JetDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagLow"      ,"JetDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagMedium"      ,"JetDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"nobTagHigh"      ,"JetDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagLow"      ,"JetDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);
##     analyze(125,"bTagHigh"      ,"JetDown"   ,"visibleTauMass","'visible #tau_{h} mass'","GeV"   ,outputDir,30,0,2,5.0,1.0,0,1.2,data, stream, version, 0);

## Decay mode
## analyze(125,"inclusive"    ,""   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);
## analyze(125,"inclusive"    ,"TauUp"   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);
## analyze(125,"inclusive"    ,"TauDown"   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);
## analyze(125,"inclusive"    ,"JetUp"   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);
## analyze(125,"inclusive"    ,"JetDown"   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);

## diTauVisMass
## analyze(125,"inclusive","" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"inclusive","TauUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"inclusive","TauDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"inclusive","JetUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"inclusive","JetDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

## analyze(125,"nobTagHigh","" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagHigh","TauUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagHigh","TauDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagHigh","JetUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagHigh","JetDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

## analyze(125,"nobTagMedium","" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagMedium","TauUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagMedium","TauDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagMedium","JetUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagMedium","JetDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

## analyze(125,"nobTagLow","" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagLow","TauUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagLow","TauDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagLow","JetUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"nobTagLow","JetDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

## analyze(125,"bTagHigh","" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"bTagHigh","TauUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"bTagHigh","TauDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"bTagHigh","JetUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"bTagHigh","JetDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);

## analyze(125,"bTagLow","" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"bTagLow","TauUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"bTagLow","TauDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"bTagLow","JetUp" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);
## analyze(125,"bTagLow","JetDown" ,"diTauVisMass","'Vis Mass'","GeV"         ,outputDir,-1,0,100,5.0,1.0,0,1.2,data,stream,version,useEmb);



##     ## diTauMasses

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
    
##     ## Other variables
    
##     analyze(125,"inclusive"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,"" ,"etaL1","'#mu #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
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
##     analyze(125,"inclusiveNoMt","" ,"MtLeg1MVA","'M_{T}(#mu#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"inclusive"    ,""   ,"ptAllB","'All b-jets p_{T}'","GeV"              ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"    ,""   ,"csvAllB","'b-discriminator'","units"            ,outputDir,40,0, 1.,5.0,1.0,0,2.,data, stream, version, useEmb);
##     analyze(125,"inclusive"    ,""   ,"ptAll","'Al jets p_{T}'","GeV"                  ,outputDir,27,20,300,5.0,1.0,0,1.2,data, stream, version, useEmb);
##     analyze(125,"inclusive"    ,""   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);

##     analyze(125,"bTagLow"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"etaL1","'#mu #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
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
##     analyze(125,"bTagLowNoMt","" ,"MtLeg1MVA","'M_{T}(#mu#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"pZetaMVA","'pZeta'","GeV"                             ,outputDir,150,-100.   ,500.,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,""   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);

##     analyze(125,"bTagHigh"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"etaL1","'#mu #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
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
##     analyze(125,"bTagHighNoMt","" ,"MtLeg1MVA","'M_{T}(#mu#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"pZetaMVA","'pZeta'","GeV"                             ,outputDir,150,-100.   ,500.,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,""   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);

##     analyze(125,"nobTagLow"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"etaL1","'#mu #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
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
##     analyze(125,"nobTagLowNoMt","" ,"MtLeg1MVA","'M_{T}(#mu#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"pZetaMVA","'pZeta'","GeV"                             ,outputDir,150,-100.   ,500.,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,""   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);

##     analyze(125,"nobTagMedium"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"etaL1","'#mu #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
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
##     analyze(125,"nobTagMediumNoMt","" ,"MtLeg1MVA","'M_{T}(#mu#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"pZetaMVA","'pZeta'","GeV"                             ,outputDir,150,-100.   ,500.,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,""   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);

##     analyze(125,"nobTagHigh"    ,"" ,"numPV","'reconstructed vertexes'","units"         ,outputDir,30,0   ,30 ,5.0,1.0,0,1.5  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"etaL1","'#mu #eta'","units"                         ,outputDir,30,-3.0,3.0,5.0,1.0,0,2.   ,data,stream,version,useEmb);
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
##     analyze(125,"nobTagHighNoMt","" ,"MtLeg1MVA","'M_{T}(#mu#nu)'","GeV"                  ,outputDir,40,0   ,160,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"pZetaMVA","'pZeta'","GeV"                             ,outputDir,150,-100.   ,500.,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,""   ,"decayMode",     "'#tau_{h} decay mode'","units" ,outputDir,5,0,5, 5.0,1.0,0,1.4,data, stream, version, useEmb);

##     analyze(125,"inclusive"    ,"" ,"ptL1","'#mu p_{T}'","GeV"                           ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"ptL1","'#mu p_{T}'","GeV"                           ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"ptL1","'#mu p_{T}'","GeV"                           ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"ptL1","'#mu p_{T}'","GeV"                           ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"ptL1","'#mu p_{T}'","GeV"                           ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"ptL1","'#mu p_{T}'","GeV"                           ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);

##     analyze(125,"inclusive"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagLow"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"bTagHigh"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagLow"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagMedium"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
##     analyze(125,"nobTagHigh"    ,"" ,"ptL2","'#tau p_{T}'","GeV"                        ,outputDir,-1,0   ,152,5.0,1.0,0,1.2  ,data,stream,version,useEmb);
