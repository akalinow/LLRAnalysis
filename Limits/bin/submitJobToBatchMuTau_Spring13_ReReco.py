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

    #stream = "MuTau"
    if(re.search("MuTau",sample)!=None):
        stream = "MuTau"
    else:
        stream = "MuTau"
    print "Stream ", stream

    os.system('python makeTreeSkimmerMuTau_Spring13_ReReco.py')
    
    if runInSeries:
         print "Running in series via the command ..."
         configFileName = "./ConfigsReReco/treeSkimmerMuTau_Spring13_%s_%s_cfg.py" % (sample,ana)
         runInSeries   = 'treeSkimmer'+stream+'_Spring13 '+configFileName+'\n'
         print runInSeries
         os.system(runInSeries)

    else:
        #nameJob = 'job_'+sample+'_'+ana+'_'+stream
        if 'Data' in sample:
            nameJob = 'job_'+sample+'_'+stream
        else:
            nameJob = 'job_'+sample+'_'+stream+'_'+ana
        fileJob = 'batch/'+nameJob+'.sh'
        fileLog = 'batch/log/'+nameJob+'.txt'
        print "Creating the shell file : "+nameJob
        ##
        f = open(fileJob,'w')    
        f.write('#!/bin/sh\n\n')
        #f.write('export WORKINGDIR="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_10_analysis/src/LLRAnalysis/Limits/bin/"\n')
        f.write('export WORKINGDIR="/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_5_3_10_SyncRecoEleIdJEC/src/LLRAnalysis/Limits/bin/"\n')
        #f.write('export WORKINGDIR="/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534p2_Spring13_Trees/src/LLRAnalysis/Limits/bin/"\n')
        f.write('')
        f.write('cd $WORKINGDIR\n')
        f.write('export SCRAM_ARCH=slc5_amd64_gcc462\n')
        f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
        f.write('eval `scram runtime -sh`\n')
        f.write('source /opt/exp_soft/cms/cms_ui_env_crab.sh\n')
        ##
        configFileName = "./ConfigsReReco/treeSkimmerMuTau_Spring13_%s_%s_cfg.py" % (sample,ana)
        f.write('treeSkimmer'+stream+'_Spring13 '+configFileName+' &> '+fileLog+'\n')
        f.close()
        os.system('chmod u+x batch/*.sh')

###########################################
###########################################

##Data
## treeSkim("nominal","Run2012A-13Jul2012-Data",False)
## treeSkim("nominal","Run2012B-13Jul2012-p1-Data",False)
## treeSkim("nominal","Run2012B-13Jul2012-p2-Data",False)
## treeSkim("nominal","Run2012B-13Jul2012-p3-Data",False)
## treeSkim("nominal","Run2012B-13Jul2012-p4-Data",False)
## treeSkim("nominal","Run2012B-13Jul2012-p5-Data",False)
## treeSkim("nominal","Run2012C-24Aug2012-Data",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-p1-Data",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-p2-Data",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-p3-Data",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-p4-Data",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-p5-Data",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-p6-Data",False)
## treeSkim("nominal","Run2012C-PromptReco-v2-p7-Data",False)
## treeSkim("nominal","Run2012D-PromptReco-v1-p1-Data",False)
## treeSkim("nominal","Run2012D-PromptReco-v1-p2-Data",False)
## treeSkim("nominal","Run2012D-PromptReco-v1-p3-Data",False)
## treeSkim("nominal","Run2012D-PromptReco-v1-p4-Data",False)
## treeSkim("nominal","Run2012D-PromptReco-v1-p5-Data",False)
## treeSkim("nominal","Run2012D-PromptReco-v1-p6-Data",False)
## treeSkim("nominal","Run2012D-PromptReco-v1-p7-Data",False)
## treeSkim("nominal","Run2012D-PromptReco-v1-p8-Data",False)
## treeSkim("nominal","Run2012D-PromptReco-v1-p9-Data",False)
        
## treeSkim("nominal","Run2012A-22Jan2013-Data",False)
## treeSkim("nominal","Run2012B-22Jan2013-p1-Data",False)
## treeSkim("nominal","Run2012B-22Jan2013-p2-Data",False)
## treeSkim("nominal","Run2012B-22Jan2013-p3-Data",False)
## treeSkim("nominal","Run2012B-22Jan2013-p4-Data",False)
## treeSkim("nominal","Run2012B-22Jan2013-p5-Data",False)
## treeSkim("nominal","Run2012C-22Jan2013-p0-Data",False)
## treeSkim("nominal","Run2012C-22Jan2013-p1-Data",False)
## treeSkim("nominal","Run2012C-22Jan2013-p2-Data",False)
## treeSkim("nominal","Run2012C-22Jan2013-p3-Data",False)
## treeSkim("nominal","Run2012C-22Jan2013-p4-Data",False)
## treeSkim("nominal","Run2012C-22Jan2013-p5-Data",False)
## treeSkim("nominal","Run2012C-22Jan2013-p6-Data",False)
## treeSkim("nominal","Run2012C-22Jan2013-p7-Data",False)
## treeSkim("nominal","Run2012D-22Jan2013-p1-Data",False)
## treeSkim("nominal","Run2012D-22Jan2013-p2-Data",False)
## treeSkim("nominal","Run2012D-22Jan2013-p3-Data",False)
## treeSkim("nominal","Run2012D-22Jan2013-p4-Data",False)
## treeSkim("nominal","Run2012D-22Jan2013-p5-Data",False)
## treeSkim("nominal","Run2012D-22Jan2013-p6-Data",False)
## treeSkim("nominal","Run2012D-22Jan2013-p7-Data",False)
## treeSkim("nominal","Run2012D-22Jan2013-p8-Data",False)
## treeSkim("nominal","Run2012D-22Jan2013-p9-Data",False)

