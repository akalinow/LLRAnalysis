##############################################
## PERFORMS LIMIT COMPUTATION AUTOMATICALLY ##
##############################################

# example : python createLimits.py mt LLR bbbW auxiliaries/datacards/collected/LLR/
# other ex: python createLimits.py mt LLR std  TestLimits/inputs/

import os
import sys
import string
import re
import shlex
import subprocess

# arguments
chan     = str(sys.argv[1]) # channel=mt,et
name     = str(sys.argv[2]) # name=LLR,...
tag      = str(sys.argv[3]) # "LLRLim"
ana      = str(sys.argv[4]) # "bin-by-bin", "bin-by-bin-WtoQCD", "2012d"
noprefit = str(sys.argv[5]) # "noprefit"
pulls    = str(sys.argv[6]) # "pulls"

print "Install directories + add bin-by-bin uncertainties"
os.system("python HiggsAnalysis/HiggsToTauTau/scripts/doSM.py -a "+ana+" -c "+chan+" -p 8TeV --update-all --inputs-"+chan+"="+name+" --label "+name+" 110-145:5")

if "pulls" in pulls:
    print "Compute the pulls"
    #for((i=90;i<=160;i+=5)) do limit.py --stable --max-likelihood LIMITS_$2/bin-by-bin/$1/$i ; done 
    os.sytem("limit.py --stable --max-likelihood LIMITS_"+name+"/"+ana+"/"+chan+"/125")


print "Generate and launch jobs to compute the limits"

if "noprefit" in noprefit:
    os.system("submit.py --asymptotic --no-prefit LIMITS_"+name+"/"+ana+"/"+chan+"/* --llr --jobdir LIMITS_"+name)
else :
    os.system("submit.py --asymptotic LIMITS_"+name+"/"+ana+"/"+chan+"/* --llr --jobdir LIMITS_"+name)

os.system("cd LIMITS_"+name+"/"+ana+"-"+chan)
os.system("./"+ana+"-"+chan+"_submit.sh")
os.system("cd ../..")

print "Plot the limits."

os.system('echo "cp -r LIMITS_'+name+'/'+ana+'/'+chan+'/ LIMITS_'+name+'/'+ana+'/'+chan+'_'+tag+' ; plot --asymptotic HiggsAnalysis/HiggsToTauTau/python/layouts/limit-sm.py LIMITS_'+name+'/'+ana+'/'+chan+'_'+tag+' ; mkdir LIMITS_'+name+'/'+ana+'/'+chan+'_'+tag+'/results ; mv '+chan+'_'+tag+'_limit.* LIMITS_'+name+'/'+ana+'/'+chan+'_'+tag+'/results" > plot_'+chan+'_'+tag+'.sh')

os.system('chmod u+x plot_'+chan+'_'+tag+'.sh')

print "Wait for your T3 jobs to be finished, then launch plot_"+cham+"_"+tag+".sh to plot the limits."

