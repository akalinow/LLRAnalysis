list=$1
dpm=$2
dir=$3
task=$4

mkdir $dir/$task

grep tree $list | awk '{print "rfcp "dpm"/"$1" "dir"/"task}' dpm=$dpm dir=$dir task=$task > $dir/$task/copy.sh

chmod +x $dir/$task/copy.sh

## EXAMPLE
## ./copyFromList.sh list_MuTau_DYJets-50-madgraph-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v5.txt /dpm/in2p3.fr/home/cms/trivcat/store/user/lmastrol/ /data_CMS/cms/htautau/PostMoriond/TREES_JetIdFix/MuTau/ DYJets-50-madgraph-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v5