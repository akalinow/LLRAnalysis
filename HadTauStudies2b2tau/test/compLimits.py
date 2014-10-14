#!/usr/bin/env python

import ROOT

import os
import re
import shlex
import string
import subprocess

jobId = '2014Jun09'

version = "v3_09"

inputFilePath  = "/data1/veelken/tmp/tauTauAnalysis2b2tau/%s_2/" % version

combineFilePath = "/afs/cern.ch/work/v/veelken/mssmHtautauLimits/CMSSW_6_1_1/src/tmp"

categoryOptions = {
    'nonresonant' : {
        'inputFileName' : 'htt_tt.inputs-2b2tau-8TeV-0_nonresonant_%s.root',
        'signal_processes' : [
            "hhTo2b2tau"
        ],
        'categories' : [
            "2bM_2tauT_nonresonant",
            "2bL_2tauT_nonresonant",
            "1b1j_2tauT_nonresonant",
            "2bM_2tauL_nonresonant",
            "2bL_2tauL_nonresonant",
            "1b1j_2tauL_nonresonant"
        ],
        'histogramsToFit' : [
            "augMT2ed",
            "svFitMassS"
        ]
    },
    'resonant' : {
        'inputFileName' : 'htt_tt.inputs-2b2tau-8TeV-0_resonant_%s.root',
        'signal_processes' : [
            "mssmH260tohh", "mssmH300tohh", "mssmH350tohh",
            "graviton270Tohh", "graviton300Tohh", "graviton500Tohh", "graviton700Tohh", "graviton1000Tohh",
            "radion300Tohh", "radion500Tohh", "radion700Tohh", "radion1000Tohh"
        ],
        'categories' : [
            "2bM_2tauT_resonant",
            "2bL_2tauT_resonant",
            "1b1j_2tauT_resonant",
            "2bM_2tauL_resonant",
            "2bL_2tauL_resonant",
            "1b1j_2tauL_resonant"
        ],
        'histogramsToFit' : [
            "augMT2ed",
            "svFitMassS",
            "HHMassM",
            "HHbRegMassM",
            "HH2bdyKinFitMassM",
            "HHbReg2bdyKinFitMassM",
            "HH4bdyKinFitMassM",
            ##"HHbReg4bdyKinFitMassM"
        ]
    }
}

