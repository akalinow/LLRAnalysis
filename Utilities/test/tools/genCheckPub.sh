grep USER list_published.txt | awk -F "-" '{print "echo "$1"-"$2"-"$3" ; grep Total "$2"/log/status.txt ; dbs search --url=http://cmsdbsprod.cern.ch/cms_dbs_ph_analysis_01/servlet/DBSServlet --query=\042find file where dataset="$1"-"$2"-"$3"\042 | grep root | wc | awk \047{print \042Published : \042$1\042 files\042}\047  "}'  > checkPublish.sh

chmod u+x checkPublish.sh