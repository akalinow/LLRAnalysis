#!/usr/bin/env python

import os
import re
import shlex
import string
import subprocess

WorkdirLoc = '/data_CMS/cms/htautau/PostMoriond/TREES/MuTau/'
OutDir   = '/data_CMS/cms/htautau/PostMoriond/NTUPLES/MuTau/temp/'

options = {
    ###mu+tau samples
    ##DATA
    'Run2012A-13Jul2012-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012A_ReReco13J_HTT_06Mar2013_Trees_MuTau_v1/',
    'outputFileName' : OutDir+'nTupleRun2012A-13Jul2012-Data_MuTau.root',
    'sample'         : 'Run2012A-13Jul2012-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012B-13Jul2012-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco13J_HTT_06Mar2013_Trees_MuTau_v1_p1/',
    'outputFileName' : OutDir+'nTupleRun2012B-13Jul2012-p1-Data_MuTau.root',
    'sample'         : 'Run2012B-13Jul2012-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012B-13Jul2012-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco13J_HTT_06Mar2013_Trees_MuTau_v1_p2/',
    'outputFileName' : OutDir+'nTupleRun2012B-13Jul2012-p2-Data_MuTau.root',
    'sample'         : 'Run2012B-13Jul2012-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012B-13Jul2012-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco13J_HTT_06Mar2013_Trees_MuTau_v1_p3/',
    'outputFileName' : OutDir+'nTupleRun2012B-13Jul2012-p3-Data_MuTau.root',
    'sample'         : 'Run2012B-13Jul2012-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012B-13Jul2012-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco13J_HTT_06Mar2013_Trees_MuTau_v1_p4/',
    'outputFileName' : OutDir+'nTupleRun2012B-13Jul2012-p4-Data_MuTau.root',
    'sample'         : 'Run2012B-13Jul2012-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012B-13Jul2012-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco13J_HTT_06Mar2013_Trees_MuTau_v1_p5/',
    'outputFileName' : OutDir+'nTupleRun2012B-13Jul2012-p5-Data_MuTau.root',
    'sample'         : 'Run2012B-13Jul2012-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    }, 
    'Run2012C-24Aug2012-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco24A_HTT_06Mar2013_Trees_MuTau_v1/',
    'outputFileName' : OutDir+'nTupleRun2012C-24Aug2012-Data_MuTau.root',
    'sample'         : 'Run2012C-24Aug2012-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 1
    },
    'Run2012C-PromptReco-v2-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_PRV2_HTT_06Mar2013_Trees_MuTau_v1_p1/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-p1-Data_MuTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012C-PromptReco-v2-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_PRV2_HTT_06Mar2013_Trees_MuTau_v1_p2/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-p2-Data_MuTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012C-PromptReco-v2-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_PRV2_HTT_06Mar2013_Trees_MuTau_v1_p3/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-p3-Data_MuTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012C-PromptReco-v2-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_PRV2_HTT_06Mar2013_Trees_MuTau_v1_p4/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-p4-Data_MuTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012C-PromptReco-v2-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_PRV2_HTT_06Mar2013_Trees_MuTau_v1_p5/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-p5-Data_MuTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012C-PromptReco-v2-p6-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_PRV2_HTT_06Mar2013_Trees_MuTau_v1_p6/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-p6-Data_MuTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012C-PromptReco-v2-p7-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_PRV2_HTT_06Mar2013_Trees_MuTau_v1_p7/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-p7-Data_MuTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_PRV1_HTT_06Mar2013_Trees_MuTau_v1_p1/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p1-Data_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_PRV1_HTT_06Mar2013_Trees_MuTau_v1_p2/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p2-Data_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_PRV1_HTT_06Mar2013_Trees_MuTau_v1_p3/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p3-Data_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_PRV1_HTT_06Mar2013_Trees_MuTau_v1_p4/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p4-Data_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_PRV1_HTT_06Mar2013_Trees_MuTau_v1_p5/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p5-Data_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p6-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_PRV1_HTT_06Mar2013_Trees_MuTau_v1_p6/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p6-Data_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p7-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_PRV1_HTT_06Mar2013_Trees_MuTau_v1_p7/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p7-Data_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p8-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_PRV1_HTT_06Mar2013_Trees_MuTau_v1_p8/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p8-Data_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p8-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p9-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_PRV1_HTT_06Mar2013_Trees_MuTau_v1_p9/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p9-Data_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p9-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    ##Embedded
    'Run2012A-13Jul2012-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012A-MuTau-53X-ReReco13Jul2012-v1-Embedded-MuJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012A-13Jul2012-Embedded_MuTau.root',
    'sample'         : 'Run2012A-13Jul2012-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012B-13Jul2012-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012B-MuTau-53X-ReReco13Jul2012-v1-Embedded-MuJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012B-13Jul2012-Embedded_MuTau.root',
    'sample'         : 'Run2012B-13Jul2012-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012A-Recover06Aug2012-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012A-MuTau-53X-recover_06Aug2012-Embedded-MuJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012A-Recover06Aug2012-Embedded_MuTau.root',
    'sample'         : 'Run2012A-Recover06Aug2012-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 1
    },
    'Run2012C-24Aug2012-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012C-MuTau-53X-ReReco24Aug2012-Embedded-MuJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012C-24Aug2012-Embedded_MuTau.root',
    'sample'         : 'Run2012C-24Aug2012-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 2
    },
    'Run2012C-PromptReco-v2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012C-MuTau-53X-PromptReco-v2-Embedded-MuJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-Embedded_MuTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 3
    },
    'Run2012D-PromptReco-v1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p1_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p1_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p1-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p2_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p2-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p3_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p3-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p4_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p4-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p5_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p5-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p6-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p6_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p6-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p7-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p7_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p7-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p8-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p8_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p8-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p8-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p9-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/Run2012D-Embed-MuTau-PromptReco-v1_07Dec_p9_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p9-Embedded_MuTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p9-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    ##Bkg MC
    'DYJets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-MuTau-50-madgraph-PUS10-v2_run_LORENZO/',
    'outputFileName' : OutDir+'nTupleDYJets_MuTau.root',
    'sample'         : 'DYJets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 2431175./9669034,
    'iJson'          : -1
    } ,
    'TTJets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/TTJets-MuTau-madgraph-PUS10-v2_run/',
    'outputFileName' : OutDir+'nTupleTTJets_MuTau.root',
    'sample'         : 'TTJets',
    'xSection'       : 225.5,
    'skimEff'        : 1.0 * 2467119./3487856,
    'iJson'          : -1
    },
    'WJets-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WJets-MuTau-madgraph-PUS10-p1_run/',
    'outputFileName' : OutDir+'nTupleWJets-p1_MuTau.root',
    'sample'         : 'WJets-p1',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 440512./2000279,
    'iJson'          : -1
    },
    'WJets-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WJets-MuTau-madgraph-PUS10-p2_run/',
    'outputFileName' : OutDir+'nTupleWJets-p2_MuTau.root',
    'sample'         : 'WJets-p2',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 1383633./6273656,
    'iJson'          : -1
    },
    'WJets1Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W1Jets-MuTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWJets1Jets_MuTau.root',
    'sample'         : 'WJets1Jets',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 1490832./23141598,
    'iJson'          : -1
    },
    'WJets2Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-MuTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWJets2Jets_MuTau.root',
    'sample'         : 'WJets2Jets',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 1034822./34044921,
    'iJson'          : -1
    },
    'WJets3Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-MuTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWJets3Jets_MuTau.root',
    'sample'         : 'WJets3Jets',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 3353306./15539503,
    'iJson'          : -1
    },
    'WJets4Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-MuTau-madgraph-PUS10-v2_run/',
    'outputFileName' : OutDir+'nTupleWJets4Jets_MuTau.root',
    'sample'         : 'WJets4Jets',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 3993529./13382803,
    'iJson'          : -1
    },
    'T-tW' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/T-tW-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleT-tW_MuTau.root',
    'sample'         : 'T-tW',
    'xSection'       : 11.1,
    'skimEff'        : 1.0 * 0.081518021248,
    'iJson'          : -1
    },
    'Tbar-tW' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/Tbar-tW-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleTbar-tW_MuTau.root',
    'sample'         : 'Tbar-tW',
    'xSection'       : 11.1,
    'skimEff'        : 1.0 * 0.0822555699536,
    'iJson'          : -1
    },
    'WWJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WWJetsTo2L2Nu-MuTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWWJetsTo2L2Nu_MuTau.root',
    'sample'         : 'WWJetsTo2L2Nu',
    'xSection'       : 5.824,
    'skimEff'        : 1.0 *  102870./1263235,
    'iJson'          : -1
    },
    'WZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WZJetsTo2L2Q-MuTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWZJetsTo2L2Q_MuTau.root',
    'sample'         : 'WZJetsTo2L2Q',
    'xSection'       : 2.207,
    'skimEff'        : 1.0 * 0.127811802097,
    'iJson'          : -1
    },
    'WZJetsTo3LNu' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WZJetsTo3LNu-MuTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWZJetsTo3LNu_MuTau.root',
    'sample'         : 'WZJetsTo3LNu',
    'xSection'       : 1.058,
    'skimEff'        : 1.0 * 0.14055,
    'iJson'          : -1
    },
    'ZZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/ZZJetsTo2L2Q-MuTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Q_MuTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    'skimEff'        : 1.0 * 0.145166994064,
    'iJson'          : -1
    },
    'ZZJetsTo4L' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/ZZJetsTo4L-MuTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleZZJetsTo4L_MuTau.root',
    'sample'         : 'ZZJetsTo4L',
    'xSection'       : 0.181,
    'skimEff'        : 1.0 * 0.145166994064,
    'iJson'          : -1
    },
    ##Higgs MC
    'GGFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GGFH110-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH110_MuTau.root',
    'sample'         : 'GGFH110',
    'xSection'       : 25.04 ,
    'skimEff'        : 7.95E-02 * 10820./399907.,
    'iJson'          : -1
    },
    'GGFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GGFH115-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH115_MuTau.root',
    'sample'         : 'GGFH115',
    'xSection'       : 22.96 ,
    'skimEff'        : 7.58E-02 * 14186./480230.,
    'iJson'          : -1
    },
    'GGFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GGFH120-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH120_MuTau.root',
    'sample'         : 'GGFH120',
    'xSection'       : 21.13 ,
    'skimEff'        : 7.04E-02 * 141092./4379561.,
    'iJson'          : -1
    },
    'GGFH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GGFH125-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH125_MuTau.root',
    'sample'         : 'GGFH125',
    'xSection'       : 19.52,
    'skimEff'        : 6.32E-02 * 23954./688134.,
    'iJson'          : -1
    },
    'GGFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GGFH130-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH130_MuTau.root',
    'sample'         : 'GGFH130',
    'xSection'       : 18.07 ,
    'skimEff'        : 5.45E-02 * 37717./989921.,
    'iJson'          : -1
    },
    'GGFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GGFH135-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH135_MuTau.root',
    'sample'         : 'GGFH135',
    'xSection'       : 16.79,
    'skimEff'        : 4.49E-02 * 29569./720729.,
    'iJson'          : -1
    },
    'GGFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GGFH140-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH140_MuTau.root',
    'sample'         : 'GGFH140',
    'xSection'       : 15.63,
    'skimEff'        : 3.52E-02 * 19042./441198.,
    'iJson'          : -1
    },
    'GGFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GGFH145-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH145_MuTau.root',
    'sample'         : 'GGFH145',
    'xSection'       : 14.59,
    'skimEff'        : 2.61E-02 * 39504./852119.,
    'iJson'          : -1
    },
    'VBFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBFH110-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH110_MuTau.root',
    'sample'         : 'VBFH110',
    'xSection'       : 1.809,
    'skimEff'        : 7.95E-02 * 22753./414385.,
    'iJson'          : -1
    },
    'VBFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBFH115-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH115_MuTau.root',
    'sample'         : 'VBFH115',
    'xSection'       : 1.729,
    'skimEff'        : 7.58E-02 * 56912./993870.,
    'iJson'          : -1
    },
    'VBFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBFH120-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH120_MuTau.root',
    'sample'         : 'VBFH120',
    'xSection'       : 1.649,
    'skimEff'        : 7.04E-02 * 170419./2834487.,
    'iJson'          : -1
    },
    'VBFH125' : {
    'inputFilePath'  : WorkdirLoc+'/HiggsSM/VBF125/',
    'outputFileName' : OutDir+'nTupleVBFH125_MuTau.root',
    'sample'         : 'VBFH125',
    'xSection'       : 1.578,
    'skimEff'        : 6.32E-02 * 61421./978836.,
    'iJson'          : -1
    },
    'VBFH125-NoRecoil' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-125_MC_H_HTT_06Mar2013_Trees_MuTau_v1_up/',
    'outputFileName' : OutDir+'nTupleVBFH125-NoRecoil_MuTau.root',
    'sample'         : 'VBFH125',
    'xSection'       : 1.578,
    'skimEff'        : 6.32E-02 * 61421./978836.,
    'iJson'          : -1
    },
    'VBFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBFH130-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH130_MuTau.root',
    'sample'         : 'VBFH130',
    'xSection'       : 1.511,
    'skimEff'        : 5.45E-02 * 50458./773790.,
    'iJson'          : -1
    },
    'VBFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBFH135-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH135_MuTau.root',
    'sample'         : 'VBFH135',
    'xSection'       : 1.448,
    'skimEff'        : 4.49E-02 * 67382./993817.,
    'iJson'          : -1
    },
    'VBFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBFH140-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH140_MuTau.root',
    'sample'         : 'VBFH140',
    'xSection'       : 1.389,
    'skimEff'        : 3.52E-02 * 34934./499496.,
    'iJson'          : -1
    },
    'VBFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBFH145-MuTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH145_MuTau.root',
    'sample'         : 'VBFH145',
    'xSection'       : 1.333,
    'skimEff'        : 2.61E-02 * 67190./918883.,
    'iJson'          : -1
    },
    'VH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VH110-MuTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH110_MuTau.root',
    'sample'         : 'VH110',
    'xSection'       : (1.060+0.5869+0.1887) ,
    'skimEff'        : 7.95E-02 * 16792./195408.,
    'iJson'          : -1
    },
    'VH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VH115-MuTau-pythia-PUS10-v2_run/',
    'outputFileName' : OutDir+'nTupleVH115_MuTau.root',
    'sample'         : 'VH115',
    'xSection'       : (0.9165+0.5117+0.1663),
    'skimEff'        : 7.58E-02 * 17146./190460.,
    'iJson'          : -1
    },
    'VH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VH120-MuTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH120_MuTau.root',
    'sample'         : 'VH120',
    'xSection'       : (0.7966+0.4483+0.1470),
    'skimEff'        : 7.04E-02 * 16147./170288.,
    'iJson'          : -1
    },
    'VH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VH125-MuTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH125_MuTau.root',
    'sample'         : 'VH125',
    'xSection'       : (0.6966+0.3943+0.1302),
    'skimEff'        : 6.32E-02 * 19299./195124.,
    'iJson'          : -1
    },
    'VH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VH130-MuTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH130_MuTau.root',
    'sample'         : 'VH130',
    'xSection'       : (0.6095+0.3473+0.1157),
    'skimEff'        : 5.45E-02 * 20383./200080.,
    'iJson'          : -1
    },
    'VH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VH135-MuTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH135_MuTau.root',
    'sample'         : 'VH135',
    'xSection'       : (0.5351+0.3074+0.1031),
    'skimEff'        : 4.49E-02 * 17609./165128.,
    'iJson'          : -1
    },
    'VH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VH140-MuTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH140_MuTau.root',
    'sample'         : 'VH140',
    'xSection'       : (0.4713+0.2728+0.09207),
    'skimEff'        : 3.52E-02 * 18328./165186.,
    'iJson'          : -1
    },
    'VH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VH145-MuTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH145_MuTau.root',
    'sample'         : 'VH145',
    'xSection'       : (0.4164+0.2424+0.08246),
    'skimEff'        : 2.61E-02 * 22184./195384.,
    'iJson'          : -1
    },
}

