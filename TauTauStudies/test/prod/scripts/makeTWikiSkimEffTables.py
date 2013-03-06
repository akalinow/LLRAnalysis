#!/usr/bin/env python

import sys, re, os
    
##############
def getEffFromTrigReport(selectionPath,skimType,fileList,opt):   
    ### Init counters ###
    filters = ["pEleTau1","pEleTau2","pMuTau1","pMuTau2"]
    NBeforePath = [0] * len(filters)
    NPassedPath = [0] * len(filters)
    NEvents = 0
    NPassedAll = 0
    #########################
    fullEntryPatternPath0 = re.compile('TrigReport ---------- Event  Summary ------------(\nTrigReport [\W\w]+?)\n')
    #ppat0 = 'Event  Summary [\W\w]+ '
    #########################
    fullEntryPatternPath = re.compile('TrigReport ---------- Path   Summary ------------(\nTrigReport [\ \# \w]+)+')
    #ppat = 'Path   Summary [\W\w]+ '
    ########################
    # Loop over all files
    if(len(fileList)<1):
        print "No crab output files present!"
        return
    for filen in fileList:
        input = file(filen)
        fullTxt = input.read() # read full txt
        #############################################
        #####Cumulative efficiency
        m0 = fullEntryPatternPath0.search(fullTxt) # extract region
        if (m0):
            lines = re.split('\n',m0.group())
            for line in lines:
                if(len(line.split())==11):
                    NEvents += int(line.split()[4])
                    NPassedAll += int(line.split()[7])
        ####Efficiency for each filter       
        m1 = fullEntryPatternPath.search(fullTxt) # extract region
        if (m1):
            lines = re.split('\n',m1.group())
            for line in lines:
                #print line
                i = 0
                for filter in filters:
                    if line.find(filter)!=-1:
                        NBeforePath[i] = NBeforePath[i] + int(line.split()[3])
                        NPassedPath[i] = NPassedPath[i] + int(line.split()[4])
                    i=i+1
        #############################################                        
        input.close()
#####
    #NEvents = NBeforePath[0] #This is taken from Event summary block
    #NPassedAll = NPassedPath[len(filters)-1] #This is taken from Event summary block
    outputString = ""
    if(opt==0):
        outputString += str(NEvents)+"</td> <td>"+str(NPassedAll)+"</td> <td>"+'%f'%(float(NPassedAll)/NEvents)+"</td>"
        return outputString

    outputString += "       "+selectionPath+" Efficiencies"
    outputString += "<table border=\"1\" bgcolor=\"black\"> <tbody> "
    outputString += "<tr> "
    outputString +="<td>Path</td> <td>Passed</td> <td>Efficiency</td> "
    outputString += "</tr> "
    eff = -1.
    # Treat first entry
    if (float(NEvents!=0)):
        eff = float(NPassedPath[0])/float(NEvents)
    print '%24s: %8d     %5.3f       %5.3f'%(filters[0],NPassedPath[0],eff,float(NPassedPath[0])/float(NEvents))
    outputString += "<tr> "
    outputString+="<td>"+filters[0]+"</td> <td>"+str(NPassedPath[0])+"</td> <td>"+'%5.3f'%(eff)+"</td>"
    outputString += "</tr> "    
    for i in range(1,len(filters)):
        if (float(NPassedPath[i-1]==0)):
            eff = -1.
        else:
            eff = float(NPassedPath[i])/float(NBeforePath[i])
        print '%24s: %8d     %5.3f       %5.3f'%(filters[i],NPassedPath[i],eff,float(NPassedPath[i])/float(NEvents))
        outputString += "<tr> "
        outputString+="<td>"+filters[i]+"</td> <td>"+str(NPassedPath[i])+"</td> <td>"+'%5.3f'%(eff)+"</td> "
        outputString += "</tr> "
    print '---------------------------------------------------------------'
    last = len(filters)-1
    print '\n*** Total events processed/passed: %8d %8d'%(NEvents,NPassedAll)

    outputString +="</tbody> </table>"
    return outputString
#####
    
    exit
