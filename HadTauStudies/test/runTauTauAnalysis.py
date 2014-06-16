#!/usr/bin/env python

from LLRAnalysis.HadTauStudies.recoSampleDefinitionsAHtoTauTau_8TeV_grid_cfi import *

import copy
import os
import re
import time

jobId = '2014Jan10'

version = "v1_12"

inputFilePath  = "/data2/veelken/CMSSW_5_3_x/Ntuples/AHtoTauTau/%s/%s" % (jobId, version)

outputFilePath = "/data1/veelken/tmp/tauTauAnalysis/%s_8/" % version

_picobarns =  1.0
_femtobarns = 1.0e-3

intLumiData = recoSampleDefinitionsAHtoTauTau_8TeV['TARGET_LUMI']
print "intLumiData = %1.1f fb^-1" % (recoSampleDefinitionsAHtoTauTau_8TeV['TARGET_LUMI']*_femtobarns)

lumiScale_DY = 2.12
stitchingWeights_DY = [ 1.0, 0.412/lumiScale_DY, 0.167/lumiScale_DY, 0.097/lumiScale_DY, 0.0759/lumiScale_DY ]

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
    'Data' : {
        'processes' : [ "data_obs" ],
        'inputFiles' : [
            ##"data_Run2012A_22Jan2013_v1",
            "data_Run2012B_22Jan2013_v1",
            "data_Run2012C_22Jan2013_v1",
            "data_Run2012D_22Jan2013_v1",
            ##"data_Jet_Run2012A_22Jan2013ReReco_v1",
            "data_JetHT_Run2012B_22Jan2013ReReco_v1",
            "data_JetHT_Run2012C_22Jan2013ReReco_v1",
            "data_JetHT_Run2012D_22Jan2013ReReco_v1"
        ]
    },
    'DYJets' : {
        'processes' : [ "ZTTmc", "ZL", "ZJ" ],
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
    'WJets' : {
        'processes' : [ "W" ],
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
        'processes' : [ "ggH_SM125" ],
        'inputFiles' : [ "HiggsGGH125" ],
        'lumiScale' : getLumiScale('HiggsGGH125')
    },
    'HiggsVBF125' : {
        'processes' : [ "qqH_SM125" ],
        'inputFiles' : [ "HiggsVBF125" ],
        'lumiScale' : getLumiScale('HiggsVBF125')
    },
    'HiggsVH125' : {
        'processes' : [ "VH_SM125" ],
        'inputFiles' : [ "HiggsVH125" ],
        'lumiScale' : getLumiScale('HiggsVH125')
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
mssmHiggsMassPoints = [ 80, 90, 100, 110, 120, 130, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 ]
for massPoint in mssmHiggsMassPoints:
    ggSampleName = "HiggsSUSYGluGlu%1.0f" % massPoint
    samples[ggSampleName] = {
        'processes' : [ "ggH%1.0f" % massPoint ],
        'inputFiles' : [ ggSampleName ],
        'lumiScale' : getLumiScale(ggSampleName, 1.*_picobarns), # CV: normalize MSSM Higgs -> tautau signal MCs to cross-section of 1pb
        'addWeights' : []
    }
    bbSampleName = "HiggsSUSYBB%1.0f" % massPoint
    if bbSampleName == "HiggsSUSYBB300":
        bbSampleName = "HiggsSUSYBB300v2"
    samples[bbSampleName] = {
        'processes' : [ "bbH%1.0f" % massPoint ],
        'inputFiles' : [ bbSampleName ],
        'lumiScale' : getLumiScale(bbSampleName, 1.*_picobarns) # CV: normalize MSSM Higgs -> tautau signal MCs to cross-section of 1pb
    }

def makeJetToTauFakeRateCorrection(par0, par1, par2, par3, x0 = 163.7):
    retVal = "%f + 0.01*(x - %f)*%f + TMath::Power(0.01*(x - %f), 2.)*%f + TMath::Power(0.01*(x - %f), 3.)*%f" % (par0, x0, par1, x0, par2, x0, par3)
    return retVal

discriminators = {
    'HPScombIso3HitsMedium' : {
        'tau1Selection' : {
            'iso'      : "l1MediumDB3HIso > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
            'relaxed'  : "l1RawDB3HIso < 4.0 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5 && !(l1MediumDB3HIso > 0.5)",
            'vrelaxed' : "l1RawDB3HIso < 4.0 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5 && !(l1MediumDB3HIso > 0.5)"
        },
        'tau2Selection' : {
            'iso'      : "l2MediumDB3HIso > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5",
            'relaxed'  : "l2RawDB3HIso < 4.0 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5 && !(l2MediumDB3HIso > 0.5)",
            'vrelaxed' : "l2RawDB3HIso < 4.0 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5 && !(l2MediumDB3HIso > 0.5)"
        },
        'tau1FRwEtaBins'             : [ -1., 1.479, 9.9 ],
        'tau2FRwEtaBins'             : [ -1., 1.479, 9.9 ],
        ##'tau1FRwEtaBins'             : [ -1., 9.9 ],
        ##'tau2FRwEtaBins'             : [ -1., 9.9 ],
        # jetToTauFakeRateCorrection taken from https://indico.cern.ch/event/304725/contribution/1/material/slides/0.pdf
        'jetToTauFakeRateCorrection' : makeJetToTauFakeRateCorrection(7.18127e-1, -1.43612e-1, -4.31415e-2, -9.81383e-2)
    },
    'MVAwLToldDMsTight' : {
        'tau1Selection' : {
            'iso'      : "l1TightMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
            'relaxed'  : "l1LooseMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5 && !(l1TightMVAwLT > 0.5)",
            'vrelaxed' : "l1VLooseMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5 && !(l1TightMVAwLT > 0.5)"
        },
        'tau2Selection' : {
            'iso'      : "l2TightMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5",
            'relaxed'  : "l2LooseMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5 && !(l2TightMVAwLT > 0.5)",
            'vrelaxed' : "l2VLooseMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5 && !(l2TightMVAwLT > 0.5)"
        },
        'tau1FRwEtaBins'             : [ -1., 1.2, 1.7, 9.9 ],
        ##'tau1FRwEtaBins'             : [ -1., 9.9 ],
        'tau2FRwEtaBins'             : [ -1., 9.9 ],
        # jetToTauFakeRateCorrectiontaken from https://indico.cern.ch/event/304725/contribution/1/material/slides/0.pdf
        'jetToTauFakeRateCorrection' : makeJetToTauFakeRateCorrection(7.58704e-1, -1.57025e-1, -2.40635e-2, -8.24741e-2)
    },
##     'MVAwLToldDMsVTight' : {
##         'tau1Selection' : {
##             'iso'      : "l1VTightMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
##             'relaxed'  : "l1LooseMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5 && !(l1VTightMVAwLT > 0.5)",
##             'vrelaxed' : "l1VLooseMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5 && !(l1VTightMVAwLT > 0.5)"
##         },
##         'tau2Selection' : {
##             'iso'      : "l2VTightMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5",
##             'relaxed'  : "l2LooseMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5 && !(l2VTightMVAwLT > 0.5)",
##             'vrelaxed' : "l2VLooseMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5 && !(l2VTightMVAwLT > 0.5)"
##         },
##         'tau1FRwEtaBins'             : [ -1., 1.2, 1.7, 9.9 ],
##         ##'tau1FRwEtaBins'             : [ -1., 9.9 ],
##         'tau2FRwEtaBins'             : [ -1., 9.9 ],
##          # CV: parameters for jetToTauFakeRateCorrection obtained for MVAwLToldDMsTight
##         'jetToTauFakeRateCorrection' : makeJetToTauFakeRateCorrection(7.58704e-1, -1.57025e-1, -2.40635e-2, -8.24741e-2)
##     }
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
    'CMS_htt_higgsPtReweight_8TeVUp' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : []
    },
    'CMS_htt_higgsPtReweight_8TeVDown' : {
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
    "SSiso1_iso2_LooseBtagFRw",
    "SSvrelaxed1FRw2ndTauLoose_vrelaxed2FRw1stTauTight_LooseBtag", "SSrelaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight_LooseBtag",
    "SSvrelaxed1FRw2ndTauTight_vrelaxed2FRw1stTauLoose_LooseBtag", "SSrelaxed1FRw2ndTauTight_relaxed2FRw1stTauLoose_LooseBtag",
    "SSvrelaxed1FRw2ndTauLoose_vrelaxed2FRw1stTauTight_LooseBtagFRw", "SSrelaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight_TightBtag",
    "SSvrelaxed1FRw2ndTauTight_vrelaxed2FRw1stTauLoose_LooseBtagFRw", "SSrelaxed1FRw2ndTauTight_relaxed2FRw1stTauLoose_TightBtag",
    "SSiso1_vrelaxed2FRw1stTauTight_LooseBtag", "SSiso1_relaxed2FRw1stTauTight_LooseBtag",
    "SSvrelaxed1FRw2ndTauTight_iso2_LooseBtag", "SSrelaxed1FRw2ndTauTight_iso2_LooseBtag",
    "SSvrelaxed1FRw2ndTauTight_iso2_LooseBtagFRw", "SSrelaxed1FRw2ndTauTight_iso2_TightBtag",
    "OSvrelaxed1FRw2ndTauLoose_vrelaxed2FRw1stTauTight_LooseBtag", "OSrelaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight_LooseBtag",
    "OSvrelaxed1FRw2ndTauTight_vrelaxed2FRw1stTauLoose_LooseBtag", "OSrelaxed1FRw2ndTauTight_relaxed2FRw1stTauLoose_LooseBtag", 
    "OSvrelaxed1FRw2ndTauLoose_vrelaxed2FRw1stTauTight_LooseBtagFRw", "OSrelaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight_TightBtag",
    "OSvrelaxed1FRw2ndTauTight_vrelaxed2FRw1stTauLoose_LooseBtagFRw", "OSrelaxed1FRw2ndTauTight_relaxed2FRw1stTauLoose_TightBtag",
    "OSvrelaxed1FRw2ndTauTight_iso2_LooseBtag", "OSrelaxed1FRw2ndTauTight_iso2_LooseBtag",
    "OSvrelaxed1FRw2ndTauTight_iso2_LooseBtagFRw", "OSrelaxed1FRw2ndTauTight_iso2_TightBtag",
    "OSiso1_iso2_LooseBtag", "OSiso1_iso2_LooseBtagFRw", "OSiso1_iso2_TightBtag"
]
qcdRegions = [
    "SSrelaxed1_relaxed2_LooseBtag", "SSvrelaxed1_vrelaxed2_LooseBtag", "SSvrelaxed1_vrelaxed2_TightBtag", 
    "SSiso1_relaxed2_LooseBtag", "SSiso1_vrelaxed2_LooseBtag", 
    "SSrelaxed1_iso2_LooseBtag", "SSvrelaxed1_iso2_LooseBtag",
    "SSiso1_iso2_LooseBtag", "SSiso1_iso2_TightBtag",
    "OSrelaxed1_relaxed2_LooseBtag", "OSvrelaxed1_vrelaxed2_LooseBtag", "OSvrelaxed1_vrelaxed2_TightBtag"
]
regions = []
regions.extend(signalRegions)
regions.extend(qcdRegions)

tauPtBins = [
    (45.,-1.), # CV: run for comparison with unbinned analysis HIG-13-021
    (45.,60.), # CV: used in no-B-tag and B-tag category
    (60.,80.), # CV: used in no-B-tag category
    (80.,-1.), # CV: used in no-B-tag category
    (60.,-1.)  # CV: used in B-tag category
]

execDir = "%s/bin/%s/" % (os.environ['CMSSW_BASE'], os.environ['SCRAM_ARCH'])

executable_FWLiteTauTauAnalyzer      = execDir + 'FWLiteTauTauAnalyzer'
executable_determineJetToTauFakeRate = execDir + 'determineJetToTauFakeRate'
executable_addBackgroundZTT          = execDir + 'addBackgroundZTT'
executable_addBackgroundQCD          = execDir + 'addBackgroundQCD'
executable_prepareTauTauDatacards    = execDir + 'prepareTauTauDatacards'
executable_makeTauTauPlots           = execDir + 'makeTauTauPlots'
executable_hadd                      = 'hadd'
executable_rm                        = 'rm -f'

nice = 'nice '

configFile_FWLiteTauTauAnalyzer             = 'FWLiteTauTauAnalyzer_cfg.py'
configFile_determineJetToTauFakeRate        = 'determineJetToTauFakeRate_cfg.py'
configFile_determineBJetLooseToTightWeight  = 'determineBJetLooseToTightWeight_cfg.py'
configFile_addBackgroundZTT                 = 'addBackgroundZTT_cfg.py'
configFile_addBackgroundQCDfromSSantiiso    = 'addBackgroundQCDfromSSantiiso_cfg.py' # for datacards (QCD shape templates taken from SS anti-isolated region, weighted by jet -> tau fake-rate)
configFile_addBackgroundQCDfromSSiso        = 'addBackgroundQCDfromSSiso_cfg.py'     # for control plots of tau ID observables (QCD shape templates taken from SS isolated region)
configFile_prepareTauTauDatacards           = 'prepareTauTauDatacards_cfg.py'
configFile_makeTauTauPlots_QCDfromSSiso     = 'makeTauTauPlots_QCDfromSSiso_cfg.py'
configFile_makeTauTauPlots_QCDfromSSantiiso = 'makeTauTauPlots_QCDfromSSantiiso_cfg.py'

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

massPoint_regex   = r'HiggsSUSY(BB|GluGlu)(?P<massPoint>[0-9]+)'
massPoint_matcher = re.compile(massPoint_regex)

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

def getTauPtLabel(tauPtMin, tauPtMax):
    tauPtBin_label = "tau1PtGt45"
    if tauPtMin > 0. and tauPtMax > 0.:
        tauPtBin_label += "tau2Pt%1.0fto%1.0f" % (tauPtMin, tauPtMax)
    elif tauPtMin > 0.:
        tauPtBin_label += "tau2PtGt%1.0f" % tauPtMin
    else:
        tauPtBin_label += "tau2PtLt%1.0f" % tauPtMax
    return tauPtBin_label

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

hadd_determineBJetLooseToTightWeight_outputFileNames = {} # key = discriminator
hadd_determineJetToTauFakeRate_outputFileNames       = {} # key = discriminator
for discriminator in discriminators.keys():
    hadd_determineBJetLooseToTightWeight_outputFileNames[discriminator] = os.path.join(outputFilePath, discriminator, "determineBJetLooseToTightWeight_%s.root" % discriminator)
    hadd_determineJetToTauFakeRate_outputFileNames[discriminator] = os.path.join(outputFilePath, discriminator, "determineJetToTauFakeRate_%s.root" % discriminator)
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running FWLiteTauTauAnalyzer macro
#
print "Info: building config files for FWLiteTauTauAnalyzer macro"
FWLiteTauTauAnalyzer_inputFileNames  = {} # key = sample, process, central_or_shift, region, tauPtBin, discriminator
FWLiteTauTauAnalyzer_configFileNames = {} # key = sample, process, central_or_shift, region, tauPtBin, discriminator
FWLiteTauTauAnalyzer_outputFileNames = {} # key = sample, process, central_or_shift, region, tauPtBin, discriminator
FWLiteTauTauAnalyzer_logFileNames    = {} # key = sample, process, central_or_shift, region, tauPtBin, discriminator
for sample in samples.keys():
    print "processing sample = %s" % sample
    for process in samples[sample]['processes']:
        for region in regions:
            if region not in [ "OSiso1_iso2_TightBtag", "SSiso1_iso2_TightBtag" ] and (sample.find("HiggsSUSYGluGlu") != -1 or sample.find("HiggsSUSYBB") != -1):
                continue
            central_or_shifts_region = copy.deepcopy(central_or_shifts)
            if region.find("relaxed1FRw") != -1 or region.find("vrelaxed1FRw") != -1 or region.find("relaxed2FRw") != -1 or region.find("vrelaxed2FRw") != -1:
                tauFR_fitFunctionNormName_Up   = None
                tauFR_fitFunctionNormName_Down = None
                if region.find("vrelaxed1FRw2ndTauLoose_vrelaxed2FRw1stTauTight") != -1 or region.find("vrelaxed1FRw2ndTauTight_vrelaxed2FRw1stTauLoose") != -1:                    
                    tauFR_fitFunctionNormName_Up   = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormUp_SSiso1_iso2_LooseBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag"
                    tauFR_fitFunctionNormName_Down = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormDown_SSiso1_iso2_LooseBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag"
                elif region.find("relaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight") != -1 or region.find("relaxed1FRw2ndTauTight_relaxed2FRw1stTauLoose") != -1:
                    tauFR_fitFunctionNormName_Up   = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormUp_SSiso1_iso2_LooseBtag_div_SSrelaxed1_relaxed2_LooseBtag"
                    tauFR_fitFunctionNormName_Down = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormDown_SSiso1_iso2_LooseBtag_div_SSrelaxed1_relaxed2_LooseBtag"
                elif region.find("iso1_vrelaxed2FRw1stTauTight") != -1:                    
                    tauFR_fitFunctionNormName_Up   = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormUp_SSiso1_iso2_LooseBtag_div_SSiso1_vrelaxed2_LooseBtag"
                    tauFR_fitFunctionNormName_Down = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormDown_SSiso1_iso2_LooseBtag_div_SSiso1_vrelaxed2_LooseBtag"
                elif region.find("iso1_relaxed2FRw1stTauTight") != -1:                    
                    tauFR_fitFunctionNormName_Up   = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormUp_SSiso1_iso2_LooseBtag_div_SSiso1_relaxed2_LooseBtag"
                    tauFR_fitFunctionNormName_Down = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormDown_SSiso1_iso2_LooseBtag_div_SSiso1_relaxed2_LooseBtag"
                elif region.find("vrelaxed1FRw2ndTauTight_iso2") != -1:                    
                    tauFR_fitFunctionNormName_Up   = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormUp_SSiso1_iso2_LooseBtag_div_SSvrelaxed1_iso2_LooseBtag"
                    tauFR_fitFunctionNormName_Down = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormDown_SSiso1_iso2_LooseBtag_div_SSvrelaxed1_iso2_LooseBtag"
                elif region.find("relaxed1FRw2ndTauTight_iso2") != -1:                    
                    tauFR_fitFunctionNormName_Up   = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormUp_SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag"
                    tauFR_fitFunctionNormName_Down = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNormDown_SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag"
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

                if (central_or_shift.find('CMS_htt_QCDfrNorm_tautau_8TeV') != -1 or central_or_shift.find('CMS_htt_QCDfrNorm_tautau_8TeV') != -1) and sample.find("HiggsSUSYGluGlu") == -1:
                    continue
                if central_or_shift.find('CMS_htt_higgsPtReweight_8TeV') != -1 and sample.find("HiggsSUSYGluGlu") == -1:
                    continue
                if central_or_shift.find('CMS_htt_ttbarPtReweight_8TeV') != -1 and not sample in [ "TTJetsHadronic", "TTJetsSemiLept", "TTJetsFullLept", "TTJets_Embedded" ]:
                    continue
                if central_or_shift.find('CMS_htt_WShape_tautau_8TeV') != -1 and not sample in [ "WJets", "WJetsExt", "W1Jets", "W2Jets", "W3Jets", "W4Jets" ]:
                    continue
                if len(central_or_shift) > 0 and central_or_shift.find('central') == -1 and sample == "Data":
                    continue
                
                inputFileNames = []
                for inputFilePath_extension_sample in samples[sample]['inputFiles']:
                    inputFileNames_sample = getInputFileNames(os.path.join(inputFilePath, central_or_shifts_region[central_or_shift]['inputFilePath_extension'], inputFilePath_extension_sample))
                    inputFileNames.extend(inputFileNames_sample)
                if len(inputFileNames) == 0:
                    raise ValueError("Failed to find input files for sample = '%s' !!" % sample)
                ##if central_or_shift == "" or central_or_shift == "central":
                print " central_or_shift = '%s': inputFileNames = %s" %  (central_or_shift, inputFileNames)

                for tauPtBin in tauPtBins:

                    tauPtMin = tauPtBin[0]
                    tauPtMax = tauPtBin[1]
                    tauPtBin_label = getTauPtLabel(tauPtMin, tauPtMax)

                    for discriminator in discriminators.keys():

                        jetToTauFakeRateCorrection = discriminators[discriminator]['jetToTauFakeRateCorrection']
                        if central_or_shift == 'CMS_htt_WShape_tautau_8TeVUp':
                            jetToTauFakeRateCorrection = "1.0"
                        elif central_or_shift == 'CMS_htt_WShape_tautau_8TeVDown':
                            jetToTauFakeRateCorrection = "TMath::Power(%s, 2.)" % jetToTauFakeRateCorrection

                        initDict(FWLiteTauTauAnalyzer_inputFileNames, [ sample, process, central_or_shift, region, tauPtBin_label, discriminator ])
                        FWLiteTauTauAnalyzer_inputFileNames[sample][process][central_or_shift][region][tauPtBin_label][discriminator] = inputFileNames
			FWLiteTauTauAnalyzer_inputFileNames[sample][process][central_or_shift][region][tauPtBin_label][discriminator] = []
                        # CV: declare dependency on output of determineJetToTauFakeRate macro
                        if region.find("LooseBtagFRw") != -1:
                            FWLiteTauTauAnalyzer_inputFileNames[sample][process][central_or_shift][region][tauPtBin_label][discriminator].append(
                              hadd_determineBJetLooseToTightWeight_outputFileNames[discriminator])
                        if region.find("relaxed1FRw") != -1 or region.find("vrelaxed1FRw") != -1 or region.find("relaxed2FRw") != -1 or region.find("vrelaxed2FRw") != -1:
                            FWLiteTauTauAnalyzer_inputFileNames[sample][process][central_or_shift][region][tauPtBin_label][discriminator].append(
                              hadd_determineJetToTauFakeRate_outputFileNames[discriminator])

                        outputFilePath_subdir = os.path.join(outputFilePath, discriminator, sample, process, region)
                        createFilePath_recursively(outputFilePath_subdir)
                        outputFileName = os.path.join(outputFilePath_subdir, "FWLiteTauTauAnalyzer_%s_%s_%s_%s_%s_%s.root" % (discriminator, sample, process, region, tauPtBin_label, central_or_shift))
                        outputFileName = outputFileName.replace("__", "_")
                        outputFileName = outputFileName.replace("_.", ".")
                        print " region = %s: outputFileName = '%s'" % (region, outputFileName)
                        initDict(FWLiteTauTauAnalyzer_outputFileNames, [ sample, process, central_or_shift, region, tauPtBin_label, discriminator ])
                        FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift][region][tauPtBin_label][discriminator] = outputFileName

                        cfgFileName_original = configFile_FWLiteTauTauAnalyzer
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
                            cfg_modified += "process.FWLiteTauTauAnalyzer.treeName = cms.string('tauTauNtupleProducerTauUp/H2TauTauTreeProducerTauTau')\n"
                        elif central_or_shifts_region[central_or_shift]['inputFilePath_extension'] == "down":
                            cfg_modified += "process.FWLiteTauTauAnalyzer.treeName = cms.string('tauTauNtupleProducerTauDown/H2TauTauTreeProducerTauTau')\n"
                        cfg_modified += "process.FWLiteTauTauAnalyzer.process = cms.string('%s')\n" % process
                        massPoint = None
                        if sample in recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'].keys() and recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['type'] == 'bsmMC':
                            massPoint_match = massPoint_matcher.match(sample)
                            if massPoint_match:
                                massPoint = massPoint_match.group('massPoint')
                                cfg_modified += "process.FWLiteTauTauAnalyzer.massPoint = cms.double(%s)\n" % massPoint
                            else:
                                raise ValueError("Failed to decode mass-point for sample = %s !!" % sample)
                        cfg_modified += "process.FWLiteTauTauAnalyzer.region = cms.string('%s')\n" % region
                        cfg_modified += "process.FWLiteTauTauAnalyzer.central_or_shift = cms.string('%s')\n" % central_or_shift
                        tau1Selection = None
                        if region.find("iso1") != -1:
                            tau1Selection = discriminators[discriminator]['tau1Selection']['iso']
                        elif region.find("vrelaxed1") != -1:
                            tau1Selection = discriminators[discriminator]['tau1Selection']['vrelaxed']
                        elif region.find("relaxed1") != -1:
                            tau1Selection = discriminators[discriminator]['tau1Selection']['relaxed']
                        else:
                            raise ValueError("No tau1 selection defined for region = '%s' !!" % region)
                        tau2Selection = None
                        if region.find("iso2") != -1:
                            tau2Selection = discriminators[discriminator]['tau2Selection']['iso']
                        elif region.find("vrelaxed2") != -1:
                            tau2Selection = discriminators[discriminator]['tau2Selection']['vrelaxed']    
                        elif region.find("relaxed2") != -1:
                            tau2Selection = discriminators[discriminator]['tau2Selection']['relaxed']                        
                        else:
                            raise ValueError("No tau2 selection defined for region = '%s' !!" % region)
                        cfg_modified += "process.FWLiteTauTauAnalyzer.tau1Selection = cms.string('%s')\n" % tau1Selection
                        cfg_modified += "process.FWLiteTauTauAnalyzer.tau1PtMin = cms.double(45.)\n"
                        cfg_modified += "process.FWLiteTauTauAnalyzer.tau1PtMax = cms.double(-1.)\n"
                        cfg_modified += "process.FWLiteTauTauAnalyzer.tau2Selection = cms.string('%s')\n" % tau2Selection
                        cfg_modified += "process.FWLiteTauTauAnalyzer.tau2PtMin = cms.double(%1.0f)\n" % tauPtMin
                        cfg_modified += "process.FWLiteTauTauAnalyzer.tau2PtMax = cms.double(%1.0f)\n" % tauPtMax
                        trigger = None
                        if recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['type'] == "Data":
                            trigger = recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['trigger']
                        else:
                            trigger = "TauPlusJet"
                        cfg_modified += "process.FWLiteTauTauAnalyzer.trigger = cms.string('%s')\n" % trigger
                        cfg_modified += "process.FWLiteTauTauAnalyzer.applyTauTriggerTurnOn = cms.string('%s')\n" % discriminator                    
                        if region.find("LooseBtag") != -1:
                            cfg_modified += "process.FWLiteTauTauAnalyzer.applyTightBtag = cms.bool(False)\n"
                        else: 
                            cfg_modified += "process.FWLiteTauTauAnalyzer.applyTightBtag = cms.bool(True)\n"
                        if region.find("LooseBtagFRw") != -1:                            
                            inputFileName = hadd_determineBJetLooseToTightWeight_outputFileNames[discriminator]
                            cfg_modified += "process.FWLiteTauTauAnalyzer.bJetLooseToTightWeight.inputFileName = cms.string('%s')\n" % inputFileName
                            cfg_modified += "process.FWLiteTauTauAnalyzer.applyBJetLooseToTightWeight = cms.bool(True)\n"
                        if region.find("relaxed1FRw") != -1 or region.find("vrelaxed1FRw") != -1 or region.find("relaxed2FRw") != -1 or region.find("vrelaxed2FRw") != -1:
                            inputFileName = hadd_determineJetToTauFakeRate_outputFileNames[discriminator]
                            cfg_modified += "process.FWLiteTauTauAnalyzer.jetToTauFakeRateLooseToTightWeight.inputFileName = cms.string('%s')\n" % inputFileName
                            cfg_modified += "process.FWLiteTauTauAnalyzer.applyJetToTauFakeRateLooseToTightWeight = cms.bool(True)\n"
                            fitFunctionNormName = None
                            if region.find("vrelaxed1FRw2ndTauLoose_vrelaxed2FRw1stTauTight") != -1 or region.find("vrelaxed1FRw2ndTauTight_vrelaxed2FRw1stTauLoose") != -1:                    
                                fitFunctionNormName = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSiso1_iso2_LooseBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag"
                            elif region.find("relaxed1FRw2ndTauLoose_relaxed2FRw1stTauTight") != -1 or region.find("relaxed1FRw2ndTauTight_relaxed2FRw1stTauLoose") != -1:
                                fitFunctionNormName = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSiso1_iso2_LooseBtag_div_SSrelaxed1_relaxed2_LooseBtag"
                            elif region.find("iso1_vrelaxed2FRw1stTauTight") != -1:                    
                                fitFunctionNormName = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSiso1_iso2_LooseBtag_div_SSiso1_vrelaxed2_LooseBtag"
                            elif region.find("iso1_relaxed2FRw1stTauTight") != -1:                    
                                fitFunctionNormName = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSiso1_iso2_LooseBtag_div_SSiso1_relaxed2_LooseBtag"
                            elif region.find("vrelaxed1FRw2ndTauTight_iso2") != -1:                    
                                fitFunctionNormName = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSiso1_iso2_LooseBtag_div_SSvrelaxed1_iso2_LooseBtag"
                            elif region.find("relaxed1FRw2ndTauTight_iso2") != -1:                    
                                fitFunctionNormName = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionNorm_SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag"
                            else:
                                raise ValueError("No fake-rate weights defined for region = '%s' !!" % region)
                            fitFunctionShapeName_tau1 = None
                            if region.find("vrelaxed1FRw2ndTauLoose") != -1:
                                fitFunctionShapeName_tau1 = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtL_SSiso1_vrelaxed2_LooseBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag"
                            elif region.find("vrelaxed1FRw2ndTauTight") != -1:
                                fitFunctionShapeName_tau1 = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtL_SSiso1_iso2_LooseBtag_div_SSvrelaxed1_iso2_LooseBtag"
                            elif region.find("relaxed1FRw2ndTauLoose") != -1:
                                fitFunctionShapeName_tau1 = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtL_SSiso1_relaxed2_LooseBtag_div_SSrelaxed1_relaxed2_LooseBtag"
                            elif region.find("relaxed1FRw2ndTauTight") != -1:
                                fitFunctionShapeName_tau1 = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau1PtL_SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag"
                            elif region.find("iso1") != -1:
                                fitFunctionShapeName_tau1 = ""
                            else:
                                raise ValueError("No fake-rate weights defined for region = '%s' !!" % region)
                            fitFunctionShapeName_tau2 = None
                            if region.find("vrelaxed2FRw1stTauLoose") != -1:
                                fitFunctionShapeName_tau2 = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau2PtL_SSvrelaxed1_iso2_LooseBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag"
                            elif region.find("vrelaxed2FRw1stTauTight") != -1:
                                fitFunctionShapeName_tau2 = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau2PtL_SSiso1_iso2_LooseBtag_div_SSiso1_vrelaxed2_LooseBtag"
                            elif region.find("relaxed2FRw1stTauLoose") != -1:
                                fitFunctionShapeName_tau2 = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau2PtL_SSrelaxed1_iso2_LooseBtag_div_SSrelaxed1_relaxed2_LooseBtag"
                            elif region.find("relaxed2FRw1stTauTight") != -1:
                                fitFunctionShapeName_tau2 = "jetToTauFakeRate/inclusive/$particleEtaBin/fitFunctionShape_tau2PtL_SSiso1_iso2_LooseBtag_div_SSiso1_relaxed2_LooseBtag"
                            elif region.find("iso2") != -1:
                                fitFunctionShapeName_tau2 = ""    
                            else:
                                raise ValueError("No fake-rate weights defined for region = '%s' !!" % region)
                            if 'fitFunctionNormName' in central_or_shifts_region[central_or_shift].keys():
                                fitFunctionNormName = central_or_shifts_region[central_or_shift]['fitFunctionNormName']
                            if 'fitFunctionShapeName_tau1' in central_or_shifts_region[central_or_shift].keys():
                                fitFunctionShapeName_tau1 = central_or_shifts_region[central_or_shift]['fitFunctionShapeName_tau1']
                            if 'fitFunctionShapeName_tau2' in central_or_shifts_region[central_or_shift].keys():
                                fitFunctionShapeName_tau2 = central_or_shifts_region[central_or_shift]['fitFunctionShapeName_tau2']
                            cfg_modified += "process.FWLiteTauTauAnalyzer.jetToTauFakeRateLooseToTightWeight.fitFunctionNormName = cms.string('%s')\n" % fitFunctionNormName
                            cfg_modified += "process.FWLiteTauTauAnalyzer.jetToTauFakeRateLooseToTightWeight.fitFunctionShapeName_tau1 = cms.string('%s')\n" % fitFunctionShapeName_tau1
                            cfg_modified += "process.FWLiteTauTauAnalyzer.jetToTauFakeRateLooseToTightWeight.fitFunctionShapeName_tau2 = cms.string('%s')\n" % fitFunctionShapeName_tau2
                            fitFunctionShapePower_tau1 = 1.
                            if 'fitFunctionShapePower_tau1' in central_or_shifts_region[central_or_shift].keys():
                                fitFunctionShapePower_tau1 = central_or_shifts_region[central_or_shift]['fitFunctionShapePower_tau1']
                            cfg_modified += "process.FWLiteTauTauAnalyzer.jetToTauFakeRateLooseToTightWeight.fitFunctionShapePower_tau1 = cms.double(%f)\n" % fitFunctionShapePower_tau1
                            fitFunctionShapePower_tau2 = 1.
                            if 'fitFunctionShapePower_tau2' in central_or_shifts_region[central_or_shift].keys():
                                fitFunctionShapePower_tau2 = central_or_shifts_region[central_or_shift]['fitFunctionShapePower_tau2']
                            cfg_modified += "process.FWLiteTauTauAnalyzer.jetToTauFakeRateLooseToTightWeight.fitFunctionShapePower_tau2 = cms.double(%f)\n" % fitFunctionShapePower_tau2
                        cfg_modified += "process.FWLiteTauTauAnalyzer.tau1EtaBins = cms.vdouble(%s)\n" % getStringRep_vdouble(discriminators[discriminator]['tau1FRwEtaBins'])
                        cfg_modified += "process.FWLiteTauTauAnalyzer.tau2EtaBins = cms.vdouble(%s)\n" % getStringRep_vdouble(discriminators[discriminator]['tau2FRwEtaBins'])
                        if 'applyJetToTauFakeRateCorrection' in samples[sample].keys():
                            cfg_modified += "process.FWLiteTauTauAnalyzer.applyJetToTauFakeRateCorrection = cms.bool(%s)\n" % getStringRep_bool(samples[sample]['applyJetToTauFakeRateCorrection'])
                            cfg_modified += "process.FWLiteTauTauAnalyzer.jetToTauFakeRateCorrection = cms.string('%s')\n" % jetToTauFakeRateCorrection                            
                        if 'lumiScale' in samples[sample].keys():
                            cfg_modified += "process.FWLiteTauTauAnalyzer.lumiScale = cms.double(%f)\n" % samples[sample]['lumiScale']
                        if 'stitchingWeights' in samples[sample].keys():
                            cfg_modified += "process.FWLiteTauTauAnalyzer.stitchingWeights = cms.vdouble(%s)\n" % getStringRep_vdouble(samples[sample]['stitchingWeights'])
                        addWeights = []
                        if 'addWeights' in samples[sample].keys():
                            addWeights = samples[sample]['addWeights']:
                        addWeights = addWeights_shift_and_remove_central(addWeights, central_or_shifts_region[central_or_shift]['addWeights_extension'])
                        cfg_modified += "process.FWLiteTauTauAnalyzer.addWeights = cms.vstring(%s)\n" % getStringRep_vstring(addWeights)
                        if sample.find("HiggsSUSYGluGlu") == -1:
                            if not massPoint:
                                raise ValueError("Failed to decode mass-point for sample = %s !!" % sample)                            
                            lutNameHiggsPtReweighting = "A_mA%s_mu200/mssmHiggsPtReweight_A_mA%s_mu200_central" % (massPoint, massPoint)
                            if central_or_shift == 'CMS_htt_higgsPtReweight_8TeVUp':
                                lutNameHiggsPtReweighting = lutNameHiggsPtReweighting.replace("_central", "tanBetaLow")
                            if central_or_shift == 'CMS_htt_higgsPtReweight_8TeVDown':
                                lutNameHiggsPtReweighting = lutNameHiggsPtReweighting.replace("_central", "tanBetaHigh")
                            cfg_modified += "process.FWLiteTauTauAnalyzer.applyHiggsPtReweighting = cms.bool(True)\n"
                            cfg_modified += "process.FWLiteTauTauAnalyzer.higgsPtReweighting.lutName = cms.string('%s')\n" % lutNameHiggsPtReweighting
                        else:
                            cfg_modified += "process.FWLiteTauTauAnalyzer.applyHiggsPtReweighting = cms.bool(False)\n"
                        if region == "OSisoTightBtag" and (central_or_shift == "" or central_or_shift == "central"):
                            selEventsFileName = outputFileName.replace(".root", "_selEvents.txt")
                            cfg_modified += "process.FWLiteTauTauAnalyzer.selEventsFileName = cms.string('%s')\n" % selEventsFileName
                        cfgFileName_modified = os.path.join(outputFilePath_subdir, cfgFileName_original.replace("_cfg.py", "_%s_%s_%s_%s_%s_%s_cfg.py" % \
                                                                                     (discriminator, sample, process, region, tauPtBin_label, central_or_shift)))
                        cfgFileName_modified = cfgFileName_modified.replace("__", "_")
                        cfgFileName_modified = cfgFileName_modified.replace("_.", ".")
                        ##print " cfgFileName_modified = '%s'" % cfgFileName_modified
                        cfgFile_modified = open(cfgFileName_modified, "w")
                        cfgFile_modified.write(cfg_modified)
                        cfgFile_modified.close()
                        initDict(FWLiteTauTauAnalyzer_configFileNames, [ sample, process, central_or_shift, region, tauPtBin_label, discriminator ])
                        FWLiteTauTauAnalyzer_configFileNames[sample][process][central_or_shift][region][tauPtBin_label][discriminator] = cfgFileName_modified
                    
                        logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
                        initDict(FWLiteTauTauAnalyzer_logFileNames, [ sample, process, central_or_shift, region, tauPtBin_label, discriminator ])
                        FWLiteTauTauAnalyzer_logFileNames[sample][process][central_or_shift][region][tauPtBin_label][discriminator] = logFileName
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" subset of histograms
# for given combination of (discriminator, sample, process and region),
# i.e. "harvest" histograms for individual systematic uncertainties and for individual tau Pt bins
#
hadd_stage1_inputFileNames  = {} # key = discriminator, sample, process, region
hadd_stage1_outputFileNames = {} # key = discriminator, sample, process, region
hadd_stage1_logFileNames    = {} # key = discriminator, sample, process, region
for discriminator in discriminators.keys():
    for sample in samples.keys():
        for process in samples[sample]['processes']:
            for region in regions:

                initDict(hadd_stage1_inputFileNames, [ discriminator, sample, process, region])
                hadd_stage1_inputFileNames[discriminator][sample][process][region] = []
                for central_or_shift in FWLiteTauTauAnalyzer_outputFileNames[sample][process].keys():
		    if region in FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift].keys():
                        for tauPtBin in FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift][region].keys():
                            hadd_stage1_inputFileNames[discriminator][sample][process][region].append(FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift][region][tauPtBin][discriminator])
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
print "Info: building config files for determineBJetLooseToTightWeight macro"
determineBJetLooseToTightWeight_outputFileNames = {} # key = (tau ID) discriminator, looseRegion
determineBJetLooseToTightWeight_configFileNames = {} # key = (tau ID) discriminator, looseRegion
determineBJetLooseToTightWeight_logFileNames    = {} # key = (tau ID) discriminator, looseRegion
for discriminator in discriminators.keys():
    for looseRegion in [ "SSvrelaxed1_vrelaxed2_LooseBtag" ]:
        for idxBJet1EtaBin in range(len(bJet1FRwEtaBins) - 1):
            bJet1EtaMin = bJet1FRwEtaBins[idxBJet1EtaBin]
            bJet1EtaMax = bJet1FRwEtaBins[idxBJet1EtaBin + 1]
            bJet1EtaBin_label = getParticleEtaLabel("bJet", bJet1EtaMin, bJet1EtaMax, -1., 9.9)

            outputFileName = os.path.join(outputFilePath, discriminator, "determineBJetLooseToTightWeight_%s_%s_%s.root" % \
              (discriminator, looseRegion, bJet1EtaBin_label))                        
            initDict(determineBJetLooseToTightWeight_outputFileNames, [ discriminator, looseRegion, bJet1EtaBin_label ])
            determineBJetLooseToTightWeight_outputFileNames[discriminator][looseRegion][bJet1EtaBin_label] = outputFileName
            
            cfgFileName_original = configFile_determineBJetLooseToTightWeight
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
            tightRegion = looseRegion.replace("LooseBtag", "TightBtag")
            cfg_modified += "process.determineJetToTauFakeRate.tightRegion = cms.string('%s')\n" % tightRegion
            cfg_modified += "process.determineJetToTauFakeRate.category = cms.string('btag')\n" 
            cfg_modified += "process.determineJetToTauFakeRate.particle1EtaBin = cms.string('%s')\n" % bJet1EtaBin_label
            cfg_modified += "process.determineJetToTauFakeRate.particle2EtaBin = cms.string('')\n"
            if bJet1EtaMax > 5.0:
                cfg_modified += "process.determineJetToTauFakeRate.fitFunction = cms.string('[0] + [1]*x')\n"
                cfg_modified += "process.determineJetToTauFakeRate.initialParameters.p0 = cms.double(1.)\n"
                cfg_modified += "process.determineJetToTauFakeRate.initialParameters.p1 = cms.double(0.1)\n"
            cfgFileName_modified = os.path.join(outputFilePath, cfgFileName_original.replace("_cfg.py", "_%s_%s_%s_cfg.py" % \
              (discriminator, looseRegion, bJet1EtaBin_label)))
            cfgFile_modified = open(cfgFileName_modified, "w")
            cfgFile_modified.write(cfg_modified)
            cfgFile_modified.close()
            initDict(determineBJetLooseToTightWeight_configFileNames, [ discriminator, looseRegion, bJet1EtaBin_label ])
            determineBJetLooseToTightWeight_configFileNames[discriminator][looseRegion][bJet1EtaBin_label] = cfgFileName_modified
                    
            logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
            initDict(determineBJetLooseToTightWeight_logFileNames, [ discriminator, looseRegion, bJet1EtaBin_label ])
            determineBJetLooseToTightWeight_logFileNames[discriminator][looseRegion][bJet1EtaBin_label] = logFileName           

