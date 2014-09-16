#!/usr/bin/env python

from LLRAnalysis.HadTauStudies.recoSampleDefinitionsAHtoTauTau_8TeV_grid_cfi import *

import copy
import os
import re
import time

jobId = '2014Jun09'

version = "v3_06"

inputFilePath  = "/data2/veelken/CMSSW_5_3_x/Ntuples/AHtoTauTau/%s/%s" % (jobId, version)

outputFilePath = "/data1/veelken/tmp/tauTauAnalysis2b2tau/%s_1/" % version

_picobarns =  1.0
_femtobarns = 1.0e-3

intLumiData = recoSampleDefinitionsAHtoTauTau_8TeV['TARGET_LUMI']
print "intLumiData = %1.1f fb^-1" % (recoSampleDefinitionsAHtoTauTau_8TeV['TARGET_LUMI']*_femtobarns)

lumiScale_DY = 2.12
stitchingWeights_DY = [ 1.0, 0.412/lumiScale_DY, 0.167/lumiScale_DY, 0.097/lumiScale_DY, 0.0759/lumiScale_DY ]

lumiScale_DY_noTauPolarization = 2.18
stitchingWeights_DY_noTauPolarization = [ 1.0, 1.0, 1.0, 1.0, 1.0 ]

lumiScale_W = 8.83
stitchingWeights_W = [ 1.0, 1.80/lumiScale_W, 0.559/lumiScale_W, 0.357/lumiScale_W, 0.340/lumiScale_W ] # CV: with "Ext" samples
##stitchingWeights_W = [ 1.0, 3.25/lumiScale_W, 1.06/lumiScale_W, 0.681/lumiScale_W, 0.340/lumiScale_W ] # CV: without "Ext" samples

def getLumiScale(sample, x_sec = -1.):
    if x_sec < 0.:
        x_sec = recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['x_sec']    
    return intLumiData*x_sec/recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['events_processed']

samples = {
    #----------------------------------------------------------------------------
    # CV: for checking event yield in individual data-taking periods
##    'Data2012runA' : {
##        'processes' : [ "data2012runA" ],
##        'inputFiles' : [
##            "data_Run2012A_22Jan2013_v1"
##        ]
##    },
##    'Data2012runB' : {
##        'processes' : [ "data2012runB" ],
##        'inputFiles' : [
##            "data_Run2012B_22Jan2013_v1"
##        ]
##    },
##    'Data2012runC' : {
##        'processes' : [ "data2012runC" ],
##        'inputFiles' : [
##            "data_Run2012C_22Jan2013_v1"
##        ]
##    },
##    'Data2012runD' : {
##        'processes' : [ "data2012runD" ],
##        'inputFiles' : [
##            "data_Run2012D_22Jan2013_v1"
##        ]
##    },
    #----------------------------------------------------------------------------
    'Data_Tau' : {
        'processes' : [ "data_obs" ],
        'inputFiles' : [
            ##"data_Tau_Run2012A_22Jan2013_v1",
            "data_TauParked_Run2012B_22Jan2013_v1",
            "data_TauParked_Run2012C_22Jan2013_v1",
            "data_TauParked_Run2012D_22Jan2013_v1",
        ]
    },
    'Data_Jet' : {
        'processes' : [ "data_obs" ],
        'inputFiles' : [
            ##"data_Jet_Run2012A_22Jan2013ReReco_v1",
            "data_JetHT_Run2012B_22Jan2013ReReco_v1",
            "data_JetHT_Run2012C_22Jan2013ReReco_v1",
            "data_JetHT_Run2012D_22Jan2013ReReco_v1"
        ]
    },
    'DYJets' : {
        'processes' : [ "ZL", "ZJ" ],
        ##'processes' : [ "ZL", "ZJ", "ZTTmc" ],
        'inputFiles' : [
            "DYJets",
            "DY1Jets",
            "DY2JetsExt",
            "DY3Jets",
            "DY4Jets"
        ],
        'lumiScale' : lumiScale_DY,
        'stitchingWeights' : stitchingWeights_DY,
        'addWeights' : []
    },
    'DYJets_noTauPolarization' : {
        'processes' : [ "ZTTmc" ],
        'inputFiles' : [
            "DYJets_noTauPolarization"
        ],
        'lumiScale' : lumiScale_DY_noTauPolarization,
        'stitchingWeights' : stitchingWeights_DY_noTauPolarization,
        'addWeights' : [ 'tauSpin' ]
    },
    'WJets' : {
        'processes' : [ "Wtmp" ],
        'inputFiles' : [
            "WJets",
            "WJetsExt",
            "W1Jets",
            "W1JetsExt",
            "W2Jets",
            "W2JetsExt",
            "W3Jets",
            "W3JetsExt",
            "W4Jets"
        ],
        'lumiScale' : lumiScale_W,
        'stitchingWeights' : stitchingWeights_W,
        'applyJetToTauFakeRateCorrection' : True
    },
    'DYJets_Embedded' : {
        'processes' : [ "ZTT_Embedded" ],
        'inputFiles' : [
            ##"pfEmbed_Run2012A_22Jan2013_v1",
            "pfEmbed_Run2012B_22Jan2013_v1",
            "pfEmbed_Run2012C_22Jan2013_v1",
            "pfEmbed_Run2012D_22Jan2013_v1"
        ],
        'lumiScale'  : 1.0
    },
    'TTJets_Embedded' : {
        'processes' : [ "TT_Embedded" ],
        'inputFiles' : [ "pfEmbed_TTJetsFullLept" ],
        'lumiScale' : getLumiScale('TTJetsFullLept'),
        'addWeights' : [ "topPtWeightNom" ]
    },
    'HiggsGGH125' : {
        'processes' : [ "singleH_SM125" ],
        'inputFiles' : [ "HiggsGGH125" ],
        'lumiScale' : getLumiScale('HiggsGGH125')
    },
    'HiggsVBF125' : {
        'processes' : [ "singleH_SM125" ],
        'inputFiles' : [ "HiggsVBF125" ],
        'lumiScale' : getLumiScale('HiggsVBF125')
    },
    'HiggsVH125' : {
        'processes' : [ "singleH_SM125" ],
        'inputFiles' : [ "HiggsVH125" ],
        'lumiScale' : getLumiScale('HiggsVH125')
    },
    'HiggsZHbb125' : {
        'processes' : [ "singleH_SM125" ],
        'inputFiles' : [ "HiggsZHbb125" ],
        'lumiScale' : getLumiScale('HiggsZHbb125')
    },
    'HiggsWHbb125' : {
        'processes' : [ "singleH_SM125" ],
        'inputFiles' : [ "HiggsWHbb125" ],
        'lumiScale' : getLumiScale('HiggsWHbb125')
    }
}
for sample in [ "TTJetsHadronic", "TTJetsSemiLept", "TTJetsFullLept" ]:
    samples[sample] = {
        'processes' : [ "TT" ],
        'inputFiles' : [ sample ],
        'lumiScale' : getLumiScale(sample),
        'addWeights' : [ "topPtWeightNom" ]
    }
for sample in [ "WWJetsTo2L2Nu", "WZJetsTo3LNu", "WZJetsTo2L2Q", "ZZJetsTo4L", "ZZJetsTo2L2Nu", "ZZJetsTo2L2Q", "Tbar_tW", "T_tW" ]:
    samples[sample] = {
        'processes' : [ "VV" ],
        'inputFiles' : [ sample ],
        'lumiScale' : getLumiScale(sample)
    }
samples["hhTo2b2tau_v2"] = {
    'processes' : [ "hhTo2b2tau" ],
    'inputFiles' : [ "hhTo2b2tau_v2" ],
    'lumiScale' : getLumiScale("hhTo2b2tau_v2", 1.*_picobarns), # CV: normalize all signal MCs to cross-section of 1pb
    'addWeights' : []
}
mssmHtohhMassPoints = [ 260, 300, 350 ]
for massPoint in mssmHtohhMassPoints:
    mssmHtohhSampleName = "mssmH%1.0ftohh" % massPoint
    samples[mssmHtohhSampleName] = {
        'processes' : [ "mssmH%1.0ftohh" % massPoint ] ,
        'inputFiles' : [ mssmHtohhSampleName ],
        'lumiScale' : getLumiScale(mssmHtohhSampleName, 1.*_picobarns), # CV: normalize all signal MCs to cross-section of 1pb
        'addWeights' : []
    }
##abelianZprimeTohhMassPoints = [ 300, 500, 700 ]
##for massPoint in abelianZprimeTohhMassPoints:
##    abelianZprimeTohhSampleName = "abelianZprime%1.0ftohh" % massPoint
##    samples[abelianZprimeTohhSampleName] = {
##        'processes' : [ "abelianZprime%1.0ftohh" % massPoint ] ,
##        'inputFiles' : [ abelianZprimeTohhSampleName ],
##        'lumiScale' : getLumiScale(abelianZprimeTohhSampleName, 1.*_picobarns), # CV: normalize all signal MCs to cross-section of 1pb
##        'addWeights' : []
##    }
gravitonTohhMassPoints = [ 270, 300, 500, 700, 1000 ]
for massPoint in gravitonTohhMassPoints:
    gravitonTohhSampleName = "graviton%1.0fTohh" % massPoint
    samples[gravitonTohhSampleName] = {
        'processes' : [ "graviton%1.0fTohh" % massPoint ] ,
        'inputFiles' : [ gravitonTohhSampleName ],
        'lumiScale' : getLumiScale(gravitonTohhSampleName, 1.*_picobarns), # CV: normalize all signal MCs to cross-section of 1pb
        'addWeights' : []
    }
radionTohhMassPoints = [ 300, 500, 700, 1000 ]
for massPoint in radionTohhMassPoints:
    radionTohhSampleName = "radion%1.0fTohh" % massPoint
    samples[radionTohhSampleName] = {
        'processes' : [ "radion%1.0fTohh" % massPoint ] ,
        'inputFiles' : [ radionTohhSampleName ],
        'lumiScale' : getLumiScale(radionTohhSampleName, 1.*_picobarns), # CV: normalize all signal MCs to cross-section of 1pb
        'addWeights' : []
    }

def makeJetToTauFakeRateCorrection(par0, par1, par2, par3, x0 = 163.7):
    retVal = "%f + 0.01*(x - %f)*%f + TMath::Power(0.01*(x - %f), 2.)*%f + TMath::Power(0.01*(x - %f), 3.)*%f" % (par0, x0, par1, x0, par2, x0, par3)
    return retVal

