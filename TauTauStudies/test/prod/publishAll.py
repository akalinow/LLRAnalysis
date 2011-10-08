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
                f.write('CMSSW.events_per_job = 5000\n')
                f.write('\n')
    f.close()

###########################################
###########################################

tasksMu    = [
    'DYToTauTau-Mu-20-PUS3',
    'DYToMuMu-20-PUS3',
    'VBFH115-Mu-powheg-PUS4',
    'VBFH125-Mu-powheg-PUS4',
    'VBFH135-Mu-powheg-PUS4',
    'GGFH115-Mu-powheg-PUS1',
    'GGFH135-Mu-powheg-PUS4',
    'GGFH125-Mu-powheg-PUS4',
    'GGFH125-Mu-powheg-PUS4',
    'VBFH125-Mu-powheg-PUS4',
    'TT-Mu-pythia-PUS3',
    'DYJets-Mu-50-madgraph-PUS4',
    'WW-Mu-pythia-PUS4',
    'WZ-Mu-pythia-PUS4',
    'ZZ-Mu-pythia-PUS4',
    'WJets-Mu-madgraph-PUS4'
    'T-Mu-t-PUS1_skim',
    'Tbar-Mu-t-PUS1_skim',
    'WZIncl-Mu-pythia-PUS4_skim',
    'TTJets-Mu-madgraph-PUS4_skim',
    'GGFH105-Mu-powheg-PUS1_skim',
    'GGFH110-Mu-powheg-PUS1_skim',
    'GGFH115-Mu-powheg-PUS1_skim',
    'GGFH120-Mu-powheg-PUS1_skim',
    'GGFH125-Mu-powheg-PUS4_skim-v2'
    'GGFH130-Mu-powheg-PUS4_skim-v2',
    'GGFH135-Mu-powheg-PUS4_skim-v2',
    'GGFH140-Mu-powheg-PUS4_skim-v2',
    'VBFH105-Mu-powheg-PUS4_skim-v2',
    'VBFH110-Mu-powheg-PUS4_skim-v2',
    'VBFH115-Mu-powheg-PUS4_skim',
    'VBFH120-Mu-powheg-PUS4_skim',
    'VBFH120-Mu-powheg-PUS4_skim',
    'VBFH125-Mu-powheg-PUS4_skim-v2',
    'VBFH130-Mu-powheg-PUS4_skim-v2',
    'VBFH135-Mu-powheg-PUS4_skim-v2',
    'VBFH140-Mu-powheg-PUS4_skim-v2'
    ]

tasksElec  = [
    'DYToEE-20-PUS3',
    'DYToTauTau-20-PUS3',
    'TT-pythia-PUS3',
    'GGFH115-powheg-PUS1',
    'GGFH125-powheg-PUS4',
    'GGFH135-powheg-PUS4',
    'VBFH115-powheg-PUS4',
    'VBFH125-powheg-PUS4'
    'VBFH135-powheg-PUS4',
    'WJets-madgraph-PUS4',
    'DYJets-50-madgraph-PUS4',
    'WW-pythia-PUS4',
    'WZ-pythia-PUS4',
    'ZZ-pythia-PUS4',
    'T-t-PUS1_skim',
    'Tbar-t-PUS1_skim',
    'WZIncl-pythia-PUS4_skim',
    'TTJets-madgraph-PUS4_skim',
    'GGFH105-powheg-PUS1_skim',
    'GGFH110-powheg-PUS1_skim',
    'GGFH115-powheg-PUS1_skim',
    'GGFH120-powheg-PUS1_skim',
    'GGFH125-powheg-PUS4_skim-v4',
    'GGFH130-powheg-PUS4_skim-v2',
    'GGFH135-powheg-PUS4_skim-v4',
    'GGFH140-powheg-PUS4_skim-v2',
    'VBFH105-powheg-PUS4_skim-v2',
    'VBFH110-powheg-PUS4_skim-v2',
    'VBFH115-powheg-PUS4_skim',
    'VBFH120-powheg-PUS4_skim-v2',
    'VBFH125-powheg-PUS4_skim-v4',
    'VBFH130-powheg-PUS4_skim-v2',
    'VBFH135-powheg-PUS4_skim-v4',
    'VBFH140-powheg-PUS4_skim-v2',
    'Run2011-05AugReReco_skim',
    'Run2011-PromptReco-v6_skim/'
    ]

tasksElecMu =[
    'DYJets-ElecMu-50-madgraph-PUS4_skim',
    'T-ElecMu-s-PUS4_skim',
    'T-ElecMu-t-PUS4_skim',
    'T-ElecMu-tW-PUS4_skim',
    'Tbar-ElecMu-s-PUS4_skim',
    'Tbar-ElecMu-t-PUS4_skim',
    'Tbar-ElecMu-tW-PUS4_skim',
    'TTJets-ElecMu-madgraph-PUS4_skim',
    'WJets-ElecMu-madgraph-PUS4_skim',
    'WW-ElecMu-madgraph-PUS4_skim',
    'WW-ElecMu-pythia-PUS4_skim',
    'WZ-ElecMu-pythia-PUS4_skim',
    'ZZ-ElecMu-pythia-PUS4_skim',
    'GGFH105-ElecMu-powheg-PUS1_skim',
    'GGFH110-ElecMu-powheg-PUS1_skim',
    'GGFH115-ElecMu-powheg-PUS1_skim',
    'GGFH120-ElecMu-powheg-PUS1_skim',
    'GGFH125-ElecMu-powheg-PUS4_skim',
    'GGFH130-ElecMu-powheg-PUS4_skim',
    'GGFH135-ElecMu-powheg-PUS4_skim',
    'GGFH140-ElecMu-powheg-PUS4_skim',
    'VBFH105-ElecMu-powheg-PUS4_skim',
    'VBFH110-ElecMu-powheg-PUS4_skim',
    'VBFH115-ElecMu-powheg-PUS4_skim',
    'VBFH120-ElecMu-powheg-PUS4_skim',
    'VBFH125-ElecMu-powheg-PUS4_skim',,
    'VBFH130-ElecMu-powheg-PUS4_skim',
    'VBFH135-ElecMu-powheg-PUS4_skim',
    'VBFH140-ElecMu-powheg-PUS4_skim'
    ]


#publishSkim( "MuTauStream_patch2",  tasksMu )
#publishSkim( "ElecTauStream_patch3",  tasksElec   )
publishSkim( "ElecMuStream",  tasksElecMu   )