print "Info: building config files for determineJetToTauFakeRate macro"
determineJetToTauFakeRate_outputFileNames = {} # key = (tau ID) discriminator, category, btagDiscriminator, looseRegion, tight Region
determineJetToTauFakeRate_configFileNames = {} # key = (tau ID) discriminator, category, btagDiscriminator, looseRegion, tight Region
determineJetToTauFakeRate_logFileNames    = {} # key = (tau ID) discriminator, category, btagDiscriminator, looseRegion, tight Region
for discriminator in discriminators.keys():
    for category in [ "inclusive" ] :
        for btagDiscriminator in [ "LooseBtag" ]:
            looseRegions = [
                "SSrelaxed1_relaxed2_%s" % btagDiscriminator, "SSvrelaxed1_vrelaxed2_%s" % btagDiscriminator,
                "SSiso1_relaxed2_%s" % btagDiscriminator, "SSiso1_vrelaxed2_%s" % btagDiscriminator, 
                "SSrelaxed1_iso2_%s" % btagDiscriminator, "SSvrelaxed1_iso2_%s" % btagDiscriminator
            ]
            for looseRegion in looseRegions:
                tightRegions = []                
                if looseRegion.find("vrelaxed1") != -1:
                    tightRegions.append(looseRegion.replace("vrelaxed1", "iso1"))
                elif looseRegion.find("relaxed1") != -1:
                    tightRegions.append(looseRegion.replace("relaxed1", "iso1"))    
                if looseRegion.find("vrelaxed2") != -1:
                    tightRegions.append(looseRegion.replace("vrelaxed2", "iso2"))                
                elif looseRegion.find("relaxed2") != -1:
                    tightRegions.append(looseRegion.replace("relaxed2", "iso2"))
                if looseRegion.find("vrelaxed1") != -1 and looseRegion.find("vrelaxed2") != -1:
                    tightRegions.append(looseRegion.replace("vrelaxed1", "iso1").replace("vrelaxed2", "iso2"))
                elif looseRegion.find("relaxed1") != -1 and looseRegion.find("relaxed2") != -1:
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

                            outputFileName = os.path.join(outputFilePath, discriminator, "determineJetToTauFakeRate_%s_%s_%s_%s_%s_%s.root" % \
                              (discriminator, category, btagDiscriminator, looseRegion, tightRegion, tauEtaBin_label))                        
                            initDict(determineJetToTauFakeRate_outputFileNames, [ discriminator, category, btagDiscriminator, looseRegion, tightRegion, tauEtaBin_label ])
                            determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion][tauEtaBin_label] = outputFileName
            
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
                            cfg_modified += "process.determineJetToTauFakeRate.particle1EtaBin = cms.string('%s')\n" % tau1EtaBin_label
                            cfg_modified += "process.determineJetToTauFakeRate.particle2EtaBin = cms.string('%s')\n" % tau2EtaBin_label
                            cfgFileName_modified = os.path.join(outputFilePath, cfgFileName_original.replace("_cfg.py", "_%s_%s_%s_%s_%s_%s_cfg.py" % \
                              (discriminator, category, btagDiscriminator, looseRegion, tightRegion, tauEtaBin_label)))
                            cfgFile_modified = open(cfgFileName_modified, "w")
                            cfgFile_modified.write(cfg_modified)
                            cfgFile_modified.close()
                            initDict(determineJetToTauFakeRate_configFileNames, [ discriminator, category, btagDiscriminator, looseRegion, tightRegion, tauEtaBin_label ])
                            determineJetToTauFakeRate_configFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion][tauEtaBin_label] = cfgFileName_modified
                    
                            logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
                            initDict(determineJetToTauFakeRate_logFileNames, [ discriminator, category, btagDiscriminator, looseRegion, tightRegion, tauEtaBin_label ])
                            determineJetToTauFakeRate_logFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion][tauEtaBin_label] = logFileName
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" output of determineJetToTauFakeRate macro
#
hadd_determineBJetLooseToTightWeight_inputFileNames  = {} # key = discriminator
hadd_determineBJetLooseToTightWeight_logFileNames    = {} # key = discriminator
for discriminator in determineBJetLooseToTightWeight_outputFileNames.keys():

    hadd_determineBJetLooseToTightWeight_inputFileNames[discriminator] = []
    for looseRegion in determineBJetLooseToTightWeight_outputFileNames[discriminator].keys():
        for bJet1EtaBin in determineBJetLooseToTightWeight_outputFileNames[discriminator][looseRegion].keys():
            hadd_determineBJetLooseToTightWeight_inputFileNames[discriminator].append(determineBJetLooseToTightWeight_outputFileNames[discriminator][looseRegion][bJet1EtaBin])

    hadd_determineBJetLooseToTightWeight_logFileNames[discriminator] = hadd_determineBJetLooseToTightWeight_outputFileNames[discriminator].replace(".root", ".log")