discriminators = {
    '2tauL' : {
        'tau1Selection' : {
            'iso'     : "l1LooseMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
            'relaxed' : "l1VLooseMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5 && !(l1LooseMVAwLT > 0.5)"
        },
        'tau2Selection' : {
            'iso'     : "l2LooseMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5 && !(l1TightMVAwLT > 0.5 && l2TightMVAwLT > 0.5)",
        },
        'tau1FRwEtaBins'             : [ -1., 1.2, 1.7, 9.9 ],
        'tau2FRwEtaBins'             : [ -1., 9.9 ],
        # jetToTauFakeRateCorrectiontaken from https://indico.cern.ch/event/304725/contribution/1/material/slides/0.pdf
        'jetToTauFakeRateCorrection' : makeJetToTauFakeRateCorrection(7.58704e-1, -1.57025e-1, -2.40635e-2, -8.24741e-2),
        'applyTauTriggerTurnOn'      : "MVAwLToldDMsLoose"
    },
    '2tauT' : {
        'tau1Selection' : {
            'iso'     : "l1TightMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
            'relaxed' : "l1VLooseMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5 && !(l1TightMVAwLT > 0.5)"
        },
        'tau2Selection' : {
            'iso'     : "l2TightMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5",
        },
        'tau1FRwEtaBins'             : [ -1., 1.2, 1.7, 9.9 ],
        'tau2FRwEtaBins'             : [ -1., 9.9 ],
        # jetToTauFakeRateCorrectiontaken from https://indico.cern.ch/event/304725/contribution/1/material/slides/0.pdf
        'jetToTauFakeRateCorrection' : makeJetToTauFakeRateCorrection(7.58704e-1, -1.57025e-1, -2.40635e-2, -8.24741e-2),
        'applyTauTriggerTurnOn'      : "MVAwLToldDMsTight"
    }
}

bJet1FRwEtaBins = [ -1., 1.0, 2.0, 9.9 ]

central_or_shifts = {
    '' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    },
    'CMS_scale_t_tautau_8TeVUp' : {
        'inputFilePath_extension' : "up",
        'addWeights_extension'    : []
    },
    'CMS_scale_t_tautau_8TeVDown' : {
        'inputFilePath_extension' : "down",
        'addWeights_extension'    : []
    },
    'CMS_eff_t_mssmHigh_tautau_8TeVUp' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    },
    'CMS_eff_t_mssmHigh_tautau_8TeVDown' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    },
    'CMS_eff_b_8TeVUp' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    },
    'CMS_eff_b_8TeVDown' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    },
    'CMS_fake_b_8TeVUp' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    },
    'CMS_fake_b_8TeVDown' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    },
    'CMS_htt_ttbarPtReweight_8TeVUp' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : [ "topPtWeightUp" ]
    },
    'CMS_htt_ttbarPtReweight_8TeVDown' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : [ "topPtWeightDown" ]
    },
    'CMS_htt_WShape_tautau_8TeVUp' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    },
    'CMS_htt_WShape_tautau_8TeVDown' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    }
}

signalRegions = [
    "SSrelaxed1FRw2ndTauTight_iso2",
    "OSrelaxed1FRw2ndTauTight_iso2",
    "OSiso1_iso2"
]
qcdRegions = [
    "SSrelaxed1_iso2",
    "SSiso1_iso2"
]
regions = []
regions.extend(signalRegions)
regions.extend(qcdRegions)

execDir = "%s/bin/%s/" % (os.environ['CMSSW_BASE'], os.environ['SCRAM_ARCH'])

executable_FWLiteTauTauAnalyzer2b2tau   = execDir + 'FWLiteTauTauAnalyzer2b2tau'
executable_determineJetToTauFakeRate    = execDir + 'determineJetToTauFakeRate'
executable_addBackgroundZTT             = execDir + 'addBackgroundZTT'
executable_addBackgroundQCD             = execDir + 'addBackgroundQCD'
executable_addBackgroundW               = execDir + 'addBackgroundW'
executable_prepareTauTauDatacards2b2tau = execDir + 'prepareTauTauDatacards2b2tau'
executable_makeTauTauPlots2b2tau        = execDir + 'makeTauTauPlots2b2tau'
executable_hadd                         = 'hadd'
executable_rm                           = 'rm -f'

##nice = 'nice '
nice = ''

configFile_FWLiteTauTauAnalyzer2b2tau             = 'FWLiteTauTauAnalyzer2b2tau_cfg.py'
configFile_determineJetToTauFakeRate              = 'determineJetToTauFakeRate_cfg.py'
configFile_addBackgroundZTT                       = 'addBackgroundZTT_cfg.py'
configFile_addBackgroundQCDfromSSantiiso          = 'addBackgroundQCDfromSSantiiso_cfg.py' # for datacards (QCD shape templates taken from SS anti-isolated region, weighted by jet -> tau fake-rate)
configFile_addBackgroundQCDfromSSiso              = 'addBackgroundQCDfromSSiso_cfg.py'     # for control plots of tau ID observables (QCD shape templates taken from SS isolated region)
configFile_addBackgroundW                         = 'addBackgroundW_cfg.py'
configFile_prepareTauTauDatacards2b2tau           = 'prepareTauTauDatacards2b2tau_cfg.py'
configFile_makeTauTauPlots2b2tau_QCDfromSSiso     = 'makeTauTauPlots2b2tau_QCDfromSSiso_cfg.py'
configFile_makeTauTauPlots2b2tau_QCDfromSSantiiso = 'makeTauTauPlots2b2tau_QCDfromSSantiiso_cfg.py'

# create outputFilePath in case it does not yet exist
def createFilePath_recursively(filePath):
    filePath_items = filePath.split('/')
    currentFilePath = "/"
    for filePath_item in filePath_items:
        currentFilePath = os.path.join(currentFilePath, filePath_item)
        if len(currentFilePath) <= 1:
            continue
        if not os.path.exists(currentFilePath):
            os.mkdir(currentFilePath)

if not os.path.isdir(outputFilePath):
    print "outputFilePath does not yet exist, creating it."
    createFilePath_recursively(outputFilePath)

def getInputFileNames(inputFilePath):
    inputFileNames = []
    try:
        inputFileNames = [ os.path.join(inputFilePath, file) for file in os.listdir(inputFilePath) ]
    except OSError:
        print "inputFilePath = %s does not exist --> skipping !!" % inputFilePath
    return inputFileNames

def getStringRep_bool(flag):
    retVal = None
    if flag:
        retVal = "True"
    else:
        retVal = "False"
    return retVal

def getStringRep_vdouble(collection):
    retVal = ""
    for item in collection:
        if len(retVal) > 0:
            retVal += ", "
        retVal += "%f" % item
    return retVal

def getStringRep_vstring(collection):
    retVal = ""
    for item in collection:
        if len(retVal) > 0:
            retVal += ", "
        retVal += "'%s'" % item
    return retVal

def addWeights_shift_and_remove_central(addWeights_central, addWeights_extension_shift):
    retVal = []
    for addWeight_shift in addWeights_extension_shift:
        if addWeight_shift.find("->") == -1:
            retVal.append(addWeight_shift)
    for addWeight_central in addWeights_central:
        matchesShift = False
        for addWeight_shift in addWeights_extension_shift:
            systematic = addWeight_shift
            systematic = systematic.rstrip("Up")
            systematic = systematic.rstrip("Down")
            if addWeight_central == systematic or addWeight_central == (systematic + "Nom"):
                matchesShift = True
        if not matchesShift:
            retVal.append(addWeight_central)
    return retVal

def initDict(dictionary, keys):
    dictionary_at_keylevel = dictionary
    numKeys = len(keys)
    for idxKey in range(numKeys - 1):
        key = keys[idxKey]
        if key not in dictionary_at_keylevel.keys():
            dictionary_at_keylevel[key] = {}
        dictionary_at_keylevel = dictionary_at_keylevel[key]

def getParticleEtaLabel(particleType, particle1EtaMin, particle1EtaMax, particle2EtaMin, particle2EtaMax):
    particleEtaBin_label = ""
    if particle1EtaMin > 0. and particle1EtaMax < 5.:
        particleEtaBin_label += "%s1Eta%1.1fto%1.1f" % (particleType, particle1EtaMin, particle1EtaMax)
    elif particle1EtaMin > 0.:
        particleEtaBin_label += "%s1EtaGt%1.1f" % (particleType, particle1EtaMin)
    elif particle1EtaMax < 5.:
        particleEtaBin_label += "%s1EtaLt%1.1f" % (particleType, particle1EtaMax)
    if particle2EtaMin > 0. and particle2EtaMax < 5.:
        particleEtaBin_label += "%s2Eta%1.1fto%1.1f" % (particleType, particle2EtaMin, particle2EtaMax)
    elif particle2EtaMin > 0.:
        particleEtaBin_label += "%s2EtaGt%1.1f" % (particleType, particle2EtaMin)
    elif particle2EtaMax < 5.:
        particleEtaBin_label += "%s2EtaLt%1.1f" % (particleType, particle2EtaMax)
    particleEtaBin_label = particleEtaBin_label.replace(".", "")
    return particleEtaBin_label

def runCommand(command):
    print "Executing '%s'" % command
    print " It is now: %s" % time.strftime("%c")
    os.system(command)

#--------------------------------------------------------------------------------
# CV: declare outputFileNames of determineJetToTauFakeRate macro in advance,
#     so that files can be used as input when  running FWLiteTauTauAnalyzer macro

