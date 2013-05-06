#############################################
## PERFORMS LIMIT COMPUTATION STEP BY STEP ##
#############################################

# example : python computeLimits.py mt Std2012D std prefit

import os
import sys
import string
import re
import shlex
import subprocess

# arguments
chan    = str(sys.argv[1]) # channel=mt,et
name    = str(sys.argv[2]) # name=LLR,...
bbb     = str(sys.argv[3]) # "bbb", "bbbW", "std"
option  = str(sys.argv[4]) # "nofit", "prefit"
src     = "/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_611_LIMITS/src/"
workDir = os.getcwd()     # current directory

print "Launch limits calculation step by step : "+chan+" "+name+" "+bbb

# Create sub-directory wrt option
if not os.path.exists("LIMITS_"+name+"/"+bbb+"/sm/"+chan+"_"+option):
    os.system("cp -r LIMITS_"+name+"/"+bbb+"/sm/"+chan+" LIMITS_"+name+"/"+bbb+"/sm/"+chan+"_"+option)

# Option
opt=""
if "nofit" in option:
    opt = " -t -1 "

# Loop over masses
listmass = ["110","115","120","125","130","135","140","145"]
#listmass = ["110"]

for mass in listmass:
    print ">>>>>>>>>>>>>>>>>>>>>>>>>>>> MASS : "+mass

    dir  = "LIMITS_"+name+"/"+bbb+"/sm/"+chan+"_"+option+"/"+mass+"/"
    os.chdir(dir)

    file = "htt_"+chan+"_sm_8TeV-"+mass+".txt"
    if os.path.exists(file):
        os.system("mv "+file+" "+file+".old")

    os.system("combineCards.py htt_"+chan+"_*_8TeV.txt > "+file)
    os.system("combine -M Asymptotic "+opt+" --run expected -n '-exp' -C 0.95 --rMin -20 --rMax 50 -m "+mass+" "+file)

    os.chdir(workDir)


# PLOT #
os.system("plot --asymptotic "+src+"/HiggsAnalysis/HiggsToTauTau/python/layouts/limit-sm.py LIMITS_"+name+"/"+bbb+"/sm/"+chan+"_"+option+"/")