hadd_determineJetToTauFakeRate_inputFileNames  = {} # key = discriminator
hadd_determineJetToTauFakeRate_logFileNames    = {} # key = discriminator
for discriminator in determineJetToTauFakeRate_outputFileNames.keys():

    hadd_determineJetToTauFakeRate_inputFileNames[discriminator] = []
    for category in determineJetToTauFakeRate_outputFileNames[discriminator].keys():
        for btagDiscriminator in determineJetToTauFakeRate_outputFileNames[discriminator][category].keys():
            for looseRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator].keys():
                for tightRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion].keys():
                    for tauEtaBin in determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion].keys():
                        hadd_determineJetToTauFakeRate_inputFileNames[discriminator].append(
                          determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion][tauEtaBin])

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
# build config files for running addBackgroundZTT and addBackgroundQCD macros
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
    cfg_modified += "process.addBackgroundZTT.tauPtBins = cms.vstring(%s)\n" % getStringRep_vstring([ getTauPtLabel(tauPtBin[0], tauPtBin[1]) for tauPtBin in tauPtBins])
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
        cfg_modified += "process.addBackgroundQCD.tauPtBins = cms.vstring(%s)\n" % getStringRep_vstring([ getTauPtLabel(tauPtBin[0], tauPtBin[1]) for tauPtBin in tauPtBins])
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

    outputFilePath_subdir = os.path.join(outputFilePath, discriminator)
    createFilePath_recursively(outputFilePath_subdir)    
    hadd_stage5_outputFileNames[discriminator] = os.path.join(outputFilePath_subdir, "hadd_stage5_%s.root" % discriminator)

    hadd_stage5_logFileNames[discriminator] = hadd_stage5_outputFileNames[discriminator].replace(".root", ".log")
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running prepareTauTauDatacards macro
#
print "Info: building config files for prepareTauTauDatacards macro"
prepareTauTauDatacards_configFileNames = {} # key = discriminator
prepareTauTauDatacards_outputFileNames = {} # key = discriminator
prepareTauTauDatacards_logFileNames    = {} # key = discriminator
for discriminator in discriminators.keys():

    outputFileName = "htt_tt.inputs-mssm-8TeV-0_%s.root" % discriminator
    prepareTauTauDatacards_outputFileNames[discriminator] = os.path.join(outputFilePath, outputFileName)
    
    cfgFileName_original = configFile_prepareTauTauDatacards
    cfgFile_original = open(cfgFileName_original, "r")
    cfg_original = cfgFile_original.read()
    cfgFile_original.close()
    cfg_modified  = cfg_original
    cfg_modified += "\n"
    cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage5_outputFileNames[discriminator]
    cfg_modified += "\n"
    cfg_modified += "process.fwliteOutput.fileName = cms.string('%s')\n" % prepareTauTauDatacards_outputFileNames[discriminator]
    cfg_modified += "\n"    
    cfg_modified += "process.prepareTauTauDatacards.tauPtBins = cms.vstring(%s)\n" % getStringRep_vstring([ getTauPtLabel(tauPtBin[0], tauPtBin[1]) for tauPtBin in tauPtBins])
    cfg_modified += "\n"
    cfgFileName_modified = os.path.join(outputFilePath, configFile_prepareTauTauDatacards.replace("_cfg.py", "_%s_cfg.py" % discriminator))
    cfgFile_modified = open(cfgFileName_modified, "w")
    cfgFile_modified.write(cfg_modified)
    cfgFile_modified.close()
    prepareTauTauDatacards_configFileNames[discriminator] = cfgFileName_modified

    logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
    prepareTauTauDatacards_logFileNames[discriminator] = logFileName
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running makeTauTauPlots macro
#
print "Info: building config files for makeTauTauPlots macro"
makeTauTauPlots_configFileNames = {} # key = discriminator, "QCDfromSSiso"/"QCDfromSSantiiso"
makeTauTauPlots_outputFileNames = {} # key = discriminator
makeTauTauPlots_logFileNames    = {} # key = discriminator, "QCDfromSSiso"/"QCDfromSSantiiso"
for discriminator in discriminators.keys():

    outputFilePath_plots = os.path.join(outputFilePath, "plots", "prefit", discriminator)
    createFilePath_recursively(outputFilePath_plots)
    outputFileName = "makeTauTauPlots_prefit_%s.png" % discriminator
    makeTauTauPlots_outputFileNames[discriminator] = os.path.join(outputFilePath_plots, outputFileName)

    for qcdOption in [ "QCDfromSSiso", "QCDfromSSantiiso" ]:
        
        cfgFileName_original = None
        if qcdOption == "QCDfromSSiso":
            cfgFileName_original = configFile_makeTauTauPlots_QCDfromSSiso
        elif qcdOption == "QCDfromSSantiiso":
            cfgFileName_original = configFile_makeTauTauPlots_QCDfromSSantiiso
        else:
            raise ValueError("Invalid qcdOption = '%s' !!" % qcdOption)
        cfgFile_original = open(cfgFileName_original, "r")
        cfg_original = cfgFile_original.read()
        cfgFile_original.close()
        cfg_modified  = cfg_original
        cfg_modified += "\n"
        cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage5_outputFileNames[discriminator]
        cfg_modified += "\n"    
        cfg_modified += "process.makeTauTauPlots.tauPtBinsToPlot = cms.vstring(%s)\n" % getStringRep_vstring([ getTauPtLabel(tauPtBin[0], tauPtBin[1]) for tauPtBin in tauPtBins])
        cfg_modified += "\n"
        cfg_modified += "process.makeTauTauPlots.outputFileName = cms.string('%s')\n" % makeTauTauPlots_outputFileNames[discriminator]
        cfg_modified += "\n"
        cfgFileName_modified = os.path.join(outputFilePath, cfgFileName_original.replace("_cfg.py", "_%s_cfg.py" % discriminator))
        cfgFile_modified = open(cfgFileName_modified, "w")
        cfgFile_modified.write(cfg_modified)
        cfgFile_modified.close()
        initDict(makeTauTauPlots_configFileNames, [ discriminator, qcdOption ])
        makeTauTauPlots_configFileNames[discriminator][qcdOption] = cfgFileName_modified

        logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
        initDict(makeTauTauPlots_logFileNames, [ discriminator, qcdOption ])
        makeTauTauPlots_logFileNames[discriminator][qcdOption] = logFileName