hadd_determineJetToTauFakeRate_outputFileNames = {} # key = discriminator
for discriminator in discriminators.keys():
    hadd_determineJetToTauFakeRate_outputFileNames[discriminator] = os.path.join(outputFilePath, discriminator, "determineJetToTauFakeRate_%s.root" % discriminator)
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running FWLiteTauTauAnalyzer macro
#
print "Info: building config files for FWLiteTauTauAnalyzer2b2tau macro"
FWLiteTauTauAnalyzer2b2tau_inputFileNames  = {} # key = sample, process, central_or_shift, region, discriminator
FWLiteTauTauAnalyzer2b2tau_configFileNames = {} # key = sample, process, central_or_shift, region, discriminator
FWLiteTauTauAnalyzer2b2tau_outputFileNames = {} # key = sample, process, central_or_shift, region, discriminator
FWLiteTauTauAnalyzer2b2tau_logFileNames    = {} # key = sample, process, central_or_shift, region, discriminator
for sample in samples.keys():        
    print "processing sample = %s" % sample
    isSignalMC = None
    if sample in recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'].keys():
        isSignalMC = recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['type'] == "bsmMC"
    else:
        isSignalMC = False
    for process in samples[sample]['processes']:
        for region in regions:
            if region not in [ "OSiso1_iso2", "SSiso1_iso2" ] and isSignalMC:
                continue
            central_or_shifts_region = copy.deepcopy(central_or_shifts)
            if region.find("relaxed1FRw") != -1 or region.find("relaxed2FRw") != -1:
                tauFR_fitFunctionNormName_Up   = None
                tauFR_fitFunctionNormName_Down = None
                if region.find("relaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight") != -1 or region.find("relaxed1FRw2ndTauTight_relaxed2FRw1stTauLoose") != -1:                    
                    tauFR_fitFunctionNormName_Up   = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormUp_SSiso1_iso2_div_SSrelaxed1_relaxed2"
                    tauFR_fitFunctionNormName_Down = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormDown_SSiso1_iso2_div_SSrelaxed1_relaxed2"
                elif region.find("iso1_relaxed2FRw1stTauTight") != -1:                    
                    tauFR_fitFunctionNormName_Up   = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormUp_SSiso1_iso2_div_SSiso1_relaxed2"
                    tauFR_fitFunctionNormName_Down = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormDown_SSiso1_iso2_div_SSiso1_relaxed2"
                elif region.find("relaxed1FRw2ndTauTight_iso2") != -1:                    
                    tauFR_fitFunctionNormName_Up   = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormUp_SSiso1_iso2_div_SSrelaxed1_iso2"
                    tauFR_fitFunctionNormName_Down = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormDown_SSiso1_iso2_div_SSrelaxed1_iso2"
                else:
                    raise ValueError("No fake-rate weights defined for region = '%s' !!" % region)
                central_or_shifts_region.update({
                    'CMS_htt_QCDfrNorm_tautau_8TeVUp' : {
                        'inputFilePath_extension'    : "nom",
                        'addWeights_extension'       : [],
                        'fitFunctionNormName'        : tauFR_fitFunctionNormName_Up
                    },
                    'CMS_htt_QCDfrNorm_tautau_8TeVDown' : {
                        'inputFilePath_extension'    : "nom",
                        'addWeights_extension'       : [] ,
                        'fitFunctionNormName'        : tauFR_fitFunctionNormName_Down
                    },
                    'CMS_htt_QCDfrShape_tautau_8TeVUp' : {
                        'inputFilePath_extension'    : "nom",
                        'addWeights_extension'       : [],
                        'fitFunctionShapePower_tau1' : 2.0,
                        'fitFunctionShapePower_tau2' : 0.
                    },
                    'CMS_htt_QCDfrShape_tautau_8TeVDown' : {
                        'inputFilePath_extension'    : "nom",
                        'addWeights_extension'       : [],
                        'fitFunctionShapePower_tau1' : 0.,
                        'fitFunctionShapePower_tau2' : 0.
                    }})                           
            for central_or_shift in central_or_shifts_region.keys():

                if (central_or_shift.find('CMS_htt_QCDfrNorm_tautau_8TeV') != -1 or central_or_shift.find('CMS_htt_QCDfrShape_tautau_8TeV') != -1) and isSignalMC:
                    continue
                if central_or_shift.find('CMS_htt_ttbarPtReweight_8TeV') != -1 and not sample in [ "TTJetsHadronic", "TTJetsSemiLept", "TTJetsFullLept", "TTJets_Embedded" ]:
                    continue
                if central_or_shift.find('CMS_htt_WShape_tautau_8TeV') != -1 and not sample in [ "WJets", "WJetsExt", "W1Jets", "W2Jets", "W3Jets", "W4Jets" ]:
                    continue
                if len(central_or_shift) > 0 and central_or_shift.find('central') == -1 and sample.find("Data") != -1:
                    continue

                inputFileNames = []
                for inputFilePath_extension_sample in samples[sample]['inputFiles']:
                    inputFileNames_sample = getInputFileNames(os.path.join(inputFilePath, central_or_shifts_region[central_or_shift]['inputFilePath_extension'], inputFilePath_extension_sample))
                    inputFileNames.extend(inputFileNames_sample)
                if len(inputFileNames) == 0:
                    raise ValueError("Failed to find input files for sample = '%s' !!" % sample)
                ##if central_or_shift == "" or central_or_shift == "central":
                print " central_or_shift = '%s': inputFileNames = %s" %  (central_or_shift, inputFileNames)

                for discriminator in discriminators.keys():

                    jetToTauFakeRateCorrection = discriminators[discriminator]['jetToTauFakeRateCorrection']
                    if central_or_shift == 'CMS_htt_WShape_tautau_8TeVUp':
                        jetToTauFakeRateCorrection = "1.0"
                    elif central_or_shift == 'CMS_htt_WShape_tautau_8TeVDown':
                        jetToTauFakeRateCorrection = "TMath::Power(%s, 2.)" % jetToTauFakeRateCorrection

                    initDict(FWLiteTauTauAnalyzer2b2tau_inputFileNames, [ sample, process, central_or_shift, region, discriminator ])
                    FWLiteTauTauAnalyzer2b2tau_inputFileNames[sample][process][central_or_shift][region][discriminator] = inputFileNames
                    FWLiteTauTauAnalyzer2b2tau_inputFileNames[sample][process][central_or_shift][region][discriminator] = []
                    # CV: declare dependency on output of determineJetToTauFakeRate macro
                    if region.find("relaxed1FRw") != -1 or region.find("relaxed2FRw") != -1:
                        FWLiteTauTauAnalyzer2b2tau_inputFileNames[sample][process][central_or_shift][region][discriminator].append(
                          hadd_determineJetToTauFakeRate_outputFileNames[discriminator])

                    outputFilePath_subdir = os.path.join(outputFilePath, discriminator, sample, process, region)
                    createFilePath_recursively(outputFilePath_subdir)
                    outputFileName = os.path.join(outputFilePath_subdir, "FWLiteTauTauAnalyzer2b2tau_%s_%s_%s_%s_%s.root" % (discriminator, sample, process, region, central_or_shift))
                    outputFileName = outputFileName.replace("__", "_")
                    outputFileName = outputFileName.replace("_.", ".")
                    print " region = %s: outputFileName = '%s'" % (region, outputFileName)
                    initDict(FWLiteTauTauAnalyzer2b2tau_outputFileNames, [ sample, process, central_or_shift, region, discriminator ])
                    FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process][central_or_shift][region][discriminator] = outputFileName

                    cfgFileName_original = configFile_FWLiteTauTauAnalyzer2b2tau
                    cfgFile_original = open(cfgFileName_original, "r")
                    cfg_original = cfgFile_original.read()
                    cfgFile_original.close()
                    cfg_modified  = cfg_original
                    cfg_modified += "\n"
                    cfg_modified += "process.fwliteInput.fileNames = cms.vstring()\n"
                    for inputFileName in inputFileNames:
                        cfg_modified += "process.fwliteInput.fileNames.append('%s')\n" % inputFileName
                    cfg_modified += "\n"
                    cfg_modified += "process.fwliteOutput.fileName = cms.string('%s')\n" % outputFileName
                    cfg_modified += "\n"
                    if central_or_shifts_region[central_or_shift]['inputFilePath_extension'] == "up":
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.treeName = cms.string('tauTauNtupleProducerTauUp/H2TauTauTreeProducerTauTau')\n"
                    elif central_or_shifts_region[central_or_shift]['inputFilePath_extension'] == "down":
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.treeName = cms.string('tauTauNtupleProducerTauDown/H2TauTauTreeProducerTauTau')\n"
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.process = cms.string('%s')\n" % process
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.region = cms.string('%s')\n" % region
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.central_or_shift = cms.string('%s')\n" % central_or_shift
                    tau1Selection = None
                    if region.find("iso1") != -1:
                        tau1Selection = discriminators[discriminator]['tau1Selection']['iso']
                    elif region.find("relaxed1") != -1:
                        tau1Selection = discriminators[discriminator]['tau1Selection']['relaxed']
                    else:
                        raise ValueError("No tau1 selection defined for region = '%s' !!" % region)
                    tau2Selection = None
                    if region.find("iso2") != -1:
                        tau2Selection = discriminators[discriminator]['tau2Selection']['iso']
                    elif region.find("relaxed2") != -1:
                        tau2Selection = discriminators[discriminator]['tau2Selection']['relaxed']    
                    else:
                        raise ValueError("No tau2 selection defined for region = '%s' !!" % region)
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.tau1Selection = cms.string('%s')\n" % tau1Selection
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.tau2Selection = cms.string('%s')\n" % tau2Selection
                    trigger = None
                    if sample.find("Data") != -1:
                        for sample_i in samples[sample]['inputFiles']:
                            if not trigger:
                                trigger = recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample_i]['trigger']
                            else:
                                if recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample_i]['trigger'] != trigger:
                                    raise ValueError("Input files of sample = '%s' do not use the same trigger !!" % sample)
                    else:
                        trigger = "TauPlusJet"
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.trigger = cms.string('%s')\n" % trigger
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.applyTauTriggerTurnOn = cms.string('%s')\n" % discriminators[discriminator]['applyTauTriggerTurnOn']
                    if region.find("relaxed1FRw") != -1 or region.find("relaxed2FRw") != -1:
                        inputFileName = hadd_determineJetToTauFakeRate_outputFileNames[discriminator]
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.inputFileName = cms.string('%s')\n" % inputFileName
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.applyJetToTauFakeRateLooseToTightWeight = cms.bool(True)\n"
                        fitFunctionNormName = None
                        if region.find("relaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight") != -1 or region.find("relaxed1FRw2ndTauTight_relaxed2FRw1stTauLoose") != -1:                    
                            fitFunctionNormName = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSiso1_iso2_div_SSrelaxed1_relaxed2"
                        elif region.find("iso1_relaxed2FRw1stTauTight") != -1:                    
                            fitFunctionNormName = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSiso1_iso2_div_SSiso1_relaxed2"
                        elif region.find("relaxed1FRw2ndTauTight_iso2") != -1:                    
                            fitFunctionNormName = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSiso1_iso2_div_SSrelaxed1_iso2"
                        else:
                            raise ValueError("No fake-rate weights defined for region = '%s' !!" % region)
                        graphShapeName_tau1 = None
                        fitFunctionShapeName_tau1_central = None
                        applyFitFunction_or_graph_tau1 = "fitFunction"
                        if region.find("vrelaxed1FRw2ndTauLoose") != -1:
                            graphShapeName_tau1 = "jetToTauFakeRate/inclusive/$particleEtaBin/jetToTauFakeRate_tau1PtS_SSiso1_vrelaxed2_div_SSvrelaxed1_vrelaxed2"
                            fitFunctionShapeName_tau1_central = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtS_SSiso1_vrelaxed2_div_SSvrelaxed1_vrelaxed2"
                        elif region.find("vrelaxed1FRw2ndTauTight") != -1:
                            graphShapeName_tau1 = "jetToTauFakeRate/inclusive/$particleEtaBin/jetToTauFakeRate_tau1PtS_SSiso1_iso2_div_SSvrelaxed1_iso2"
                            fitFunctionShapeName_tau1_central = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtS_SSiso1_iso2_div_SSvrelaxed1_iso2"
                        elif region.find("relaxed1FRw2ndTauLoose") != -1:
                            graphShapeName_tau1 = "jetToTauFakeRate/inclusive/$particleEtaBin/jetToTauFakeRate_tau1PtS_SSiso1_relaxed2_div_SSrelaxed1_relaxed2"
                            fitFunctionShapeName_tau1_central = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtS_SSiso1_relaxed2_div_SSrelaxed1_relaxed2"
                        elif region.find("relaxed1FRw2ndTauTight") != -1:
                            graphShapeName_tau1 = "jetToTauFakeRate/inclusive/$particleEtaBin/jetToTauFakeRate_tau1PtS_SSiso1_iso2_div_SSrelaxed1_iso2"
                            fitFunctionShapeName_tau1_central = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtS_SSiso1_iso2_div_SSrelaxed1_iso2"
                        elif region.find("iso1") != -1:
                            graphShapeName_tau1 = ""
                            fitFunctionShapeName_tau1_central = ""
                            applyFitFunction_or_graph_tau1 = "notApplied"
                        else:
                            raise ValueError("No fake-rate weights defined for region = '%s' !!" % region)
                        graphShapeName_tau2 = None
                        fitFunctionShapeName_tau2_central = None
                        applyFitFunction_or_graph_tau2 = "graph"
                        if region.find("vrelaxed2FRw1stTauLoose") != -1:
                            graphShapeName_tau2 = "jetToTauFakeRate/inclusive/$particleEtaBin/jetToTauFakeRate_tau2PtS_SSvrelaxed1_iso2_div_SSvrelaxed1_vrelaxed2"
                            fitFunctionShapeName_tau2_central = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau2PtS_SSvrelaxed1_iso2_div_SSvrelaxed1_vrelaxed2"
                        elif region.find("vrelaxed2FRw1stTauTight") != -1:
                            graphShapeName_tau2 = "jetToTauFakeRate/inclusive/$particleEtaBin/jetToTauFakeRate_tau2PtS_SSiso1_iso2_div_SSiso1_vrelaxed2"
                            fitFunctionShapeName_tau2_central = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau2PtS_SSiso1_iso2_div_SSiso1_vrelaxed2"
                        elif region.find("relaxed2FRw1stTauLoose") != -1:
                            graphShapeName_tau2 = "jetToTauFakeRate/inclusive/$particleEtaBin/jetToTauFakeRate_tau2PtS_SSrelaxed1_iso2_div_SSrelaxed1_relaxed2"
                            fitFunctionShapeName_tau2_central = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau2PtS_SSrelaxed1_iso2_div_SSrelaxed1_relaxed2"
                        elif region.find("relaxed2FRw1stTauTight") != -1:
                            graphShapeName_tau2 = "jetToTauFakeRate/inclusive/$particleEtaBin/jetToTauFakeRate_tau2PtS_SSiso1_iso2_div_SSiso1_relaxed2"
                            fitFunctionShapeName_tau2_central = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau2PtS_SSiso1_iso2_div_SSiso1_relaxed2"
                        elif region.find("iso2") != -1:
                            graphShapeName_tau2 = ""
                            fitFunctionShapeName_tau2_central = ""
                            applyFitFunction_or_graph_tau2 = "notApplied"
                        else:
                            raise ValueError("No fake-rate weights defined for region = '%s' !!" % region)
                        fitFunctionShapeName_tau1_shift = ""
                        fitFunctionShapeName_tau2_shift = ""
                        if 'fitFunctionNormName' in central_or_shifts_region[central_or_shift].keys():
                            fitFunctionNormName = central_or_shifts_region[central_or_shift]['fitFunctionNormName']
                        if 'fitFunctionShapeName_tau1' in central_or_shifts_region[central_or_shift].keys():
                            fitFunctionShapeName_tau1_shift = central_or_shifts_region[central_or_shift]['fitFunctionShapeName_tau1']
                        if 'fitFunctionShapeName_tau2' in central_or_shifts_region[central_or_shift].keys():
                            fitFunctionShapeName_tau2_shift = central_or_shifts_region[central_or_shift]['fitFunctionShapeName_tau2']
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.fitFunctionNormName = cms.string('%s')\n" % fitFunctionNormName
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.graphShapeName_tau1 = cms.string('%s')\n" % graphShapeName_tau1
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.fitFunctionShapeName_tau1_central = cms.string('%s')\n" % fitFunctionShapeName_tau1_central
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.fitFunctionShapeName_tau1_shift = cms.string('%s')\n" % fitFunctionShapeName_tau1_shift
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.applyFitFunction_or_graph_tau1 = cms.string('%s')\n" % applyFitFunction_or_graph_tau1
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.graphShapeName_tau2 = cms.string('%s')\n" % graphShapeName_tau2
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.fitFunctionShapeName_tau2_central = cms.string('%s')\n" % fitFunctionShapeName_tau2_central
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.fitFunctionShapeName_tau2_shift = cms.string('%s')\n" % fitFunctionShapeName_tau2_shift
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.applyFitFunction_or_graph_tau2 = cms.string('%s')\n" % applyFitFunction_or_graph_tau2
                        fitFunctionShapePower_tau1 = 1.
                        if 'fitFunctionShapePower_tau1' in central_or_shifts_region[central_or_shift].keys():
                            fitFunctionShapePower_tau1 = central_or_shifts_region[central_or_shift]['fitFunctionShapePower_tau1']
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.fitFunctionShapePower_tau1 = cms.double(%f)\n" % fitFunctionShapePower_tau1
                        fitFunctionShapePower_tau2 = 1.
                        if 'fitFunctionShapePower_tau2' in central_or_shifts_region[central_or_shift].keys():
                            fitFunctionShapePower_tau2 = central_or_shifts_region[central_or_shift]['fitFunctionShapePower_tau2']
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateLooseToTightWeight.fitFunctionShapePower_tau2 = cms.double(%f)\n" % fitFunctionShapePower_tau2
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.tau1EtaBins = cms.vdouble(%s)\n" % getStringRep_vdouble(discriminators[discriminator]['tau1FRwEtaBins'])
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.tau2EtaBins = cms.vdouble(%s)\n" % getStringRep_vdouble(discriminators[discriminator]['tau2FRwEtaBins'])
                    if 'applyJetToTauFakeRateCorrection' in samples[sample].keys():
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.applyJetToTauFakeRateCorrection = cms.bool(%s)\n" % getStringRep_bool(samples[sample]['applyJetToTauFakeRateCorrection'])
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.jetToTauFakeRateCorrection = cms.string('%s')\n" % jetToTauFakeRateCorrection                            
                    if 'lumiScale' in samples[sample].keys():
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.lumiScale = cms.double(%f)\n" % samples[sample]['lumiScale']
                    if 'stitchingWeights' in samples[sample].keys():
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.stitchingWeights = cms.vdouble(%s)\n" % getStringRep_vdouble(samples[sample]['stitchingWeights'])
                    addWeights = []
                    if 'addWeights' in samples[sample].keys():
                        addWeights = samples[sample]['addWeights']
                    addWeights = addWeights_shift_and_remove_central(addWeights, central_or_shifts_region[central_or_shift]['addWeights_extension'])
                    cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.addWeights = cms.vstring(%s)\n" % getStringRep_vstring(addWeights)
                    if region == "OSiso1_iso2" and (central_or_shift == "" or central_or_shift == "central"):
                        selEventsFileName = outputFileName.replace(".root", "_selEvents.txt")
                        cfg_modified += "process.FWLiteTauTauAnalyzer2b2tau.selEventsFileName = cms.string('%s')\n" % selEventsFileName
                    cfgFileName_modified = os.path.join(outputFilePath_subdir, cfgFileName_original.replace("_cfg.py", "_%s_%s_%s_%s_%s_cfg.py" % \
                                                                                     (discriminator, sample, process, region, central_or_shift)))
                    cfgFileName_modified = cfgFileName_modified.replace("__", "_")
                    cfgFileName_modified = cfgFileName_modified.replace("_.", ".")
                    ##print " cfgFileName_modified = '%s'" % cfgFileName_modified
                    cfgFile_modified = open(cfgFileName_modified, "w")
                    cfgFile_modified.write(cfg_modified)
                    cfgFile_modified.close()
                    initDict(FWLiteTauTauAnalyzer2b2tau_configFileNames, [ sample, process, central_or_shift, region, discriminator ])
                    FWLiteTauTauAnalyzer2b2tau_configFileNames[sample][process][central_or_shift][region][discriminator] = cfgFileName_modified
                    
                    logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
                    initDict(FWLiteTauTauAnalyzer2b2tau_logFileNames, [ sample, process, central_or_shift, region, discriminator ])
                    FWLiteTauTauAnalyzer2b2tau_logFileNames[sample][process][central_or_shift][region][discriminator] = logFileName
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" subset of histograms
# for given combination of (discriminator, sample, process and region),
# i.e. "harvest" histograms for individual systematic uncertainties and for individual tau Pt bins
#
print "FWLiteTauTauAnalyzer2b2tau_outputFileNames = ", FWLiteTauTauAnalyzer2b2tau_outputFileNames
hadd_stage1_inputFileNames  = {} # key = discriminator, sample, process, region
hadd_stage1_outputFileNames = {} # key = discriminator, sample, process, region
hadd_stage1_logFileNames    = {} # key = discriminator, sample, process, region
for discriminator in discriminators.keys():
    for sample in samples.keys():
        for process in samples[sample]['processes']:
            for region in regions:
                initDict(hadd_stage1_inputFileNames, [ discriminator, sample, process, region])
                hadd_stage1_inputFileNames[discriminator][sample][process][region] = []
                for central_or_shift in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process].keys():
		    if region in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process][central_or_shift].keys():
                        hadd_stage1_inputFileNames[discriminator][sample][process][region].append(FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process][central_or_shift][region][discriminator])
                if len(hadd_stage1_inputFileNames[discriminator][sample][process][region]) == 0:
                    continue

                outputFilePath_subdir = os.path.join(outputFilePath, discriminator, sample, process, region)
                createFilePath_recursively(outputFilePath_subdir)
                initDict(hadd_stage1_outputFileNames, [ discriminator, sample, process, region ])
                hadd_stage1_outputFileNames[discriminator][sample][process][region] = os.path.join(outputFilePath_subdir, "hadd_stage1_%s_%s_%s_%s.root" % (discriminator, sample, process, region))

                initDict(hadd_stage1_logFileNames, [ discriminator, sample, process, region ])
                hadd_stage1_logFileNames[discriminator][sample][process][region] = hadd_stage1_outputFileNames[discriminator][sample][process][region].replace(".root", ".log")
