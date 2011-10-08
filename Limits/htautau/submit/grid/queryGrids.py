#!/usr/bin/env python

import commands
import re
import os

import sys

###########################################
###########################################

def publishSkim( stream , variable ):

    print stream, " with " , variable
    
    f = open('results_'+stream+'_SM_'+variable+'.txt', 'w')
    nameInDatacard = ""
    if re.search("eleTau", stream )!=None:
        nameInDatacard = "eTau"
    if re.search("muTau",  stream)!=None:
        nameInDatacard = "muTau"
    
    for mass in [105, 110, 115, 120, 125, 130, 135, 140]:

        print mass
        
        medianExpected     = 0
        OneSUpExpected     = 0
        TwoSUpExpected     = 0
        OneSDownExpected   = 0
        TwoSDownExpected   = 0
        observed           = 0

        f.write(str(mass)+" ")
            
        getMedianExp = "combine "+nameInDatacard+"_SM_mH"+str(mass)+"_"+variable+".txt -M HybridNew --freq --testStat LHC --grid="+stream+"_SM_"+variable+"_mH"+str(mass)+".root --expectedFromGrid 0.5"
        outputMedianExp = commands.getoutput( getMedianExp )
        linesMedianExp  = re.split(r'\n', outputMedianExp )
        #print linesMedianExp
        for line in linesMedianExp:
            if  re.search("@ 95% CL", line )!=None:
                words = re.split(r'\s', line)
                print "Median => ", words
                medianExpected = float(words[3])
        f.write(str(medianExpected)+" ")

        getOneSUpExpected = "combine "+nameInDatacard+"_SM_mH"+str(mass)+"_"+variable+".txt -M HybridNew --freq --testStat LHC --grid="+stream+"_SM_"+variable+"_mH"+str(mass)+".root --expectedFromGrid 0.84"
        outputOneSUpExpected = commands.getoutput( getOneSUpExpected )
        linesOneSUpExpected  = re.split(r'\n', outputOneSUpExpected )
        #print linesOneSUpExpected
        for line in linesOneSUpExpected:
            if  re.search("@ 95% CL", line )!=None:
                words = re.split(r'\s', line)
                print "1s Up => " , words
                OneSUpExpected = float(words[3])
        f.write(str(OneSUpExpected)+" ")
        
        getTwoSUpExpected = "combine "+nameInDatacard+"_SM_mH"+str(mass)+"_"+variable+".txt -M HybridNew --freq --testStat LHC --grid="+stream+"_SM_"+variable+"_mH"+str(mass)+".root --expectedFromGrid 0.975"
        outputTwoSUpExpected = commands.getoutput( getTwoSUpExpected )
        linesTwoSUpExpected  = re.split(r'\n', outputTwoSUpExpected )
        #print linesTwoSUpExpected
        for line in linesTwoSUpExpected:
            if  re.search("@ 95% CL", line )!=None:
                words = re.split(r'\s', line)
                print "2s Up => " , words
                TwoSUpExpected = float(words[3])
        f.write(str(TwoSUpExpected)+" ")
  
        getOneSDownExpected = "combine "+nameInDatacard+"_SM_mH"+str(mass)+"_"+variable+".txt -M HybridNew --freq --testStat LHC --grid="+stream+"_SM_"+variable+"_mH"+str(mass)+".root --expectedFromGrid 0.16"
        outputOneSDownExpected = commands.getoutput( getOneSDownExpected )
        linesOneSDownExpected  = re.split(r'\n', outputOneSDownExpected )
        #print linesOneSDownExpected
        for line in linesOneSDownExpected:
            if  re.search("@ 95% CL", line )!=None:
                words = re.split(r'\s', line)
                print "1s Down => " , words
                OneSDownExpected = float(words[3])
        f.write(str(OneSDownExpected)+" ")
  
        getTwoSDownExpected = "combine "+nameInDatacard+"_SM_mH"+str(mass)+"_"+variable+".txt -M HybridNew --freq --testStat LHC --grid="+stream+"_SM_"+variable+"_mH"+str(mass)+".root --expectedFromGrid 0.0275"
        outputTwoSDownExpected = commands.getoutput( getTwoSDownExpected )
        linesTwoSDownExpected  = re.split(r'\n', outputTwoSDownExpected )
        #print linesTwoSDownExpected
        for line in linesTwoSDownExpected:
            if  re.search("@ 95% CL", line )!=None:
                words = re.split(r'\s', line)
                print "2s Down => " ,words
                TwoSDownExpected = float(words[3])
        f.write(str(TwoSDownExpected)+" ")

        getObserved = "combine "+nameInDatacard+"_SM_mH"+str(mass)+"_"+variable+".txt -M HybridNew --freq --testStat LHC --grid="+stream+"_SM_"+variable+"_mH"+str(mass)+".root"
        outputObserved = commands.getoutput( getObserved )
        linesObserved  = re.split(r'\n', outputObserved )
        #print linesObserved
        for line in linesObserved:
            if  re.search("@ 95% CL", line )!=None:
                words = re.split(r'\s', line)
                print "Observed => ", words
                observed = float(words[3])
        f.write(str(observed))
        
        f.write('\n')

    f.close()


###########################################
###########################################

publishSkim("eleTau", "diTauVisMass")
publishSkim("eleTau", "diTauNSVfitMass")
publishSkim("muTau", "diTauVisMass")
publishSkim("muTau", "diTauNSVfitMass")
