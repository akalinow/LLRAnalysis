#!/usr/bin/env python

from LLRAnalysis.HadTauStudies.recoSampleDefinitionsAHtoTauTau_8TeV_grid_cfi import *

import os
import re
import time

jobId = '2014Jan10'

version = "v1_4"

inputFilePath  = "/data2/veelken/CMSSW_5_3_x/Ntuples/AHtoTauTau/%s/%s" % (jobId, version)

outputFilePath = "/data1/veelken/tmp/tauTauAnalysis/%s_1/" % version

_picobarns =  1.0
_femtobarns = 1.0e-3

intLumiData = recoSampleDefinitionsAHtoTauTau_8TeV['TARGET_LUMI']
print "intLumiData = %1.1f fb^-1" % (recoSampleDefinitionsAHtoTauTau_8TeV['TARGET_LUMI']*_femtobarns)

lumiScale_DY = 2.12
stitchingWeights_DY = [ 1.0, 0.412/lumiScale_DY, 0.167/lumiScale_DY, 0.097/lumiScale_DY, 0.0759/lumiScale_DY ]

lumiScale_W = 8.83
stitchingWeights_W = [ 1.0, 1.80/lumiScale_W, 0.559/lumiScale_W, 0.357/lumiScale_W, 0.340/lumiScale_W ]

def getLumiScale(sample, x_sec = -1.):
    if x_sec < 0.:
        x_sec = recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['x_sec']    
    return intLumiData*x_sec/recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['events_processed']

samples = {
    'Data' : {
        'processes' : [ "data_obs" ],
        'inputFiles' : [
            "data_Run2012A_22Jan2013_v1",
            "data_Run2012B_22Jan2013_v1",
            "data_Run2012C_22Jan2013_v1",
            "data_Run2012D_22Jan2013_v1"
        ]
    },
    'DYJets' : {
        'processes' : [ "ZTT", "ZLL", "ZJ" ],
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
            "W2Jets",
            "W3Jets",
            "W4Jets"
        ],
        'lumiScale' : lumiScale_W,
        'stitchingWeights' : stitchingWeights_W
    },
    'DYJets_Embedded' : {
        'processes' : [ "ZTT_Embedded" ],
        'inputFiles' : [
            "pfEmbed_Run2012A_22Jan2013_v1",
            "pfEmbed_Run2012B_22Jan2013_v1",
            "pfEmbed_Run2012C_22Jan2013_v1",
            "pfEmbed_Run2012D_22Jan2013_v1"
        ],
        'lumiScale'  : 1.0
    },
    'TTJets_Embedded' : {
        'processes' : [ "TT_Embedded" ],
        'inputFiles' : [ "rhEmbed_TTJetsFullLept" ],
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
    }    
}
for sample in [ "TTJetsHadronic", "TTJetsSemiLept", "TTJetsFullLept", "Tbar_tW", "T_tW" ]:
    samples[sample] = {
        'processes' : [ "TT" ],
        'inputFiles' : [ sample ],
        'lumiScale' : getLumiScale(sample),
        'addWeights' : [ "topPtWeightNom" ]
    }
for sample in [ "WWJetsTo2L2Nu", "WZJetsTo3LNu", "WZJetsTo2L2Q", "ZZJetsTo4L", "ZZJetsTo2L2Nu", "ZZJetsTo2L2Q" ]:
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
        'lumiScale' : getLumiScale(ggSampleName, 1.*_picobarns) # CV: normalize MSSM Higgs -> tautau signal MCs to cross-section of 1pb
    }
    bbSampleName = "HiggsSUSYBB%1.0f" % massPoint
    samples[bbSampleName] = {
        'processes' : [ "bbH%1.0f" % massPoint ],
        'inputFiles' : [ bbSampleName ],
        'lumiScale' : getLumiScale(bbSampleName, 1.*_picobarns) # CV: normalize MSSM Higgs -> tautau signal MCs to cross-section of 1pb
    }