#--------------------------------------------------------------------------------

def isRegionOfType(region, regionType):
    if regionType == "signalRegion" and region in signalRegions:
        return True
    elif regionType == "qcdRegion" and region in qcdRegions:
        return True
    else:
        return False

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" subset of histograms
# for given combination of (discriminator, process, "OS" or "SS"),
# i.e. "harvest" histograms for individual samples and regions
#
hadd_stage2_inputFileNames  = {} # key = discriminator, process, regionType
hadd_stage2_outputFileNames = {} # key = discriminator, process, regionType
hadd_stage2_logFileNames    = {} # key = discriminator, process, regionType
for discriminator in discriminators.keys():
    for sample in samples.keys():
        for process in samples[sample]['processes']:            
            for regionType in [ "signalRegion", "qcdRegion" ]:

                initDict(hadd_stage2_inputFileNames, [ discriminator, process, regionType ])
                if not regionType in hadd_stage2_inputFileNames[discriminator][process].keys():
                    hadd_stage2_inputFileNames[discriminator][process][regionType] = []
                    
                for region in regions:
                    if isRegionOfType(region, regionType):
                        if region in hadd_stage1_outputFileNames[discriminator][sample][process]:
                            hadd_stage2_inputFileNames[discriminator][process][regionType].append(hadd_stage1_outputFileNames[discriminator][sample][process][region])
                if len(hadd_stage2_inputFileNames[discriminator][process][regionType]) == 0:
                    continue

                outputFilePath_subdir = os.path.join(outputFilePath, discriminator)
                createFilePath_recursively(outputFilePath_subdir)
                initDict(hadd_stage2_outputFileNames, [ discriminator, process, regionType ])
                hadd_stage2_outputFileNames[discriminator][process][regionType] = os.path.join(outputFilePath_subdir, "hadd_stage2_%s_%s_%s.root" % (discriminator, process, regionType))

                initDict(hadd_stage2_logFileNames, [ discriminator, process, regionType ])
                hadd_stage2_logFileNames[discriminator][process][regionType] = hadd_stage2_outputFileNames[discriminator][process][regionType].replace(".root", ".log")
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" subset of histograms
# for given combination of (discriminator, "OS" or "SS"),
# i.e. "harvest" histograms for individual processes
#
hadd_stage3_inputFileNames  = {} # key = discriminator, regionType
hadd_stage3_outputFileNames = {} # key = discriminator, regionType
hadd_stage3_logFileNames    = {} # key = discriminator, regionType
for discriminator in discriminators.keys():
    for regionType in [ "signalRegion", "qcdRegion" ]:
        
        initDict(hadd_stage3_inputFileNames, [ discriminator, regionType ])
        hadd_stage3_inputFileNames[discriminator][regionType] = []
        for sample in samples.keys():
            for process in samples[sample]['processes']:
                if regionType in hadd_stage2_outputFileNames[discriminator][process].keys() and \
                  not hadd_stage2_outputFileNames[discriminator][process][regionType] in hadd_stage3_inputFileNames[discriminator][regionType]:
                    hadd_stage3_inputFileNames[discriminator][regionType].append(hadd_stage2_outputFileNames[discriminator][process][regionType])
        if len(hadd_stage3_inputFileNames[discriminator][regionType]) == 0:
            continue

        outputFilePath_subdir = os.path.join(outputFilePath, discriminator)
        createFilePath_recursively(outputFilePath_subdir)
        initDict(hadd_stage3_outputFileNames, [ discriminator, regionType ])
        hadd_stage3_outputFileNames[discriminator][regionType] = os.path.join(outputFilePath_subdir, "hadd_stage3_%s_%s.root" % (discriminator, regionType))

        initDict(hadd_stage3_logFileNames, [ discriminator, regionType ])
        hadd_stage3_logFileNames[discriminator][regionType] = hadd_stage3_outputFileNames[discriminator][regionType].replace(".root", ".log")
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running determineJetToTauFakeRate macro
#
print "Info: building config files for determineJetToTauFakeRate macro"
determineJetToTauFakeRate_outputFileNames = {} # key = (tau ID) discriminator, category, looseRegion, tight Region
determineJetToTauFakeRate_configFileNames = {} # key = (tau ID) discriminator, category, looseRegion, tight Region
determineJetToTauFakeRate_logFileNames    = {} # key = (tau ID) discriminator, category, looseRegion, tight Region
for discriminator in discriminators.keys():
    for category in [ "inclusive" ] :
        looseRegions = [
            "SSrelaxed1_iso2"
        ]
        for looseRegion in looseRegions:
            tightRegions = []                
            if looseRegion.find("relaxed1") != -1:
                tightRegions.append(looseRegion.replace("relaxed1", "iso1"))
            if looseRegion.find("relaxed2") != -1:
                tightRegions.append(looseRegion.replace("relaxed2", "iso2"))                
            if looseRegion.find("relaxed1") != -1 and looseRegion.find("relaxed2") != -1:
                tightRegions.append(looseRegion.replace("relaxed1", "iso1").replace("relaxed2", "iso2"))
            for tightRegion in tightRegions:
                for idxTau1EtaBin in range(len(discriminators[discriminator]['tau1FRwEtaBins']) - 1):
                    tau1EtaMin = discriminators[discriminator]['tau1FRwEtaBins'][idxTau1EtaBin]
                    tau1EtaMax = discriminators[discriminator]['tau1FRwEtaBins'][idxTau1EtaBin + 1]
                    tau1EtaBin_label = getParticleEtaLabel("tau", tau1EtaMin, tau1EtaMax, -1., 9.9)
                    for idxTau2EtaBin in range(len(discriminators[discriminator]['tau2FRwEtaBins']) - 1):
                        tau2EtaMin = discriminators[discriminator]['tau2FRwEtaBins'][idxTau2EtaBin]
                        tau2EtaMax = discriminators[discriminator]['tau2FRwEtaBins'][idxTau2EtaBin + 1]
                        tau2EtaBin_label = getParticleEtaLabel("tau", -1., 9.9, tau2EtaMin, tau2EtaMax)
                        
                        tauEtaBin_label = getParticleEtaLabel("tau", tau1EtaMin, tau1EtaMax, tau2EtaMin, tau2EtaMax)

                        outputFileName = os.path.join(outputFilePath, discriminator, "determineJetToTauFakeRate_%s_%s_%s_%s_%s.root" % \
                          (discriminator, category, looseRegion, tightRegion, tauEtaBin_label))                        
                        initDict(determineJetToTauFakeRate_outputFileNames, [ discriminator, category, looseRegion, tightRegion, tauEtaBin_label ])
                        determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion][tightRegion][tauEtaBin_label] = outputFileName
            
                        cfgFileName_original = configFile_determineJetToTauFakeRate
                        cfgFile_original = open(cfgFileName_original, "r")
                        cfg_original = cfgFile_original.read()
                        cfgFile_original.close()
                        cfg_modified  = cfg_original
                        cfg_modified += "\n"
                        cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage3_outputFileNames[discriminator]["qcdRegion"]
                        cfg_modified += "\n"
                        cfg_modified += "process.fwliteOutput.fileName = cms.string('%s')\n" % outputFileName
                        cfg_modified += "\n"
                        cfg_modified += "process.determineJetToTauFakeRate.looseRegion = cms.string('%s')\n" % looseRegion
                        cfg_modified += "process.determineJetToTauFakeRate.tightRegion = cms.string('%s')\n" % tightRegion
                        cfg_modified += "process.determineJetToTauFakeRate.category = cms.string('%s')\n" % category
                        cfg_modified += "process.determineJetToTauFakeRate.tauPtBin = cms.string('%s')\n" % "tau1PtGtXXtau2PtGtXX"
                        cfg_modified += "process.determineJetToTauFakeRate.particle1EtaBin = cms.string('%s')\n" % tau1EtaBin_label
                        cfg_modified += "process.determineJetToTauFakeRate.particle2EtaBin = cms.string('%s')\n" % tau2EtaBin_label
                        cfgFileName_modified = os.path.join(outputFilePath, cfgFileName_original.replace("_cfg.py", "_%s_%s_%s_%s_%s_cfg.py" % \
                          (discriminator, category, looseRegion, tightRegion, tauEtaBin_label)))
                        cfgFile_modified = open(cfgFileName_modified, "w")
                        cfgFile_modified.write(cfg_modified)
                        cfgFile_modified.close()
                        initDict(determineJetToTauFakeRate_configFileNames, [ discriminator, category, looseRegion, tightRegion, tauEtaBin_label ])
                        determineJetToTauFakeRate_configFileNames[discriminator][category][looseRegion][tightRegion][tauEtaBin_label] = cfgFileName_modified
                    
                        logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
                        initDict(determineJetToTauFakeRate_logFileNames, [ discriminator, category, looseRegion, tightRegion, tauEtaBin_label ])
                        determineJetToTauFakeRate_logFileNames[discriminator][category][looseRegion][tightRegion][tauEtaBin_label] = logFileName
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" output of determineJetToTauFakeRate macro
#
hadd_determineJetToTauFakeRate_inputFileNames  = {} # key = discriminator
hadd_determineJetToTauFakeRate_logFileNames    = {} # key = discriminator
for discriminator in determineJetToTauFakeRate_outputFileNames.keys():

    hadd_determineJetToTauFakeRate_inputFileNames[discriminator] = []
    for category in determineJetToTauFakeRate_outputFileNames[discriminator].keys():
        for looseRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category].keys():
            for tightRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion].keys():
                for tauEtaBin in determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion][tightRegion].keys():
                    hadd_determineJetToTauFakeRate_inputFileNames[discriminator].append(
                      determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion][tightRegion][tauEtaBin])

    hadd_determineJetToTauFakeRate_logFileNames[discriminator] = hadd_determineJetToTauFakeRate_outputFileNames[discriminator].replace(".root", ".log")
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" subset of histograms
# for given discriminator, i.e. "harvest" histograms for "OS" and "SS" regions
#
hadd_stage4_inputFileNames  = {} # key = discriminator
hadd_stage4_outputFileNames = {} # key = discriminator
hadd_stage4_logFileNames    = {} # key = discriminator
for discriminator in discriminators.keys():

    hadd_stage4_inputFileNames[discriminator] = []
    for regionType in [ "signalRegion", "qcdRegion" ]:
        hadd_stage4_inputFileNames[discriminator].append(hadd_stage3_outputFileNames[discriminator][regionType])

    outputFilePath_subdir = os.path.join(outputFilePath, discriminator)
    createFilePath_recursively(outputFilePath_subdir)    
    hadd_stage4_outputFileNames[discriminator] = os.path.join(outputFilePath_subdir, "hadd_stage4_%s.root" % discriminator)

    hadd_stage4_logFileNames[discriminator] = hadd_stage4_outputFileNames[discriminator].replace(".root", ".log")
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running addBackgroundZTT, addBackgroundQCD and addBackgroundW macros
#
print "Info: building config files for addBackgroundZTT macro"
addBackgroundZTT_configFileNames = {} # key = discriminator
addBackgroundZTT_outputFileNames = {} # key = discriminator
addBackgroundZTT_logFileNames    = {} # key = discriminator
for discriminator in discriminators.keys():
    
    outputFileName = "addBackgroundZTT_%s.root" % discriminator
    addBackgroundZTT_outputFileNames[discriminator] = os.path.join(outputFilePath, discriminator, outputFileName)

    cfgFileName_original = configFile_addBackgroundZTT
    cfgFile_original = open(cfgFileName_original, "r")
    cfg_original = cfgFile_original.read()
    cfgFile_original.close()
    cfg_modified  = cfg_original
    cfg_modified += "\n"
    cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage4_outputFileNames[discriminator]
    cfg_modified += "\n"
    cfg_modified += "process.fwliteOutput.fileName = cms.string('%s')\n" % addBackgroundZTT_outputFileNames[discriminator]
    cfg_modified += "\n"
    cfg_modified += "process.addBackgroundZTT.tauPtBins = cms.vstring('%s')\n" % "tau1PtGtXXtau2PtGtXX"
    cfg_modified += "process.addBackgroundZTT.tauPtBin_inclusive = cms.string('%s')\n" % "tau1PtGtXXtau2PtGtXX"
    cfg_modified += "\n"
    cfgFileName_modified = os.path.join(outputFilePath, configFile_addBackgroundZTT.replace("_cfg.py", "_%s_cfg.py" % discriminator))
    cfgFile_modified = open(cfgFileName_modified, "w")
    cfgFile_modified.write(cfg_modified)
    cfgFile_modified.close()
    addBackgroundZTT_configFileNames[discriminator] = cfgFileName_modified

    logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
    addBackgroundZTT_logFileNames[discriminator] = logFileName

