## Launch automatically commands for limits computation
## chan : channel = et / mt
## dir  : source directory in auxiliaries/datacards/collected/ (add this as a label in doSM.py) e.g. "LIMITS_LLR"
## tag  : name of the directory for future hadding (limit comparison) e.g. "moriond"

chan=$1
dir=$2
tag=$3

echo "Install directories + add bin-by-bin uncertainties"
python HiggsAnalysis/HiggsToTauTau/scripts/doSM.py -a bin-by-bin -c $1 -p 8TeV --update-all --inputs-$1=$2 --label $2 90-160:5 

echo "Compute the pulls"
for((i=90;i<=160;i+=5)) do limit.py --stable --max-likelihood LIMITS_$2/bin-by-bin/$1/$i ; done 

echo "Generate and launch jobs to compute the limits"
submit.py --asymptotic LIMITS_$2/bin-by-bin/$1/* --llr --jobdir LIMITS_$2
cd LIMITS_$2/bin-by-bin-$1/
./bin-by-bin-$1_submit.sh
cd ../..

echo "Plot the limits."
echo "cp -r LIMITS_"$2"/bin-by-bin/"$1"/ LIMITS_"$2"/bin-by-bin/"$1"_"$3" ; plot --asymptotic HiggsAnalysis/HiggsToTauTau/python/layouts/limit-sm.py LIMITS_"$2"/bin-by-bin/"$1"_"$3" ; mkdir LIMITS_"$2"/bin-by-bin/"$1"_"$3"/results ; mv "$1"_"$3"_limit.* LIMITS_"$2"/bin-by-bin/"$1"_"$3"/results" > plot_$1_$3.sh
chmod u+x plot_$1_$3.sh
echo "Wait for your T3 jobs to be finished, then launch plot_"$1"_"$3".sh to plot the limits."

