chan=$1
task=$2

grep "file =  tree"$chan"Stream_" $task/res/CMSSW_*.stdout | awk '{print $4}' > list_$1_$2.txt

