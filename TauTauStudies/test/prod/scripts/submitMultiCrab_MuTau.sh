#!/bin/sh

#back up
cp ../runMuTauStreamAnalyzerFullAnalysis_A_cfg.py ../runMuTauStreamAnalyzerFullAnalysis_A_cfg.py.mine

sed 's/runOnMC     = True/runOnMC     = False/g' ../runMuTauStreamAnalyzerFullAnalysis_A_cfg.py
echo 'multicrab -create -submit -cfg multicrab_run_MuTau_13Oct2011_DATA.cfg'
#multicrab -create -submit -cfg multicrab_run_MuTau_13Oct2011_DATA.cfg
sed 's/runOnMC     = False/runOnMC     = True/g' ../runMuTauStreamAnalyzerFullAnalysis_A_cfg.py
echo 'multicrab -create -submit -cfg multicrab_run_MuTau_13Oct2011_MC.cfg'
#multicrab -create -submit -cfg multicrab_run_MuTau_13Oct2011_MC.cfg