print "Info: building config files for addBackgroundQCD macro"
addBackgroundQCD_configFileNames = {} # key = discriminator, "QCDfromSSiso"/"QCDfromSSantiiso"
addBackgroundQCD_outputFileNames = {} # key = discriminator, "QCDfromSSiso"/"QCDfromSSantiiso"
addBackgroundQCD_logFileNames    = {} # key = discriminator, "QCDfromSSiso"/"QCDfromSSantiiso"
for discriminator in discriminators.keys():

    for qcdOption in [ "QCDfromSSiso", "QCDfromSSantiiso" ]:
        
        cfgFileName_original = None
        if qcdOption == "QCDfromSSiso":
            cfgFileName_original = configFile_addBackgroundQCDfromSSiso
        elif qcdOption == "QCDfromSSantiiso":
            cfgFileName_original = configFile_addBackgroundQCDfromSSantiiso
        else:
            raise ValueError("Invalid qcdOption = '%s' !!" % qcdOption)
    
        outputFileName = "addBackgroundQCD_%s_%s.root" % (discriminator, qcdOption)
        initDict(addBackgroundQCD_outputFileNames, [ discriminator, qcdOption ])
        addBackgroundQCD_outputFileNames[discriminator][qcdOption] = os.path.join(outputFilePath, discriminator, outputFileName)

        cfgFile_original = open(cfgFileName_original, "r")
        cfg_original = cfgFile_original.read()
        cfgFile_original.close()
        cfg_modified  = cfg_original
        cfg_modified += "\n"
        cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage4_outputFileNames[discriminator]
        cfg_modified += "\n"
        cfg_modified += "process.fwliteOutput.fileName = cms.string('%s')\n" % addBackgroundQCD_outputFileNames[discriminator][qcdOption]
        cfg_modified += "\n"
        cfg_modified += "process.addBackgroundQCD.tauPtBins = cms.vstring('%s')\n" % "tau1PtGtXXtau2PtGtXX"
        cfg_modified += "\n"
        cfgFileName_modified = os.path.join(outputFilePath, cfgFileName_original.replace("_cfg.py", "_%s_cfg.py" % discriminator))
        cfgFile_modified = open(cfgFileName_modified, "w")
        cfgFile_modified.write(cfg_modified)
        cfgFile_modified.close()
        initDict(addBackgroundQCD_configFileNames, [ discriminator, qcdOption ])
        addBackgroundQCD_configFileNames[discriminator][qcdOption] = cfgFileName_modified

        logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
        initDict(addBackgroundQCD_logFileNames, [ discriminator, qcdOption ])
        addBackgroundQCD_logFileNames[discriminator][qcdOption] = logFileName

