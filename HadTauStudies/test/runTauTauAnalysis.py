#!/usr/bin/env python

from LLRAnalysis.HadTauStudies.recoSampleDefinitionsAHtoTauTau_8TeV_grid_cfi import *

import os

jobId = '2014Jan10'

version = "v1_4"

inputFilePath  = "/store/group/phys_higgs/cmshtt/CMSSW_5_3_x/Ntuples/AHtoTauTau/%s/%s" % (jobId, version)

outputFilePath = "/data1/veelken/tmp/tauTauAnalysis/%s_1/" % version

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
        'processes' : [ "Data" ],
        'inputFiles' : [
            "data_Run2012A_22Jan2013_v1",
            "data_Run2012B_22Jan2013_v1",
            "data_Run2012C_22Jan2013_v1",
            "data_Run2012D_22Jan2013_v1"
        ]
    ],
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
    ],
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
        'lumiScale' : getLumiScale(sample)
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
        'inputFiles' : [ sample ],
        'lumiScale' : getLumiScale(ggSampleName, 1.*_picobarns) # CV: normalize MSSM Higgs -> tautau signal MCs to cross-section of 1pb
    }
    bbSampleName = "HiggsSUSYBB%1.0f" % massPoint
    samples[bbSampleName] = {
        'processes' : [ "bbH%1.0f" % massPoint ],
        'inputFiles' : [ sample ],
        'lumiScale' : getLumiScale(bbSampleName, 1.*_picobarns) # CV: normalize MSSM Higgs -> tautau signal MCs to cross-section of 1pb
    }

discriminators = {
    'HPScombIso3HitsMedium' : {
        'tau1Selection'         : "l1MediumDB3HIso > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
        'tau2Selection'         : "l2MediumDB3HIso > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5",
        'tau1Selection_relaxed' : "l1RawDB3HIso < 4.0 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
        'tau2Selection_relaxed' : "l2RawDB3HIso < 4.0 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5"
    },
    'MVAwLToldDMsTight' = {
        'tau1Selection'         : "l1TightMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
        'tau2Selection'         : "l2TightMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5",
        'tau1Selection_relaxed' : "l1VLooseMVAwLT > 0.5 && l1againstMuonLoose2 > 0.5 && l1againstElectronLoose > 0.5",
        'tau2Selection_relaxed' : "l2VLooseMVAwLT > 0.5 && l2againstMuonLoose2 > 0.5 && l2againstElectronLoose > 0.5 && l2againstElectronLooseMVA3 > 0.5"
    }
}

central_or_shifts = {
    '' : {
        'inputFilePath_extension' : "nom",
        'addWeightss_extension'   : []
    },
    'CMS_scale_t_tautau_8TeVUp' : {
        'inputFilePath_extension' : "up",
        'addWeightss_extension'   : []
    }
    'CMS_scale_t_tautau_8TeVDown' : {
        'inputFilePath_extension' : "down",
        'addWeightss_extension'   : []
    },
    'CMS_ttbarPtReweight_8TeVUp' : {
        'inputFilePath_extension' : "nom",
        'addWeightss_extension'   : [ "topPtWeightUp" ]
    },
    'CMS_ttbarPtReweight_8TeVDown' : {
        'inputFilePath_extension' : "nom",
        'addWeightss_extension'   : [ "topPtWeightDown" ]
    }
}

regions = [ "OSisoLooseBtag", "OSantiisoLooseBtag", "SSisoLooseBtag", "SSantiisoLooseBtag", "OSisoTightBtag", "OSantiisoTightBtag", "SSisoTightBtag", "SSantiisoTightBtag" ]

execDir = "%s/bin/%s/" % (os.environ['CMSSW_BASE'], os.environ['SCRAM_ARCH'])

executable_FWLiteTauTauAnalyzer = execDir + 'FWLiteTauTauAnalyzer'
executable_hadd                 = 'hadd'
executable_rm                   = 'rm -f'

