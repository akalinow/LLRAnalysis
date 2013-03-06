#!/bin/sh

#back up
cp ../patTuple_PAT_SkimElecTauStream_cfg.py ../patTuple_PAT_SkimElecTauStream_cfg.py.mine

for version in "30Mar2012"
do

rm ../patTuple_PAT_SkimElecTauStream_DATA_cfg.py
sed 's/runOnMC     = True/runOnMC     = False/g' ../patTuple_PAT_SkimElecTauStream_cfg.py >> ../patTuple_PAT_SkimElecTauStream_DATA_cfg.py
echo 'multicrab -create -submit -cfg multicrab_skim_ElecTau_${version}_DATA.cfg'
multicrab -create -submit -cfg multicrab_skim_ElecTau_${version}_DATA.cfg

rm ../patTuple_PAT_SkimElecTauStream_MC_cfg.py
sed 's/runOnMC     = True/runOnMC     = True/g' ../patTuple_PAT_SkimElecTauStream_cfg.py >> ../patTuple_PAT_SkimElecTauStream_MC_cfg.py
echo 'multicrab -create -submit -cfg multicrab_skim_ElecTau_${version}_MC.cfg'
multicrab -create -submit -cfg multicrab_skim_ElecTau_${version}_MC.cfg
