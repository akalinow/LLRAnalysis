#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def treeSkim( ana, sample, runInSeries=False):

    #os.system( 'mkdir batch/' )

    #stream = "ElecTau"
    if(re.search("MuTau",sample)!=None):
        stream = "MuTau"
    else:
        stream = "ElecTau"
    print "Stream ", stream

    os.system('python makeTreeSkimmerElecTau_Spring13.py')

    if runInSeries:
         print "Running in series via the command ..."
         configFileName = "./Configs/treeSkimmerElecTau_Spring13_%s_%s_cfg.py" % (sample,ana)
         runInSeries   = 'treeSkimmer'+stream+'_Spring13 '+configFileName+'\n'
         print runInSeries
         os.system(runInSeries)

    else:
        print "Creating the shell file for the batch..."
        nameJob = 'job_'+sample+'_'+ana
        fileJob = 'batch/'+nameJob+'.sh'
        fileLog = 'batch/log/'+nameJob+'.txt'
        ##
        f = open(fileJob,'w')    
        f.write('#!/bin/sh\n\n')
        f.write('export WORKINGDIR="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_4_Oct12/src/Bianchi/Limits/bin/"\n')
        f.write('')
        f.write('cd $WORKINGDIR\n')
        f.write('export SCRAM_ARCH=slc5_amd64_gcc462\n')
        f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
        f.write('eval `scram runtime -sh`\n')
        f.write('source /opt/exp_soft/cms/cms_ui_env_crab.sh\n')
        ##
        configFileName = "./Configs/treeSkimmerElecTau_Spring13_%s_%s_cfg.py" % (sample,ana)
        f.write('treeSkimmer'+stream+'_Spring13 '+configFileName+' > '+fileLog+'\n')
        f.close()
        os.system('chmod u+x batch/*.sh')

  
###########################################
###########################################
##Data
## treeSkim("nominal","Run2012A-13Jul2012-Data",False)
## #treeSkim("nominal","Run2012B-13Jul2012-Data",False)
## treeSkim("nominal","Run2012C-24Aug2012-Data",False)
## # treeSkim("nominal","Run2012C-PromptReco-v2-Data",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Residual-Data",False)

## ##Split
## treeSkim("nominal","Run2012B-13Jul2012-Data-h0",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h1",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h2",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h3",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h4",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h5",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h6",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h7",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h8",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h9",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h10",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h11",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h12",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h13",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h14",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h15",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h16",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h17",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h18",False)
## treeSkim("nominal","Run2012B-13Jul2012-Data-h19",False)

## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h0",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h1",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h2",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h3",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h4",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h5",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h6",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h7",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h8",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h9",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h10",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h11",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h12",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h13",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h14",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h15",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h16",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h17",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h18",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-Data-h19",False)

treeSkim("nominal","Run2012D-PromptReco-v1-p1-Data",False)
treeSkim("nominal","Run2012D-PromptReco-v1-p2-Data",False)
treeSkim("nominal","Run2012D-PromptReco-v1-p3-Data",False)
treeSkim("nominal","Run2012D-PromptReco-v1-p4-Data",False)
treeSkim("nominal","Run2012D-PromptReco-v1-p5-Data",False)
treeSkim("nominal","Run2012D-PromptReco-v1-p6-Data",False)
treeSkim("nominal","Run2012D-PromptReco-v1-p7-Data",False)
treeSkim("nominal","Run2012D-PromptReco-v1-p8-Data",False)
treeSkim("nominal","Run2012D-PromptReco-v1-p9-Data",False)

###Embedded samples
anaEmb = ['nominal','TauUp','TauDown','ElecUp','ElecDown']
#anaEmb = ['nominal']
#anaEmb = ['TauUp','TauDown','ElecUp','ElecDown']

