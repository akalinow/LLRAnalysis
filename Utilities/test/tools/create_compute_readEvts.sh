ls | grep "HTT" | awk '{print "grep \042TrigReport Events total\042 "$1"/res/CMSSW_*.stdout | awk \047{t+=$5 ; u+=$8 ; v+=$11}END{if(t!=0) print \042"$1" : \042t\042 \042u\042 \042v\042 \042u/t}\047"}' > compute_readEvts.sh
grep "HTT" compute_readEvts.sh > compute_readEvts_.sh
mv compute_readEvts_.sh compute_readEvts.sh
chmod u+x compute_readEvts.sh
./compute_readEvts.sh > log_readEvts.txt