discriminators = {
    'HPScombIso3HitsMedium' : {
        'tau1Selection'         : "l1MediumDB3HIso > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
        'tau2Selection'         : "l2MediumDB3HIso > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5",
        'tau1Selection_relaxed' : "l1RawDB3HIso < 4.0 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
        'tau2Selection_relaxed' : "l2RawDB3HIso < 4.0 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5"
    },
    'MVAwLToldDMsTight' : {
        'tau1Selection'         : "l1TightMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
        'tau2Selection'         : "l2TightMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5",
        'tau1Selection_relaxed' : "l1VLooseMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
        'tau2Selection_relaxed' : "l2VLooseMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5"
    }
}

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
    'CMS_ttbarPtReweight_8TeVUp' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : [ "topPtWeightUp" ]
    },
    'CMS_ttbarPtReweight_8TeVDown' : {
        'inputFilePath_extension' : "nom",
        'addWeights_extension'    : [ "topPtWeightDown" ]
    }
}

regions = [ "OSisoLooseBtag", "OSantiisoLooseBtag", "SSisoLooseBtag", "SSantiisoLooseBtag", "OSisoTightBtag", "OSantiisoTightBtag", "SSisoTightBtag", "SSantiisoTightBtag" ]

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
executable_prepareTauTauDatacards    = execDir + 'prepareTauTauDatacards'
executable_hadd                      = 'hadd'
executable_rm                        = 'rm -f'

nice = 'nice '

configFile_FWLiteTauTauAnalyzer      = 'FWLiteTauTauAnalyzer_cfg.py'
configFile_determineJetToTauFakeRate = 'determineJetToTauFakeRate_cfg.py'
configFile_prepareTauTauDatacards    = 'prepareTauTauDatacards_cfg.py'

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
    retVal.extend(addWeights_extension_shift)
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