##################
##################
def getReportForSample(path,skimType,crabDirPattern=""):
    if crabDirPattern=="":
        crabDirPattern="crab_0_"
    print "Looking for crab dirs like \'"+crabDirPattern+"\'"
    for crabDir in os.listdir(path):
        if not os.path.isdir(crabDir):
            continue
        if re.search(crabDirPattern,crabDir)!=None:
            print "----------------------------------------------"
            print "Checking: ",crabDir
        ##############            
            crabLogPath = path+crabDir+"/log/crab.log"
            crabLog = open(crabLogPath,"r")
            publishName=""
            dashboardLink=""
            for line in crabLog:
                #if re.search("<User Dataset Name>",line)!=None and re.search(skimType,line)!=None:
                if re.search("<User Dataset Name>",line)!=None:
                    publishName = line.split("=")[1].rstrip("\n")
                if re.search("CMS Dashboard:",line)!=None:
                    dashboardLink = line.split(" ")[2].rstrip("\n")
            crabCfgPath = path+crabDir+"/share/crab.cfg"
            crabCfg = open(crabCfgPath,"r")
            pset = ""
            datasetpath = ""
            storage_element = ""
            publish_data_name = ""
            for line in crabCfg:
                if re.search("pset",line)!=None:
                    pset = line.split("=")[1].rstrip("\n")
                if re.search("datasetpath",line)!=None:
                    datasetpath = line.split("=")[1].rstrip("\n")
                if re.search("storage_element",line)!=None:
                    storage_element = line.split("=")[1].rstrip("\n")
                if re.search("publish_data_name",line)!=None:
                    publish_data_name = line.split("=")[1].rstrip("\n")
                if re.search("dbs_url_for_publication",line)!=None:
                    tmp = line.split("=")[1].rstrip("\n")
                    dbsInst = tmp.split("/")[3]
                    tmp1 = dbsInst[:dbsInst.rfind("_writer")]
                    dbsInst = tmp1
            print "datasetpath: ",datasetpath," pset: ",pset," SE: ",storage_element
            print "publish_data_name: ",publish_data_name
            dbsDatasetName="/"+datasetpath.split("/")[1]+"/akalinow-"+publish_data_name
            print "DBS Instance: ",dbsInst
            print "DBS Dataset name: ",dbsDatasetName
            print "DBS Link: ","https://cmsweb.cern.ch/dbs_discovery/aSearch?caseSensitive=on&userMode=user&sortOrder=desc&sortName=&grid=1&method=dbsapi&dbsInst="+dbsInst+"&userInput=find+dataset+where+dataset+like+*"+dbsDatasetName+"*"+skimType+"*+and+dataset.status+like+VALID*"
            print "DAS Link: ","https://cmsweb.cern.ch/das/request?view=list&limit=10&instance="+dbsInst+"&input=dataset%3D"+dbsDatasetName.replace("/","%2F")+"*"+skimType            
        ##############
            selectionPaths = ['']
            if  re.search("muTau",skimType)!=None:
                selectionPaths = ['muPFTauSkimPath','muCaloTauSkimPath']    
            if  re.search("diTau",skimType)!=None:
                selectionPaths = ['caloTauSkimPath','pfTauSkimPath','mixedTauSkimPathI','mixedTauSkimPathII']
            if  re.search("elecTau",skimType)!=None:
                selectionPaths = ['elecPFTauSkimPath','elecCaloTauSkimPath']
            if  re.search("elecMu",skimType)!=None:
                selectionPaths = ['elecMuSkimPath']
            if  re.search("wTauNu",skimType)!=None:
                selectionPaths = ['PFTauNuSkimPath','CaloTauNuSkimPath']
                                            
            fileListTmp = os.listdir(path+"/"+crabDir+"/res/")
            fileList = []
            for tmp in fileListTmp:
                if re.search("stdout",tmp)!=None:
                    jobNumber = tmp.split("_")[1].split(".")[0]
                    stdoutFileName = "CMSSW_"+str(jobNumber)+".stdout"
                    stdoutFile = open(path+"/"+crabDir+"/res/"+stdoutFileName,"r")
                    exitCode = 9999
                    for line in stdoutFile:
                        if re.search("JobExitCode",line)!=None:
                            exitCode = line.split("=")[1].rstrip("\n")
                    if int(exitCode)==0:
                        fileList.append(path+"/"+crabDir+"/res/"+tmp)
                    if int(exitCode)!=0:
                        print "Will skip job number:",jobNumber," with exitCode= ",exitCode
                    pass
                              
            outputString=""
            outputString += getEffFromTrigReport(selectionPaths[0],skimType,fileList,0)
            outputString += "<td> \n %TREEBROWSER{\"thread\" noroot=\"on\" noindent=\"on\" autotoggle=\"on\" style=\"dmenu\"}%\n"
            outputString += "   * Click for details \n"
            outputString += getEffFromTrigReport(selectionPaths[0],skimType,fileList,1)
            

            if len(publishName)==0:
                publishName = dbsDatasetName+" (Not published)"
            print "datasetpath: ",datasetpath," pset: ",pset," SE: ",storage_element
            print "publish_data_name: ",publish_data_name
            dbsDatasetName="/"+datasetpath.split("/")[1]+"/akalinow-"+publish_data_name
            print "DBS Instance: ",dbsInst
            print "dbsDatasetName: ",dbsDatasetName
            print "publishName: ",publishName
            print "dashboardLink: ",dashboardLink
            fileName = "TWiki_"+skimType+".txt"
            f = open(fileName,"a")
            
            f.write("<tr> <td>[[https://cmsweb.cern.ch/das/request?view=list&limit=10&instance="+dbsInst+"&input=dataset%3D"+dbsDatasetName.replace("/","%2F")+"*"+skimType+"]["+publishName+"]]"
                    + "</td> <td>" + "[["+dashboardLink+"][Link]]"
                    + "</td> <td>" + "[[https://cmsweb.cern.ch/filemover/]["+storage_element+"]]"
                    + "</td> <td>" + outputString
                    + " </td> </tr> \n \n \n")
            f.close()
##################
##################
#rootPath = "/home/akalinow/scratch/CMS/test/"
rootPath = "/home/llr/cms/mbluj/WorkDir/CMSSW/CMSSW_5_3_4_patch1/src/LLRAnalysis/TauTauStudies/test/skim/"

getReportForSample(rootPath,"","LepTauTrg-Skim_AOD")


