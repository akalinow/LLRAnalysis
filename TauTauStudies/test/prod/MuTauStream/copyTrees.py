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
        
        #if (re.search("pat", samplesplit[len(samplesplit)-1])!=None ):
        #   continue

        outFiles = re.split(r'\n',output)
        for name in outFiles:
            splitname = re.split(r'/',name)

            #if (re.search("", splitname[ len(splitname) -1 ])!=None ):
            #   continue
             

            print "copying file "+splitname[ len(splitname) -1 ]
            cp = "rfcp /dpm/in2p3.fr/home/cms/trivcat/store/user/lbianchini/"+directory+"/"+samplesplit[len(samplesplit)-1]+"/"+splitname[ len(splitname) -1 ]+" ."
            print cp
            os.system(cp)
        hadd = "hadd -f  treeMuTauStream_"+samplesplit[len(samplesplit)-1]+".root treeMuTauStream_*_*_*.root"
        #hadd = "hadd -f  skimMuTauStream.root skimMuTauStream_*_*_*.root"
        print hadd
        os.system(hadd)
        #os.system("cp  treeMuTauStream_"+samplesplit[len(samplesplit)-1]+".root ../")
        remove = "rm -r treeMuTauStream_*_*_*.root"
        #remove = "rm -r skimMuTauStream_*_*_*.root"
        os.system(remove)
 
###########################################
###########################################


#copyAndAdd( "DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball/MuTauStream-30Mar2012_DYJets-MuTau-madgraph-Tarball-v2_skim/" )
copyAndAdd( "MuTauStreamSpring12_30Mar2012" )