#--------------------------------------------------------------------------------
#
# build config files for running FWLiteTauTauAnalyzer macro
#
print "Info: building config files for FWLiteTauTauAnalyzer macro"
FWLiteTauTauAnalyzer_configFileNames = {} # key = sample, central_or_shift, region, tauPtBin, discriminator
FWLiteTauTauAnalyzer_outputFileNames = {} # key = sample, central_or_shift, region, tauPtBin, discriminator
FWLiteTauTauAnalyzer_logFileNames    = {} # key = sample, central_or_shift, region, tauPtBin, discriminator
for sample in samples.keys():
    
    print "processing sample = %s" % sample

    FWLiteTauTauAnalyzer_configFileNames[sample] = {}
    FWLiteTauTauAnalyzer_outputFileNames[sample] = {}
    FWLiteTauTauAnalyzer_logFileNames[sample]    = {}

    for central_or_shift in central_or_shifts.keys():

        if central_or_shift in [ 'CMS_ttbarPtReweight_8TeVUp', 'CMS_ttbarPtReweight_8TeVDown' ] and not sample in [ "TTJetsHadronic", "TTJetsSemiLept", "TTJetsFullLept", "TTJets_Embedded" ]:
            continue

        inputFileNames = []
        for inputFilePath_extension_sample in samples[sample]['inputFiles']:
            inputFileNames_sample = getInputFileNames(os.path.join(inputFilePath, central_or_shifts[central_or_shift]['inputFilePath_extension'], inputFilePath_extension_sample))
            inputFileNames.extend(inputFileNames_sample)
        if len(inputFileNames) == 0:
            raise ValueError("Failed to find input files for sample = '%s' !!" % sample)
        if central_or_shift == "":
            print " central_or_shift = '%s': inputFileNames = %s" %  (central_or_shift, inputFileNames)
        
        FWLiteTauTauAnalyzer_configFileNames[sample][central_or_shift] = {}
        FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift] = {}
        FWLiteTauTauAnalyzer_logFileNames[sample][central_or_shift]    = {}

        for region in regions:
            
            FWLiteTauTauAnalyzer_configFileNames[sample][central_or_shift][region] = {}
            FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region] = {}
            FWLiteTauTauAnalyzer_logFileNames[sample][central_or_shift][region]    = {}

            for tauPtBin in tauPtBins:

                tauPtMin = tauPtBin[0]
                tauPtMax = tauPtBin[1]
                tauPtBin_label = None
                if tauPtMin > 0. and tauPtMax > 0.:
                    tauPtBin_label = "tauPt%1.0fto%1.0f" % (tauPtMin, tauPtMax)
                elif tauPtMin > 0.:
                    tauPtBin_label = "tauPtGt%1.0f" % tauPtMin
                elif tauPtMax > 0.:
                    tauPtBin_label = "tauPtLt%1.0f" % tauPtMax
                else:
                    tauPtBin_label = "allTauPt"

                FWLiteTauTauAnalyzer_configFileNames[sample][central_or_shift][region][tauPtBin_label] = {}
                FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][tauPtBin_label] = {}
                FWLiteTauTauAnalyzer_logFileNames[sample][central_or_shift][region][tauPtBin_label]    = {}

                for discriminator in discriminators.keys():

                    outputFileName = os.path.join(outputFilePath, "FWLiteTauTauAnalyzer_%s_%s_%s_%s_%s.root" % (discriminator, sample, region, tauPtBin_label, central_or_shift))
                    outputFileName = outputFileName.replace("__", "_")
                    outputFileName = outputFileName.replace("_.", ".")
                    ##print " region = %s: outputFileName = '%s'" % (region, outputFileName)
                    FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][tauPtBin_label][discriminator] = outputFileName

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
                    cfg_modified += "process.FWLiteTauTauAnalyzer.process = cms.string('%s')\n" % sample
                    if sample in recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'].keys() and recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['type'] == 'bsmMC':
                        massPoint_match = massPoint_matcher.match(sample)
                        if massPoint_match:
                            massPoint = massPoint_match.group('massPoint')
                            cfg_modified += "process.FWLiteTauTauAnalyzer.massPoint = cms.double(%s)" % massPoint
                        else:
                            raise ValueError("Failed to decode mass-point for sample = %s !!" % sample)
                    cfg_modified += "process.FWLiteTauTauAnalyzer.region = cms.string('%s')\n" % region
                    cfg_modified += "process.FWLiteTauTauAnalyzer.central_or_shift = cms.string('%s')\n" % central_or_shift
                    tau1Selection = None
                    tau2Selection = None
                    if region.find("OSiso") != -1 or region.find("SSiso") != -1:
                        tau1Selection = discriminators[discriminator]['tau1Selection']
                        tau2Selection = discriminators[discriminator]['tau2Selection']
                    elif region.find("OSantiiso") != -1 or region.find("SSantiiso") != -1:
                        tau1Selection = discriminators[discriminator]['tau1Selection_relaxed']
                        tau2Selection = discriminators[discriminator]['tau2Selection_relaxed']
                    else:
                        raise ValueError("No tau selection defined for region = '%s' !!" % region)
                    cfg_modified += "process.FWLiteTauTauAnalyzer.tau1Selection = cms.string('%s')\n" % tau1Selection
                    cfg_modified += "process.FWLiteTauTauAnalyzer.tau1PtMin = cms.double(45.)\n"
                    cfg_modified += "process.FWLiteTauTauAnalyzer.tau1PtMax = cms.double(-1.)\n"
                    cfg_modified += "process.FWLiteTauTauAnalyzer.tau2Selection = cms.string('%s')\n" % tau2Selection
                    cfg_modified += "process.FWLiteTauTauAnalyzer.tau2PtMin = cms.double(%1.0f)\n" % tauPtMin
                    cfg_modified += "process.FWLiteTauTauAnalyzer.tau2PtMax = cms.double(%1.0f)\n" % tauPtMax
                    if region.find("LooseBtag") != -1:
                        cfg_modified += "process.FWLiteTauTauAnalyzer.applyTightBtag = cms.bool(False)\n"
                    else: 
                        cfg_modified += "process.FWLiteTauTauAnalyzer.applyTightBtag = cms.bool(True)\n"
                    if 'lumiScale' in samples[sample].keys():
                        cfg_modified += "process.FWLiteTauTauAnalyzer.lumiScale = cms.double(%f)\n" % samples[sample]['lumiScale']
                    if 'stitchingWeights' in samples[sample].keys():
                        cfg_modified += "process.FWLiteTauTauAnalyzer.stitchingWeights = cms.vdouble(%s)\n" % getStringRep_vdouble(samples[sample]['stitchingWeights'])
                    addWeights = []
                    if 'addWeights' in samples[sample].keys():
                        addWeights.extend(samples[sample]['addWeights'])
                    addWeights = addWeights_shift_and_remove_central(addWeights, central_or_shifts[central_or_shift]['addWeights_extension'])
                    cfg_modified += "process.FWLiteTauTauAnalyzer.addWeights = cms.vstring(%s)\n" % getStringRep_vstring(addWeights)
                    cfgFileName_modified = os.path.join(outputFilePath, cfgFileName_original.replace("_cfg.py", "_%s_%s_%s_%s_%s_cfg.py" % (discriminator, sample, region, tauPtBin_label, central_or_shift)))
                    cfgFileName_modified = cfgFileName_modified.replace("__", "_")
                    cfgFileName_modified = cfgFileName_modified.replace("_.", ".")
                    ##print " cfgFileName_modified = '%s'" % cfgFileName_modified
                    cfgFile_modified = open(cfgFileName_modified, "w")
                    cfgFile_modified.write(cfg_modified)
                    cfgFile_modified.close()
                    FWLiteTauTauAnalyzer_configFileNames[sample][central_or_shift][region][tauPtBin_label][discriminator] = cfgFileName_modified
                    
                    logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
                    FWLiteTauTauAnalyzer_logFileNames[sample][central_or_shift][region][tauPtBin_label][discriminator] = logFileName
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" subset of histograms
# produced by FWLiteTauIdEffAnalyzer macro that are neccessary to compute the jet -> tau fake-rates
#
hadd_stage1_inputFileNames  = {} # key = (tau ID) discriminator, btagDiscriminator
hadd_stage1_outputFileNames = {} # key = (tau ID) discriminator, btagDiscriminator
for discriminator in discriminators.keys():

    hadd_stage1_inputFileNames[discriminator]  = {}
    hadd_stage1_outputFileNames[discriminator] = {}
    
    for btagDiscriminator in [ "LooseBtag", "TightBtag" ]:
        for sample in FWLiteTauTauAnalyzer_outputFileNames.keys():
            for central_or_shift in FWLiteTauTauAnalyzer_outputFileNames[sample].keys():        
                for region in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift].keys():
                    if region.find(btagDiscriminator) != -1:
                        for tauPtBin in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region].keys():
                            hadd_inputFileNames[discriminator][btagDiscriminator].append(FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][tauPtBin][discriminator])
        hadd_stage1_outputFileName[discriminator][btagDiscriminator] = os.path.join(outputFilePath, "hadd_%s_%s.root" % (discriminator, btagDiscriminator))
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running determineJetToTauFakeRate macro
#
print "Info: building config files for determineJetToTauFakeRate macro"
determineJetToTauFakeRate_configFileNames = {} # key = (tau ID) discriminator, btagDiscriminator
determineJetToTauFakeRate_outputFileNames = {} # key = (tau ID) discriminator, btagDiscriminator
determineJetToTauFakeRate_logFileNames    = {} # key = (tau ID) discriminator, btagDiscriminator
for discriminator in discriminators.keys():

    determineJetToTauFakeRate_configFileNames[discriminator] = {}
    determineJetToTauFakeRate_outputFileNames[discriminator] = {}
    determineJetToTauFakeRate_logFileNames[discriminator]    = {}
    
    for btagDiscriminator in [ "LooseBtag", "TightBtag" ]:

        outputFileName = os.path.join(outputFilePath, "determineJetToTauFakeRate_%s_%s.root" % (discriminator, btagDiscriminator))
        determineJetToTauFakeRate_outputFileNames[discriminator][btagDiscriminator] = outputFileName

        cfgFileName_original = configFile_determineJetToTauFakeRate
        cfgFile_original = open(cfgFileName_original, "r")
        cfg_original = cfgFile_original.read()
        cfgFile_original.close()
        cfg_modified  = cfg_original
        cfg_modified += "\n"
        cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage1_outputFileName[discriminator][btagDiscriminator]
        cfg_modified += "\n"
        cfg_modified += "process.fwliteOutput.fileName = cms.string('%s')\n" % outputFileName
        cfg_modified += "\n"
        looseRegion = cms.string("SSantiiso" % btagDiscriminator)
        cfg_modified += "process.determineJetToTauFakeRate.looseRegion = cms.string('%s')\n" % looseRegion
        tightRegion = cms.string("SSiso" % btagDiscriminator)
        cfg_modified += "process.determineJetToTauFakeRate.tightRegion = cms.string('%s')\n" % tightRegion
        cfgFileName_modified = os.path.join(outputFilePath, cfgFileName_original.replace("_cfg.py", "_%s_%s_cfg.py" % (discriminator, btagDiscriminator)))
        cfgFile_modified = open(cfgFileName_modified, "w")
        cfgFile_modified.write(cfg_modified)
        cfgFile_modified.close()
        determineJetToTauFakeRate_configFileNames[discriminator][btagDiscriminator] = cfgFileName_modified

        logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
        determineJetToTauFakeRate_configFileNames[discriminator][btagDiscriminator] = logFileName
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" all histograms
# produced by FWLiteTauIdEffAnalyzer macro
#
hadd_stage2_inputFileNames = []
for sample in FWLiteTauTauAnalyzer_outputFileNames.keys():
    for central_or_shift in FWLiteTauTauAnalyzer_outputFileNames[sample].keys():
        for region in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift].keys():
            for tauPtBin in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region].keys():
                for discriminator in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][tauPtBin].keys():
                    hadd_inputFileNames.append(FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][tauPtBin][discriminator])
