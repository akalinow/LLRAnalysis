#!/bin/sh

export WORKINGDIR="/home/llr/cms/lbianchini/CMSSW_3_8_6_patch1/src/LLRAnalysis/TauTauStudies/test"
source /opt/exp_soft/cms/cmsset_default.sh
cd $WORKINGDIR
eval `scram runtime -sh`
source /opt/exp_soft/cms/cms_ui_env_crab.sh
cmsRun test_cfg.py