samplesEmb = [
##     'Run2012A-Recover06Aug2012-Embedded',
##     'Run2012A-13Jul2012-Embedded',
##     'Run2012B-13Jul2012-Embedded',
##     'Run2012C-24Aug2012-Embedded',
##     'Run2012C-PromptReco-v2-Embedded',
##     'Run2012D-PromptReco-v1-Embedded'
    
##     'Run2012D-PromptReco-v1-p1-Embedded',
##     'Run2012D-PromptReco-v1-p2-Embedded',
##     'Run2012D-PromptReco-v1-p3-Embedded',
##     'Run2012D-PromptReco-v1-p4-Embedded',
##     'Run2012D-PromptReco-v1-p5-Embedded',
##     'Run2012D-PromptReco-v1-p6-Embedded',
##     'Run2012D-PromptReco-v1-p7-Embedded',
##     'Run2012D-PromptReco-v1-p8-Embedded',
##     'Run2012D-PromptReco-v1-p9-Embedded'
    ]
json    = [1,0,0,2,3]

for iSEmb in range(0,len(samplesEmb)):
    for iAnEmb in range(0,len(anaEmb)):
        treeSkim( anaEmb[iAnEmb] , samplesEmb[iSEmb],False )



##MC
#anaMC = ['nominal','TauUp','TauDown','JetUp','JetDown']
anaMC = ['nominal']
#anaMC = ['TauUp','TauDown','JetUp','JetDown']

## for iAnMC in range(0,len(anaMC)):
    ##BkgMC
##         treeSkim( anaMC[iAnMC] , "DYJets"        , False)
## ## ##         treeSkim( anaMC[iAnMC] , "DYJets-Embedded", False)
## ## ##         treeSkim( anaMC[iAnMC] , "DYJ_TauTau"    , False)
## ## ##         treeSkim( anaMC[iAnMC] , "DYJ_EToTau"    , False)
## ## ##         treeSkim( anaMC[iAnMC] , "DYJ_JetToTau"  , False)
##         treeSkim( anaMC[iAnMC] , "WJets1Jets"    , False)
##         treeSkim( anaMC[iAnMC] , "WJets2Jets"    , False)
##         treeSkim( anaMC[iAnMC] , "WJets3Jets"    , False)
##         treeSkim( anaMC[iAnMC] , "WJets4Jets"    , False)
##         treeSkim( anaMC[iAnMC] , "WJets-p1"      , False)
##         treeSkim( anaMC[iAnMC] , "WJets-p2"      , False)
##         treeSkim( anaMC[iAnMC] , "TTJets"        , False)
##         treeSkim( anaMC[iAnMC] , "T-tW"          , False)
##         treeSkim( anaMC[iAnMC] , "Tbar-tW"       , False)
##         treeSkim( anaMC[iAnMC] , "WWJetsTo2L2Nu" , False)
##         treeSkim( anaMC[iAnMC] , "WZJetsTo2L2Q"  , False)
##         treeSkim( anaMC[iAnMC] , "WZJetsTo3LNu"  , False)
##         treeSkim( anaMC[iAnMC] , "ZZJetsTo2L2Q"  , False)
##         treeSkim( anaMC[iAnMC] , "ZZJetsTo4L"    , False)

