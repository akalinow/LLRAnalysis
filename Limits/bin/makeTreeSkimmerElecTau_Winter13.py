#!/usr/bin/env python

import os
import re
import shlex
import string
import subprocess

WorkdirLoc = '/data_CMS/cms/htautau/PostMoriond/TREES_OlivierTES/EleTau/'
WorkdirLocSM = '/data_CMS/cms/htautau/PostMoriond/TREES_OlivierTES/EleTau/'
WorkdirLocData = '/data_CMS/cms/htautau/PostMoriond/TREES_NewTauIDVariables/EleTau/'
OutDir   = '/data_CMS/cms/htautau/PostMoriond/NTUPLES_OlivierTES/EleTau/temp/'

options = {
    ###e+tau samples

    ##DATA
    'Run2012A-22Jan2013-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012A_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleRun2012A-22Jan2013-Data_ElecTau.root',
    'sample'         : 'Run2012A-22Jan2013-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },   
    'Run2012B-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p1-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p2-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p3/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p3-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p4-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p5-Data_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },   
    'Run2012C-22Jan2013-p0-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p0/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p0-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p0-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p1-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p2-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p3/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p3-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p4-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p5-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p6-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p6/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p6-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p7-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p7/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p7-Data_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p1-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p2-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p3/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p3-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p4-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p5-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p6-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p6/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p6-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p7-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p7/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p7-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p8-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p8/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p8-Data_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p8-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p9-Data' : {
    'inputFilePath'  : WorkdirLocData+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_EleTau_v2_p9/',
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
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012A_ReReco22Jan_HTT_11Mar14_Trees_v2/',
    'outputFileName' : OutDir+'nTupleRun2012A-22Jan2013-Embedded_ElecTau.root',
    'sample'         : 'Run2012A-22Jan2013-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_11Mar14_Trees_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p1-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_11Mar14_Trees_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p2-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_11Mar14_Trees_v2_p3/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p3-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_11Mar14_Trees_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p4-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012B-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012B_ReReco22Jan_HTT_11Mar14_Trees_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p5-Embedded_ElecTau.root',
    'sample'         : 'Run2012B-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p0-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_11Mar14_Trees_v2_p0/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p0-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p0-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_11Mar14_Trees_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p1-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_11Mar14_Trees_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p2-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_11Mar14_Trees_v2_p3/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p3-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_11Mar14_Trees_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p4-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_11Mar14_Trees_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p5-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p6-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_11Mar14_Trees_v2_p6/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p6-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012C-22Jan2013-p7-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012C_ReReco22Jan_HTT_11Mar14_Trees_v2_p7/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p7-Embedded_ElecTau.root',
    'sample'         : 'Run2012C-22Jan2013-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_11Mar14_Trees_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p1-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_11Mar14_Trees_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p2-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_11Mar14_Trees_v2_p3/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p3-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_11Mar14_Trees_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p4-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_11Mar14_Trees_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p5-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p6-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_11Mar14_Trees_v2_p6/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p6-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p7-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_11Mar14_Trees_v2_p7/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p7-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p8-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_11Mar14_Trees_v2_p8/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p8-Embedded_ElecTau.root',
    'sample'         : 'Run2012D-22Jan2013-p8-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Run2012D-22Jan2013-p9-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_2012D_ReReco22Jan_HTT_11Mar14_Trees_v2_p9/',
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
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJetsTauTau_ElecTau.root',
    'sample'         : 'DYJetsTauTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJetsZTTL' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJetsZTTL_ElecTau.root',
    'sample'         : 'DYJetsZTTL',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJetsZTTJ' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJetsZTTJ_ElecTau.root',
    'sample'         : 'DYJetsZTTJ',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJetsEToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJetsEToTau_ElecTau.root',
    'sample'         : 'DYJetsEToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJetsJetToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
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
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY1JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets1Jets_ElecTau.root',
    'sample'         : 'DYJets1Jets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets1JetsTauTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY1JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsTauTau_ElecTau.root',
    'sample'         : 'DYJets1JetsTauTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets1JetsZTTL' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY1JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTL_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTL',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets1JetsZTTJ' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY1JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsZTTJ_ElecTau.root',
    'sample'         : 'DYJets1JetsZTTJ',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets1JetsEToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY1JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsEToTau_ElecTau.root',
    'sample'         : 'DYJets1JetsEToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets1JetsJetToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY1JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets1JetsJetToTau_ElecTau.root',
    'sample'         : 'DYJets1JetsJetToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ##DY2Jets
    'DYJets2Jets' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY2JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets2Jets_ElecTau.root',
    'sample'         : 'DYJets2Jets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets2JetsTauTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY2JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsTauTau_ElecTau.root',
    'sample'         : 'DYJets2JetsTauTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets2JetsZTTL' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY2JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTL_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTL',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets2JetsZTTJ' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY2JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsZTTJ_ElecTau.root',
    'sample'         : 'DYJets2JetsZTTJ',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets2JetsEToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY2JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsEToTau_ElecTau.root',
    'sample'         : 'DYJets2JetsEToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets2JetsJetToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY2JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets2JetsJetToTau_ElecTau.root',
    'sample'         : 'DYJets2JetsJetToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ##DY3Jets
    'DYJets3Jets' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY3JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets3Jets_ElecTau.root',
    'sample'         : 'DYJets3Jets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets3JetsTauTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY3JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsTauTau_ElecTau.root',
    'sample'         : 'DYJets3JetsTauTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets3JetsZTTL' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY3JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTL_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTL',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets3JetsZTTJ' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY3JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsZTTJ_ElecTau.root',
    'sample'         : 'DYJets3JetsZTTJ',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets3JetsEToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY3JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsEToTau_ElecTau.root',
    'sample'         : 'DYJets3JetsEToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets3JetsJetToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY3JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets3JetsJetToTau_ElecTau.root',
    'sample'         : 'DYJets3JetsJetToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ##DY4Jets
    'DYJets4Jets' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY4JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets4Jets_ElecTau.root',
    'sample'         : 'DYJets4Jets',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets4JetsTauTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY4JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsTauTau_ElecTau.root',
    'sample'         : 'DYJets4JetsTauTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets4JetsZTTL' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY4JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTL_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTL',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets4JetsZTTJ' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY4JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsZTTJ_ElecTau.root',
    'sample'         : 'DYJets4JetsZTTJ',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets4JetsEToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY4JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsEToTau_ElecTau.root',
    'sample'         : 'DYJets4JetsEToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'DYJets4JetsJetToTau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/DY4JetsToLL_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleDYJets4JetsJetToTau_ElecTau.root',
    'sample'         : 'DYJets4JetsJetToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1,
    'iDiv'           : 1,
    'nDiv'           : 1
    },
    ##WJets
    'WJets-p1' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WJets-madgraph-PUS10-p1_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets-p1_ElecTau.root',
    'sample'         : 'WJets-p1',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.108752 * 533250./2000279,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets-p2' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WJets-madgraph-PUS10-p2_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets-p2_ElecTau.root',
    'sample'         : 'WJets-p2',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.108710 * 1674658./6273656,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets1Jets' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/W1Jets-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets1Jets_ElecTau.root',
    'sample'         : 'WJets1Jets',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 1850955./22581598 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets2Jets' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/W2Jets-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets2Jets_ElecTau.root',
    'sample'         : 'WJets2Jets',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 5867536./34044921,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets3Jets' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/W3Jets-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets3Jets_ElecTau.root',
    'sample'         : 'WJets3Jets',
    'xSection'       : 36257,
##     'skimEff'        :  1.0 * 3353306./15539503,##
    'skimEff'        :  1.0 * 3829211./15219503,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets4Jets' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/W4Jets-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets4Jets_ElecTau.root',
    'sample'         : 'WJets4Jets',
    'xSection'       : 36257,
##     'skimEff'        : 1.0 * 3993529./13382803,##
    'skimEff'        : 1.0 * 4542508./13382803,##
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    # High stat samples V19
    'WJets1JetsV19' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/W1Jets-madgraph-PUS10_START53_V19_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets1JetsV19_ElecTau.root',
    'sample'         : 'WJets1JetsV19',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 1486190./22581598 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets2JetsV19' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/W2Jets-madgraph-PUS10_START53_V19_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets2JetsV19_ElecTau.root',
    'sample'         : 'WJets2JetsV19',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 1486190./22581598 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WJets3JetsV19' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/W3Jets-madgraph-PUS10_START53_V19_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWJets3JetsV19_ElecTau.root',
    'sample'         : 'WJets3JetsV19',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 1486190./22581598 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'TTJets-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_etau_TTJets-madgraph-PUS10_MC_Bkg_HTT_11Mar14_Trees_v1/',
    'outputFileName' : OutDir+'nTupleTTJets-Embedded_ElecTau.root',
    'sample'         : 'TTJets-Embedded',
    'xSection'       : 26.1975,
    'skimEff'        : 1.0 * 630093./12011428,##12011428 ##758691
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'TTJets' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/TTJets-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleTTJets_ElecTau.root',
    'sample'         : 'TTJets',
    'xSection'       : 225.5,
    'skimEff'        : 1.0 * 0.503752 * 2660824./3487856,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'TTJets_SemiLept' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/TT_SemiLept_MC_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleTTJets_SemiLept_ElecTau.root',
    'sample'         : 'TTJets_SemiLept',
    'xSection'       : 109.281,
    #'xSection'       : 249.5,
    #'skimEff'        : 0.438 * 0.561995,
    #'skimEff'        : 0.438 * 13732045./23474208,
    'skimEff'        : 1.0 * 13732045./23474208,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'TTJets_FullLept' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/TT_FullLept_MC_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleTTJets_FullLept_ElecTau.root',
    'sample'         : 'TTJets_FullLept',
    'xSection'       : 26.1975,
    #'xSection'       : 249.5,
    #'skimEff'        : 0.105 * 0.648373,
    #'skimEff'        : 0.105 * 8187961./11921428,
    'skimEff'        : 1.0 * 8187961./11921428,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'TTJets_Had' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/TT_Hadronic_ext_MC_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleTTJets_Had_ElecTau.root',
    'sample'         : 'TTJets_Had',
    'xSection'       : 114.0215,
    #'xSection'       : 249.5,
    #'skimEff'        : 0.457 * 0.120926,
    #'skimEff'        : 0.457 * 3847510./31023821,
    'skimEff'        : 1.0 * 3847510./31023821,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'T-tW' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/T-tW-powheg-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleT-tW_ElecTau.root',
    'sample'         : 'T-tW',
    'xSection'       : 11.1,
    #'skimEff'        : 0.437682 * 160607./490658,
    'skimEff'        : 1.0 * 160607./490658,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'Tbar-tW' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/Tbar-tW-powheg-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleTbar-tW_ElecTau.root',
    'sample'         : 'Tbar-tW',
    'xSection'       : 11.1,
    #'skimEff'        : 0.437851 * 148847./216062,
    'skimEff'        : 1.0 * 161536./493460,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WWJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WWJetsTo2L2Nu-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWWJetsTo2L2Nu_ElecTau.root',
    'sample'         : 'WWJetsTo2L2Nu',
    'xSection'       : 5.824,
    #'skimEff'        : 0.535690 * 423373./1014187,
    'skimEff'        : 0.535690 * 459248./1007187,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WZJetsTo2L2Q-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWZJetsTo2L2Q_ElecTau.root',
    'sample'         : 'WZJetsTo2L2Q',
    'xSection'       : 2.207,
    #'skimEff'        : 0.471732 * 1011659./1517085,
    'skimEff'        : 1.0 * 1135312./3215990,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WZJetsTo3LNu' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WZJetsTo3LNu-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWZJetsTo3LNu_ElecTau.root',
    'sample'         : 'WZJetsTo3LNu',
    'xSection'       : 1.058,
    #'skimEff'        : 0.618550 * 609046./1248321,
    'skimEff'        : 1.0 * 670103./2017979,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'ZZJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/ZZJetsTo2L2Nu-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Nu_ElecTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    #'skimEff'        : 0.468227 * 144262./437751,
    'skimEff'        : 1.0 * 170171./954911,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'ZZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/ZZJetsTo2L2Q-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Q_ElecTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    #'skimEff'        : 0.533349 * 673294./1025952,
    'skimEff'        : 1.0 * 762432./1929727,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'ZZJetsTo4L' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/ZZJetsTo4L-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleZZJetsTo4L_ElecTau.root',
    'sample'         : 'ZZJetsTo4L',
    'xSection'       : 0.181,
    #'skimEff'        : 0.577947 *  1028639./2225997,
    'skimEff'        : 1.0 * 1396571./4793893,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WGToLNuG' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WGToLNuG-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWGToLNuG_ElecTau.root',
    'sample'         : 'WGToLNuG',
    'xSection'       : 461.6,
    'skimEff'        : 1.0 * 248374./4772358,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WGstarToLNu2E' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WGstarToLNu2E-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWGstarToLNu2E_ElecTau.root',
    'sample'         : 'WGstarToLNu2E',
    'xSection'       : 5.873,
    'skimEff'        : 1.0 * 10134./314653 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WGstarToLNu2Mu' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WGstarToLNu2Mu-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWGstarToLNu2Mu_ElecTau.root',
    'sample'         : 'WGstarToLNu2Mu',
    'xSection'       : 1.914,
    'skimEff'        : 1.0 * 9233./299973,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'WGstarToLNu2Tau' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WGstarToLNu2Tau-madgraph-PUS10_MC_Bkg_HTT_11Mar2014_Trees_EleTau_v1/',
    'outputFileName' : OutDir+'nTupleWGstarToLNu2Tau_ElecTau.root',
    'sample'         : 'WGstarToLNu2Tau',
    'xSection'       : 0.336,
    'skimEff'        : 1.0 * 1287./49997,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ##Higgs MC
    'GGFH90' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-90_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH90_ElecTau.root',
    'sample'         : 'GGFH90',
    'xSection'       : 36.80 ,
    'skimEff'        : 8.33E-02 * 0.127154 * 77665./127141,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH95' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-95_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH95_ElecTau.root',
    'sample'         : 'GGFH95',
    'xSection'       : 33.19 ,
    'skimEff'        : 8.32E-02 * 0.139953 * 86265./139942,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH100' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-100_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH100_ElecTau.root',
    'sample'         : 'GGFH100',
    'xSection'       : 30.12 ,
    'skimEff'        : 8.28E-02 * 0.153989 * 95257./153974,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH105' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-105_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH105_ElecTau.root',
    'sample'         : 'GGFH105',
    'xSection'       : 27.39 ,
    'skimEff'        : 8.17E-02 * 0.166444 * 103705./166428,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH110' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-110_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH110_ElecTau.root',
    'sample'         : 'GGFH110',
    'xSection'       : 25.04 ,
    'skimEff'        : 7.95E-02 * 0.180104 * 113059./180087,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH115' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-115_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH115_ElecTau.root',
    'sample'         : 'GGFH115',
    'xSection'       : 22.96 ,
    'skimEff'        : 7.58E-02 * 0.193332 * 111420./177445,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH120' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-120_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH120_ElecTau.root',
    'sample'         : 'GGFH120',
    'xSection'       : 21.13 ,
    'skimEff'        : 7.04E-02 * 0.205738 * 646081./1028601,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH125' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-125_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH125_ElecTau.root',
    'sample'         : 'GGFH125',
    'xSection'       : 19.52,
    'skimEff'        : 6.32E-02 * 0.218151 * 133131./211199,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFHH125' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHHTo2B2Tau_M-125_MC_H_HTT_11Mar2014_Trees_EleTau_v1',
    'outputFileName' : OutDir+'nTupleGGFHH125_ElecTau.root',
    'sample'         : 'GGFHH125',
    'xSection'       : 0.00796,
    'skimEff'        : 7.3E-02 * 41238./94889,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH130' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-130_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH130_ElecTau.root',
    'sample'         : 'GGFH130',
    'xSection'       : 18.07 ,
    'skimEff'        : 5.45E-02 * 0.205738 * 145227./230102,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH135' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-135_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH135_ElecTau.root',
    'sample'         : 'GGFH135',
    'xSection'       : 16.79,
    'skimEff'        : 4.49E-02 * 0.242698 * 131605./208897,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH140' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-140_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH140_ElecTau.root',
    'sample'         : 'GGFH140',
    'xSection'       : 15.63,
    'skimEff'        : 3.52E-02 * 0.254905 * 161106./254888,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH145' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-145_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH145_ElecTau.root',
    'sample'         : 'GGFH145',
    'xSection'       : 14.59,
    'skimEff'        : 2.61E-02 * 0.266176 * 161728./256093,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH150' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-150_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH150_ElecTau.root',
    'sample'         : 'GGFH150',
    'xSection'       : 13.65,
    'skimEff'        : 1.79E-02 * 0.276603 * 174915./276412,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH155' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-155_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH155_ElecTau.root',
    'sample'         : 'GGFH155',
    'xSection'       : 12.79,
    'skimEff'        : 1.05E-02 * 0.286868 * 164187./260422,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'GGFH160' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/GluGluToHToTauTau_M-160_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH160_ElecTau.root',
    'sample'         : 'GGFH160',
    'xSection'       : 11.95,
    'skimEff'        : 3.96E-03 * 0.297555 * 188424./297529,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH90' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-90_MC_H_HTT_11Mar2014_Trees_EleTau_v3',
    'outputFileName' : OutDir+'nTupleVBFH90_ElecTau.root',
    'sample'         : 'VBFH90',
    'xSection'       : 2.191 ,
    'skimEff'        : 8.33E-02 * 0.288851 * 188397./288419,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH95' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-95_MC_H_HTT_11Mar2014_Trees_EleTau_v3',
    'outputFileName' : OutDir+'nTupleVBFH95_ElecTau.root',
    'sample'         : 'VBFH95',
    'xSection'       : 2.084 ,
    'skimEff'        : 8.32E-02 * 0.298869 * 195284./298399,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH100' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-100_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH100_ElecTau.root',
    'sample'         : 'VBFH100',
    'xSection'       : 1.988,
    'skimEff'        : 8.28E-02 * 0.308857 * 202816./308342,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH105' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-105_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH105_ElecTau.root',
    'sample'         : 'VBFH105',
    'xSection'       : 1.897,
    'skimEff'        : 8.17E-02 * 0.318593 * 209532./318051,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH110' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-110_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH110_ElecTau.root',
    'sample'         : 'VBFH110',
    'xSection'       : 1.809,
    'skimEff'        : 7.95E-02 * 0.328027 * 108497./ 163812,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH115' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-115_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH115_ElecTau.root',
    'sample'         : 'VBFH115',
    'xSection'       : 1.729,
    'skimEff'        : 7.58E-02 * 0.336526 * 222739./336146,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH120' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-120_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH120_ElecTau.root',
    'sample'         : 'VBFH120',
    'xSection'       : 1.649,
    'skimEff'        : 7.04E-02 * 0.346365 * 1102780./1662432,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH125' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-125_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVBFH125_ElecTau.root',
    'sample'         : 'VBFH125',
    'xSection'       : 1.578,
    'skimEff'        : 6.32E-02 * 0.356445 * 235921./356030,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH130' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-130_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH130_ElecTau.root',
    'sample'         : 'VBFH130',
    'xSection'       : 1.511,
    'skimEff'        : 5.45E-02 * 0.364656 * 241698./364215 ,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH135' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-135_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH135_ElecTau.root',
    'sample'         : 'VBFH135',
    'xSection'       : 1.448,
    'skimEff'        : 4.49E-02 * 0.372054 * 246549./371614,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH140' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-140_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH140_ElecTau.root',
    'sample'         : 'VBFH140',
    'xSection'       : 1.389,
    'skimEff'        : 3.52E-02 * 0.379645 * 252563./379225,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH145' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-145_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH145_ElecTau.root',
    'sample'         : 'VBFH145',
    'xSection'       : 1.333,
    'skimEff'        : 2.61E-02 * 0.388766 * 259483./388332,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH150' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-150_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH150_ElecTau.root',
    'sample'         : 'VBFH150',
    'xSection'       : 1.280 ,
    'skimEff'        : 1.79E-02 * 0.397113 * 265232./396316,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH155' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-155_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH155_ElecTau.root',
    'sample'         : 'VBFH155',
    'xSection'       : 1.231 ,
    'skimEff'        : 1.05E-02 * 0.402992 * 134529./201281,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VBFH160' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/VBF_HToTauTau_M-160_MC_H_HTT_11Mar2014_Trees_EleTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH160_ElecTau.root',
    'sample'         : 'VBFH160',
    'xSection'       : 1.185 ,
    'skimEff'        : 3.96E-03 * 0.410462 * 137174./204975,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH90' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-90_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH90_ElecTau.root',
    'sample'         : 'VH90',
    'xSection'       : (1.972 + 1.057 + 0.3233) ,
    'skimEff'        : 8.33E-02 * 0.3508 * 49931./70160,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH95' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-95_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH95_ElecTau.root',
    'sample'         : 'VH95',
    'xSection'       : (1.676 + 0.9060 + 0.2812) ,
    'skimEff'        : 8.32E-02 * 0.367755 * 52738./73551,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH100' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-100_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH100_ElecTau.root',
    'sample'         : 'VH100',
    'xSection'       : (1.432 + 0.7807 + 0.2454) ,
    'skimEff'        : 8.28E-02 * 0.3795 * 54529./75900,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH105' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-105_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH105_ElecTau.root',
    'sample'         : 'VH105',
    'xSection'       : (1.229 + 0.6750 + 0.2149) ,
    'skimEff'        : 8.17E-02 * 0.391685 * 56589./78337,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH110' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-110_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH110_ElecTau.root',
    'sample'         : 'VH110',
    'xSection'       : (1.060+0.5869+0.1887) ,
    'skimEff'        : 7.95E-02 * 0.408058 * 58944./81778,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH115' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-115_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH115_ElecTau.root',
    'sample'         : 'VH115',
    'xSection'       : (0.9165+0.5117+0.1663),
    'skimEff'        : 7.58E-02 * 0.419465 * 60957./84086,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH120' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-120_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH120_ElecTau.root',
    'sample'         : 'VH120',
    'xSection'       : (0.7966+0.4483+0.1470),
    'skimEff'        : 7.04E-02 * 0.432465 * 62664./85884,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH125' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-125_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH125_ElecTau.root',
    'sample'         : 'VH125',
    'xSection'       : (0.6966+0.3943+0.1302),
    'skimEff'        : 6.32E-02 * 0.444300 * 64812./88915,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH130' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-130_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH130_ElecTau.root',
    'sample'         : 'VH130',
    'xSection'       : (0.6095+0.3473+0.1157),
    'skimEff'        : 5.45E-02 * 0.455723 * 66566./91181,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH135' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-135_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH135_ElecTau.root',
    'sample'         : 'VH135',
    'xSection'       : (0.5351+0.3074+0.1031),
    'skimEff'        : 4.49E-02 * 0.466761 * 68414./93412,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH140' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-140_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH140_ElecTau.root',
    'sample'         : 'VH140',
    'xSection'       : (0.4713+0.2728+0.09207),
    'skimEff'        : 3.52E-02 * 0.47803 * 69975./95695,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH145' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-145_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH145_ElecTau.root',
    'sample'         : 'VH145',
    'xSection'       : (0.4164+0.2424+0.08246),
    'skimEff'        : 2.61E-02 * 0.486596 * 71690./97506,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH150' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-150_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH150_ElecTau.root',
    'sample'         : 'VH150',
    'xSection'       : (0.3681 + 0.2159 + 0.07403),
    'skimEff'        : 1.79E-02 * 0.497987 * 73369./99839,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH155' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-155_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH155_ElecTau.root',
    'sample'         : 'VH155',
    'xSection'       : (0.3252 + 0.1923 + 0.06664),
    'skimEff'        : 1.05E-02 * 0.507346 * 75021./101664,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'VH160' : {
    'inputFilePath'  : WorkdirLocSM+'HiggsSM/WH_ZH_TTH_HToTauTau_M-160_MC_H_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleVH160_ElecTau.root',
    'sample'         : 'VH160',
    'xSection'       : (0.2817 + 0.1687 + 0.06013),
    'skimEff'        : 3.96E-03 * 0.515163 * 75928./103115,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    ######MSSM
    'SUSYGGH80' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH80-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH80_ElecTau.root',
    'sample'         : 'SUSYGGH80',
    'xSection'       : 1.0,
    #'skimEff'        : 0.104221299534 * 55755./103014,
    'skimEff'        : 1.0 * 58499./904416,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH90-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH90_ElecTau.root',
    'sample'         : 'SUSYGGH90',
    'xSection'       : 1.0,
    #'skimEff'        : 0.130604288499 * 73465./130650,
    'skimEff'        : 1.0 * 83252./1000350,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH100-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH100_ElecTau.root',
    'sample'         : 'SUSYGGH100',
    'xSection'       : 1.0,
    #'skimEff'        : 0.156415486761 * 90127./156428,
    'skimEff'        : 1.0 * 101128./1000080,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH110-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH110_ElecTau.root',
    'sample'         : 'SUSYGGH110',
    'xSection'       : 1.0,
    #'skimEff'        : 0.182624446693 * 106926./182688,
    'skimEff'        : 1.0 * 118873./1000348,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH120-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH120_ElecTau.root',
    'sample'         : 'SUSYGGH120',
    'xSection'       : 1.0,
    #'skimEff'        : 0.208174931542 * 123290./208304,
    'skimEff'        : 1.0 * 136189./1000620,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH130-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH130_ElecTau.root',
    'sample'         : 'SUSYGGH130',
    'xSection'       : 1.0,
    #'skimEff'        : 0.232187901671 * 137499./231222,
    'skimEff'        : 1.0 * 149741./987840,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH140-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH140_ElecTau.root',
    'sample'         : 'SUSYGGH140',
    'xSection'       : 1.0,
    #'skimEff'        : 0.256235995519 * 153919./256154,
    'skimEff'        : 1.0 * 168011./999680,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH160-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH160_ElecTau.root',
    'sample'         : 'SUSYGGH160',
    'xSection'       : 1.0,
    #'skimEff'        : 0.298341423948 * 178301./295000,
    'skimEff'        : 1.0 * 193343./988800,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH180' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH180-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH180_ElecTau.root',
    'sample'         : 'SUSYGGH180',
    'xSection'       : 1.0,
    #'skimEff'        : 0.336519253911 * 204775./335577,
    'skimEff'        : 1.0 * 220794./997200,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH200' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH200-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH200_ElecTau.root',
    'sample'         : 'SUSYGGH200',
    'xSection'       : 1.0,
    #'skimEff'        : 0.368996454854 * 222769./363777,
    'skimEff'        : 1.0 * 239175./985855,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH250' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH250-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH250_ElecTau.root',
    'sample'         : 'SUSYGGH250',
    'xSection'       : 1.0,
    #'skimEff'        : 0.43499716625 * 268579./435189,
    'skimEff'        : 1.0 * 286505./1000441,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH300' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH300-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH300_ElecTau.root',
    'sample'         : 'SUSYGGH300',
    'xSection'       : 1.0,
    #'skimEff'        : 0.48091174761 * 296334./476572,
    'skimEff'        : 1.0 * 304102./956976,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH350' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH350-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH350_ElecTau.root',
    'sample'         : 'SUSYGGH350',
    'xSection'       : 1.0,
    #'skimEff'        : 0.499360752503 * 312599./499558,
    'skimEff'        : 1.0 * 331333./1000395,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH400' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH400-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH400_ElecTau.root',
    'sample'         : 'SUSYGGH400',
    'xSection'       : 1.0,
    #'skimEff'        : 0.518683274021 * 326585./516538,
    'skimEff'        : 1.0 * 345297./995864,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH450' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH450-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH450_ElecTau.root',
    'sample'         : 'SUSYGGH450',
    'xSection'       : 1.0,
    #'skimEff'        : 0.521099815897 * 331358./520808,
    'skimEff'        : 1.0 * 349559./997440,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH500' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH500-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH500_ElecTau.root',
    'sample'         : 'SUSYGGH500',
    'xSection'       : 1.0,
    #'skimEff'        : 0.500466543342 * 314766./491302,
    'skimEff'        : 1.0 * 331242./977688,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH600' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH600-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH600_ElecTau.root',
    'sample'         : 'SUSYGGH600',
    'xSection'       : 1.0,
    #'skimEff'        : 0.276240518363 * 174464./270803,
    'skimEff'        : 1.0 * 184727./980316,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH700' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH700-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH700_ElecTau.root',
    'sample'         : 'SUSYGGH700',
    'xSection'       : 1.0,
    #'skimEff'        : 0.222982349361 * 142697./219816,
    'skimEff'        : 1.0 * 132549./865800,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH800' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH800-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH800_ElecTau.root',
    'sample'         : 'SUSYGGH800',
    'xSection'       : 1.0,
    #'skimEff'        : 0.18059437019 * 115548./178355,
    'skimEff'        : 1.0 * 122445./983600,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH900' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH900-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH900_ElecTau.root',
    'sample'         : 'SUSYGGH900',
    'xSection'       : 1.0,
    #'skimEff'        : 0.14921024367 * 94040./145591,
    'skimEff'        : 1.0 * 79617./771744,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH1000' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH1000-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYGGH1000_ElecTau.root',
    'sample'         : 'SUSYGGH1000',
    'xSection'       : 1.0,
    #'skimEff'        : 0.126561286395 * 81354./126609,
    'skimEff'        : 1.0 * 86963./1000377,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH80' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH80-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH80_ElecTau.root',
    'sample'         : 'SUSYBBH80',
    'xSection'       : 1.0,
    #'skimEff'        : 0.0925855314913 * 47010./92270,
    'skimEff'        : 1.0 * 43174./800592,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH90-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH90_ElecTau.root',
    'sample'         : 'SUSYBBH90',
    'xSection'       : 1.0,
    #'skimEff'        : 0.118757029242 * 63712./118263,
    'skimEff'        : 1.0 * 71374./989840,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH100-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH100_ElecTau.root',
    'sample'         : 'SUSYBBH100',
    'xSection'       : 1.0,
    #'skimEff'        : 0.146602087332 * 81635./146649,
    'skimEff'        : 1.0 * 90876./1000320,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH110-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH110_ElecTau.root',
    'sample'         : 'SUSYBBH110',
    'xSection'       : 1.0,
    #'skimEff'        : 0.173093237295 * 98647./173024,
    'skimEff'        : 1.0 * 107675./989600,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH120-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH120_ElecTau.root',
    'sample'         : 'SUSYBBH120',
    'xSection'       : 1.0,
    #'skimEff'        : 0.200630498844 * 114705./199008,
    'skimEff'        : 1.0 * 124519./985913,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH130-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH130_ElecTau.root',
    'sample'         : 'SUSYBBH130',
    'xSection'       : 1.0,
    #'skimEff'        : 0.22481120151 * 131242./224813,
    'skimEff'        : 1.0 * 142612./1000008,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH140-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH140_ElecTau.root',
    'sample'         : 'SUSYBBH140',
    'xSection'       : 1.0,
    #'skimEff'        : 0.25027979066 * 146177./248678,
    'skimEff'        : 1.0 * 158024./993600,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH160-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH160_ElecTau.root',
    'sample'         : 'SUSYBBH160',
    'xSection'       : 1.0,
    #'skimEff'        : 0.295152820513 * 170912./287774,
    'skimEff'        : 1.0 * 183217./975000,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH180' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH180-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH180_ElecTau.root',
    'sample'         : 'SUSYBBH180',
    'xSection'       : 1.0,
    #'skimEff'        : 0.333586810489 * 195229./327256,
    'skimEff'        : 1.0 * 208168./981022,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH200' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH200-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH200_ElecTau.root',
    'sample'         : 'SUSYBBH200',
    'xSection'       : 1.0,
    #'skimEff'        : 0.370115108144 * 221906./369896,
    'skimEff'        : 1.0 * 235778./999408,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH250' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH250-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH250_ElecTau.root',
    'sample'         : 'SUSYBBH250',
    'xSection'       : 1.0,
    #'skimEff'        : 0.437450920866 * 262237./432289,
    'skimEff'        : 1.0 * 276597./988200,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH300' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH300-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH300_ElecTau.root',
    'sample'         : 'SUSYBBH300',
    'xSection'       : 1.0,
    #'skimEff'        : 0.487159715972 * 297096./487111,
    'skimEff'        : 1.0 * 312406./999900,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH350' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH350-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH350_ElecTau.root',
    'sample'         : 'SUSYBBH350',
    'xSection'       : 1.0,
    #'skimEff'        : 0.524930759205 * 316045./515524,
    'skimEff'        : 1.0 * 328055./972080,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH400' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH400-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH400_ElecTau.root',
    'sample'         : 'SUSYBBH400',
    'xSection'       : 1.0,
    #'skimEff'        : 0.554657946014 * 341667./553938,
    'skimEff'        : 1.0 * 357873./998702,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH450' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH450-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH450_ElecTau.root',
    'sample'         : 'SUSYBBH450',
    'xSection'       : 1.0,
    #'skimEff'        : 0.578385344465 * 349676./563249,
    'skimEff'        : 1.0 * 360247./959830,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH500' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH500-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH500_ElecTau.root',
    'sample'         : 'SUSYBBH500',
    'xSection'       : 1.0,
    #'skimEff'        : 0.595235285647 * 372346./595221,
    'skimEff'        : 1.0 * 375575./965976,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH600' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH600-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH600_ElecTau.root',
    'sample'         : 'SUSYBBH600',
    'xSection'       : 1.0,
    #'skimEff'        : 0.614847426291 * 387790./610581,
    'skimEff'        : 1.0 * 398503./979061,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH700' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH700-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH700_ElecTau.root',
    'sample'         : 'SUSYBBH700',
    'xSection'       : 1.0,
    #'skimEff'        : 0.62296681913 * 399540./621073,
    'skimEff'        : 1.0 * 412318./986960,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH800' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH800-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH800_ElecTau.root',
    'sample'         : 'SUSYBBH800',
    'xSection'       : 1.0,
    #'skimEff'        : 0.625660317922 * 402974./617674,
    'skimEff'        : 1.0 * 425602./1000432,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH900' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH900-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH900_ElecTau.root',
    'sample'         : 'SUSYBBH900',
    'xSection'       : 1.0,
    #'skimEff'        : 0.622572102711 * 409592./621124,
    'skimEff'        : 1.0 * 415960./971674,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH1000' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH1000-MC_HTT_11Mar2014_Trees_EleTau_v2/',
    'outputFileName' : OutDir+'nTupleSUSYBBH1000_ElecTau.root',
    'sample'         : 'SUSYBBH1000',
    'xSection'       : 1.0,
    #'skimEff'        : 0.615910570101 * 402063./604487,
    'skimEff'        : 1.0 * 420475./983947,
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
        configFileName = "treeSkimmerElecTau_Winter13_%s_%s_cfg.py" % (sample,analysis)
        configFileName_full = os.path.join(submissionDirectory, configFileName)
        configFile = open(configFileName_full, 'w')
        configConfig = config_template.substitute(configOptions)
        configFile.write(configConfig)
        configFile.close()
