#!/usr/bin/env python

import commands
import re
import os

import sys

###########################################
###########################################

def compare( name1 , name2 ):

    f1 = open(name1, 'r')

    nEvents1        = 0
    nEvents1Matched = 0

    for line1 in f1.readlines():
        if ( re.search("\*",line1)==None or
             re.search("Row",line1)!=None or
             re.search("==>",line1)!=None or
             re.search("\*\*",line1)!=None):
            continue
        nEvents1 = nEvents1 + 1
        words1  = re.split('\W+', line1 )
        #print words1
        run  = words1[2]
        lumi = words1[3]
        event= words1[4]

        #print "Checking run ", run, " in file1"
        matched = False
        f2 = open(name2, 'r')
        for line2 in f2.readlines():
            if not ( re.search("\*",line2)==None or
                     re.search("Row",line2)!=None or
                     re.search("==>",line2)!=None or
                     re.search("\*\*",line2)!=None):
                words2  = re.split('\W+', line2 )
                #print " ==> matching with ", eval(words2[2])
                if( eval(run)== eval(words2[2]) and
                    eval(lumi)== eval(words2[3]) and
                    (eval(event)-eval(words2[4]))/eval(event) < 1E-03 ):
                    nEvents1Matched = nEvents1Matched + 1
                    matched = True
    
        if not matched:
            print run,":",lumi,":",event


    print nEvents1Matched, "/" , nEvents1 , " = ", nEvents1Matched/nEvents1
        

############################################
############################################


compare( "eventListLB.txt", "eventListMB.txt")
