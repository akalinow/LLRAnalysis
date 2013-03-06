ls HTT_* | awk -F ":" '{print "./summarizeStatus.sh "$1" no"}' | grep "HTT" > sumAll.sh
ls HTT_* | awk -F ":" '{print "nohup ./summarizeStatus.sh "$1" up > "$1"/log/launch_sum.txt &"}' | grep "HTT" > sumAllUp.sh
chmod u+x sumAll.sh sumAllUp.sh
