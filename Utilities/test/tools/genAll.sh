ls *HTT* | awk -F ":" '{print "nohup ./checkTask.sh  "$1" noforce  pub > "$1"/log/launcher.txt &"}'  | grep "HTT" > checkAll.sh
ls *HTT* | awk -F ":" '{print "nohup ./checkTask.sh  "$1" force  pub > "$1"/log/launcher.txt &"}'    | grep "HTT" > checkAllForce.sh
ls *HTT* | awk -F ":" '{print "nohup ./checkTask.sh  "$1" forceAll  pub > "$1"/log/launcher.txt &"}' | grep "HTT" > checkAllForceAll.sh
chmod u+x checkAll*.sh 
