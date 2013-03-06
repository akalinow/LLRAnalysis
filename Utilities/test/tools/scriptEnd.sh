# check if script execution is finished

v_launch=$(grep "launch script" */log/treat.txt  | wc | awk '{print $1}')
v_end=$(   grep "end script"    */log/treat.txt  | wc | awk '{print $1}')

grep "launch script" */log/treat.txt | awk -F "/" '{print $1}' > launch.tmp
grep "end script"    */log/treat.txt | awk -F "/" '{print $1}' > end.tmp

echo "############################"
echo "## launched "$v_launch" scripts    ##"
echo "## finished "$v_end" scripts    ##"
echo "## differences :          ##"
diff launch.tmp end.tmp | grep "HTT_" | awk '{print "## "$2" ##"}'
echo "############################"

v_sub_succ=$(grep "jobs submitted" */log/treat.txt  | wc | awk '{print $1}')
v_sub_fail=$(grep "not submitted"  */log/treat.txt  | wc | awk '{print $1}')

echo "## Submission : launched"
diff launch.tmp end.tmp | grep HTT | awk '{print "grep submit treat_"$2".sh | awk \047{print $3}\047"}' > isSubmitTMP.sh
chmod u+x isSubmitTMP.sh
./isSubmitTMP.sh
rm isSubmitTMP.sh
echo "############################"

echo "## Submission : "$v_sub_succ" successful"
grep "jobs submitted" */log/treat.txt | awk -F "/" '{print $1}'
echo "############################"

echo "## Submission : "$v_sub_fail" failed"
grep "not submitted" */log/treat.txt  | awk -F "/" '{print $1}'
echo "############################"

echo "## Published datasets :"
grep "in dataset" */log/publish.txt | awk -F ":" '{print $2$3$4}'
grep "in dataset" */log/publish.txt | awk -F ":" '{print $2$3$4}' > log_published.txt
grep "in dataset" */log/publish.txt | awk -F ":" '{print $4}' > list_published.txt
echo "############################"

rm launch.tmp end.tmp