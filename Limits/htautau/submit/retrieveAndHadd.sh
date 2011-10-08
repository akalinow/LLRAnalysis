#! /bin/sh

mkdir grid

for mass in  105 110 115 120 125 130 135 140

do

echo ">>>>>>> Now doing eleTau"

#crab -status -c crab_eleTau_SM_diTauNSVfitMass_mH${mass}
#crab -getoutput -c crab_eleTau_SM_diTauNSVfitMass_mH${mass}
#hadd -f crab_eleTau_SM_diTauNSVfitMass_mH${mass}/res/eleTau_SM_diTauNSVfitMass_mH${mass}.root crab_eleTau_SM_diTauNSVfitMass_mH${mass}/res/eleTau_SM_diTauNSVfitMass_mH${mass}_*.root
#cp crab_eleTau_SM_diTauNSVfitMass_mH${mass}/res/eleTau_SM_diTauNSVfitMass_mH${mass}.root grid/

#crab -status -c crab_eleTau_SM_diTauVisMass_mH${mass}
#crab -getoutput -c crab_eleTau_SM_diTauVisMass_mH${mass}
#hadd -f crab_eleTau_SM_diTauVisMass_mH${mass}/res/eleTau_SM_diTauVisMass_mH${mass}.root crab_eleTau_SM_diTauVisMass_mH${mass}/res/eleTau_SM_diTauVisMass_mH${mass}_*.root
#cp crab_eleTau_SM_diTauVisMass_mH${mass}/res/eleTau_SM_diTauVisMass_mH${mass}.root grid/

echo ">>>>>>> Now doing muTau"

crab -status -c crab_muTau_SM_diTauNSVfitMass_mH${mass}
crab -getoutput -c crab_muTau_SM_diTauNSVfitMass_mH${mass}
hadd -f crab_muTau_SM_diTauNSVfitMass_mH${mass}/res/muTau_SM_diTauNSVfitMass_mH${mass}.root crab_muTau_SM_diTauNSVfitMass_mH${mass}/res/muTau_SM_diTauNSVfitMass_mH${mass}_*.root
cp crab_muTau_SM_diTauNSVfitMass_mH${mass}/res/muTau_SM_diTauNSVfitMass_mH${mass}.root grid/

crab -status -c crab_muTau_SM_diTauVisMass_mH${mass}
crab -getoutput -c crab_muTau_SM_diTauVisMass_mH${mass}
hadd -f crab_muTau_SM_diTauVisMass_mH${mass}/res/muTau_SM_diTauVisMass_mH${mass}.root crab_muTau_SM_diTauVisMass_mH${mass}/res/muTau_SM_diTauVisMass_mH${mass}_*.root
cp crab_muTau_SM_diTauVisMass_mH${mass}/res/muTau_SM_diTauVisMass_mH${mass}.root grid/

done