###Embedded samples
#anaEmb = ['nominal','TauUp','TauDown','MuUp','MuDown']
anaEmb = ['nominal','TauUp','TauDown']
#anaEmb = ['TauUp','TauDown','MuUp','MuDown']

samplesEmb = [
##     'Run2012A-06Aug2012-Embedded',
##     'Run2012A-13Jul2012-Embedded',
##     'Run2012B-13Jul2012-p1-Embedded',
##     'Run2012B-13Jul2012-p2-Embedded',
##     'Run2012B-13Jul2012-p3-Embedded',
##     'Run2012B-13Jul2012-p4-Embedded',
##     'Run2012B-13Jul2012-p5-Embedded',
##     'Run2012C-24Aug2012-Embedded',
##     'Run2012C-PromptReco-v2-p1-Embedded',
##     'Run2012C-PromptReco-v2-p2-Embedded',
##     'Run2012C-PromptReco-v2-p3-Embedded',
##     'Run2012C-PromptReco-v2-p4-Embedded',
##     'Run2012C-PromptReco-v2-p5-Embedded',
##     'Run2012C-PromptReco-v2-p6-Embedded',
##     'Run2012C-PromptReco-v2-p7-Embedded',    
##     'Run2012D-PromptReco-v1-p1-Embedded',
##     'Run2012D-PromptReco-v1-p2-Embedded',
##     'Run2012D-PromptReco-v1-p3-Embedded',
##     'Run2012D-PromptReco-v1-p4-Embedded',
##     'Run2012D-PromptReco-v1-p5-Embedded',
##     'Run2012D-PromptReco-v1-p6-Embedded',
##     'Run2012D-PromptReco-v1-p7-Embedded',
##     'Run2012D-PromptReco-v1-p8-Embedded',
##     'Run2012D-PromptReco-v1-p9-Embedded'
    
    'Run2012A-22Jan2013-Embedded',
    'Run2012B-22Jan2013-p1-Embedded',
    'Run2012B-22Jan2013-p2-Embedded',
    'Run2012B-22Jan2013-p3-Embedded',
    'Run2012B-22Jan2013-p4-Embedded',
    'Run2012B-22Jan2013-p5-Embedded',
    'Run2012C-22Jan2013-p0-Embedded',
    'Run2012C-22Jan2013-p1-Embedded',
    'Run2012C-22Jan2013-p2-Embedded',
    'Run2012C-22Jan2013-p3-Embedded',
    'Run2012C-22Jan2013-p4-Embedded',
    'Run2012C-22Jan2013-p5-Embedded',
    'Run2012C-22Jan2013-p6-Embedded',
    'Run2012C-22Jan2013-p7-Embedded',    
    'Run2012D-22Jan2013-p1-Embedded',
    'Run2012D-22Jan2013-p2-Embedded',
    'Run2012D-22Jan2013-p3-Embedded',
    'Run2012D-22Jan2013-p4-Embedded',
    'Run2012D-22Jan2013-p5-Embedded',
    'Run2012D-22Jan2013-p6-Embedded',
    'Run2012D-22Jan2013-p7-Embedded',
    'Run2012D-22Jan2013-p8-Embedded',
    'Run2012D-22Jan2013-p9-Embedded',
    'Run2012D-22Jan2013-p1-EmbeddedLowPt',
    'Run2012D-22Jan2013-p2-EmbeddedLowPt',
    'Run2012D-22Jan2013-p3-EmbeddedLowPt',
    'Run2012D-22Jan2013-p4-EmbeddedLowPt',
    'Run2012D-22Jan2013-p5-EmbeddedLowPt',
    'Run2012D-22Jan2013-p6-EmbeddedLowPt',
    'Run2012D-22Jan2013-p7-EmbeddedLowPt',
    'Run2012D-22Jan2013-p8-EmbeddedLowPt',
    'Run2012D-22Jan2013-p9-EmbeddedLowPt'
    ]
json    = [1,0,0,2,3]

for iSEmb in range(0,len(samplesEmb)):
    for iAnEmb in range(0,len(anaEmb)):
        treeSkim( anaEmb[iAnEmb] , samplesEmb[iSEmb],False )