#--------------------------------------------------------------------------------

def make_MakeFile_vstring(list_of_strings):
    retVal = ""
    for i, string_i in enumerate(list_of_strings):
        if i > 0:
            retVal += " "
        retVal += string_i
    return retVal

# done building config files, now build Makefile...
makeFileName = os.path.join(outputFilePath, "Makefile_runTauTauAnalysis")
makeFile = open(makeFileName, "w")
makeFile.write("\n")
outputFileNames = []
for sample in FWLiteTauTauAnalyzer_outputFileNames.keys():
    for process in FWLiteTauTauAnalyzer_outputFileNames[sample].keys():
        for central_or_shift in FWLiteTauTauAnalyzer_outputFileNames[sample][process].keys():
            for region in FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift].keys():
                for tauPtBin in FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift][region].keys():
                    for discriminator in FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift][region][tauPtBin].keys():
                        outputFileNames.append(FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift][region][tauPtBin][discriminator])
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
for discriminator in determineBJetLooseToTightWeight_outputFileNames.keys():
    for looseRegion in determineBJetLooseToTightWeight_outputFileNames[discriminator].keys():
        for bJet1EtaBin in determineBJetLooseToTightWeight_outputFileNames[discriminator][looseRegion].keys():
            outputFileNames.append(determineBJetLooseToTightWeight_outputFileNames[discriminator][looseRegion][bJet1EtaBin])
