#! /bin/sh

echo "Clean the area"

rm eleTau_SM_*
rm muTau_SM_*
rm Comb_SM_*
rm *txt
rm *root

for mass in  105 110 115 120 125 130 135 140

do

echo ">>>>>>> Now doing eleTau"

cp ../datacards/eleTauSM_diTauNSVfitMass.root ./
cp ../datacards/eTau_SM0_mH${mass}_diTauNSVfitMass.txt ./
cp ../datacards/eTau_SM2_mH${mass}_diTauNSVfitMass.txt ./
combineCards.py Name1=eTau_SM0_mH${mass}_diTauNSVfitMass.txt Name2=eTau_SM2_mH${mass}_diTauNSVfitMass.txt  > eTau_SM_mH${mass}_diTauNSVfitMass.txt
python makeGridUsingCrab.py eTau_SM_mH${mass}_diTauNSVfitMass.txt 0.5 80 -n 160 -I 10 -T 500 -r -j 100 -o eleTau_SM_diTauNSVfitMass_mH${mass}
crab -create -submit -cfg eleTau_SM_diTauNSVfitMass_mH${mass}.cfg -USER.ui_working_dir=crab_eleTau_SM_diTauNSVfitMass_mH${mass} -CMSSW.total_number_of_events=100

cp ../datacards/eleTauSM_diTauVisMass.root ./
cp ../datacards/eTau_SM0_mH${mass}_diTauVisMass.txt ./
cp ../datacards/eTau_SM2_mH${mass}_diTauVisMass.txt ./
combineCards.py Name1=eTau_SM0_mH${mass}_diTauVisMass.txt Name2=eTau_SM2_mH${mass}_diTauVisMass.txt  > eTau_SM_mH${mass}_diTauVisMass.txt
python makeGridUsingCrab.py eTau_SM_mH${mass}_diTauVisMass.txt  0.5 80 -n 160 -I 10 -T 500 -r -j 100 -o eleTau_SM_diTauVisMass_mH${mass}
crab -create -submit -cfg eleTau_SM_diTauVisMass_mH${mass}.cfg -USER.ui_working_dir=crab_eleTau_SM_diTauVisMass_mH${mass} -CMSSW.total_number_of_events=100

echo ">>>>>>> Now doing muTau"

cp ../datacards/muTauSM_diTauNSVfitMass.root ./
cp ../datacards/muTau_SM0_mH${mass}_diTauNSVfitMass.txt ./
cp ../datacards/muTau_SM2_mH${mass}_diTauNSVfitMass.txt ./
combineCards.py Name1=muTau_SM0_mH${mass}_diTauNSVfitMass.txt Name2=muTau_SM2_mH${mass}_diTauNSVfitMass.txt  > muTau_SM_mH${mass}_diTauNSVfitMass.txt
python makeGridUsingCrab.py muTau_SM_mH${mass}_diTauNSVfitMass.txt 0.5 60. -n 120 -I 10 -T 500 -r -j 100 -o muTau_SM_diTauNSVfitMass_mH${mass}
crab -create -submit -cfg muTau_SM_diTauNSVfitMass_mH${mass}.cfg -USER.ui_working_dir=crab_muTau_SM_diTauNSVfitMass_mH${mass} -CMSSW.total_number_of_events=100

cp ../datacards/muTauSM_diTauVisMass.root ./
cp ../datacards/muTau_SM0_mH${mass}_diTauVisMass.txt ./
cp ../datacards/muTau_SM2_mH${mass}_diTauVisMass.txt ./
combineCards.py Name1=muTau_SM0_mH${mass}_diTauVisMass.txt Name2=muTau_SM2_mH${mass}_diTauVisMass.txt  > muTau_SM_mH${mass}_diTauVisMass.txt
python makeGridUsingCrab.py muTau_SM_mH${mass}_diTauVisMass.txt  0.5 60. -n 120 -I 10 -T 500 -r -j 100 -o muTau_SM_diTauVisMass_mH${mass}
crab -create -submit -cfg muTau_SM_diTauVisMass_mH${mass}.cfg -USER.ui_working_dir=crab_muTau_SM_diTauVisMass_mH${mass} -CMSSW.total_number_of_events=100

done
