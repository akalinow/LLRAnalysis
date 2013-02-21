#!/usr/bin/env python

import commands
import re
import os

import sys
#sys.path.append('')

###########################################
###########################################

def remove( directory ):
    
    outSamples = re.split(r'\n',commands.getoutput("dpns-ls /dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+directory))

    for sample in  outSamples:
          
        if  (re.search("MuTauStream-16Nov2011", sample )==None ):
            continue
    

        rm = "rfrm -rf /dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/"+directory+"/"+sample
        print rm
        os.system(rm)
 
###########################################
###########################################


#remove( "DoubleMu" )
remove( "W3Jets_TuneZ2_7TeV-madgraph-tauola")
remove( "SUSYBBHToTauTau_M-100_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-100_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-120_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-130_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-140_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-160_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-180_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-200_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-250_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-300_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-350_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-400_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-450_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-500_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-600_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-700_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-800_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-900_7TeV-pythia6-tauola")
remove( "SUSYBBHToTauTau_M-90_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-100_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-120_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-130_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-140_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-160_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-180_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-200_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-250_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-300_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-350_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-400_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-450_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-500_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-600_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-700_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-800_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-900_7TeV-pythia6-tauola")
remove( "SUSYGluGluToHToTauTau_M-90_7TeV-pythia6-tauola")