for discriminator in determineJetToTauFakeRate_outputFileNames.keys():
    for category in determineJetToTauFakeRate_outputFileNames[discriminator].keys():
        for btagDiscriminator in determineJetToTauFakeRate_outputFileNames[discriminator][category].keys():
            for looseRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator].keys():
                for tightRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion].keys():
                    for tauEtaBin in determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion].keys():
                        outputFileNames.append(determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion][tauEtaBin])
for discriminator in hadd_determineBJetLooseToTightWeight_outputFileNames.keys():
    outputFileNames.append(hadd_determineBJetLooseToTightWeight_outputFileNames[discriminator])                    
for discriminator in hadd_determineJetToTauFakeRate_outputFileNames.keys():
    outputFileNames.append(hadd_determineJetToTauFakeRate_outputFileNames[discriminator])
for discriminator in hadd_stage4_outputFileNames.keys():
    outputFileNames.append(hadd_stage4_outputFileNames[discriminator])
for discriminator in addBackgroundZTT_outputFileNames.keys():
    outputFileNames.append(addBackgroundZTT_outputFileNames[discriminator])
for discriminator in addBackgroundQCD_outputFileNames.keys():
    for qcdOption in addBackgroundQCD_outputFileNames[discriminator].keys():
        outputFileNames.append(addBackgroundQCD_outputFileNames[discriminator][qcdOption])
