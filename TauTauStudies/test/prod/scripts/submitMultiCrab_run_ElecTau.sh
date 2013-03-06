#!/bin/sh


cp ../runElecTauStreamAnalyzerFullAnalysis_Recoil_cfg.py ../runElecTauStreamAnalyzerFullAnalysis_Recoil_cfg.py.mine

for version in "06Mar2012"
do

rm  ../runElecTauStreamAnalyzerFullAnalysis_Recoil_DATA_cfg.py
sed 's/runOnMC     = True/runOnMC     = False/g' ../runElecTauStreamAnalyzerFullAnalysis_Recoil_cfg.py >> ../runElecTauStreamAnalyzerFullAnalysis_Recoil_DATA_cfg.py
echo "multicrab -create -submit -cfg multicrab_run_ElecTau_${version}_DATA.cfg"
multicrab -create -submit -cfg multicrab_run_ElecTau_${version}_DATA.cfg

#exit

rm ../runElecTauStreamAnalyzerFullAnalysis_Recoil_MC_cfg.py
sed 's/runOnMC     = True/runOnMC     = True/g' ../runElecTauStreamAnalyzerFullAnalysis_Recoil_cfg.py >> ../runElecTauStreamAnalyzerFullAnalysis_Recoil_MC_cfg.py
echo "multicrab -create -submit -cfg multicrab_run_ElecTau_${version}_MC.cfg"
multicrab -create -submit -cfg multicrab_run_ElecTau_${version}_MC.cfg

done