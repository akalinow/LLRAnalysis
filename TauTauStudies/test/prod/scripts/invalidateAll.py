#!/bin/csh -f

set dataSets=(`dbs search --url=http://cmsdbsprod.cern.ch/cms_dbs_ph_analysis_01/servlet/DBSServlet --query="find dataset where dataset like *bianchi* and dataset.status=VALID" | grep USER | grep $1`)
foreach  dataSet (${dataSets})
    echo "DBSInvalidateDataset.py --DBSURL=https://cmsdbsprod.cern.ch:8443/cms_dbs_ph_analysis_01_writer/servlet/DBSServlet  --datasetPath=${dataSet} --files"
end