hadd_stage2_outputFileName = os.path.join(outputFilePath, "FWLiteTauTauAnalyzer_all.root")
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build config files for running prepareTauTauDatacards macro
#
print "Info: building config files for prepareTauTauDatacards macro"
prepareTauTauDatacards_configFileName = os.path.join(outputFilePath, configFile_prepareTauTauDatacards)
prepareTauTauDatacards_outputFileName = "htt_tt.inputs-mssm-8TeV-0.root"
prepareTauTauDatacards_logFileName    = prepareTauTauDatacards_configFileName.replace("_cfg.py", ".log")

cfgFileName_original = configFile_prepareTauTauDatacards
cfgFile_original = open(cfgFileName_original, "r")
cfg_original = cfgFile_original.read()
cfgFile_original.close()
cfg_modified  = cfg_original
cfg_modified += "\n"
cfg_modified += "process.fwliteInput.fileNames = cms.vstring('%s')\n" % hadd_stage2_outputFileName
cfg_modified += "\n"
cfg_modified += "process.fwliteOutput.fileName = cms.string('%s')\n" % prepareTauTauDatacards_outputFileName
cfgFile_modified = open(prepareTauTauDatacards_configFileName, "w")
cfgFile_modified.write(cfg_modified)
cfgFile_modified.close()
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
    for central_or_shift in FWLiteTauTauAnalyzer_outputFileNames[sample].keys():
        for region in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift].keys():
            for tauPtBin in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region].keys():
                for discriminator in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][tauPtBin].keys():
                    outputFileNames.append(FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][tauPtBin][discriminator])