print "Info: building config files for addBackgroundW macro"
addBackgroundW_configFileNames = {} # key = discriminator
addBackgroundW_outputFileNames = {} # key = discriminator
addBackgroundW_logFileNames    = {} # key = discriminator
for discriminator in discriminators.keys():
    
    outputFileName = "addBackgroundW_%s.root" % discriminator
    addBackgroundW_outputFileNames[discriminator] = os.path.join(outputFilePath, discriminator, outputFileName)

    cfgFileName_original = configFile_addBackgroundW
    cfgFile_original = open(cfgFileName_original, "r")
    cfg_original = cfgFile_original.read()
    cfgFile_original.close()
    cfg_modified  = cfg_original
    cfg_modified += "\n"
    cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage4_outputFileNames[discriminator]
    cfg_modified += "\n"
    cfg_modified += "process.fwliteOutput.fileName = cms.string('%s')\n" % addBackgroundW_outputFileNames[discriminator]
    cfg_modified += "\n"
    cfg_modified += "process.addBackgroundW.tauPtBins = cms.vstring('%s')\n" % "tau1PtGtXXtau2PtGtXX"
    cfg_modified += "\n"
    cfgFileName_modified = os.path.join(outputFilePath, configFile_addBackgroundW.replace("_cfg.py", "_%s_cfg.py" % discriminator))
    cfgFile_modified = open(cfgFileName_modified, "w")
    cfgFile_modified.write(cfg_modified)
    cfgFile_modified.close()
    addBackgroundW_configFileNames[discriminator] = cfgFileName_modified

    logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
    addBackgroundW_logFileNames[discriminator] = logFileName        
