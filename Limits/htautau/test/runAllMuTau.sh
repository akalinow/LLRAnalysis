#! /bin/sh

#for mass in  110 115 120 125 130 135 140 145
for mass in 115

do

echo "> mH = ${mass} GeV"


echo ">> ${met}"
 
combineCards.py Name1=muTau_SM0_mH${mass}_diTauSVFitMass.txt Name2=muTau_SM1_mH${mass}_diTauSVFitMass.txt  Name3=muTau_SM2_mH${mass}_diTauSVFitMass.txt  > muTau_SM_mH${mass}_diTauSVFitMass.txt 

echo " ---- Now doing combine SM0"

#combine -M Asymptotic muTau_SM0_mH${mass}_diTauSVFitMass.txt --rMin 5 --rMax 30 -H ProfileLikelihood | grep "50.0%" >> combine_mH${mass}_SM0.log
#combine -M Asymptotic muTau_SM0_mH${mass}_diTauSVFitMass.txt -m ${mass} -n MuTauSM0 -H ProfileLikelihood

echo " ---- Now doing combine SM1"

#combine -M Asymptotic muTau_SM1_mH${mass}_diTauSVFitMass.txt --rMin 4 --rMax 20 -H ProfileLikelihood -t -1 | grep "50.0%" >> combine_mH${mass}_SM1.log
#combine -M Asymptotic muTau_SM1_mH${mass}_diTauSVFitMass.txt -m ${mass} -n MuTauSM1

echo " ---- Now doing combine SM2"

#combine -M Asymptotic muTau_SM2_mH${mass}_diTauSVFitMass.txt --rMin 3 --rMax 10 -H ProfileLikelihood -t -1  | grep "50.0%" >> combine_mH${mass}_SM2.log
#combine -M Asymptotic muTau_SM2_mH${mass}_diTauSVFitMass.txt -m ${mass} -n MuTauSM2

echo " ---- Now doing combine SM"

#combine -M Asymptotic muTau_SM_mH${mass}_diTauSVFitMass.txt --rMin 2 --rMax 10 -H ProfileLikelihood -t -1  | grep "50.0%" >> combine_mH${mass}_SM.log
#combine -M Asymptotic muTau_SM_mH${mass}_diTauSVFitMass.txt -m ${mass} -n MuTauSM


done