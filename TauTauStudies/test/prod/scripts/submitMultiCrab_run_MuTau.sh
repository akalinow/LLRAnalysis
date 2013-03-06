#!/bin/sh


cp ../runMuTauStreamAnalyzerFullAnalysis_Recoil_cfg.py ../runMuTauStreamAnalyzerFullAnalysis_Recoil_cfg.py.mine

for version in "06Mar2012"
do

rm ../runMuTauStreamAnalyzerFullAnalysis_Recoil_DATA_cfg.py
sed 's/runOnMC     = True/runOnMC     = False/g' ../runMuTauStreamAnalyzerFullAnalysis_Recoil_cfg.py >> ../runMuTauStreamAnalyzerFullAnalysis_Recoil_DATA_cfg.py
echo "multicrab -create -submit -cfg multicrab_run_MuTau_${version}_DATA.cfg"
multicrab -create -submit -cfg multicrab_run_MuTau_${version}_DATA.cfg

#exit

rm ../runMuTauStreamAnalyzerFullAnalysis_Recoil_MC_cfg.py
sed 's/runOnMC     = True/runOnMC     = True/g' ../runMuTauStreamAnalyzerFullAnalysis_Recoil_cfg.py >> ../runMuTauStreamAnalyzerFullAnalysis_Recoil_MC_cfg.py
echo "multicrab -create -submit -cfg multicrab_run_MuTau_${version}_MC.cfg"
multicrab -create -submit -cfg multicrab_run_MuTau_${version}_MC.cfg

done