#! /bin/sh

#for mass in  110 115 120 125 130 135 140 145
for mass in 140

do

echo "> mH = ${mass} GeV"


echo ">> ${met}"
 
combineCards.py Name1=eTau_SM0_mH${mass}_diTauSVFitMass.txt Name2=eTau_SM1_mH${mass}_diTauSVFitMass.txt  Name3=eTau_SM2_mH${mass}_diTauSVFitMass.txt  > eTau_SM_mH${mass}_diTauSVFitMass.txt 

echo " ---- Now doing combine SM0"

#combine -M Asymptotic eTau_SM0_mH${mass}_diTauSVFitMass.txt --rMin 5 --rMax 30 -H ProfileLikelihood | grep "50.0%" >> combine_mH${mass}_SM0.log
combine -M Asymptotic eTau_SM0_mH${mass}_diTauSVFitMass.txt --rMin 5 --rMax 30   -m ${mass} -n ElecTauSM0

echo " ---- Now doing combine SM1"

#combine -M Asymptotic eTau_SM1_mH${mass}_diTauSVFitMass.txt --rMin 4 --rMax 20 -H ProfileLikelihood -t -1 | grep "50.0%" >> combine_mH${mass}_SM1.log
#combine -M Asymptotic eTau_SM1_mH${mass}_diTauSVFitMass.txt -m ${mass} -n ElecTauSM1

echo " ---- Now doing combine SM2"

#combine -M Asymptotic eTau_SM2_mH${mass}_diTauSVFitMass.txt --rMin 3 --rMax 10 -H ProfileLikelihood -t -1  | grep "50.0%" >> combine_mH${mass}_SM2.log
#combine -M Asymptotic eTau_SM2_mH${mass}_diTauSVFitMass.txt -m ${mass} -n ElecTauSM2

echo " ---- Now doing combine SM"

#combine -M Asymptotic eTau_SM_mH${mass}_diTauSVFitMass.txt --rMin 2 --rMax 10 -H ProfileLikelihood -t -1  | grep "50.0%" >> combine_mH${mass}_SM.log
#combine -M Asymptotic eTau_SM_mH${mass}_diTauSVFitMass.txt -m ${mass} -n ElecTauSM


done