datacard_template = string.Template('''
imax    1     number of categories 
jmax    7     number of samples minus one 
kmax    *     number of nuisance parameters 
------------------------------------------------------------------------------- 
shapes * * $inputFileName $$CHANNEL/$$PROCESS $$CHANNEL/$$PROCESS_$$SYSTEMATIC 
------------------------------------------------------------------------------- 
bin                                                                                                                 $category
observation                                                                                                         $yield_data_obs                 
------------------------------------------------------------------------------- 
bin                                                                                                                 $category            $category            $category            $category            $category            $category            $category            $category
process                                                                                                             0                    1                    2                    3                    4                    5                    6                    7                     
process                                                                                                             $process_signal      ZTT                  ZJ                   QCD                  TT                   W                    VV                   ZL                    
rate                                                                                                                $yield_signal        $yield_ZTT           $yield_ZJ            $yield_QCD           $yield_TT            $yield_W             $yield_VV            $yield_ZL
------------------------------------------------------------------------------- 
lumi_8TeV lnN                                                                                                       1.026                -                    -                    -                    -                    -                    -                    -                     
CMS_eff_t_tautau_8TeV lnN                                                                                           1.19                 1.190                -                    -                    1.190                -                    1.190                -                     
CMS_eff_t_mssmHigh_tautau_8TeV shape                                                                                   1                 -                    -                    -                    -                    -                    -                    -                     
CMS_scale_t_tautau_8TeV shape                                                                                          1                 1.000                -                    -                    -                    -                    -                    -                     
CMS_scale_j_8TeV lnN                                                                                                1.05                 -                    -                    -                    1.010                -                    1.030                -                     
CMS_eff_b_8TeV lnN                                                                                                  0.98                 -                    0.980                -                    0.980                -                    0.980                0.980                 
CMS_fake_b_8TeV lnN                                                                                                 0.98                 -                    0.980                -                    0.980                -                    0.980                0.980                 
CMS_htt_WNorm_2b2tau_tt_8TeV lnN										    -                    -                    -                    -                    -                    1.300                -                    -                     
CMS_htt_QCDSyst_2b2tau_tt_8TeV lnN										    -                    -                    -                    1.350                -                    -                    -                    -                     
CMS_htt_QCDfrNorm_tautau_8TeV shape                                                                                 -                    -                    -                    1.000                -                    -                    -                    -                     
CMS_htt_QCDfrShape_tautau_8TeV shape                                                                                -                    -                    -                    1.000                -                    -                    -                    -                     
CMS_htt_zttNorm_8TeV lnN                                                                                            -                    1.033                1.033                -                    -                    -                    -                    1.033                 
CMS_htt_extrap_ztt_2b2tau_tt_8TeV lnN										    -                    1.030                -                    -                    -                    -                    -                    -                     
CMS_htt_ttbarNorm_8TeV lnN                                                                                          -                    -                    -                    -                    1.100                -                    -                    -                     
CMS_htt_ttbarNorm_2b2tau_tt_8TeV lnN										    -                    -                    -                    -                    1.050                -                    -                    -                     
CMS_htt_ttbarPtReweight_8TeV shape                                                                                  -                    -                    -                    -                    1.000                -                    -                    -                     
CMS_htt_DiBosonNorm_8TeV lnN                                                                                        -                    -                    -                    -                    -                    -                    1.150                -                     
CMS_htt_DiBosonNorm_2b2tau_tt_8TeV lnN										    -                    -                    -                    -                    -                    -                    1.150                -                     
CMS_htt_ZJetFakeTau_tautau_8TeV lnN                                                                                 -                    -                    1.200                -                    -                    -                    -                    -                     
CMS_htt_ZLeptonFakeTau_tautau_8TeV lnN                                                                              -                    -                    -                    -                    -                    -                    -                    1.200                 
CMS_htt_WShape_tautau_8TeV shape                                                                                    -                    -                    -                    -                    -                    1.000                -                    -                     
''')

channel = "tauTau"

def makeDatacard(inputFileName, channel, category, signal_process, datacardFileName):

    datacard_parameters = {}
    datacard_parameters["inputFileName"] = inputFileName
    datacard_parameters["category"] = "%s_%s" % (channel, category)
    datacard_parameters["process_signal"] = signal_process

    background_processes = [ "ZTT", "ZJ", "QCD", "TT", "W", "VV", "ZL" ]

    ##print "reading histograms from file = %s" % os.path.join(inputFilePath, inputFileName)
    inputFile = ROOT.TFile(os.path.join(inputFilePath, inputFileName))
    processes = [ "data_obs" ]
    processes.append(signal_process)
    processes.extend(background_processes)
    for process in processes:
        histogramName = "%s_%s/%s" % (channel, category, process)
        histogram = inputFile.Get(histogramName)
        key = None
        if process == "data_obs":
            key = "yield_data_obs"        
        elif process == signal_process:
            key = "yield_signal"
        else:
            key = "yield_%s" % process
        ##print " histogram = %s: %s" % (histogramName, histogram)
        value = histogram.Integral()
        datacard_parameters[key] = value
    inputFile.Close()

    datacard = datacard_template.substitute(datacard_parameters)
    
    datacardFile = open(datacardFileName, "w")
    datacardFile.write(datacard)
    datacardFile.close()

def initDict(dictionary, keys):
    dictionary_at_keylevel = dictionary
    numKeys = len(keys)
    for idxKey in range(numKeys - 1):
        key = keys[idxKey]
        if key not in dictionary_at_keylevel.keys():
            dictionary_at_keylevel[key] = {}
        dictionary_at_keylevel = dictionary_at_keylevel[key]

limit_inputFileNames    = {} # key = categoryOption, category, signal_process, histogramToFit
limit_datacardFileNames = {} # key = categoryOption, category, signal_process, histogramToFit
limit_logFileNames      = {} # key = categoryOption, category, signal_process, histogramToFit

categories_and_discriminators = {} # key = categoryOption