config_template = string.Template('''
import FWCore.ParameterSet.Config as cms

import os

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    maxEvents = cms.int32(-1),
    #outputEvery = cms.uint32(1000)
)

inputFilePath = '$inputFilePath'
inputFiles = os.listdir(inputFilePath)
process.fwliteInput.fileNames = cms.vstring([ os.path.join(inputFilePath, inputFile) for inputFile in inputFiles ])

process.fwliteOutput = cms.PSet(
    fileName = cms.string('$outputFileName')
)

process.treeSkimmerMuTau = cms.PSet(
    sample = cms.string('$sample'),
    analysis = cms.string('$analysis'),
    xSection = cms.double($xSection),
    skimEff = cms.double($skimEff),
    iJson = cms.int32($iJson)
)
''')

currentDirectory    = os.getcwd()
submissionDirectory = os.path.join(currentDirectory, "Configs")

for sample, option in options.items():
    for analysis in [ 'nominal', 'TauUp', 'TauDown','JetUp','JetDown','MuUp','MuDown' ]:
        if  re.search("Data",sample)!=None and analysis != 'nominal':
            continue
        configOptions = option.copy()
        configOptions['analysis'] = analysis
        if  re.search("Data",sample)==None :
            configOptions['outputFileName'] = configOptions['outputFileName'].replace('.root', '_%s.root' % analysis) 
        configFileName = "treeSkimmerMuTau_Spring13_%s_%s_cfg.py" % (sample,analysis)
        configFileName_full = os.path.join(submissionDirectory, configFileName)
        configFile = open(configFileName_full, 'w')
        configConfig = config_template.substitute(configOptions)
        configFile.write(configConfig)
        configFile.close()
