#! /bin/sh

for mass in  110 115 120 125 130 135 140 145
#for mass in 120

do

echo "> mH = ${mass} GeV"


echo ">> ${met}"
 
combineCards.py Name1=muTau_SM0_mH${mass}_diTauSVFitMass.txt Name2=muTau_SM1_mH${mass}_diTauSVFitMass.txt  Name3=muTau_SM2_mH${mass}_diTauSVFitMass.txt  > muTau_SM_mH${mass}_diTauSVFitMass.txt 
combineCards.py Name1=eTau_SM0_mH${mass}_diTauSVFitMass.txt Name2=eTau_SM1_mH${mass}_diTauSVFitMass.txt  Name3=eTau_SM2_mH${mass}_diTauSVFitMass.txt  > eTau_SM_mH${mass}_diTauSVFitMass.txt 
combineCards.py Name1=muTau_SM_mH${mass}_diTauSVFitMass.txt Name2=eTau_SM_mH${mass}_diTauSVFitMass.txt > Combined_SM_mH${mass}_diTauSVFitMass.txt


echo " ---- Now doing combine SM"

combine -M Asymptotic Combined_SM_mH${mass}_diTauSVFitMass.txt -m ${mass} -n CombinedSM


done