#--------------------------------------------------------------------------------    

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to add histograms for ZTT and QCD
#
hadd_stage5_inputFileNames  = {} # key = discriminator
hadd_stage5_outputFileNames = {} # key = discriminator
hadd_stage5_logFileNames    = {} # key = discriminator
for discriminator in discriminators.keys():

    hadd_stage5_inputFileNames[discriminator] = [
        hadd_stage4_outputFileNames[discriminator],
        addBackgroundZTT_outputFileNames[discriminator]
    ]
    for qcdOption in addBackgroundQCD_outputFileNames[discriminator].keys():
        hadd_stage5_inputFileNames[discriminator].append(addBackgroundQCD_outputFileNames[discriminator][qcdOption])
    hadd_stage5_inputFileNames[discriminator].append(addBackgroundW_outputFileNames[discriminator])

    outputFilePath_subdir = os.path.join(outputFilePath, discriminator)
    createFilePath_recursively(outputFilePath_subdir)    
    hadd_stage5_outputFileNames[discriminator] = os.path.join(outputFilePath_subdir, "hadd_stage5_%s.root" % discriminator)

    hadd_stage5_logFileNames[discriminator] = hadd_stage5_outputFileNames[discriminator].replace(".root", ".log")
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running prepareTauTauDatacards2b2tau macro
#
print "Info: building config files for prepareTauTauDatacards2b2tau macro"
prepareTauTauDatacards2b2tau_configFileNames = {} # key = discriminator, "nonresonant"/"resonant", histogramToFit
prepareTauTauDatacards2b2tau_outputFileNames = {} # key = discriminator, "nonresonant"/"resonant", histogramToFit
prepareTauTauDatacards2b2tau_logFileNames    = {} # key = discriminator, "nonresonant"/"resonant", histogramToFit
for discriminator in discriminators.keys():
    for categoryOption in [ "_nonresonant", "_resonant" ]:

        histogramsToFit = None
        if categoryOption == "_nonresonant":
            histogramsToFit = [ "augMT2ed", "svFitMassS" ]
        elif categoryOption == "_resonant":
            histogramsToFit = [ "augMT2ed", "svFitMassS", "HHMassM", "HHbRegMassM", "HH2bdyKinFitMassM", "HHbReg2bdyKinFitMassM", "HH4bdyKinFitMassM", "HHbReg4bdyKinFitMassM" ]
        else:
            raise ValueError("Invalid categoryOption = '%s' !!" % categoryOption)        
        for histogramToFit in histogramsToFit:
            
            histogramToFit_fine_binning = histogramToFit
            if histogramToFit_fine_binning[len(histogramToFit_fine_binning) - 1] == "S" or histogramToFit_fine_binning[len(histogramToFit_fine_binning) - 1] == "M":
                histogramToFit_fine_binning = histogramToFit_fine_binning[:len(histogramToFit_fine_binning) - 1] + "L"

            outputFileName = os.path.join(outputFilePath, "htt_tt.inputs-2b2tau-8TeV-0_%s%s_%s.root" % (discriminator, categoryOption, histogramToFit))
            initDict(prepareTauTauDatacards2b2tau_outputFileNames, [ discriminator, categoryOption, histogramToFit ])
            prepareTauTauDatacards2b2tau_outputFileNames[discriminator][categoryOption][histogramToFit] = os.path.join(outputFilePath, outputFileName)
    
            cfgFileName_original = configFile_prepareTauTauDatacards2b2tau
            cfgFile_original = open(cfgFileName_original, "r")
            cfg_original = cfgFile_original.read()
            cfgFile_original.close()
            cfg_modified  = cfg_original
            cfg_modified += "\n"
            cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage5_outputFileNames[discriminator]
            cfg_modified += "\n"
            cfg_modified += "process.fwliteOutput.fileName = cms.string('%s')\n" % outputFileName
            cfg_modified += "\n"
            if categoryOption == "_nonresonant":
                cfg_modified += "process.prepareTauTauDatacards2b2tau.categories = cms.vstring('inclusive', '2bM_nonresonant', '2bL_nonresonant', '1b1j_nonresonant', '2j_nonresonant')\n"
            elif categoryOption == "_resonant":
                cfg_modified += "process.prepareTauTauDatacards2b2tau.categories = cms.vstring('inclusive', '2bM_resonant', '2bL_resonant', '1b1j_resonant', '2j_resonant')\n"
            cfg_modified += "process.prepareTauTauDatacards2b2tau.discriminator = cms.string('%s')\n" % discriminator
            cfg_modified += "process.prepareTauTauDatacards2b2tau.histogramToFit = cms.string('%s')\n" % histogramToFit
            cfg_modified += "process.prepareTauTauDatacards2b2tau.histogramToFit_fine_binning = cms.string('%s')\n" % histogramToFit_fine_binning
            cfg_modified += "\n"
            cfgFileName_modified = os.path.join(outputFilePath, configFile_prepareTauTauDatacards2b2tau.replace("_cfg.py", "_%s%s_%s_cfg.py" % (discriminator, categoryOption, histogramToFit)))
            cfgFile_modified = open(cfgFileName_modified, "w")
            cfgFile_modified.write(cfg_modified)
            cfgFile_modified.close()
            initDict(prepareTauTauDatacards2b2tau_configFileNames, [ discriminator, categoryOption, histogramToFit ])
            prepareTauTauDatacards2b2tau_configFileNames[discriminator][categoryOption][histogramToFit] = cfgFileName_modified

            logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
            initDict(prepareTauTauDatacards2b2tau_logFileNames, [ discriminator, categoryOption, histogramToFit ])
            prepareTauTauDatacards2b2tau_logFileNames[discriminator][categoryOption][histogramToFit] = logFileName
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running makeTauTauPlots macro
#
print "Info: building config files for makeTauTauPlots macro"
makeTauTauPlots2b2tau_configFileNames = {} # key = discriminator, ""/"nonresonant"/"resonant", "QCDfromSSiso"/"QCDfromSSantiiso"
makeTauTauPlots2b2tau_outputFileNames = {} # key = discriminator, ""/"nonresonant"/"resonant"
makeTauTauPlots2b2tau_logFileNames    = {} # key = discriminator, ""/"nonresonant"/"resonant", "QCDfromSSiso"/"QCDfromSSantiiso"
for discriminator in discriminators.keys():

    outputFilePath_plots = os.path.join(outputFilePath, "plots", "prefit", discriminator)
    createFilePath_recursively(outputFilePath_plots)

    for categoryOption in [ "", "_nonresonant", "_resonant" ]:

        outputFileName = "makeTauTauPlots2b2tau_prefit_%s%s.png" % (discriminator, categoryOption)
        initDict(makeTauTauPlots2b2tau_outputFileNames, [ discriminator, categoryOption ])
        makeTauTauPlots2b2tau_outputFileNames[discriminator][categoryOption] = os.path.join(outputFilePath_plots, outputFileName)
        
        for qcdOption in [ "QCDfromSSiso", "QCDfromSSantiiso" ]:
        
            cfgFileName_original = None
            if qcdOption == "QCDfromSSiso":
                cfgFileName_original = configFile_makeTauTauPlots2b2tau_QCDfromSSiso
            elif qcdOption == "QCDfromSSantiiso":
                cfgFileName_original = configFile_makeTauTauPlots2b2tau_QCDfromSSantiiso
            else:
                raise ValueError("Invalid qcdOption = '%s' !!" % qcdOption)
            cfgFile_original = open(cfgFileName_original, "r")
            cfg_original = cfgFile_original.read()
            cfgFile_original.close()
            cfg_modified  = cfg_original
            for category in [ "2bM", "2bL", "1b1j", "2j" ]:
                cfg_modified  = cfg_modified.replace(category, "%s%s" % (category, categoryOption))
            cfg_modified += "\n"
            cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage5_outputFileNames[discriminator]
            cfg_modified += "\n"    
            cfg_modified += "process.makeTauTauPlots2b2tau.outputFileName = cms.string('%s')\n" % makeTauTauPlots2b2tau_outputFileNames[discriminator][categoryOption]
            cfg_modified += "\n"
            if categoryOption == "_nonresonant":
                cfg_modified += "delattr(process.makeTauTauPlots2b2tau, 'signal2')\n"
            elif categoryOption == "_resonant":
                cfg_modified += "process.makeTauTauPlots2b2tau.signal1 = cms.PSet(\n"
                cfg_modified += "    process_ggH = cms.string('mssmH300tohh'),\n"
                cfg_modified += "    sf_ggH = cms.double(1.*0.577*0.0632*2.),\n"
                cfg_modified += "    process_bbH = cms.string(''),\n"
                cfg_modified += "    sf_bbH = cms.double(0.),\n"
                cfg_modified += "    legendEntry = cms.string('H #rightarrow hh #rightarrow 2b 2#tau (m=300 GeV)')\n"
                cfg_modified += ")\n"
                cfg_modified += "process.makeTauTauPlots2b2tau.signal2 = cms.PSet(\n"
                cfg_modified += "    process_ggH = cms.string('graviton500Tohh'),\n"
                cfg_modified += "    sf_ggH = cms.double(1.*0.577*0.0632*2.),\n"
                cfg_modified += "    process_bbH = cms.string(''),\n"
                cfg_modified += "    sf_bbH = cms.double(0.),\n"
                cfg_modified += "    legendEntry = cms.string(\"G #rightarrow hh #rightarrow 2b 2#tau (m=500 GeV)\")\n"
                cfg_modified += ")\n"
                cfg_modified += "process.makeTauTauPlots2b2tau.signal3 = cms.PSet(\n"
                cfg_modified += "    process_ggH = cms.string('radion700Tohh'),\n"
                cfg_modified += "    sf_ggH = cms.double(1.*0.577*0.0632*2.),\n"
                cfg_modified += "    process_bbH = cms.string(''),\n"
                cfg_modified += "    sf_bbH = cms.double(0.),\n"
                cfg_modified += "    legendEntry = cms.string(\"R #rightarrow hh #rightarrow 2b 2#tau (m=700 GeV)\")\n"
                cfg_modified += ")\n"
            cfg_modified += "\n"
            cfgFileName_modified = os.path.join(outputFilePath, cfgFileName_original.replace("_cfg.py", "_%s%s_cfg.py" % (discriminator, categoryOption)))
            cfgFile_modified = open(cfgFileName_modified, "w")
            cfgFile_modified.write(cfg_modified)
            cfgFile_modified.close()
            initDict(makeTauTauPlots2b2tau_configFileNames, [ discriminator, categoryOption, qcdOption ])
            makeTauTauPlots2b2tau_configFileNames[discriminator][categoryOption][qcdOption] = cfgFileName_modified

            logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
            initDict(makeTauTauPlots2b2tau_logFileNames, [ discriminator, categoryOption, qcdOption ])
            makeTauTauPlots2b2tau_logFileNames[discriminator][categoryOption][qcdOption] = logFileName
#--------------------------------------------------------------------------------

def make_MakeFile_vstring(list_of_strings):
    retVal = ""
    for i, string_i in enumerate(list_of_strings):
        if i > 0:
            retVal += " "
        retVal += string_i
    return retVal

# done building config files, now build Makefile...
makeFileName = os.path.join(outputFilePath, "Makefile_runTauTauAnalysis2b2tau")
makeFile = open(makeFileName, "w")
makeFile.write("\n")
outputFileNames = []
for sample in FWLiteTauTauAnalyzer2b2tau_outputFileNames.keys():
    for process in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample].keys():
        for central_or_shift in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process].keys():
            for region in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process][central_or_shift].keys():
                for discriminator in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process][central_or_shift][region].keys():
                    outputFileNames.append(FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process][central_or_shift][region][discriminator])
for region in hadd_stage1_outputFileNames.keys():
    for process in hadd_stage1_outputFileNames[region].keys():
        for sample in hadd_stage1_outputFileNames[region][process].keys():
            for discriminator in hadd_stage1_outputFileNames[region][process][sample].keys():
                outputFileNames.append(hadd_stage1_outputFileNames[region][process][sample][discriminator])
for discriminator in hadd_stage2_outputFileNames.keys():
    for process in hadd_stage2_outputFileNames[discriminator].keys():
        for regionType in hadd_stage2_outputFileNames[discriminator][process].keys():
            outputFileNames.append(hadd_stage2_outputFileNames[discriminator][process][regionType])
for discriminator in hadd_stage3_outputFileNames.keys():
    for regionType in hadd_stage3_outputFileNames[discriminator].keys():
        outputFileNames.append(hadd_stage3_outputFileNames[discriminator][regionType])
for discriminator in determineJetToTauFakeRate_outputFileNames.keys():
    for category in determineJetToTauFakeRate_outputFileNames[discriminator].keys():
        for looseRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category].keys():
            for tightRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion].keys():
                for tauEtaBin in determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion][tightRegion].keys():
                    outputFileNames.append(determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion][tightRegion][tauEtaBin])
for discriminator in hadd_determineJetToTauFakeRate_outputFileNames.keys():
    outputFileNames.append(hadd_determineJetToTauFakeRate_outputFileNames[discriminator])
for discriminator in hadd_stage4_outputFileNames.keys():
    outputFileNames.append(hadd_stage4_outputFileNames[discriminator])
for discriminator in addBackgroundZTT_outputFileNames.keys():
    outputFileNames.append(addBackgroundZTT_outputFileNames[discriminator])
for discriminator in addBackgroundQCD_outputFileNames.keys():
    for qcdOption in addBackgroundQCD_outputFileNames[discriminator].keys():
        outputFileNames.append(addBackgroundQCD_outputFileNames[discriminator][qcdOption])
for discriminator in addBackgroundW_outputFileNames.keys():
    outputFileNames.append(addBackgroundW_outputFileNames[discriminator])                
for discriminator in hadd_stage5_outputFileNames.keys():
    outputFileNames.append(hadd_stage5_outputFileNames[discriminator])    
for discriminator in prepareTauTauDatacards2b2tau_outputFileNames.keys():
    for categoryOption in prepareTauTauDatacards2b2tau_outputFileNames[discriminator].keys():
        for histogramToFit in prepareTauTauDatacards2b2tau_outputFileNames[discriminator][categoryOption].keys():
            outputFileNames.append(prepareTauTauDatacards2b2tau_outputFileNames[discriminator][categoryOption][histogramToFit])
for discriminator in makeTauTauPlots2b2tau_outputFileNames.keys():
    for categoryOption in makeTauTauPlots2b2tau_outputFileNames[discriminator].keys():
        outputFileNames.append(makeTauTauPlots2b2tau_outputFileNames[discriminator][categoryOption])
# CV: check existing output files and delete corrupted files (of size < 10 kb)
for outputFileName in outputFileNames:
    if (outputFileName.find("FWLiteTauTauAnalyzer") != -1 or outputFileName.find("hadd") != -1) and os.path.isfile(outputFileName):
        ##isSignalRegion_hadd = False
        ##if outputFileName.find("hadd") != -1:
        ##    for signalRegion in signalRegions:
        ##        if outputFileName.find(signalRegion) != -1:
        ##            isSignalRegion_hadd = True
        ##if isSignalRegion_hadd:
        ##    print "file = %s is 'hadd' file for signal region --> deleting it." % outputFileName
        ##    command = "%s %s" % (executable_rm, outputFileName)
        ##    runCommand(command)
        ##    continue
        outputFileSize = os.stat(outputFileName).st_size
        #if outputFileSize < 10000 or outputFileName.find("TT_Embedded") != -1:
        if outputFileSize < 10000:
            print "file = %s has size = %i --> deleting it." % (outputFileName, outputFileSize)
            command = "%s %s" % (executable_rm, outputFileName)
            runCommand(command)
        ##else:
        ##    print "file = %s has size = %i --> keeping it." % (outputFileName, outputFileSize)