for categoryOption in categoryOptions.keys():
    categories_and_discriminators[categoryOption] = []
    for category in categoryOptions[categoryOption]['categories']:
        for histogramToFit in categoryOptions[categoryOption]['histogramsToFit']:
            for signal_process in categoryOptions[categoryOption]['signal_processes']:

                category_and_discriminator = category
                if not category_and_discriminator in categories_and_discriminators[categoryOption]:
                    categories_and_discriminators[categoryOption].append(category_and_discriminator)

                inputFileName = categoryOptions[categoryOption]['inputFileName'] % histogramToFit
                initDict(limit_inputFileNames, [ categoryOption, category_and_discriminator, signal_process, histogramToFit ])
                limit_inputFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit] = inputFileName
                    
                datacardFileName = os.path.join(combineFilePath, "compLimits_%s_%s_%s_%s.txt" % (categoryOption, category_and_discriminator, signal_process, histogramToFit))
                makeDatacard(inputFileName, channel, category_and_discriminator, signal_process, datacardFileName)
                initDict(limit_datacardFileNames, [ categoryOption, category_and_discriminator, signal_process, histogramToFit ])
                limit_datacardFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit] = datacardFileName
        
                logFileName = os.path.join(combineFilePath, "compLimits_%s_%s_%s_%s.log" % (categoryOption, category_and_discriminator, signal_process, histogramToFit))
                initDict(limit_logFileNames, [ categoryOption, category_and_discriminator, signal_process, histogramToFit ])
                limit_logFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit] = logFileName

executable_cmsenv = "eval `scramv1 runtime -csh`"

shellScript = "#!/bin/csh -f\n"
for categoryOption in categoryOptions.keys():
    for histogramToFit in categoryOptions[categoryOption]['histogramsToFit']:
        for signal_process in categoryOptions[categoryOption]['signal_processes']:
            
            datacardFileNames_input = []
            for category_and_discriminator in categories_and_discriminators[categoryOption]:
                datacardFileNames_input.append(limit_datacardFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit])
                
            datacardFileName_output = os.path.join(combineFilePath, "compLimits_%s_%s_%s_%s.txt" % (categoryOption, "combined", signal_process, histogramToFit))
            initDict(limit_datacardFileNames, [ categoryOption, "combined", signal_process, histogramToFit ])
            limit_datacardFileNames[categoryOption]["combined"][signal_process][histogramToFit] = datacardFileName_output
            
            shellScript += "cd %s\n" % combineFilePath
            shellScript += "%s\n" % executable_cmsenv
            combineCards_parameters = ""
            for idxDatacardFileName_input in range(len(datacardFileNames_input)):
                ##combineCards_parameters += "Name%i=%s" % (idxDatacardFileName_input + 1, datacardFileNames_input[idxDatacardFileName_input])
                combineCards_parameters += "%s" % datacardFileNames_input[idxDatacardFileName_input]
                combineCards_parameters += " "
            shellScript += "combineCards.py %s > %s\n" % (combineCards_parameters, datacardFileName_output)
            
            logFileName = os.path.join(combineFilePath, "compLimits_%s_%s_%s_%s.log" % (categoryOption, "combined", signal_process, histogramToFit))
            initDict(limit_logFileNames, [ categoryOption, "combined", signal_process, histogramToFit ])
            limit_logFileNames[categoryOption]["combined"][signal_process][histogramToFit] = logFileName
            
    categories_and_discriminators[categoryOption].append("combined")
for categoryOption in categoryOptions.keys():
    for category_and_discriminator in categories_and_discriminators[categoryOption]:
        for histogramToFit in categoryOptions[categoryOption]['histogramsToFit']:
            for signal_process in categoryOptions[categoryOption]['signal_processes']:
                if category_and_discriminator != "combined":
                    shellScript += "cp %s %s\n" % (os.path.join(inputFilePath, limit_inputFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit]), combineFilePath)
                shellScript += "cd %s\n" % combineFilePath
                shellScript += "%s\n" % executable_cmsenv
                shellScript += "echo \"executing 'combine -M Asymptotic %s >&! %s'\"\n" % \
                  (limit_datacardFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit],
                   limit_logFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit])
                shellScript += "combine -M Asymptotic %s >&! %s\n" % \
                  (limit_datacardFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit],
                   limit_logFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit])
