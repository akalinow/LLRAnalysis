echo "Copie : "$1" dans : "$2

# Generates script able to create all sub-directories and a copy.sh for each
rfdir $1 | awk '{print "mkdir "dir"/"$9" ; ./dorfcp_noDoublon.sh "dpm"/"$9"/ "dir"/"$9"/"}' dpm=$1 dir=$2 > copy_$3.sh
chmod u+x copy_$3.sh

# Create all sub-directories and a copy.sh for each
./copy_$3.sh

# Generates script able to launch the copy : modify it by hand (remove the first "cd ..")
ls | grep HTT | awk '{print "cd .. ; cd "$1" ; nohup ./copy.sh &"}' > launch.sh
chmod u+x launch.sh

# Generates script able to copy checkPresence.sh in each sub-directory and to execute it to spot files not copied wrt copy.sh
ls | grep HTT | awk '{print "cp checkPresence.sh "$1" ; cd "$1" ; echo "$1" ; ./checkPresence.sh | grep \042such\042 ; cd .."}' > docheck.sh 
chmod u+x docheck.sh 
