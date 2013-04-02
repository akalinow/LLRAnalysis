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
for((i=90;i<=160;i+=5)) do limit.py --stable --max-likelihood $2/bin-by-bin/$1/$i ; done 

echo "generate and launch jobs to compute the limits"
submit.py --asymptotic $2/bin-by-bin/$1/* --llr --jobdir $2
cd $2/bin-by-bin-$1/
./bin-by-bin-$1_submit.sh
cd ../..

echo "prepare the limits comparison"
cp -r $2/bin-by-bin/$1/ $2/bin-by-bin/$1_$3

echo "plot the limits"
plot --asymptotic HiggsAnalysis/HiggsToTauTau/python/layouts/limit-sm.py $2/bin-by-bin/$1_$3
mkdir $2/bin-by-bin/$1_$3/results
mv $1_$3_limit.* $2/bin-by-bin/$1_$3/results