nice = 'nice '

configFile_FWLiteTauTauAnalyzer = 'FWLiteTauTauAnalyzer_cfg.py'

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
massPoint_matcher = re.compile(key_value_pair_regex)

def getInputFileNames(inputFilePath):
    inputFileNames = []
    try:
        inputFileNames = [ os.path.join(inputFilePath, file) for file in os.listdir(inputFilePath) ]
    except OSError:
        print "inputFilePath = %s does not exist --> skipping !!" % inputFilePath
        continue
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
        retVal += "%s" % item
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
print "Info: building config files for FWLiteTauTauAnalyzer"
FWLiteTauTauAnalyzer_configFileNames = {} # key = sample, central_or_shift, region, discriminator
FWLiteTauTauAnalyzer_outputFileNames = {} # key = sample, central_or_shift, region, discriminator
FWLiteTauTauAnalyzer_logFileNames    = {} # key = sample, central_or_shift, region, discriminator
for sample in samples.keys():
    
    print "processing sample = %s" % sample

    FWLiteTauTauAnalyzer_configFileNames[sample] = {}
    FWLiteTauTauAnalyzer_outputFileNames[sample] = {}
    FWLiteTauTauAnalyzer_logFileNames[sample]    = {}

    for central_or_shift in central_or_shifts.keys():

        inputFileNames = getInputFileNames(os.path.join(inputFilePath, central_or_shifts[central_or_shift]['inputFilePath_extension'], sample))
        print "central_or_shift = %s: inputFileNames = %s" %  (central_or_shift, inputFileNames)
        
        FWLiteTauTauAnalyzer_configFileNames[sample][central_or_shift] = {}
        FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift] = {}
        FWLiteTauTauAnalyzer_logFileNames[sample][central_or_shift]    = {}

        for region in regions:
            
            FWLiteTauTauAnalyzer_configFileNames[sample][central_or_shift][region] = {}
            FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region] = {}
            FWLiteTauTauAnalyzer_logFileNames[sample][central_or_shift][region]    = {}

            for discriminator in discriminators.keys():

                outputFileName = os.path.join(outputFilePath, "FWLiteTauTauAnalyzer_%s_%s_%s_%s.root" % (discriminator, sample, region, central_or_shift))
                print "region = %s: outputFileName = '%s'" % (region, outputFileName)
                FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][discriminator] = outputFileName

                cfgFileName_original = configFile_FWLiteTauTauAnalyzer_outputFileNames
                cfgFile_original = open(cfgFileName_original, "r")
                cfg_original = cfgFile_original.read()
                cfgFile_original.close()
                cfg_modified  = cfg_original
                cfg_modified += "\n"
                cfg_modified += "process.fwliteInput.fileNames = cms.vstring()\n"
                for inputFileName in inputFileNames:
                    cfg_modified += "process.fwliteInput.fileNames.append('%s')\n" % inputFileName
                cfg_modified += "\n"
                cfg_modified += "process.fwliteOutput.fileNames = cms.string('%s')\n" % outputFileName
                cfg_modified += "\n"
                cfg_modified += "process.FWLiteTauTauAnalyzer.sample = cms.string('%s')\n" % sample
                if recoSampleDefinitionsAHtoTauTau_8TeV['RECO_SAMPLES'][sample]['type'] == 'bsmMC':
                    massPoint_match = massPoint_matcher.match(sample)
                    if massPoint_match:
                        massPoint = massPoint_match.group('massPoint')
                        cfg_modified += "process.FWLiteTauTauAnalyzer.massPoint = cms.double(%s)" % massPoint
                    else:
                        raise ValueError("Failed to decode mass-point for sample = %s !!" % sample)
                cfg_modified += "process.FWLiteTauTauAnalyzer.region = cms.string('%s')\n" % region
                cfg_modified += "process.FWLiteTauTauAnalyzer.central_or_shift = cms.string('%s')\n" % central_or_shift
                cfg_modified += "process.FWLiteTauTauAnalyzer.tau1PtMin = cms.double(45.)\n" % tau1PtMin
                cfg_modified += "process.FWLiteTauTauAnalyzer.tau1PtMax = cms.double(-1.)\n" % tau1PtMin
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
                addWeights_shift_and_remove_central(addWeights, central_or_shifts[central_or_shift]['addWeights_extension'])
                cfg_modified += "process.FWLiteTauTauAnalyzer.addWeights = cms.vdouble(%s)\n" % getStringRep_vstring(samples[sample]['addWeights'])
                cfgFileName_modified = os.path.join(outputFilePath, cfgFileName_original.replace("_cfg.py", "_%s_%s_cfg.py" % (discriminator, sample)))
                print " cfgFileName_modified = '%s'" % cfgFileName_modified
                cfgFile_modified = open(cfgFileName_modified, "w")
                cfgFile_modified.write(cfg_modified)
                cfgFile_modified.close()
                FWLiteTauTauAnalyzer_configFileNames[discriminator][sample] = cfgFileName_modified

                logFileName = cfgFileName_modified.replace("_cfg.py", ".log")
                FWLiteTauTauAnalyzer_logFileNames[discriminator][sample] = logFileName
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" histograms
# produced by FWLiteTauIdEffAnalyzer macro
#
hadd_inputFileNames = []
for sample in FWLiteTauTauAnalyzer_outputFileNames.keys():
    for central_or_shift in FWLiteTauTauAnalyzer_outputFileNames[sample].keys():
        for region in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift].keys():
            for discriminator in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region].keys():
                hadd_inputFileNames.append(FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][discriminator])