makeFile.write("all: %s\n" % make_MakeFile_vstring(outputFileNames))
makeFile.write("\techo 'Finished running hh -> 2b 2tau -> bb tau_h tau_h analysis.'\n")
makeFile.write("\n")
for sample in FWLiteTauTauAnalyzer2b2tau_outputFileNames.keys():
    for process in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample].keys():
        for central_or_shift in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process].keys():
            for region in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process][central_or_shift].keys():
                for discriminator in FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process][central_or_shift][region].keys():
                    makeFile.write("%s: %s\n" %
                      (FWLiteTauTauAnalyzer2b2tau_outputFileNames[sample][process][central_or_shift][region][discriminator],
                       make_MakeFile_vstring(FWLiteTauTauAnalyzer2b2tau_inputFileNames[sample][process][central_or_shift][region][discriminator])))
                    makeFile.write("\t%s%s %s &> %s\n" %
                      (nice, executable_FWLiteTauTauAnalyzer2b2tau,
                       FWLiteTauTauAnalyzer2b2tau_configFileNames[sample][process][central_or_shift][region][discriminator],
                       FWLiteTauTauAnalyzer2b2tau_logFileNames[sample][process][central_or_shift][region][discriminator]))
makeFile.write("\n")
for region in hadd_stage1_outputFileNames.keys():
    for process in hadd_stage1_outputFileNames[region].keys():
        for sample in hadd_stage1_outputFileNames[region][process].keys():
            for discriminator in hadd_stage1_outputFileNames[region][process][sample].keys():
                makeFile.write("%s: %s\n" %
                  (hadd_stage1_outputFileNames[region][process][sample][discriminator],
                   make_MakeFile_vstring(hadd_stage1_inputFileNames[region][process][sample][discriminator])))
                makeFile.write("\t%s%s %s\n" %
                  (nice, executable_rm,
                   hadd_stage1_outputFileNames[region][process][sample][discriminator]))
                makeFile.write("\t%s%s %s %s &> %s\n" %
                  (nice, executable_hadd,
                   hadd_stage1_outputFileNames[region][process][sample][discriminator],
                   make_MakeFile_vstring(hadd_stage1_inputFileNames[region][process][sample][discriminator]),
                   hadd_stage1_logFileNames[region][process][sample][discriminator]))
makeFile.write("\n")
for discriminator in hadd_stage2_outputFileNames.keys():
    for process in hadd_stage2_outputFileNames[discriminator].keys():
        for regionType in hadd_stage2_outputFileNames[discriminator][process].keys():
            makeFile.write("%s: %s\n" %
              (hadd_stage2_outputFileNames[discriminator][process][regionType],
               make_MakeFile_vstring(hadd_stage2_inputFileNames[discriminator][process][regionType])))
            makeFile.write("\t%s%s %s\n" %
              (nice, executable_rm,
               hadd_stage2_outputFileNames[discriminator][process][regionType]))
            makeFile.write("\t%s%s %s %s &> %s\n" %
              (nice, executable_hadd,
               hadd_stage2_outputFileNames[discriminator][process][regionType],
               make_MakeFile_vstring(hadd_stage2_inputFileNames[discriminator][process][regionType]),
               hadd_stage2_logFileNames[discriminator][process][regionType]))
makeFile.write("\n")
for discriminator in hadd_stage3_outputFileNames.keys():
    for regionType in hadd_stage3_outputFileNames[discriminator].keys():
        makeFile.write("%s: %s\n" %
          (hadd_stage3_outputFileNames[discriminator][regionType],
           make_MakeFile_vstring(hadd_stage3_inputFileNames[discriminator][regionType])))
        makeFile.write("\t%s%s %s\n" %
          (nice, executable_rm,
           hadd_stage3_outputFileNames[discriminator][regionType]))
        makeFile.write("\t%s%s %s %s &> %s\n" %
          (nice, executable_hadd,
           hadd_stage3_outputFileNames[discriminator][regionType],
           make_MakeFile_vstring(hadd_stage3_inputFileNames[discriminator][regionType]),
           hadd_stage3_logFileNames[discriminator][regionType]))
makeFile.write("\n")
for discriminator in determineJetToTauFakeRate_outputFileNames.keys():
    for category in determineJetToTauFakeRate_outputFileNames[discriminator].keys():
        for looseRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category].keys():
            for tightRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion].keys():
                for tauEtaBin in determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion][tightRegion].keys():
                    makeFile.write("%s: %s\n" %                       
                      (determineJetToTauFakeRate_outputFileNames[discriminator][category][looseRegion][tightRegion][tauEtaBin],
                       hadd_stage3_outputFileNames[discriminator]["qcdRegion"]))
                    makeFile.write("\t%s%s %s &> %s\n" %
                      (nice, executable_determineJetToTauFakeRate,
                       determineJetToTauFakeRate_configFileNames[discriminator][category][looseRegion][tightRegion][tauEtaBin],
                       determineJetToTauFakeRate_logFileNames[discriminator][category][looseRegion][tightRegion][tauEtaBin]))
makeFile.write("\n")
for discriminator in hadd_determineJetToTauFakeRate_outputFileNames.keys():
    makeFile.write("%s: %s\n" %
      (hadd_determineJetToTauFakeRate_outputFileNames[discriminator],
       make_MakeFile_vstring(hadd_determineJetToTauFakeRate_inputFileNames[discriminator])))
    makeFile.write("\t%s%s %s\n" %
      (nice, executable_rm,
       hadd_determineJetToTauFakeRate_outputFileNames[discriminator]))
    makeFile.write("\t%s%s %s %s &> %s\n" %
      (nice, executable_hadd,
       hadd_determineJetToTauFakeRate_outputFileNames[discriminator],
       make_MakeFile_vstring(hadd_determineJetToTauFakeRate_inputFileNames[discriminator]),
       hadd_determineJetToTauFakeRate_logFileNames[discriminator]))
makeFile.write("\n")
for discriminator in hadd_stage4_outputFileNames.keys():
    makeFile.write("%s: %s\n" %
      (hadd_stage4_outputFileNames[discriminator],
       make_MakeFile_vstring(hadd_stage4_inputFileNames[discriminator])))
    makeFile.write("\t%s%s %s\n" %
      (nice, executable_rm,
       hadd_stage4_outputFileNames[discriminator]))
    makeFile.write("\t%s%s %s %s &> %s\n" %
      (nice, executable_hadd,
       hadd_stage4_outputFileNames[discriminator],
       make_MakeFile_vstring(hadd_stage4_inputFileNames[discriminator]),
       hadd_stage4_logFileNames[discriminator]))
makeFile.write("\n")    
for discriminator in addBackgroundZTT_outputFileNames.keys():
    makeFile.write("%s: %s\n" %
      (addBackgroundZTT_outputFileNames[discriminator],
       hadd_stage4_outputFileNames[discriminator]))
    makeFile.write("\t%s%s %s &> %s\n" %
      (nice, executable_addBackgroundZTT,
       addBackgroundZTT_configFileNames[discriminator],
       addBackgroundZTT_logFileNames[discriminator]))
for discriminator in addBackgroundQCD_outputFileNames.keys():
    for qcdOption in addBackgroundQCD_outputFileNames[discriminator].keys():
        makeFile.write("%s: %s\n" %
          (addBackgroundQCD_outputFileNames[discriminator][qcdOption],
           hadd_stage4_outputFileNames[discriminator]))
        makeFile.write("\t%s%s %s &> %s\n" %
          (nice, executable_addBackgroundQCD,
           addBackgroundQCD_configFileNames[discriminator][qcdOption],
           addBackgroundQCD_logFileNames[discriminator][qcdOption]))
for discriminator in addBackgroundW_outputFileNames.keys():
    makeFile.write("%s: %s\n" %
      (addBackgroundW_outputFileNames[discriminator],
       hadd_stage4_outputFileNames[discriminator]))
    makeFile.write("\t%s%s %s &> %s\n" %
      (nice, executable_addBackgroundW,
       addBackgroundW_configFileNames[discriminator],
       addBackgroundW_logFileNames[discriminator]))           
makeFile.write("\n")
for discriminator in hadd_stage5_outputFileNames.keys():
    makeFile.write("%s: %s\n" %
      (hadd_stage5_outputFileNames[discriminator],
       make_MakeFile_vstring(hadd_stage5_inputFileNames[discriminator])))
    makeFile.write("\t%s%s %s\n" %
      (nice, executable_rm,
       hadd_stage5_outputFileNames[discriminator]))
    makeFile.write("\t%s%s %s %s &> %s\n" %
      (nice, executable_hadd,
       hadd_stage5_outputFileNames[discriminator],
       make_MakeFile_vstring(hadd_stage5_inputFileNames[discriminator]),
       hadd_stage5_logFileNames[discriminator]))
makeFile.write("\n")
for discriminator in prepareTauTauDatacards2b2tau_outputFileNames.keys():
    for categoryOption in prepareTauTauDatacards2b2tau_outputFileNames[discriminator].keys():
        for histogramToFit in prepareTauTauDatacards2b2tau_outputFileNames[discriminator][categoryOption].keys():
            makeFile.write("%s: %s\n" %
              (prepareTauTauDatacards2b2tau_outputFileNames[discriminator][categoryOption][histogramToFit],
               hadd_stage5_outputFileNames[discriminator]))
            makeFile.write("\t%s%s %s &> %s\n" %
              (nice, executable_prepareTauTauDatacards2b2tau,
               prepareTauTauDatacards2b2tau_configFileNames[discriminator][categoryOption][histogramToFit],
               prepareTauTauDatacards2b2tau_logFileNames[discriminator][categoryOption][histogramToFit]))
makeFile.write("\n")    
for discriminator in makeTauTauPlots2b2tau_outputFileNames.keys():
    for categoryOption in makeTauTauPlots2b2tau_outputFileNames[discriminator].keys():
        makeFile.write("%s: %s\n" %
          (makeTauTauPlots2b2tau_outputFileNames[discriminator][categoryOption],
           hadd_stage5_outputFileNames[discriminator]))
        for qcdOption in makeTauTauPlots2b2tau_configFileNames[discriminator][categoryOption].keys():       
            makeFile.write("\t%s%s %s &> %s\n" %
              (nice, executable_makeTauTauPlots2b2tau,
               makeTauTauPlots2b2tau_configFileNames[discriminator][categoryOption][qcdOption],
               makeTauTauPlots2b2tau_logFileNames[discriminator][categoryOption][qcdOption]))    
makeFile.write("\n")
makeFile.write(".PHONY: clean\n")
makeFile.write("clean:\n")
makeFile.write("\t%s %s\n" % (executable_rm, make_MakeFile_vstring(outputFileNames)))
makeFile.write("\techo 'Finished deleting old files.'\n")
makeFile.write("\n")
makeFile.close()

print("Finished building Makefile. Now execute 'make -j 8 -f %s'." % makeFileName)
