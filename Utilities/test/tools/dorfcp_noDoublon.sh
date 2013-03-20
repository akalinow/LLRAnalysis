echo Copy $1 into $2

rfdir $1 > $2/list_dpm.txt
python ~/LookDPM/keepLastDoublon.py $2/list_dpm.txt 1 8 $1/ $2/ rfcp > $2/copy.sh
chmod u+x $2/copy.sh
#nohup ./$2/copy.sh > $2/log_copy.txt &
