#!/bin/sh

#back up
cp ../runElecTauStreamAnalyzerFullAnalysis_Recoil_cfg.py ../runElecTauStreamAnalyzerFullAnalysis_Recoil_cfg.py.mine

sed 's/runOnMC     = True/runOnMC     = False/g' ../runElecTauStreamAnalyzerFullAnalysis_Recoil_cfg.py >> ../runElecTauStreamAnalyzerFullAnalysis_Recoil_DATA_cfg.py
echo 'multicrab -create -submit -cfg multicrab_run_ElecTau_08Nov2011_DATA.cfg'
multicrab -create -submit -cfg multicrab_run_ElecTau_08Nov2011_DATA.cfg

sed 's/runOnMC     = True/runOnMC     = True/g' ../runElecTauStreamAnalyzerFullAnalysis_Recoil_cfg.py >> ../runElecTauStreamAnalyzerFullAnalysis_Recoil_MC_cfg.py
echo 'multicrab -create -submit -cfg multicrab_run_ElecTau_08Nov2011_MC.cfg'
multicrab -create -submit -cfg multicrab_run_ElecTau_08Nov2011_MC.cfg