## ##     ##HiggsMC
##         treeSkim( anaMC[iAnMC] , "GGFH110"       , False)
##         treeSkim( anaMC[iAnMC] , "GGFH115"       , False)
##         treeSkim( anaMC[iAnMC] , "GGFH120"       , False)
##         treeSkim( anaMC[iAnMC] , "GGFH125"       , False)
##         treeSkim( anaMC[iAnMC] , "GGFH130"       , False)
##         treeSkim( anaMC[iAnMC] , "GGFH135"       , False)
##         treeSkim( anaMC[iAnMC] , "GGFH140"       , False)
##         treeSkim( anaMC[iAnMC] , "GGFH145"       , False)
##         treeSkim( anaMC[iAnMC] , "VBFH110"       , False)
##         treeSkim( anaMC[iAnMC] , "VBFH115"       , False)
##         treeSkim( anaMC[iAnMC] , "VBFH120"       , False)
##         treeSkim( anaMC[iAnMC] , "VBFH125"       , False)
##         treeSkim( anaMC[iAnMC] , "VBFH130"       , False)
##         treeSkim( anaMC[iAnMC] , "VBFH135"       , False)
##         treeSkim( anaMC[iAnMC] , "VBFH140"       , False)
##         treeSkim( anaMC[iAnMC] , "VBFH145"       , False)
##         treeSkim( anaMC[iAnMC] , "VH110"         , False)
##         treeSkim( anaMC[iAnMC] , "VH115"         , False)
##         treeSkim( anaMC[iAnMC] , "VH120"         , False)
##         treeSkim( anaMC[iAnMC] , "VH125"         , False)
##         treeSkim( anaMC[iAnMC] , "VH130"         , False)
##         treeSkim( anaMC[iAnMC] , "VH135"         , False)
##         treeSkim( anaMC[iAnMC] , "VH140"         , False)
##         treeSkim( anaMC[iAnMC] , "VH145"         , False)


#SUSY
samplesSUSY = [
##     'SUSYGGH80',
##     'SUSYGGH90',
##     'SUSYGGH100',
##     'SUSYGGH110',
##     'SUSYGGH120',
##     'SUSYGGH130',
##     'SUSYGGH140',
##     'SUSYGGH160',
##     'SUSYGGH180',
##     'SUSYGGH200',
##     'SUSYGGH250',
##     'SUSYGGH300',
##     'SUSYGGH350',
##     'SUSYGGH400',
##     'SUSYGGH450',
##     'SUSYGGH500',
##     'SUSYGGH600',
##     'SUSYGGH700',
##     'SUSYGGH800',
##     'SUSYGGH900',
##     'SUSYGGH1000',
##     'SUSYBBH80',
##     'SUSYBBH90',
##     'SUSYBBH100',
##     'SUSYBBH110',
##     'SUSYBBH120',
##     'SUSYBBH130',
##     'SUSYBBH140',
##     'SUSYBBH160',
##     'SUSYBBH180',
##     'SUSYBBH200',
##     'SUSYBBH250',
##     'SUSYBBH350',
##     'SUSYBBH400',
##     'SUSYBBH450',
##     'SUSYBBH500',
##     'SUSYBBH600',
##     'SUSYBBH700',
##     'SUSYBBH800',
##     'SUSYBBH900',
##     'SUSYBBH1000',
    ]

effSUSY = [
    0.0120698167573,
    0.0169360723747,
    0.0220568034906,
    0.0274764381995,
    0.0330035378066,
    0.038544344473,
    0.0442641645327,
    0.0547153893068,
    0.0647823906939,
    0.0735097960653,
    0.0924692210735,
    0.107448616314,
    0.115083542001,
    0.123453604107,
    0.126559873529,
    0.122935189184,
    0.0684748591271,
    0.0553935562806,
    0.0442753918176,
    0.0352997288678,
    0.028819135186,
    0.00880604119436,
    0.0133063544344,
    0.0186820735759,
    0.0240026010404,
    0.0292888590028,
    0.0349787201702,
    0.0407588566828,
    0.0512933333333,
    0.0603309609774,
    0.069482133423,
    0.087608783647,
    0.114846276661,
    0.124353410727,
    0.13212778411,
    0.138737329696,
    0.147496551724,
    0.153466538276,
    0.157252520623, 
    0.158490246352,
    0.157811097154,
    ]
    

for iSSUSY in range(0,len(samplesSUSY)):
    for iAnMC in range(0,len(anaMC)):
        
        treeSkim( anaMC[iAnMC] , samplesSUSY[iSSUSY] , 1.0 * 1.0 * 1.0 *effSUSY[iSSUSY]  ,  False)




#Embedded MC
## for iAnMC in range(0,len(anaMC)):
##     treeSkim(anaMC[iAnMC],'DYJets-Embedded', 3504 * 1.0 * 0.087645376959 , False) 
