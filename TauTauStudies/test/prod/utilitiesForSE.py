#!/usr/bin/env python

import commands
import re
import os
import sys

###########################################
###########################################

def copyToAnotherSE():
   

    output = commands.getoutput("lcg-ls srm://polgrid4.in2p3.fr:8446/srm/managerv2?SFN=/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1])

    outFiles = re.split(r'\n',output)

    for name in outFiles:
        splitname = re.split(r'/',name)
        print "copying file "+splitname[ len(splitname) -1 ]
        cp = "lcg-cp -b -D srmv2 srm://polgrid4.in2p3.fr:8446/srm/managerv2?SFN=/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1]+splitname[len(splitname) -1]+" srm://srm-cms.cern.ch:8443/srm/managerv2?SFN=/castor/cern.ch/user/b/bianchi/CMSSW382/"+splitname[len(splitname) -1]
        #print cp
        os.system(cp)

###########################################
###########################################

def copyToLocal():
   

    output = commands.getoutput("lcg-ls srm://polgrid4.in2p3.fr:8446/srm/managerv2?SFN=/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1])

    outFiles = re.split(r'\n',output)

    for name in outFiles:
        splitname = re.split(r'/',name)
        print "copying file "+splitname[ len(splitname) -1 ]
        cp = "lcg-cp -b -D srmv2 srm://polgrid4.in2p3.fr:8446/srm/managerv2?SFN=/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1]+splitname[len(splitname) -1]+" file:./"+sys.argv[2]+"/"+splitname[len(splitname) -1]
        print cp
        os.system(cp)

###########################################
###########################################

def rmFromSE():
    output = commands.getoutput("lcg-ls srm://polgrid4.in2p3.fr:8446/srm/managerv2?SFN=/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1])

    outFiles = re.split(r'\n',output)

    for name in outFiles:
        splitname = re.split(r'/',name)
        #print splitname
        print "removing file "+splitname[len(splitname) -1]
        rm = "srmrm srm://polgrid4.in2p3.fr:8446/srm/managerv2?SFN=/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1]+splitname[len(splitname) -1]
        print rm
        os.system(rm)

###########################################
###########################################

def checkFile():
    output = commands.getoutput("lcg-ls srm://polgrid4.in2p3.fr:8446/srm/managerv2?SFN=/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1])

    outFiles = re.split(r'\n',output)

    for name in outFiles:
        splitname = re.split(r'/',name)
        #print splitname
        print "checking file "+splitname[len(splitname) -1]
        check = "edmFileUtil -f root://polgrid4.in2p3.fr//dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1]+splitname[len(splitname) -1]
        outcheck = commands.getoutput(check)
        outchecksplit = re.split(r'\n',outcheck)
        #print re.split(r'\n',outcheck)
        if re.search("events",outchecksplit[1])!=None:
            print "ok"

############################################
############################################
            
def rmFromSEUnwantedFiles():
    output = commands.getoutput("lcg-ls srm://polgrid4.in2p3.fr:8446/srm/managerv2?SFN=/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1])

    outFiles = re.split(r'\n',output)
    
    for name in outFiles:
        if re.search("patTuples",name)==None:
            continue
        splitname = re.split(r'/',name)
        filename = splitname[len(splitname) -1]
        print "***************\n"
        print "In the directory there is file: ", filename
        jobIDsplit = re.split(r'_',filename)
        #print jobIDsplit
        jobID = jobIDsplit[2]
        fName =  './'+sys.argv[2]+'/res/CMSSW_'+jobID+'.stdout'
        print "Crab dir = ", fName
        try:
            out = open(fName,'r')
            lines = out.readlines()
            found = 0
            for line in lines:
                if re.search("LFN:",line)!=None and re.search("patTuples",line)!=None and found==0:
                    found = 1
                    linesplit = re.split(r' ',line)
                    #print linesplit
                    print "In the crab dir there is file: ",linesplit[len(linesplit) -1].replace('\n','')
                    if re.search(jobIDsplit[ len(jobIDsplit)-1],linesplit[len(linesplit) -1])!=None:
                        print "Matched"
                    else:
                        print "Not Matched: remove file "+filename
                        #rm = "srmrm srm://polgrid4.in2p3.fr:8446/srm/managerv2?SFN=/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1]+filename
                        rm = "rfrm /dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+sys.argv[1]+filename
                        print rm
                        os.system(rm)
                        
            out.close()
        except IOError:
            print 'cannot open ', fName 

###########################################
###########################################



#rmFromSE()
#copyToAnotherSE()
#checkFile()
rmFromSEUnwantedFiles()
#copyToLocal()
