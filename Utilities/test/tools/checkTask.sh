cp $1/log/status.txt    $1/log/status.old
cp $1/log/treat.txt     $1/log/treat.old

echo "Check Status of task : "$1
crab -c $1 -status > $1/log/status.txt

echo "Generate script to treat the task"
python checkTask.py $1/log/status.txt $1 $2 $3 > treat_$1.sh
chmod u+x treat_$1.sh

echo "Launch script"
more treat_$1.sh
./treat_$1.sh > $1/log/treat.txt
echo "end script"

echo "Check Status of task : "$1
crab -c $1 -status > $1/log/status.txt
tail -20 $1/log/status.txt

cp $1/log/status.txt    $1/log/status.new
cp $1/log/treat.txt     $1/log/treat.new

echo "End"
