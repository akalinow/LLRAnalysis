#!/bin/sh

#AK test2

export WORKINGDIR="/home/llr/cms/lbianchini/CMSSW_5_2_2/src/LLRAnalysis/TauTauStudies/test/"
export SCRAM_ARCH=slc5_amd64_gcc462
source /opt/exp_soft/cms/cmsset_default.sh
cd $WORKINGDIR
eval `scram runtime -sh`
source /opt/exp_soft/cms/cms_ui_env_crab.sh
cmsRun runElecTauStreamAnalyzerFullAnalysis_Recoil_cfg.py
