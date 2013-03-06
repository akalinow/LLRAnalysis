if [ $2 = "up" ]
then
    echo "Update status"
    crab -c $1 -status > $1/log/status.txt
fi

python summarizeStatus.py $1/log/status.txt $1

exit 0