#!/usr/bin/env python

import commands
import re
import os

import sys
sys.path.append('./')

###########################################
###########################################

def treeSkim( ana, sample, runInSeries=False):

    os.system( 'mkdir batch/' )

    #stream = "ElecTau"
    if(re.search("MuTau",sample)!=None):
        stream = "MuTau"
    else:
        stream = "ElecTau"
    print "Stream ", stream

    os.system('python makeTreeSkimmerElecTau.py')
    
    if runInSeries:
         print "Running in series via the command ..."
         configFileName = "./Configs/treeSkimmerElecTau_%s_%s_cfg.py" % (sample,ana)
         runInSeries   = 'treeSkimmer'+stream+'_MVA_53X '+configFileName+'\n'
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
        f.write('export WORKINGDIR="/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_4_Oct12/src/LLRAnalysis/Limits/bin/"\n')
        f.write('')
        f.write('cd $WORKINGDIR\n')
        f.write('export SCRAM_ARCH=slc5_amd64_gcc462\n')
        f.write('source /opt/exp_soft/cms/cmsset_default.sh\n')
        f.write('eval `scram runtime -sh`\n')
        f.write('source /opt/exp_soft/cms/cms_ui_env_crab.sh\n')
        ##
        configFileName = "./Configs/treeSkimmerElecTau_%s_%s_cfg.py" % (sample,ana)
        f.write('treeSkimmer'+stream+'_MVA_53X '+configFileName+' > '+fileLog+'\n')
        f.close()
        os.system('chmod u+x batch/*.sh')

  
###########################################
###########################################
##Data
## treeSkim("nominal","Run2012A-13Jul2012-Data",False)
#treeSkim("nominal","Run2012B-13Jul2012-Data",False)
#treeSkim("nominal","Run2012C-24Aug2012-Data",False)
## treeSkim("nominal","Run2012C-24Aug2012-v2",False)
#treeSkim("nominal","Run2012C-PromptReco-v2-Data",False)
treeSkim("nominal","Run2012C-PromptReco-v2-Residual-Data",False)

###Embedded samples
anaEmb = ['nominal','TauUp','TauDown','ElecUp','ElecDown']

samplesEmb = [
##     'Run2012A-06Aug2012-Embedded',
##     'Run2012A-13Jul2012-Embedded',
##     'Run2012B-13Jul2012-Embedded',
##     'Run2012C-24Aug2012-Embedded',
##     'Run2012C-PRV2-Embedded'
    ]
json    = [1,0,0,2,3]

for iSEmb in range(0,len(samplesEmb)):
    for iAnEmb in range(0,len(anaEmb)):
        treeSkim( anaEmb[iAnEmb] , samplesEmb[iSEmb],False )



##MC
anaMC = ['nominal','TauUp','TauDown']

#for iAnMC in range(0,len(anaMC)):
    ##BkgMC
##         treeSkim( anaMC[iAnMC] , "DYJets"        , 3504  * 1.0 * 1249322/24330458. , False)
##         treeSkim( anaMC[iAnMC] , "WJets1Jets"        , 36257    * 1.0 * 344731./22491598 , False)
##         treeSkim( anaMC[iAnMC] , "WJets2Jets",False)
##         treeSkim( anaMC[iAnMC] , "WJets3Jets"        , 36257    * 1.0 * 341616./6117001. , False)
##         treeSkim( anaMC[iAnMC] , "WJets4Jets"     , 36257     * 1.0 * 574303./7314266 , False)
##         treeSkim( anaMC[iAnMC] , "WJets-p1"      , 36257 * 1.0 * 0.00634753521127 , False)
##         treeSkim( anaMC[iAnMC] , "WJets-p2"      , 36257 * 1.0 * 0.00636491514722 , False)
##         treeSkim( anaMC[iAnMC] , "TTJets"        , 225.5 * 1.0 * 510296./5423750. , False)
##         treeSkim( anaMC[iAnMC] , "T-tW"          , 11.1  * 1.0 * 0.081518021248 , False)
##         treeSkim( anaMC[iAnMC] , "Tbar-tW"       , 11.1  * 1.0 * 0.0822555699536 , False)
##         treeSkim( anaMC[iAnMC] , "WWJetsTo2L2Nu" , 5.824 * 1.0 *  102870./1263235 , False)
##         treeSkim( anaMC[iAnMC] , "WZJetsTo2L2Q"  , 2.207 * 1.0 * 0.127811802097 , False)
##         treeSkim( anaMC[iAnMC] , "WZJetsTo3LNu"  , 1.058 * 1.0 * 0.14055 , False)
##         treeSkim( anaMC[iAnMC] , "ZZJetsTo2L2Q"  , 2.502 * 1.0 * 0.145166994064 , False)

