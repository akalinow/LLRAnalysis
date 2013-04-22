echo "Copie : "$1" dans : "$2
rfdir $1 | awk '{print "mkdir "dir"/"$9" ; ./dorfcp_noDoublon.sh "dpm"/"$9"/ "dir"/"$9"/"}' dpm=$1 dir=$2 > copy_$3.sh
chmod u+x copy_$3.sh
./copy_$3.sh > do_copy_$3.sh
chmod u+x do_copy_$3.sh
ls | grep HTT | awk '{print "cp checkPresence.sh "$1" ; cd "$1" ; echo "$1" ; ./checkPresence.sh | grep \042such\042 ; cd .."}' > docheck.sh 
chmod u+x docheck.sh 