##MC
anaMC = ['nominal','TauUp','TauDown','JetUp','JetDown']
#anaMC = ['nominal']
#anaMC = ['TauUp','TauDown','JetUp','JetDown']

ListFinalState = ['TauTau','JetToTau','MuToTau']

#for iAnMC in range(0,len(anaMC)):
##     ## BkgMC

##     ## DY
##     for finalState in ListFinalState:
##         treeSkim( anaMC[iAnMC] , "DYJets_"+finalState      , False)
##         treeSkim( anaMC[iAnMC] , "DYJets1Jets_"+finalState , False)
##         treeSkim( anaMC[iAnMC] , "DYJets2Jets_"+finalState , False)
##         treeSkim( anaMC[iAnMC] , "DYJets3Jets_"+finalState , False)
##         treeSkim( anaMC[iAnMC] , "DYJets4Jets_"+finalState , False)

##     ## W
##     treeSkim( anaMC[iAnMC] , "WJets1Jets"    , False)
##     treeSkim( anaMC[iAnMC] , "WJets2Jets"    , False)
##     treeSkim( anaMC[iAnMC] , "WJets3Jets"    , False)
##     treeSkim( anaMC[iAnMC] , "WJets4Jets"    , False)
##     treeSkim( anaMC[iAnMC] , "WJets-p1"      , False)
##     treeSkim( anaMC[iAnMC] , "WJets-p2"      , False)

##     treeSkim( anaMC[iAnMC] , "TTJets"        , False)
##     treeSkim( anaMC[iAnMC] , "T-tW"          , False)
##     treeSkim( anaMC[iAnMC] , "Tbar-tW"       , False)
##     treeSkim( anaMC[iAnMC] , "WWJetsTo2L2Nu" , False)
##     treeSkim( anaMC[iAnMC] , "WZJetsTo2L2Q"  , False)
##     treeSkim( anaMC[iAnMC] , "WZJetsTo3LNu"  , False)
##     treeSkim( anaMC[iAnMC] , "ZZJetsTo2L2Q"  , False)
##     treeSkim( anaMC[iAnMC] , "ZZJetsTo2L2Nu"  , False)
##     treeSkim( anaMC[iAnMC] , "ZZJetsTo4L"    , False)

##     ## HiggsMC
##     treeSkim( anaMC[iAnMC] , "GGFH90"        , False)
##     treeSkim( anaMC[iAnMC] , "GGFH95"        , False)
##     treeSkim( anaMC[iAnMC] , "GGFH100"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH105"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH110"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH115"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH120"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH125"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH130"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH135"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH140"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH145"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH150"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH155"       , False)
##     treeSkim( anaMC[iAnMC] , "GGFH160"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH90"        , False)
##     treeSkim( anaMC[iAnMC] , "VBFH95"        , False)
##     treeSkim( anaMC[iAnMC] , "VBFH100"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH105"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH110"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH115"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH120"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH125"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH130"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH135"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH140"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH145"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH150"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH155"       , False)
##     treeSkim( anaMC[iAnMC] , "VBFH160"       , False)
##     treeSkim( anaMC[iAnMC] , "VH90"          , False)
##     treeSkim( anaMC[iAnMC] , "VH95"          , False)
##     treeSkim( anaMC[iAnMC] , "VH100"         , False)
##     treeSkim( anaMC[iAnMC] , "VH105"         , False)
##     treeSkim( anaMC[iAnMC] , "VH110"         , False)
##     treeSkim( anaMC[iAnMC] , "VH115"         , False)
##     treeSkim( anaMC[iAnMC] , "VH120"         , False)
##     treeSkim( anaMC[iAnMC] , "VH125"         , False)
##     treeSkim( anaMC[iAnMC] , "VH130"         , False)
##     treeSkim( anaMC[iAnMC] , "VH135"         , False)
##     treeSkim( anaMC[iAnMC] , "VH140"         , False)
##     treeSkim( anaMC[iAnMC] , "VH145"         , False)
##     treeSkim( anaMC[iAnMC] , "VH150"         , False)
##     treeSkim( anaMC[iAnMC] , "VH155"         , False)
##     treeSkim( anaMC[iAnMC] , "VH160"         , False)

##     #### MSSM Higgs MC
##     treeSkim( anaMC[iAnMC] , "SUSYGGH80"         , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH90"         , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH100"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH110"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH120"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH130"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH140"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH160"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH180"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH200"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH250"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH300"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH350"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH400"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH450"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH500"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH600"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH700"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH800"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH900"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYGGH1000"       , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH80"         , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH90"         , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH100"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH110"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH120"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH130"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH140"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH160"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH180"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH200"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH250"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH300"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH350"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH400"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH450"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH500"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH600"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH700"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH800"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH900"        , False)
##     treeSkim( anaMC[iAnMC] , "SUSYBBH1000"       , False)
