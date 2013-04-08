# checkValidityFiles.sh
# arg 1 : directory e.g. /data_CMS/cms/htautau/PostMoriond/NTUPLES/EleTau/temp/SoftAnalysis/
# arg 2 : tag (EleTau / MuTau)

python checkValidityFiles.py $1 &> log_check_$2.txt ; grep "not closed" log_check_$2.txt | awk '{print "rm "$4}' > batch/erase_$2.sh ; chmod u+x batch/erase_$2.sh

python checkValidityFiles.py $1 &> log_check_$2.txt ; grep "not closed" log_check_$2.txt | awk '{print $4}' | awk -F "/nTuple" '{print $2}' | awk -F "." '{print "/opt/exp_soft/cms/t3/t3submit -V job_"$1".sh"}' > batch/resubmit_$2.sh ; chmod u+x batch/resubmit_$2.sh
