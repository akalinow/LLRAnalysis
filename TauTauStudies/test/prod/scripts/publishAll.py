#!/usr/bin/env python

import commands
import re
import os

import sys

###########################################
###########################################

def publishSkim( stream, tasks ):

    f = open(stream+'_Skim.txt', 'w')
    
    for task in tasks:
        READ = 0
        SKIM = 0
        #status = "crab -status -c "+task+"_skim/"
        status = "crab -status -c "+task
        print status
        os.system( status )
        #getoutput = "crab -getoutput -c "+task+"_skim/"
        getoutput = "crab -getoutput -c "+task
        print getoutput
        os.system( getoutput )
        #report = "crab -report -c "+task+"_skim/"
        report = "crab -report -c "+task
        print report
        output0 = commands.getoutput( report )
        reportLines = re.split(r'\n',output0)
        for line in reportLines:
            if  re.search("Total Events read:", line )!=None:
                words = re.split(r'\s', line)
                print words
                READ = float(words[3])
                
        #publish = "crab -publish -c "+task+"_skim/"
        publish = "crab -publish -c "+task
        print publish
        #os.system( publish )
        output1 = commands.getoutput( publish )
        pusblishLines = re.split(r'\n',output1)
        for line in pusblishLines:
            if  re.search("total events:", line )!=None:
                words0 = re.split(r'/',line)
                words1 = re.split(r'\s', line)
                print words1, words0
                SKIM = float(words1[3])
                datasetpath = "/"+words0[1]+"/"+words0[2]+"/"+words0[3]
                #print words
                f.write('['+task+'_run]\n')
                f.write('#skim efficiency: '+str(SKIM)+'/'+str(READ)+' = '+str(SKIM/READ)+'\n')
                f.write('CMSSW.datasetpath='+datasetpath+'\n')
                f.write('CMSSW.total_number_of_events= -1\n')
                f.write('CMSSW.events_per_job = 2000\n')
                f.write('\n')
    f.close()

###########################################
###########################################




tasksMuTauSummer12 = [

    ]

tasksElecTauSummer12 = [
##     #'Run2012A-ElecTau-ReReco13Jul2012-v1-iter3_skim', 
## #'Run2012B-ElecTau-ReReco13Jul2012-v1-iter3_skim', 
'Run2012C-ElecTau-ReReco24Aug2012-v1-iter3_skim', 
## #'Run2012C-ElecTau-PromptReco-v2-iter3_skim',
## #'Run2012C-ElecTau-PromptReco-v2p2-198934-203002_skim',

############################
#####Embedded ETau
##########################

##     #'Run2012A-ElecTau-53X-ReReco13Jul2012-v1-Embedded-EleJet-iter3_skim', 
##     #'Run2012B-ElecTau-53X-ReReco13Jul2012-v1-Embedded-EleJet-iter3_skim', 
##     #'Run2012C-ElecTau-53X-PromptReco-v2-Embedded-EleJet-iter3_skim',
##     #'Run2012C-ElecTau-53X-ReReco24Aug2012-Embedded-EleJet-iter3_skim',
##     #'Run2012C-ElecTau-53X-PromptReco-v2p2-198934-203002-Embedded-EleJet_skim',


#global dbs
## #'Run2012A-ElecTau-53X-ReReco13Jul2012-v1-Embedded-EleJet-iter4_skim', 
## #'Run2012A-ElecTau-53X-recover_06Aug2012-Embedded-EleJet-iter4_skim', 
## 'Run2012B-ElecTau-53X-ReReco13Jul2012-v1-Embedded-EleJet-iter5_skim', 
## #'Run2012C-ElecTau-53X-ReReco24Aug2012-Embedded-EleJet-iter4_skim', 
## #'Run2012C-ElecTau-53X-PromptReco-v2-Embedded-EleJet-iter4_skim',


## ##52X

## #'Run2012A-ElecTau-PromptReco-v1-Embedded-EleJet_skim',
##     #'Run2012B-ElecTau-PromptReco-v1-p1-Embedded-EleJet_skim',   
## #'Run2012B-ElecTau-PromptReco-v1-p2-Embedded-EleJet_skim', 
##     #'Run2012B-ElecTau-PromptReco-v1-p3-Embedded-EleJet_skim',


    ###Not to publish
    ###'Run2012C-ElecTau-ReReco24Aug2012-Embedded-EleJet_skim', 


#################################################################
###################MC
#################################################################


#########################################################################
##Signal

##     #'SUSYGGH80-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH90-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH100-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH110-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH120-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH130-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH140-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH160-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH180-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH200-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH250-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH300-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH350-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH400-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH450-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH500-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH600-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH700-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH800-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH900-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYGGH1000-ElecTau-pythia-tauola-iter1_skim',

###

##     #'SUSYBBH80-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH90-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH100-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH110-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH120-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH130-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH140-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH160-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH180-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH200-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH250-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH350-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH400-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH450-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH500-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH600-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH700-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH800-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH900-ElecTau-pythia-tauola-iter1_skim',
##     #'SUSYBBH1000-ElecTau-pythia-tauola-iter1_skim',

#########################################################################
##Embedded MC
#########################################################################
## #'DYJets-ElecTau-8TeV-50-madgraph-PUS7-Embedded_skim'


    ]




#publishSkim( "MuTauStream_30Mar2012_patch50X_v4",  tasksMuTauSummer12)
publishSkim( "ElecTauStream_07Oct2012-part6",tasksElecTauSummer12)

