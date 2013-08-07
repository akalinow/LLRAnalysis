#!/usr/bin/env python

import os
import re
import shlex
import string
import subprocess


WorkdirLoc = '/data_CMS/cms/htautau/PostMoriond/TREES_Summer13_TES/EleTau/'
OutDir   = '/data_CMS/cms/htautau/PostMoriond/NTUPLES_Summer13_TES/EleTau/temp/'

options = {
    ###e+tau samples

    ##DATA
    'Run2012A-22Jan2013-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012A_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleRun2012A-22Jan2013-Data_ElecTau.root',
    'sample'         : 'Run2012A-22Jan2013-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },   
    'Run2012B-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_v1_p1/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p1-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p2-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_v1_p3/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p3-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_v3_p4/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p4-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_v1_p5/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p5-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },   
    'Run2012C-22Jan2013-p0-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v1_p0/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p0-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p0-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v1_p1/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p1-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p2-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v3_p3/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p3-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v3_p4/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p4-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p5-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p6-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v1_p6/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p6-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p7-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v1_p7/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p7-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p1-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v1_p2/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p2-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v1_p3/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p3-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p4-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v1_p5/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p5-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p6-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v1_p6/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p6-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p7-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v1_p7/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p7-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p8-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v2_p8/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p8-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p8-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p9-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_EleTau_v2_p9/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p9-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p9-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ##Embedded
    'Run2012A-22Jan2013-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012A_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3/',
    'outputFileName' : OutDir+'nTupleRun2012A-22Jan2013-Embedded_ElecTau.root',
    'sample'         : 'Run2012A-22Jan2013-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p1/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p1-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_12July2013_Trees_EleTau_v5_p2/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p2-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_12July2013_Trees_EleTau_v5_p3/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p3-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p4/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p4-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p5/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p5-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p0-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p0/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p0-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p0-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p1/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p1-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p2/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p2-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_12July2013_Trees_EleTau_v5_p3/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p3-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p4/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p4-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p5/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p5-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p6-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_12July2013_Trees_EleTau_v5_p6/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p6-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p7-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_12July2013_Trees_EleTau_v5_p7/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p7-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p1/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p1-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p2/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p2-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p3/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p3-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p4/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p4-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p5/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p5-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p6-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p6/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p6-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p7-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p7/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p7-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p8-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p8/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p8-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p8-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p9-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_12July2013_Trees_EleTau_v3_p9/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p9-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p9-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ##Bkg MC
    'DYJetsTauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJetsTauTau_ElecTau.root',
    'sample'         : 'DYJetsTauTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJetsZTTL' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJetsZTTL_ElecTau.root',
    'sample'         : 'DYJetsZTTL',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJetsEToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJetsEToTau_ElecTau.root',
    'sample'         : 'DYJetsEToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJetsJetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJetsJetToTau_ElecTau.root',
    'sample'         : 'DYJetsJetToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ##DY1Jets
    'DYJets1Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1Jets_ElecTau.root',
    'sample'         : 'DYJets1Jets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets1JetsTauTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p0_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets1JetsTauTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p1_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets1JetsTauTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p2_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets1JetsTauTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p3_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets1JetsTauTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p4_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets1JetsTauTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p5_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets1JetsTauTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p6_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets1JetsTauTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p7_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets1JetsTauTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p8_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets1JetsTauTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau-p9_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p0_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p1_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p2_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p3_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p4_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p5_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p6_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p7_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p8_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets1JetsZTTL-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL-p9_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p0_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p1_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p2_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p3_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p4_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p5_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p6_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p7_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p8_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets1JetsEToTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau-p9_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p0_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p1_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p2_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p3_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p4_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p5_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p6_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p7_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p8_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets1JetsJetToTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau-p9_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    ##DY2Jets
    'DYJets2Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2Jets_ElecTau.root',
    'sample'         : 'DYJets2Jets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets2JetsTauTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p0_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets2JetsTauTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p1_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets2JetsTauTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p2_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets2JetsTauTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p3_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets2JetsTauTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p4_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets2JetsTauTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p5_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets2JetsTauTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p6_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets2JetsTauTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p7_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets2JetsTauTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p8_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets2JetsTauTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau-p9_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p0_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p1_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p2_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p3_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p4_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p5_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p6_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p7_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p8_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets2JetsZTTL-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL-p9_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p0_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p1_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p2_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p3_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p4_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p5_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p6_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p7_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p8_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets2JetsEToTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau-p9_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p0_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p1_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p2_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p3_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p4_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p5_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p6_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p7_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p8_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets2JetsJetToTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau-p9_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    ##DY3Jets
    'DYJets3Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3Jets_ElecTau.root',
    'sample'         : 'DYJets3Jets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets3JetsTauTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p0_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets3JetsTauTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p1_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets3JetsTauTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p2_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets3JetsTauTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p3_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets3JetsTauTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p4_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets3JetsTauTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p5_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets3JetsTauTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p6_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets3JetsTauTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p7_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets3JetsTauTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p8_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets3JetsTauTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau-p9_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p0_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p1_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p2_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p3_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p4_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p5_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p6_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p7_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p8_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets3JetsZTTL-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL-p9_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p0_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p1_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p2_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p3_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p4_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p5_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p6_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p7_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p8_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets3JetsEToTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau-p9_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p0_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p1_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p2_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p3_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p4_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p5_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p6_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p7_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p8_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets3JetsJetToTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau-p9_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    ##DY4Jets
    'DYJets4Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4Jets_ElecTau.root',
    'sample'         : 'DYJets4Jets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets4JetsTauTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p0_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets4JetsTauTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p1_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets4JetsTauTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p2_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets4JetsTauTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p3_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets4JetsTauTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p4_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets4JetsTauTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p5_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets4JetsTauTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p6_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets4JetsTauTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p7_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets4JetsTauTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p8_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets4JetsTauTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau-p9_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p0_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p1_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p2_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p3_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p4_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p5_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p6_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p7_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p8_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets4JetsZTTL-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL-p9_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p0_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p1_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p2_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p3_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p4_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p5_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p6_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p7_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p8_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets4JetsEToTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau-p9_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p0_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p0',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p1_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p1',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p2_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p2',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p3_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p3',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p4_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p4',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p5_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p5',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p6_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p6',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p7_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p7',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p8_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p8',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'DYJets4JetsJetToTau-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau-p9_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau-p9',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    ##WJets
    'WJets-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WJets-madgraph-PUS10-p1_MC_Bkg_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets-p1_ElecTau.root',
    'sample'         : 'WJets-p1',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.108752 * 450650./2000279,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WJets-madgraph-PUS10-p2_MC_Bkg_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets-p2_ElecTau.root',
    'sample'         : 'WJets-p2',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.108710 * 1415230./6273656,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets1Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets1Jets_ElecTau.root',
    'sample'         : 'WJets1Jets',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 1490832./23141598,##
    'skimEff'        :  1.0 * 1486190./22581598 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets1Jets-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets1Jets-p0_ElecTau.root',
    'sample'         : 'WJets1Jets-p0',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 1490832./23141598,##
    'skimEff'        :  1.0 * 1486190./22581598 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 2
    },
    'WJets1Jets-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W1Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets1Jets-p1_ElecTau.root',
    'sample'         : 'WJets1Jets-p1',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 1490832./23141598,##
    'skimEff'        :  1.0 * 1486190./22581598 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 2
    },
    'WJets2Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets_ElecTau.root',
    'sample'         : 'WJets2Jets',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets2Jets-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p0_ElecTau.root',
    'sample'         : 'WJets2Jets-p0',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'WJets2Jets-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p1_ElecTau.root',
    'sample'         : 'WJets2Jets-p1',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'WJets2Jets-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p2_ElecTau.root',
    'sample'         : 'WJets2Jets-p2',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'WJets2Jets-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p3_ElecTau.root',
    'sample'         : 'WJets2Jets-p3',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'WJets2Jets-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p4_ElecTau.root',
    'sample'         : 'WJets2Jets-p4',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'WJets2Jets-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p5_ElecTau.root',
    'sample'         : 'WJets2Jets-p5',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'WJets2Jets-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p6_ElecTau.root',
    'sample'         : 'WJets2Jets-p6',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'WJets2Jets-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p7_ElecTau.root',
    'sample'         : 'WJets2Jets-p7',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'WJets2Jets-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p8_ElecTau.root',
    'sample'         : 'WJets2Jets-p8',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'WJets2Jets-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets2Jets-p9_ElecTau.root',
    'sample'         : 'WJets2Jets-p9',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 1034822./34044921,##
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'WJets3Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets_ElecTau.root',
    'sample'         : 'WJets3Jets',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets3Jets-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p0_ElecTau.root',
    'sample'         : 'WJets3Jets-p0',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'WJets3Jets-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p1_ElecTau.root',
    'sample'         : 'WJets3Jets-p1',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'WJets3Jets-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p2_ElecTau.root',
    'sample'         : 'WJets3Jets-p2',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'WJets3Jets-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p3_ElecTau.root',
    'sample'         : 'WJets3Jets-p3',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'WJets3Jets-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p4_ElecTau.root',
    'sample'         : 'WJets3Jets-p4',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'WJets3Jets-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p5_ElecTau.root',
    'sample'         : 'WJets3Jets-p5',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'WJets3Jets-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p6_ElecTau.root',
    'sample'         : 'WJets3Jets-p6',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'WJets3Jets-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p7_ElecTau.root',
    'sample'         : 'WJets3Jets-p7',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'WJets3Jets-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p8_ElecTau.root',
    'sample'         : 'WJets3Jets-p8',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'WJets3Jets-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets3Jets-p9_ElecTau.root',
    'sample'         : 'WJets3Jets-p9',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'WJets4Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets_ElecTau.root',
    'sample'         : 'WJets4Jets',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets4Jets-p0' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p0_ElecTau.root',
    'sample'         : 'WJets4Jets-p0',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 10
    },
    'WJets4Jets-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p1_ElecTau.root',
    'sample'         : 'WJets4Jets-p1',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 10
    },
    'WJets4Jets-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p2_ElecTau.root',
    'sample'         : 'WJets4Jets-p2',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 2,
    'nDiv'           : 10
    },
    'WJets4Jets-p3' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p3_ElecTau.root',
    'sample'         : 'WJets4Jets-p3',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 3,
    'nDiv'           : 10
    },
    'WJets4Jets-p4' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p4_ElecTau.root',
    'sample'         : 'WJets4Jets-p4',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 4,
    'nDiv'           : 10
    },
    'WJets4Jets-p5' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p5_ElecTau.root',
    'sample'         : 'WJets4Jets-p5',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 5,
    'nDiv'           : 10
    },
    'WJets4Jets-p6' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p6_ElecTau.root',
    'sample'         : 'WJets4Jets-p6',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 6,
    'nDiv'           : 10
    },
    'WJets4Jets-p7' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p7_ElecTau.root',
    'sample'         : 'WJets4Jets-p7',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 7,
    'nDiv'           : 10
    },
    'WJets4Jets-p8' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p8_ElecTau.root',
    'sample'         : 'WJets4Jets-p8',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 8,
    'nDiv'           : 10
    },
    'WJets4Jets-p9' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleWJets4Jets-p9_ElecTau.root',
    'sample'         : 'WJets4Jets-p9',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1,
    'iDiv'           : 9,
    'nDiv'           : 10
    },
    'TTJets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/TTJets-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v5/',
    'outputFileName' : OutDir+'nTupleTTJets_ElecTau.root',
    'sample'         : 'TTJets',
    'xSection'       : 225.5,
    'skimEff'        : 1.0 * 0.503752 * 2553164./3487856,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'TTJets_SemiLept' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/TTJets-madgraph-SemiLeptMGDecays_MC_Bkg_HTT_25July2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleTTJets_SemiLept_MuTau.root',
    'sample'         : 'TTJets_SemiLept',
    'xSection'       : 249.5,
    'skimEff'        : 0.438 * 0.561995,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'TTJets_FullLept' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/TTJets-madgraph-FullLeptMGDecay_MC_Bkg_HTT_25July2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleTTJets_FullLept_MuTau.root',
    'sample'         : 'TTJets_FullLept',
    'xSection'       : 249.5,
    'skimEff'        : 0.105 * 0.648373,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'TTJets_Had' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/TTJets-madgraph-HadronicMGDecays_MC_Bkg_HTT_25July2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleTTJets_Had_MuTau.root',
    'sample'         : 'TTJets_Had',
    'xSection'       : 249.5,
    'skimEff'        : 0.457 * 0.120926,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'T-tW' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/T-tW-powheg-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleT-tW_ElecTau.root',
    'sample'         : 'T-tW',
    'xSection'       : 11.1,
    'skimEff'        : 0.437682 * 150203./217816,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Tbar-tW' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/Tbar-tW-powheg-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleTbar-tW_ElecTau.root',
    'sample'         : 'Tbar-tW',
    'xSection'       : 11.1,
    'skimEff'        : 0.437851 * 148847./216062,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WWJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WWJetsTo2L2Nu-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWWJetsTo2L2Nu_ElecTau.root',
    'sample'         : 'WWJetsTo2L2Nu',
    'xSection'       : 5.824,
    'skimEff'        : 0.535690 * 423373./1014187,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WZJetsTo2L2Q-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleWZJetsTo2L2Q_ElecTau.root',
    'sample'         : 'WZJetsTo2L2Q',
    'xSection'       : 2.207,
    'skimEff'        : 0.471732 * 1011659./1517085,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WZJetsTo3LNu' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WZJetsTo3LNu-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWZJetsTo3LNu_ElecTau.root',
    'sample'         : 'WZJetsTo3LNu',
    'xSection'       : 1.058,
    'skimEff'        : 0.618550 * 609046./1248321,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'ZZJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/ZZJetsTo2L2Nu-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Nu_ElecTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    'skimEff'        : 0.468227 * 144262./437751,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'ZZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/ZZJetsTo2L2Q-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Q_ElecTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    'skimEff'        : 0.533349 * 673294./1025952,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'ZZJetsTo4L' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/ZZJetsTo4L-madgraph-PUS10_MC_Bkg_HTT_12July2013_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleZZJetsTo4L_ElecTau.root',
    'sample'         : 'ZZJetsTo4L',
    'xSection'       : 0.181,
    'skimEff'        : 0.577947 *  1028639./2225997,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ##Higgs MC
    'GGFH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-90_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH90_ElecTau.root',
    'sample'         : 'GGFH90',
    'xSection'       : 36.80 ,
    'skimEff'        : 8.33E-02 * 0.127154 * 70530./127141,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH95' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-95_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH95_ElecTau.root',
    'sample'         : 'GGFH95',
    'xSection'       : 33.19 ,
    'skimEff'        : 8.32E-02 * 0.139953 * 78879./139942,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-100_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH100_ElecTau.root',
    'sample'         : 'GGFH100',
    'xSection'       : 30.12 ,
    'skimEff'        : 8.28E-02 * 0.153989 * 87528./153974,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH105' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-105_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH105_ElecTau.root',
    'sample'         : 'GGFH105',
    'xSection'       : 27.39 ,
    'skimEff'        : 8.17E-02 * 0.166444 * 95701./166428,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-110_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH110_ElecTau.root',
    'sample'         : 'GGFH110',
    'xSection'       : 25.04 ,
    'skimEff'        : 7.95E-02 * 0.180104 * 104781./180087,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-115_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH115_ElecTau.root',
    'sample'         : 'GGFH115',
    'xSection'       : 22.96 ,
    'skimEff'        : 7.58E-02 * 0.193332 * 103342./177445,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-120_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH120_ElecTau.root',
    'sample'         : 'GGFH120',
    'xSection'       : 21.13 ,
    'skimEff'        : 7.04E-02 * 0.205738 * 601400./1028601,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-125_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH125_ElecTau.root',
    'sample'         : 'GGFH125',
    'xSection'       : 19.52,
    'skimEff'        : 6.32E-02 * 0.218151 * 124232./211199,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-130_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH130_ElecTau.root',
    'sample'         : 'GGFH130',
    'xSection'       : 18.07 ,
    'skimEff'        : 5.45E-02 * 0.205738 * 135906./230102,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-135_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH135_ElecTau.root',
    'sample'         : 'GGFH135',
    'xSection'       : 16.79,
    'skimEff'        : 4.49E-02 * 0.242698 * 123500./208897,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-140_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH140_ElecTau.root',
    'sample'         : 'GGFH140',
    'xSection'       : 15.63,
    'skimEff'        : 3.52E-02 * 0.254905 * 151488./254888,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-145_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH145_ElecTau.root',
    'sample'         : 'GGFH145',
    'xSection'       : 14.59,
    'skimEff'        : 2.61E-02 * 0.266176 * 152486./256093,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH150' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-150_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH150_ElecTau.root',
    'sample'         : 'GGFH150',
    'xSection'       : 13.65,
    'skimEff'        : 1.79E-02 * 0.276603 * 165126./276412,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH155' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-155_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH155_ElecTau.root',
    'sample'         : 'GGFH155',
    'xSection'       : 12.79,
    'skimEff'        : 1.05E-02 * 0.286868 * 155229./260422,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-160_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFH160_ElecTau.root',
    'sample'         : 'GGFH160',
    'xSection'       : 11.95,
    'skimEff'        : 3.96E-03 * 0.297555 * 178456./297529,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-90_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH90_ElecTau.root',
    'sample'         : 'VBFH90',
    'xSection'       : 2.191 ,
    'skimEff'        : 8.33E-02 * 0.288851 * 176816./288419,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH95' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-95_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH95_ElecTau.root',
    'sample'         : 'VBFH95',
    'xSection'       : 2.084 ,
    'skimEff'        : 8.32E-02 * 0.298869 * 183431./298399,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-100_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH100_ElecTau.root',
    'sample'         : 'VBFH100',
    'xSection'       : 1.988,
    'skimEff'        : 8.28E-02 * 0.308857 * 190861./308342,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH105' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-105_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH105_ElecTau.root',
    'sample'         : 'VBFH105',
    'xSection'       : 1.897,
    'skimEff'        : 8.17E-02 * 0.318593 * 197395./318051,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-110_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH110_ElecTau.root',
    'sample'         : 'VBFH110',
    'xSection'       : 1.809,
    'skimEff'        : 7.95E-02 * 0.328027 * 102290./ 163812,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-115_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH115_ElecTau.root',
    'sample'         : 'VBFH115',
    'xSection'       : 1.729,
    'skimEff'        : 7.58E-02 * 0.336526 * 210279./336146,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-120_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH120_ElecTau.root',
    'sample'         : 'VBFH120',
    'xSection'       : 1.649,
    'skimEff'        : 7.04E-02 * 0.346365 * 1042932./1662432,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-125_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVBFH125_ElecTau.root',
    'sample'         : 'VBFH125',
    'xSection'       : 1.578,
    'skimEff'        : 6.32E-02 * 0.356445 * 223334./356030,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-130_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH130_ElecTau.root',
    'sample'         : 'VBFH130',
    'xSection'       : 1.511,
    'skimEff'        : 5.45E-02 * 0.364656 * 229034./364215 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-135_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH135_ElecTau.root',
    'sample'         : 'VBFH135',
    'xSection'       : 1.448,
    'skimEff'        : 4.49E-02 * 0.372054 * 234075./371614,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-140_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH140_ElecTau.root',
    'sample'         : 'VBFH140',
    'xSection'       : 1.389,
    'skimEff'        : 3.52E-02 * 0.379645 * 239700./379225,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-145_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH145_ElecTau.root',
    'sample'         : 'VBFH145',
    'xSection'       : 1.333,
    'skimEff'        : 2.61E-02 * 0.388766 * 246840./388332,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH150' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-150_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH150_ElecTau.root',
    'sample'         : 'VBFH150',
    'xSection'       : 1.280 ,
    'skimEff'        : 1.79E-02 * 0.397113 * 252492./396316,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH155' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-155_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH155_ElecTau.root',
    'sample'         : 'VBFH155',
    'xSection'       : 1.231 ,
    'skimEff'        : 1.05E-02 * 0.402992 * 128195./201281,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-160_MC_H_HTT_12July2013_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleVBFH160_ElecTau.root',
    'sample'         : 'VBFH160',
    'xSection'       : 1.185 ,
    'skimEff'        : 3.96E-03 * 0.410462 * 130961./204975,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-90_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH90_ElecTau.root',
    'sample'         : 'VH90',
    'xSection'       : (1.972 + 1.057 + 0.3233) ,
    'skimEff'        : 8.33E-02 * 0.3508 * 47117./70160,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH95' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-95_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH95_ElecTau.root',
    'sample'         : 'VH95',
    'xSection'       : (1.676 + 0.9060 + 0.2812) ,
    'skimEff'        : 8.32E-02 * 0.367755 * 49776./73551,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-100_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH100_ElecTau.root',
    'sample'         : 'VH100',
    'xSection'       : (1.432 + 0.7807 + 0.2454) ,
    'skimEff'        : 8.28E-02 * 0.3795 * 51501./75900,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH105' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-105_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH105_ElecTau.root',
    'sample'         : 'VH105',
    'xSection'       : (1.229 + 0.6750 + 0.2149) ,
    'skimEff'        : 8.17E-02 * 0.391685 * 53618./78337,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-110_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH110_ElecTau.root',
    'sample'         : 'VH110',
    'xSection'       : (1.060+0.5869+0.1887) ,
    'skimEff'        : 7.95E-02 * 0.408058 * 55990./81778,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-115_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH115_ElecTau.root',
    'sample'         : 'VH115',
    'xSection'       : (0.9165+0.5117+0.1663),
    'skimEff'        : 7.58E-02 * 0.419465 * 57925./84086,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-120_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH120_ElecTau.root',
    'sample'         : 'VH120',
    'xSection'       : (0.7966+0.4483+0.1470),
    'skimEff'        : 7.04E-02 * 0.432465 * 59517./85884,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-125_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH125_ElecTau.root',
    'sample'         : 'VH125',
    'xSection'       : (0.6966+0.3943+0.1302),
    'skimEff'        : 6.32E-02 * 0.444300 * 61842./88915,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-130_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH130_ElecTau.root',
    'sample'         : 'VH130',
    'xSection'       : (0.6095+0.3473+0.1157),
    'skimEff'        : 5.45E-02 * 0.455723 * 63369./91181,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-135_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH135_ElecTau.root',
    'sample'         : 'VH135',
    'xSection'       : (0.5351+0.3074+0.1031),
    'skimEff'        : 4.49E-02 * 0.466761 * 65248./93412,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-140_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH140_ElecTau.root',
    'sample'         : 'VH140',
    'xSection'       : (0.4713+0.2728+0.09207),
    'skimEff'        : 3.52E-02 * 0.47803 * 66990./95695,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-145_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH145_ElecTau.root',
    'sample'         : 'VH145',
    'xSection'       : (0.4164+0.2424+0.08246),
    'skimEff'        : 2.61E-02 * 0.486596 * 68646./97506,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH150' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-150_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH150_ElecTau.root',
    'sample'         : 'VH150',
    'xSection'       : (0.3681 + 0.2159 + 0.07403),
    'skimEff'        : 1.79E-02 * 0.497987 * 70167./99839,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH155' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-155_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH155_ElecTau.root',
    'sample'         : 'VH155',
    'xSection'       : (0.3252 + 0.1923 + 0.06664),
    'skimEff'        : 1.05E-02 * 0.507346 * 71905./101664,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-160_MC_H_HTT_12July2013_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleVH160_ElecTau.root',
    'sample'         : 'VH160',
    'xSection'       : (0.2817 + 0.1687 + 0.06013),
    'skimEff'        : 3.96E-03 * 0.515163 * 72871./103115,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ######MSSM
    'SUSYGGH80' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH80-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH80_ElecTau.root',
    'sample'         : 'SUSYGGH80',
    'xSection'       : 1.0,
    'skimEff'        : 0.104221299534 * 55755./103014,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH90-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH90_ElecTau.root',
    'sample'         : 'SUSYGGH90',
    'xSection'       : 1.0,
    'skimEff'        : 0.130604288499 * 73465./130650,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH100-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH100_ElecTau.root',
    'sample'         : 'SUSYGGH100',
    'xSection'       : 1.0,
    'skimEff'        : 0.156415486761 * 90127./156428,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH110-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH110_ElecTau.root',
    'sample'         : 'SUSYGGH110',
    'xSection'       : 1.0,
    'skimEff'        : 0.182624446693 * 106926./182688,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH120-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH120_ElecTau.root',
    'sample'         : 'SUSYGGH120',
    'xSection'       : 1.0,
    'skimEff'        : 0.208174931542 * 123290./208304,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH130-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH130_ElecTau.root',
    'sample'         : 'SUSYGGH130',
    'xSection'       : 1.0,
    'skimEff'        : 0.232187901671 * 137499./231222,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH140-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH140_ElecTau.root',
    'sample'         : 'SUSYGGH140',
    'xSection'       : 1.0,
    'skimEff'        : 0.256235995519 * 153919./256154,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH160-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH160_ElecTau.root',
    'sample'         : 'SUSYGGH160',
    'xSection'       : 1.0,
    'skimEff'        : 0.298341423948 * 178301./295000,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH180' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH180-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH180_ElecTau.root',
    'sample'         : 'SUSYGGH180',
    'xSection'       : 1.0,
    'skimEff'        : 0.336519253911 * 204775./335577,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH200' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH200-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH200_ElecTau.root',
    'sample'         : 'SUSYGGH200',
    'xSection'       : 1.0,
    'skimEff'        : 0.368996454854 * 222769./363777,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH250' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH250-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH250_ElecTau.root',
    'sample'         : 'SUSYGGH250',
    'xSection'       : 1.0,
    'skimEff'        : 0.43499716625 * 268579./435189,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH300' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH300-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH300_ElecTau.root',
    'sample'         : 'SUSYGGH300',
    'xSection'       : 1.0,
    'skimEff'        : 0.48091174761 * 296334./476572,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH350' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH350-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH350_ElecTau.root',
    'sample'         : 'SUSYGGH350',
    'xSection'       : 1.0,
    'skimEff'        : 0.499360752503 * 312599./499558,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH400' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH400-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH400_ElecTau.root',
    'sample'         : 'SUSYGGH400',
    'xSection'       : 1.0,
    'skimEff'        : 0.518683274021 * 326585./516538,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH450' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH450-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH450_ElecTau.root',
    'sample'         : 'SUSYGGH450',
    'xSection'       : 1.0,
    'skimEff'        : 0.521099815897 * 331358./520808,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH500' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH500-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH500_ElecTau.root',
    'sample'         : 'SUSYGGH500',
    'xSection'       : 1.0,
    'skimEff'        : 0.500466543342 * 314766./491302,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH600' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH600-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH600_ElecTau.root',
    'sample'         : 'SUSYGGH600',
    'xSection'       : 1.0,
    'skimEff'        : 0.276240518363 * 174464./270803,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH700' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH700-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH700_ElecTau.root',
    'sample'         : 'SUSYGGH700',
    'xSection'       : 1.0,
    'skimEff'        : 0.222982349361 * 142697./219816,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH800' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH800-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH800_ElecTau.root',
    'sample'         : 'SUSYGGH800',
    'xSection'       : 1.0,
    'skimEff'        : 0.18059437019 * 115548./178355,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH900' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH900-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH900_ElecTau.root',
    'sample'         : 'SUSYGGH900',
    'xSection'       : 1.0,
    'skimEff'        : 0.14921024367 * 94040./145591,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH1000' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH1000-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH1000_ElecTau.root',
    'sample'         : 'SUSYGGH1000',
    'xSection'       : 1.0,
    'skimEff'        : 0.126561286395 * 81354./126609,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH80' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH80-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH80_ElecTau.root',
    'sample'         : 'SUSYBBH80',
    'xSection'       : 1.0,
    'skimEff'        : 0.0925855314913 * 47010./92270,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH90-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH90_ElecTau.root',
    'sample'         : 'SUSYBBH90',
    'xSection'       : 1.0,
    'skimEff'        : 0.118757029242 * 63712./118263,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH100-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH100_ElecTau.root',
    'sample'         : 'SUSYBBH100',
    'xSection'       : 1.0,
    'skimEff'        : 0.146602087332 * 81635./146649,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH110-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH110_ElecTau.root',
    'sample'         : 'SUSYBBH110',
    'xSection'       : 1.0,
    'skimEff'        : 0.173093237295 * 98647./173024,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH120-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH120_ElecTau.root',
    'sample'         : 'SUSYBBH120',
    'xSection'       : 1.0,
    'skimEff'        : 0.200630498844 * 114705./199008,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH130-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH130_ElecTau.root',
    'sample'         : 'SUSYBBH130',
    'xSection'       : 1.0,
    'skimEff'        : 0.22481120151 * 131242./224813,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH140-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH140_ElecTau.root',
    'sample'         : 'SUSYBBH140',
    'xSection'       : 1.0,
    'skimEff'        : 0.25027979066 * 146177./248678,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH160-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH160_ElecTau.root',
    'sample'         : 'SUSYBBH160',
    'xSection'       : 1.0,
    'skimEff'        : 0.295152820513 * 170912./287774,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH180' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH180-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH180_ElecTau.root',
    'sample'         : 'SUSYBBH180',
    'xSection'       : 1.0,
    'skimEff'        : 0.333586810489 * 195229./327256,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH200' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH200-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH200_ElecTau.root',
    'sample'         : 'SUSYBBH200',
    'xSection'       : 1.0,
    'skimEff'        : 0.370115108144 * 221906./369896,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH250' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH250-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH250_ElecTau.root',
    'sample'         : 'SUSYBBH250',
    'xSection'       : 1.0,
    'skimEff'        : 0.437450920866 * 262237./432289,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH300' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH300-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH300_ElecTau.root',
    'sample'         : 'SUSYBBH300',
    'xSection'       : 1.0,
    'skimEff'        : 0.487159715972 * 297096./487111,
   'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH350' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH350-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH350_ElecTau.root',
    'sample'         : 'SUSYBBH350',
    'xSection'       : 1.0,
    'skimEff'        : 0.524930759205 * 316045./515524,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH400' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH400-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH400_ElecTau.root',
    'sample'         : 'SUSYBBH400',
    'xSection'       : 1.0,
    'skimEff'        : 0.554657946014 * 341667./553938,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH450' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH450-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH450_ElecTau.root',
    'sample'         : 'SUSYBBH450',
    'xSection'       : 1.0,
    'skimEff'        : 0.578385344465 * 349676./563249,
   'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH500' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH500-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH500_ElecTau.root',
    'sample'         : 'SUSYBBH500',
    'xSection'       : 1.0,
    'skimEff'        : 0.595235285647 * 372346./595221,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH600' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH600-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH600_ElecTau.root',
    'sample'         : 'SUSYBBH600',
    'xSection'       : 1.0,
    'skimEff'        : 0.614847426291 * 387790./610581,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH700' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH700-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH700_ElecTau.root',
    'sample'         : 'SUSYBBH700',
    'xSection'       : 1.0,
    'skimEff'        : 0.62296681913 * 399540./621073,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH800' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH800-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH800_ElecTau.root',
    'sample'         : 'SUSYBBH800',
    'xSection'       : 1.0,
    'skimEff'        : 0.625660317922 * 402974./617674,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH900' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH900-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH900_ElecTau.root',
    'sample'         : 'SUSYBBH900',
    'xSection'       : 1.0,
    'skimEff'        : 0.622572102711 * 409592./621124,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH1000' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH1000-pythia-PUS10_27July2013_EleTau_Trees_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH1000_ElecTau.root',
    'sample'         : 'SUSYBBH1000',
    'xSection'       : 1.0,
    'skimEff'        : 0.615910570101 * 402063./604487,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
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

process.treeSkimmerElecTau = cms.PSet(
    sample = cms.string('$sample'),
    analysis = cms.string('$analysis'),
    xSection = cms.double($xSection),
    skimEff = cms.double($skimEff),
    iJson = cms.int32($iJson),
    iDiv = cms.int32($iDiv),
    nDiv = cms.int32($nDiv)
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
        configFileName = "treeSkimmerElecTau_Summer13_%s_%s_cfg.py" % (sample,analysis)
        configFileName_full = os.path.join(submissionDirectory, configFileName)
        configFile = open(configFileName_full, 'w')
        configConfig = config_template.substitute(configOptions)
        configFile.write(configConfig)
        configFile.close()