currentFilePath = os.getcwd()
shellScript += "cd %s\n" % currentFilePath
shellScript += "%s\n" % executable_cmsenv
shellScriptFileName = "compLimits.csh"
shellScriptFile = open(shellScriptFileName, "w")
shellScriptFile.write(shellScript)
shellScriptFile.close()
subprocess.call('chmod +x %s' % shellScriptFileName, shell = True)
print "executing './%s'" % shellScriptFileName
##subprocess.call('./%s' % shellScriptFileName, shell = True)

##raise ValueError("STOP.")

def makeGraph(categoryOption, category_and_discriminator, signal_processes, histogramToFit):
    numPoints = len(signal_processes)
    graph = ROOT.TGraph(numPoints)
    expLimit_regex = r'Expected 50.0%: r < (?P<expLimit>[0-9.eE+-]+)\w*'
    expLimit_matcher = re.compile(expLimit_regex)
    for iPoint in range(numPoints):
        ( signal_process, massPoint ) = signal_processes[iPoint]
        logFileName = limit_logFileNames[categoryOption][category_and_discriminator][signal_process][histogramToFit]
        logFile = open(logFileName, "r") 
        lines = logFile.readlines()
        expLimit = None
        for line in lines:
            expLimit_match = expLimit_matcher.match(line)
            if expLimit_match:
                expLimit = float(expLimit_match.group('expLimit'))
                br = 0.577*0.0632*2.
                expLimit /= br 
        logFile.close()
        if expLimit:
            print "category/discriminator = %s, process = %s (M = %1.0f), histogramToFit = %s: expLimit = %1.3f pb" % (category_and_discriminator, signal_process, massPoint, histogramToFit, expLimit)
        else:
            raise ValueError("Failed to read expected limit from log-file = %s !!" % logFileName)
        graph.SetPoint(iPoint, massPoint, expLimit)
    return graph

for categoryOption in categoryOptions.keys():
    for category_and_discriminator in categories_and_discriminators[categoryOption]:
        for histogramToFit in categoryOptions[categoryOption]['histogramsToFit']:

            outputFileName = None
            if category_and_discriminator != "combined":
                outputFileName = "compLimits_%s_%s.root" % (category_and_discriminator, histogramToFit)
            else:
                outputFileName = "compLimits_combined_%s_%s.root" % (categoryOption, histogramToFit)
            print "category/discriminator = %s, histogramToFit = %s: writing graphs to file = %s" % (category_and_discriminator, histogramToFit, outputFileName)
            outputFile = ROOT.TFile("compLimits/%s" % outputFileName, "RECREATE")
            if categoryOption == "resonant":
                graph_mssmHtohh = makeGraph(
                  categoryOption, category_and_discriminator,
                  [ ("mssmH260tohh", 260.), ("mssmH300tohh", 300.), ("mssmH350tohh", 350.) ],
                  histogramToFit)
                graph_mssmHtohh.SetName("graph_mssmHtohh")
                graph_mssmHtohh.Write()
                graph_graviton = makeGraph(
                  categoryOption, category_and_discriminator,
                  [ ("graviton270Tohh", 270.), ("graviton300Tohh", 300.), ("graviton500Tohh", 500.), ("graviton700Tohh", 700.), ("graviton1000Tohh", 1000.) ],
                  histogramToFit)
                graph_graviton.SetName("graph_graviton")
                graph_graviton.Write()
                graph_radion = makeGraph(
                  categoryOption, category_and_discriminator,
                  [ ("radion300Tohh", 300.), ("radion500Tohh", 500.), ("radion700Tohh", 700.), ("radion1000Tohh", 1000.) ],
                  histogramToFit)
                graph_radion.SetName("graph_radion")
                graph_radion.Write()
            if categoryOption == "nonresonant":
                graph_hhTo2b2tau = makeGraph(
                  categoryOption, category_and_discriminator,
                  [ ("hhTo2b2tau", 200.), ("hhTo2b2tau", 750.), ("hhTo2b2tau", 1050.) ],
                  histogramToFit)
                graph_hhTo2b2tau.SetName("graph_hhTo2b2tau")
                graph_hhTo2b2tau.Write()
            outputFile.Close()
