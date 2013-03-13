#!/usr/bin/env python

import os
import re
import shlex
import string
import subprocess

WorkdirLoc = '/data_CMS/cms/htautau/PostMoriond/TREES/EleTau/'
OutDir   = '/data_CMS/cms/htautau/PostMoriond/NTUPLES/EleTau/IvoTest/'

options = {
    ###e+tau samples
    ##DATA
    'Run2012A-13Jul2012-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012A-ElecTau-ReReco13Jul2012-v1_run/',
    'outputFileName' : OutDir+'nTupleRun2012A-13Jul2012-Data_ElecTau.root',
    'sample'         : 'Run2012A-13Jul2012-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012B-13Jul2012-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012B-ElecTau-ReReco13Jul2012-v1_run_all/',
    'outputFileName' : OutDir+'nTupleRun2012B-13Jul2012-Data_ElecTau.root',
    'sample'         : 'Run2012B-13Jul2012-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },    
    'Run2012C-24Aug2012-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012C-ElecTau-24Aug_run/',
    'outputFileName' : OutDir+'nTupleRun2012C-24Aug2012-Data_ElecTau.root',
    'sample'         : 'Run2012C-24Aug2012-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 2
    },
    'Run2012C-PromptReco-v2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012C-ElecTau-PRV2_run/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-Data_ElecTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 3
    },
    'Run2012D-PromptReco-v1-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012D-EleTau-PromptReco-v1_06Mar13_p1_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p1-Data_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012D-EleTau-PromptReco-v1_06Mar13_p2_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p2-Data_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012D-EleTau-PromptReco-v1_06Mar13_p3_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p3-Data_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012D-EleTau-PromptReco-v1_06Mar13_p4_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p4-Data_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012D-EleTau-PromptReco-v1_06Mar13_p5_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p5-Data_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p6-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012D-EleTau-PromptReco-v1_06Mar13_p6_run_full/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p6-Data_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p7-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012D-EleTau-PromptReco-v1_06Mar13_p7_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p7-Data_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p8-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012D-EleTau-PromptReco-v1_06Mar13_p8_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p8-Data_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p8-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p9-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Run2012D-EleTau-PromptReco-v1_06Mar13_p9_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p9-Data_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p9-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    ##Embedded
    'Run2012A-13Jul2012-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012A-ElecTau-53X-ReReco13Jul2012-v1-Embedded-EleJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012A-13Jul2012-Embedded_ElecTau.root',
    'sample'         : 'Run2012A-13Jul2012-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012B-13Jul2012-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012B-ElecTau-53X-ReReco13Jul2012-v1-Embedded-EleJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012B-13Jul2012-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-13Jul2012-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 0
    },
    'Run2012A-Recover06Aug2012-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012A-ElecTau-53X-recover_06Aug2012-Embedded-EleJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012A-Recover06Aug2012-Embedded_ElecTau.root',
    'sample'         : 'Run2012A-Recover06Aug2012-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 1
    },
    'Run2012C-24Aug2012-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012C-ElecTau-53X-ReReco24Aug2012-Embedded-EleJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012C-24Aug2012-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-24Aug2012-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 2
    },
    'Run2012C-PromptReco-v2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012C-ElecTau-53X-PromptReco-v2-Embedded-EleJet-iter6_run/',
    'outputFileName' : OutDir+'nTupleRun2012C-PromptReco-v2-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-PromptReco-v2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 3
    },
    'Run2012D-PromptReco-v1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p1_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p1_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p1-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p2_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p2-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p3_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p3-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p4_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p4-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p5_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p5-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p6-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p6_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p6-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p7-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p7_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p7-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p8-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p8_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p8-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p8-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    'Run2012D-PromptReco-v1-p9-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbedUpDown/Run2012D-Embed-ElecTau-PromptReco-v1_07Dec_p9_run/',
    'outputFileName' : OutDir+'nTupleRun2012D-PromptReco-v1-p9-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-PromptReco-v1-p9-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 4
    },
    ##Bkg MC
    'DYJets' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/DYJets-ElecTau-50-madgraph-PUS10-v2_run_LORENZO/',
    'outputFileName' : OutDir+'nTupleDYJets_ElecTau.root',
    'sample'         : 'DYJets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1249322/24330458.,
    'iJson'          : -1
    } ,
    'DYJets-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/DYJets-ElecTau-8TeV-50-madgraph-PUS7-Embedded_run_IVO/',
    'outputFileName' : OutDir+'nTupleDYJets-Embedded_ElecTau.root',
    'sample'         : 'DYJets-Embedded',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1249322/24330458.,
    'iJson'          : -1
    } ,
    'T-tW' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/T-tW-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleT-tW_ElecTau.root',
    'sample'         : 'T-tW',
    'xSection'       : 11.1,
    'skimEff'        : 1.0 * 0.081518021248,
    'iJson'          : -1
    },
    'TTJets' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/TTJets-ElecTau-madgraph-PUS10-v2_run/',
    'outputFileName' : OutDir+'nTupleTTJets_ElecTau.root',
    'sample'         : 'TTJets',
    'xSection'       : 225.5,
    'skimEff'        : 1.0 * 510296./5423750.,
    'iJson'          : -1
    },
    'Tbar-tW' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/Tbar-tW-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleTbar-tW_ElecTau.root',
    'sample'         : 'Tbar-tW',
    'xSection'       : 11.1,
    'skimEff'        : 1.0 * 0.0822555699536,
    'iJson'          : -1
    },
    'WJets1Jets' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/W1Jets-ElecTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWJets1Jets_ElecTau.root',
    'sample'         : 'WJets1Jets',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 344731./22491598,
    'iJson'          : -1
    },
    'WJets2Jets' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/W2Jets-ElecTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWJets2Jets_ElecTau.root',
    'sample'         : 'WJets2Jets',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.00634753521127,
    'iJson'          : -1
    },
    'WJets3Jets' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/W3Jets-ElecTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWJets3Jets_ElecTau.root',
    'sample'         : 'WJets3Jets',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 341616./6117001.,
    'iJson'          : -1
    },
    'WJets4Jets' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/W4Jets-ElecTau-madgraph-PUS10-v2_run/',
    'outputFileName' : OutDir+'nTupleWJets4Jets_ElecTau.root',
    'sample'         : 'WJets4Jets',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 574303./7314266.,
    'iJson'          : -1
    },
    'WJets-p1' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/WJets-ElecTau-madgraph-PUS10-p1_run/',
    'outputFileName' : OutDir+'nTupleWJets-p1_ElecTau.root',
    'sample'         : 'WJets-p1',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.00634753521127,
    'iJson'          : -1
    },
    'WJets-p2' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/WJets-ElecTau-madgraph-PUS10-p2_run/',
    'outputFileName' : OutDir+'nTupleWJets-p2_ElecTau.root',
    'sample'         : 'WJets-p2',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.00636491514722,
    'iJson'          : -1
    },
    'WWJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/WWJetsTo2L2Nu-ElecTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWWJetsTo2L2Nu_ElecTau.root',
    'sample'         : 'WWJetsTo2L2Nu',
    'xSection'       : 5.824,
    'skimEff'        : 1.0 *  102870./1263235,
    'iJson'          : -1
    },
    'WZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/WZJetsTo2L2Q-ElecTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWZJetsTo2L2Q_ElecTau.root',
    'sample'         : 'WZJetsTo2L2Q',
    'xSection'       : 2.207,
    'skimEff'        : 1.0 * 0.127811802097,
    'iJson'          : -1
    },
    'WZJetsTo3LNu' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/WZJetsTo3LNu-ElecTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleWZJetsTo3LNu_ElecTau.root',
    'sample'         : 'WZJetsTo3LNu',
    'xSection'       : 1.058,
    'skimEff'        : 1.0 * 0.14055,
    'iJson'          : -1
    },
    'ZZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/ZZJetsTo2L2Q-ElecTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Q_ElecTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    'skimEff'        : 1.0 * 0.145166994064,
    'iJson'          : -1
    },
    'ZZJetsTo4L' : {
    'inputFilePath'  : WorkdirLoc+'BkgMC/ZZJetsTo4L-ElecTau-madgraph-PUS10_run/',
    'outputFileName' : OutDir+'nTupleZZJetsTo4L_ElecTau.root',
    'sample'         : 'ZZJetsTo4L',
    'xSection'       : 0.181,
    'skimEff'        : 1.0 * 0.145166994064,
    'iJson'          : -1
    },
    ##Higgs MC
    'GGFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/GGFH110-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH110_ElecTau.root',
    'sample'         : 'GGFH110',
    'xSection'       : 25.04 ,
    'skimEff'        : 7.95E-02 * 10820./399907.,
    'iJson'          : -1
    },
    'GGFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/GGFH115-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH115_ElecTau.root',
    'sample'         : 'GGFH115',
    'xSection'       : 22.96 ,
    'skimEff'        : 7.58E-02 * 14186./480230.,
    'iJson'          : -1
    },
    'GGFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/GGFH120-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH120_ElecTau.root',
    'sample'         : 'GGFH120',
    'xSection'       : 21.13 ,
    'skimEff'        : 7.04E-02 * 141092./4379561.,
    'iJson'          : -1
    },
    'GGFH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/GGFH125-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH125_ElecTau.root',
    'sample'         : 'GGFH125',
    'xSection'       : 19.52,
    'skimEff'        : 6.32E-02 * 23954./688134.,
    'iJson'          : -1
    },
    'GGFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/GGFH130-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH130_ElecTau.root',
    'sample'         : 'GGFH130',
    'xSection'       : 18.07 ,
    'skimEff'        : 5.45E-02 * 37717./989921.,
    'iJson'          : -1
    },
    'GGFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/GGFH135-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH135_ElecTau.root',
    'sample'         : 'GGFH135',
    'xSection'       : 16.79,
    'skimEff'        : 4.49E-02 * 29569./720729.,
    'iJson'          : -1
    },
    'GGFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/GGFH140-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH140_ElecTau.root',
    'sample'         : 'GGFH140',
    'xSection'       : 15.63,
    'skimEff'        : 3.52E-02 * 19042./441198.,
    'iJson'          : -1
    },
    'GGFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/GGFH145-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleGGFH145_ElecTau.root',
    'sample'         : 'GGFH145',
    'xSection'       : 14.59,
    'skimEff'        : 2.61E-02 * 39504./852119.,
    'iJson'          : -1
    },
    'VBFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VBFH110-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH110_ElecTau.root',
    'sample'         : 'VBFH110',
    'xSection'       : 1.809,
    'skimEff'        : 7.95E-02 * 22753./414385.,
    'iJson'          : -1
    },
    'VBFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VBFH115-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH115_ElecTau.root',
    'sample'         : 'VBFH115',
    'xSection'       : 1.729,
    'skimEff'        : 7.58E-02 * 56912./993870.,
    'iJson'          : -1
    },
    'VBFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VBFH120-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH120_ElecTau.root',
    'sample'         : 'VBFH120',
    'xSection'       : 1.649,
    'skimEff'        : 7.04E-02 * 170419./2834487.,
    'iJson'          : -1
    },
    'VBFH125' : {
    'inputFilePath'  : WorkdirLoc+'VBFH125-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH125_ElecTau.root',
    'sample'         : 'VBFH125',
    'xSection'       : 1.578,
    'skimEff'        : 6.32E-02 * 61421./978836.,
    'iJson'          : -1
    },
    'VBFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VBFH130-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH130_ElecTau.root',
    'sample'         : 'VBFH130',
    'xSection'       : 1.511,
    'skimEff'        : 5.45E-02 * 50458./773790.,
    'iJson'          : -1
    },
    'VBFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VBFH135-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH135_ElecTau.root',
    'sample'         : 'VBFH135',
    'xSection'       : 1.448,
    'skimEff'        : 4.49E-02 * 67382./993817.,
    'iJson'          : -1
    },
    'VBFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VBFH140-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH140_ElecTau.root',
    'sample'         : 'VBFH140',
    'xSection'       : 1.389,
    'skimEff'        : 3.52E-02 * 34934./499496.,
    'iJson'          : -1
    },
    'VBFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VBFH145-ElecTau-powheg-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVBFH145_ElecTau.root',
    'sample'         : 'VBFH145',
    'xSection'       : 1.333,
    'skimEff'        : 2.61E-02 * 67190./918883.,
    'iJson'          : -1
    },
    'VH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VH110-ElecTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH110_ElecTau.root',
    'sample'         : 'VH110',
    'xSection'       : (1.060+0.5869+0.1887) ,
    'skimEff'        : 7.95E-02 * 16792./195408.,
    'iJson'          : -1
    },
    'VH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VH115-ElecTau-pythia-PUS10-v2_run/',
    'outputFileName' : OutDir+'nTupleVH115_ElecTau.root',
    'sample'         : 'VH115',
    'xSection'       : (0.9165+0.5117+0.1663),
    'skimEff'        : 7.58E-02 * 17146./190460.,
    'iJson'          : -1
    },
    'VH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VH120-ElecTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH120_ElecTau.root',
    'sample'         : 'VH120',
    'xSection'       : (0.7966+0.4483+0.1470),
    'skimEff'        : 7.04E-02 * 16147./170288.,
    'iJson'          : -1
    },
    'VH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VH125-ElecTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH125_ElecTau.root',
    'sample'         : 'VH125',
    'xSection'       : (0.6966+0.3943+0.1302),
    'skimEff'        : 6.32E-02 * 19299./195124.,
    'iJson'          : -1
    },
    'VH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VH130-ElecTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH130_ElecTau.root',
    'sample'         : 'VH130',
    'xSection'       : (0.6095+0.3473+0.1157),
    'skimEff'        : 5.45E-02 * 20383./200080.,
    'iJson'          : -1
    },
    'VH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VH135-ElecTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH135_ElecTau.root',
    'sample'         : 'VH135',
    'xSection'       : (0.5351+0.3074+0.1031),
    'skimEff'        : 4.49E-02 * 17609./165128.,
    'iJson'          : -1
    },
    'VH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VH140-ElecTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH140_ElecTau.root',
    'sample'         : 'VH140',
    'xSection'       : (0.4713+0.2728+0.09207),
    'skimEff'        : 3.52E-02 * 18328./165186.,
    'iJson'          : -1
    },
    'VH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM_2/VH145-ElecTau-pythia-PUS10_run/',
    'outputFileName' : OutDir+'nTupleVH145_ElecTau.root',
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

process.treeSkimmerElecTau_HCP = cms.PSet(
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
    for analysis in [ 'nominal', 'TauUp', 'TauDown','JetUp','JetDown','ElecUp','ElecDown' ]:
        if  re.search("Data",sample)!=None and analysis != 'nominal':
            continue
        configOptions = option.copy()
        configOptions['analysis'] = analysis
        if  re.search("Data",sample)==None :
            configOptions['outputFileName'] = configOptions['outputFileName'].replace('.root', '_%s.root' % analysis) 
        configFileName = "treeSkimmerElecTau_Spring13_%s_%s_cfg.py" % (sample,analysis)
        configFileName_full = os.path.join(submissionDirectory, configFileName)
        configFile = open(configFileName_full, 'w')
        configConfig = config_template.substitute(configOptions)
        configFile.write(configConfig)
        configFile.close()
