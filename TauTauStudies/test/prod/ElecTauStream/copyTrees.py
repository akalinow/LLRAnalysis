#!/usr/bin/env python

import commands
import re
import os

import sys
#sys.path.append('')

###########################################
###########################################

def copyAndAdd( directory ):
    
    outSamples = re.split(r'\n',commands.getoutput("dpns-ls /dpm/in2p3.fr/home/cms/trivcat/store/user/lbianchini/"+directory))

    for sample in  outSamples:
        samplesplit =  re.split(r'/',sample)
        output = commands.getoutput("dpns-ls /dpm/in2p3.fr/home/cms/trivcat/store/user/lbianchini/"+directory+"/"+samplesplit[len(samplesplit)-1])
        
        #if  re.search("pat", samplesplit[len(samplesplit)-1])!=None:
        #    continue

        outFiles = re.split(r'\n',output)
        for name in outFiles:
            splitname = re.split(r'/',name)

            #if (re.search("pat", splitname[ len(splitname) -1 ])!=None ):
            #    continue

            print "copying file "+splitname[ len(splitname) -1 ]
            cp = "rfcp /dpm/in2p3.fr/home/cms/trivcat/store/user/lbianchini/"+directory+"/"+samplesplit[len(samplesplit)-1]+"/"+splitname[ len(splitname) -1 ]+" ."
            print cp
            os.system(cp)
        hadd = "hadd -f  treeElecTauStream_"+samplesplit[len(samplesplit)-1]+".root treeElecTauStream_*_*_*.root"
        print hadd
        os.system(hadd)
        os.system("mv  treeElecTauStream_"+samplesplit[len(samplesplit)-1]+".root ../")
        remove = "rm -r treeElecTauStream_*_*_*.root"
        os.system(remove)
 
###########################################
###########################################

#copyAndAdd( "ElecTauStreamFall11_06Dec2011" )
#copyAndAdd( "DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball/ElecTauStream-30Mar2012_DYJets-ElecTau-madgraph-Tarball-v2_skim/")
copyAndAdd( "ElecTauStreamSpring12_30Mar2012" )