for discriminator in determineJetToTauFakeRate_outputFileNames.keys():
    for btagDiscriminator determineJetToTauFakeRate_outputFileNames[discriminator].keys():
        outputFileNames.append(hadd_stage1_outputFileName[discriminator][btagDiscriminator])
        outputFileNames.append(determineJetToTauFakeRate_outputFileNames[discriminator][btagDiscriminator])
outputFileNames.append(hadd_stage2_outputFileName)
outputFileNames.append(prepareTauTauDatacards_outputFileName)
makeFile.write("all: %s\n" % make_MakeFile_vstring(outputFileNames))
makeFile.write("\techo 'Finished running MSSM Higgs -> tau tau -> tau_h tau_h analysis.'\n")
makeFile.write("\n")
for sample in FWLiteTauTauAnalyzer_outputFileNames.keys():
    for central_or_shift in FWLiteTauTauAnalyzer_outputFileNames[sample].keys():
        for region in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift].keys():
            for tauPtBin in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region].keys():
                for discriminator in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][tauPtBin].keys():
                    makeFile.write("%s:\n" %
                      (FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][tauPtBin][discriminator]))
                    makeFile.write("\t%s%s %s &> %s\n" %
                      (nice, executable_FWLiteTauTauAnalyzer,
                       FWLiteTauTauAnalyzer_configFileNames[sample][central_or_shift][region][tauPtBin][discriminator],
                       FWLiteTauTauAnalyzer_logFileNames[sample][central_or_shift][region][tauPtBin][discriminator]))