hadd_outputFileName = os.path.join(outputFilePath, "FWLiteTauTauAnalyzer_all.root")
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
            for discriminator in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region].keys():
                outputFileNames.append(FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region][discriminator])
outputFileNames.append(hadd_outputFileName)    
makeFile.write("all: %s\n" % make_MakeFile_vstring(outputFileNames))
makeFile.write("\techo 'Finished running MSSM Higgs -> tau tau -> tau_h tau_h analysis.'\n")
makeFile.write("\n")
for sample in FWLiteTauTauAnalyzer_outputFileNames.keys():
    for central_or_shift in FWLiteTauTauAnalyzer_outputFileNames[sample].keys():
        for region in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift].keys():
            for discriminator in FWLiteTauTauAnalyzer_outputFileNames[sample][central_or_shift][region].keys():
                makeFile.write("%s:\n" %
                  (FWLiteTauTauAnalyzer_outputFileNames[discriminator][sample]))
                makeFile.write("\t%s%s %s &> %s\n" %
                  (nice, executable_FWLiteTauTauAnalyzer,
                   FWLiteTauTauAnalyzer_configFileNames[discriminator][sample],
                   FWLiteTauTauAnalyzer_logFileNames[discriminator][sample]))
makeFile.write("\n")    
makeFile.write("%s: %s\n" %
  (hadd_outputFileName,
   make_MakeFile_vstring(hadd_inputFileNames)))
makeFile.write("\t%s%s %s\n" %
  (nice, executable_rm,
   hadd_outputFileName))
makeFile.write("\t%s%s %s %s\n" %
  (nice, executable_hadd,
   hadd_outputFileName, make_MakeFile_vstring(hadd_inputFileNames)))
makeFile.write("\n")
makeFile.write(".PHONY: clean\n")
makeFile.write("clean:\n")
makeFile.write("\t%s %s\n" % (executable_rm, make_MakeFile_vstring(outputFileNames)))
makeFile.write("\techo 'Finished deleting old files.'\n")
makeFile.write("\n")
makeFile.close()

print("Finished building Makefile. Now execute 'make -f %s'." % makeFileName)