for discriminator in hadd_stage5_outputFileNames.keys():
    outputFileNames.append(hadd_stage5_outputFileNames[discriminator])    
for discriminator in prepareTauTauDatacards_outputFileNames.keys():
    outputFileNames.append(prepareTauTauDatacards_outputFileNames[discriminator])
for discriminator in makeTauTauPlots_outputFileNames.keys():
    outputFileNames.append(makeTauTauPlots_outputFileNames[discriminator])
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
makeFile.write("\techo 'Finished running MSSM Higgs -> tau tau -> tau_h tau_h analysis.'\n")
makeFile.write("\n")
for sample in FWLiteTauTauAnalyzer_outputFileNames.keys():
    for process in FWLiteTauTauAnalyzer_outputFileNames[sample].keys():
        for central_or_shift in FWLiteTauTauAnalyzer_outputFileNames[sample][process].keys():
            for region in FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift].keys():
                for tauPtBin in FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift][region].keys():
                    for discriminator in FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift][region][tauPtBin].keys():
                        makeFile.write("%s: %s\n" %
                          (FWLiteTauTauAnalyzer_outputFileNames[sample][process][central_or_shift][region][tauPtBin][discriminator],
                           make_MakeFile_vstring(FWLiteTauTauAnalyzer_inputFileNames[sample][process][central_or_shift][region][tauPtBin_label][discriminator])))
                        makeFile.write("\t%s%s %s &> %s\n" %
                          (nice, executable_FWLiteTauTauAnalyzer,
                           FWLiteTauTauAnalyzer_configFileNames[sample][process][central_or_shift][region][tauPtBin][discriminator],
                           FWLiteTauTauAnalyzer_logFileNames[sample][process][central_or_shift][region][tauPtBin][discriminator]))
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
for discriminator in determineBJetLooseToTightWeight_outputFileNames.keys():
    for looseRegion in determineBJetLooseToTightWeight_outputFileNames[discriminator].keys():
        for bJet1EtaBin in determineBJetLooseToTightWeight_outputFileNames[discriminator][looseRegion].keys():
            makeFile.write("%s: %s\n" %                       
              (determineBJetLooseToTightWeight_outputFileNames[discriminator][looseRegion][bJet1EtaBin],
               hadd_stage3_outputFileNames[discriminator]["qcdRegion"]))
            makeFile.write("\t%s%s %s &> %s\n" %
              (nice, executable_determineJetToTauFakeRate,
               determineBJetLooseToTightWeight_configFileNames[discriminator][looseRegion][bJet1EtaBin],
               determineBJetLooseToTightWeight_logFileNames[discriminator][looseRegion][bJet1EtaBin]))
