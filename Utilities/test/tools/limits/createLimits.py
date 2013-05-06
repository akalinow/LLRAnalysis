#############################################
## PERFORMS LIMIT COMPUTATION STEP BY STEP ##
#############################################

# example : python createLimits.py mt LLR bbbW auxiliaries/datacards/collected/LLR/
# other ex: python createLimits.py mt LLR std  TestLimits/inputs/

import os
import sys
import string
import re
import shlex
import subprocess

# arguments
chan = str(sys.argv[1]) # channel=mt,et
name = str(sys.argv[2]) # name=LLR,...
bbb  = str(sys.argv[3]) # "bbb", "bbbW", "std"
file = str(sys.argv[4]) # path to input root file (from src)
dir  = str(sys.argv[5]) # working directory path wrt src
src  = "/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_611_LIMITS/src/"

print "Create limits environment step by step : "+chan+" "+name+" "+bbb

################
# CREATE SETUP #
################

print "Create setups and aux"
if not os.path.isdir("setups_"+name+"/input"):
    os.system("mkdir -p setups_"+name)
    os.system("cp -r "+src+"HiggsAnalysis/HiggsToTauTau/setup setups_"+name+"/input")

if not os.path.exists("setups_"+name+"/input/"+chan+"/htt_"+chan+".inputs-sm-8TeV.root"):
    os.system("cp "+src+"/"+file+" setups_"+name+"/input/"+chan+"/htt_"+chan+".inputs-sm-8TeV.root")


#############################
# ADD BBB ERRORS (optional) #
#############################

bbbchain = "'"+chan+":8TeV:01,03,05:ZL,ZJ,QCD>W'"
if "bbbW" in bbb:
    bbbchain = "'"+chan+":8TeV:01,03,05:ZL,ZJ,W>QCD'"
if "bbb" in bbb:
    print "Add bbb errors : "+bbbchain
    os.system("add_bbb_errors.py "+bbbchain+" -i setups_"+name+"/input -o setups_"+name+"/"+bbb+" --normalize --threshold 0.10")
else:
    print "No bbb errors"

###############################
# create datacards and LIMITS #
###############################

input="input"
if "bbb" in bbb:
    input=bbb

print "Create datacards in : aux_"+name+"/"+bbb+"/sm"
if not os.path.exists("aux_"+name+"/"+bbb+"/sm"):
    os.system("mkdir -p aux_"+name+"/"+bbb+"/sm")
os.system("setup-datacards.py -i "+dir+"/setups_"+name+"/"+input+" -o aux_"+name+"/"+bbb+"/sm -a sm -p 8TeV -c "+chan+" 110-145:5")

print "Create LIMITS_"+name+"/"+bbb
if not os.path.exists("LIMITS_"+name+"/"+bbb):
    os.system("mkdir -p LIMITS_"+name+"/"+bbb)
os.system("setup-htt.py -i aux_"+name+"/"+bbb+"/sm -o LIMITS_"+name+"/"+bbb+"/sm -c '"+chan+"' -p 8TeV 110-145:5")