makeFile.write("\n")
for discriminator in hadd_stage1_outputFileNames.keys():
    for btagDiscriminator hadd_stage1_outputFileNames[discriminator].keys():
        makeFile.write("%s: %s\n" %
          (hadd_stage1_outputFileName[discriminator][btagDiscriminator],
           make_MakeFile_vstring(hadd_stage1_inputFileNames[discriminator][btagDiscriminator])))
        makeFile.write("\t%s%s %s\n" %
          (nice, executable_rm,
           hadd_stage1_outputFileName[discriminator][btagDiscriminator]))
        makeFile.write("\t%s%s %s %s\n" %
          (nice, executable_hadd,
           hadd_stage1_outputFileName[discriminator][btagDiscriminator], make_MakeFile_vstring(hadd_stage1_inputFileNames[discriminator][btagDiscriminator])))
makeFile.write("\n")
for discriminator in determineJetToTauFakeRate_outputFileNames.keys():
    for btagDiscriminator determineJetToTauFakeRate_outputFileNames[discriminator].keys():
        makeFile.write("%s: %s\n" %                       
          (determineJetToTauFakeRate_outputFileNames[discriminator][btagDiscriminator],
           hadd_stage1_outputFileName[discriminator][btagDiscriminator]))
        makeFile.write("\t%s%s &> %s\n" %
          (nice, executable_determineJetToTauFakeRate,
           determineJetToTauFakeRate_configFileNames[discriminator][btagDiscriminator],
           determineJetToTauFakeRate_logFileNames[discriminator][btagDiscriminator]))
makeFile.write("\n")    
makeFile.write("%s: %s\n" %
  (hadd_stage2_outputFileName,
   make_MakeFile_vstring(hadd_stage2_inputFileNames)))
makeFile.write("\t%s%s %s\n" %
  (nice, executable_rm,
   hadd_stage2_outputFileName))
makeFile.write("\t%s%s %s %s\n" %
  (nice, executable_hadd,
   hadd_stage2_outputFileName, make_MakeFile_vstring(hadd_stage2_inputFileNames)))
makeFile.write("%s: %s\n" %
  (prepareTauTauDatacards_outputFileName,
   hadd_stage2_outputFileName))
makeFile.write("\t%s%s %s &> %s\n" %
  (nice, executable_prepareTauTauDatacards,
   prepareTauTauDatacards_configFileName,
   prepareTauTauDatacards_logFileName))
makeFile.write("\n")
makeFile.write(".PHONY: clean\n")
makeFile.write("clean:\n")
makeFile.write("\t%s %s\n" % (executable_rm, make_MakeFile_vstring(outputFileNames)))
makeFile.write("\techo 'Finished deleting old files.'\n")
makeFile.write("\n")
makeFile.close()

print("Finished building Makefile. Now execute 'make -j 4 -f %s'." % makeFileName)