##     ##HiggsMC
##         treeSkim( anaMC[iAnMC] , "GGFH110" , 25.04                   * 7.95E-02 * 10820./399907.   , False)
##         treeSkim( anaMC[iAnMC] , "GGFH115" , 22.96                   * 7.58E-02 * 14186./480230.   , False)
##         treeSkim( anaMC[iAnMC] , "GGFH120" , 21.13                   * 7.04E-02 * 141092./4379561. , False)
##         treeSkim( anaMC[iAnMC] , "GGFH125" , 19.52                   * 6.32E-02 * 23954./688134.   , False)
##         treeSkim( anaMC[iAnMC] , "GGFH130" , 18.07                   * 5.45E-02 * 37717./989921.   , False)
##         treeSkim( anaMC[iAnMC] , "GGFH135" , 16.79                   * 4.49E-02 * 29569./720729.   , False)
##         treeSkim( anaMC[iAnMC] , "GGFH140" , 15.63                   * 3.52E-02 * 19042./441198.   , False)
##         treeSkim( anaMC[iAnMC] , "GGFH145" , 14.59                   * 2.61E-02 * 39504./852119.   , False)
##         treeSkim( anaMC[iAnMC] , "VBFH110" , 1.809                   * 7.95E-02 * 22753./414385.   , False)
##         treeSkim( anaMC[iAnMC] , "VBFH115" , 1.729                   * 7.58E-02 * 56912./993870.   , False)
##         treeSkim( anaMC[iAnMC] , "VBFH120" , 1.649                   * 7.04E-02 * 170419./2834487. , False)
##         treeSkim( anaMC[iAnMC] , "VBFH125" , 1.578                   * 6.32E-02 * 61421./978836.   , False)
##         treeSkim( anaMC[iAnMC] , "VBFH130" , 1.511                   * 5.45E-02 * 50458./773790.   , False)
##         treeSkim( anaMC[iAnMC] , "VBFH135" , 1.448                   * 4.49E-02 * 67382./993817.   , False)
##         treeSkim( anaMC[iAnMC] , "VBFH140" , 1.389                   * 3.52E-02 * 34934./499496.   , False)
##         treeSkim( anaMC[iAnMC] , "VBFH145" , 1.333                   * 2.61E-02 * 67190./918883.   , False)
##         treeSkim( anaMC[iAnMC] , "VH110"   , (1.060+0.5869+0.1887)   * 7.95E-02 * 16792./195408.   , False)
##         treeSkim( anaMC[iAnMC] , "VH115"   , (0.9165+0.5117+0.1663)  * 7.58E-02 * 17146./190460.   , False)
##         treeSkim( anaMC[iAnMC] , "VH120"   , (0.7966+0.4483+0.1470)  * 7.04E-02 * 16147./170288.   , False)
##         treeSkim( anaMC[iAnMC] , "VH125"   , (0.6966+0.3943+0.1302)  * 6.32E-02 * 19299./195124.   , False)
##         treeSkim( anaMC[iAnMC] , "VH130"   , (0.6095+0.3473+0.1157)  * 5.45E-02 * 20383./200080.   , False)
##         treeSkim( anaMC[iAnMC] , "VH135"   , (0.5351+0.3074+0.1031)  * 4.49E-02 * 17609./165128.   , False)
##         treeSkim( anaMC[iAnMC] , "VH140"   , (0.4713+0.2728+0.09207) * 3.52E-02 * 18328./165186.   , False)
##         treeSkim( anaMC[iAnMC] , "VH145"   , (0.4164+0.2424+0.08246) * 2.61E-02 * 22184./195384.   , False)


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
