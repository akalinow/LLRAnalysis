#! /bin/sh

for mass in  105 120 140

do

echo "> mH = ${mass} GeV"


echo ">> ${met}"
 
combineCards.py Name1=eTau_SM0_mH${mass}_diTauSVFitMass.txt Name2=eTau_SM1_mH${mass}_diTauSVFitMass.txt Name3=eTau_SM2_mH${mass}_diTauSVFitMass.txt > eTau_SM_mH${mass}_diTauSVFitMass.txt

echo " ---- Now doing combine SM0"

combine -M Asymptotic eTau_SM0_mH${mass}_diTauSVFitMass.txt -H ProfileLikelihood -t -1 | grep "50.0%" >> combine_mH${mass}_SM0.log

echo " ---- Now doing combine SM1"

combine -M Asymptotic eTau_SM1_mH${mass}_diTauSVFitMass.txt -H ProfileLikelihood -t -1 | grep "50.0%" >> combine_mH${mass}_SM1.log

echo " ---- Now doing combine SM2"

combine -M Asymptotic eTau_SM2_mH${mass}_diTauSVFitMass.txt -H ProfileLikelihood -t -1  | grep "50.0%" >> combine_mH${mass}_SM2.log

echo " ---- Now doing combine SM"

combine -M Asymptotic eTau_SM_mH${mass}_diTauSVFitMass.txt -H ProfileLikelihood -t -1  | grep "50.0%" >> combine_mH${mass}_SM.log


done