for discriminator in determineJetToTauFakeRate_outputFileNames.keys():
    for category in determineJetToTauFakeRate_outputFileNames[discriminator].keys():
        for btagDiscriminator in determineJetToTauFakeRate_outputFileNames[discriminator][category].keys():
            for looseRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator].keys():
                for tightRegion in determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion].keys():
                    for tauEtaBin in determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion].keys():
                        makeFile.write("%s: %s\n" %                       
                          (determineJetToTauFakeRate_outputFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion][tauEtaBin],
                           hadd_stage3_outputFileNames[discriminator]["qcdRegion"]))
                        makeFile.write("\t%s%s %s &> %s\n" %
                          (nice, executable_determineJetToTauFakeRate,
                           determineJetToTauFakeRate_configFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion][tauEtaBin],
                           determineJetToTauFakeRate_logFileNames[discriminator][category][btagDiscriminator][looseRegion][tightRegion][tauEtaBin]))
makeFile.write("\n")
for discriminator in hadd_determineBJetLooseToTightWeight_outputFileNames.keys():
    makeFile.write("%s: %s\n" %
      (hadd_determineBJetLooseToTightWeight_outputFileNames[discriminator],
       make_MakeFile_vstring(hadd_determineBJetLooseToTightWeight_inputFileNames[discriminator])))
    makeFile.write("\t%s%s %s\n" %
      (nice, executable_rm,
       hadd_determineBJetLooseToTightWeight_outputFileNames[discriminator]))
    makeFile.write("\t%s%s %s %s &> %s\n" %
      (nice, executable_hadd,
       hadd_determineBJetLooseToTightWeight_outputFileNames[discriminator],
       make_MakeFile_vstring(hadd_determineBJetLooseToTightWeight_inputFileNames[discriminator]),
       hadd_determineBJetLooseToTightWeight_logFileNames[discriminator]))
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
for discriminator in prepareTauTauDatacards_outputFileNames.keys():
    makeFile.write("%s: %s\n" %
      (prepareTauTauDatacards_outputFileNames[discriminator],
       hadd_stage5_outputFileNames[discriminator]))
    makeFile.write("\t%s%s %s &> %s\n" %
      (nice, executable_prepareTauTauDatacards,
       prepareTauTauDatacards_configFileNames[discriminator],
       prepareTauTauDatacards_logFileNames[discriminator]))
makeFile.write("\n")    
for discriminator in makeTauTauPlots_outputFileNames.keys():
    makeFile.write("%s: %s\n" %
      (makeTauTauPlots_outputFileNames[discriminator],
       hadd_stage5_outputFileNames[discriminator]))
    for qcdOption in makeTauTauPlots_configFileNames[discriminator].keys():       
        makeFile.write("\t%s%s %s &> %s\n" %
          (nice, executable_makeTauTauPlots,
           makeTauTauPlots_configFileNames[discriminator][qcdOption],
           makeTauTauPlots_logFileNames[discriminator][qcdOption]))    
makeFile.write("\n")
makeFile.write(".PHONY: clean\n")
makeFile.write("clean:\n")
makeFile.write("\t%s %s\n" % (executable_rm, make_MakeFile_vstring(outputFileNames)))
makeFile.write("\techo 'Finished deleting old files.'\n")
makeFile.write("\n")
makeFile.close()

print("Finished building Makefile. Now execute 'make -j 8 -f %s'." % makeFileName)
