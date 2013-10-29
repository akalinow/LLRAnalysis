#!/usr/bin/env python

import os
import re
import shlex
import string
import subprocess

WorkdirLoc = '/data_CMS/cms/htautau/PostMoriond/TREES_NewTauID/MuTau/'
OutDir   = '/data_CMS/cms/htautau/PostMoriond/NTUPLES_NewTauID/MuTau/temp/'

options = {
    ###mu+tau samples
    
    ##DATA
    'Run2012A-22Jan2013-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012A_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1/',
    'outputFileName' : OutDir+'nTupleRun2012A-22Jan2013-Data_MuTau.root',
    'sample'         : 'Run2012A-22Jan2013-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p1/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p1-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p2/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p2-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p3/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p3-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p4/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p4-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p5/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p5-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p0-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p0/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p0-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p0-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p1/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p1-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p2/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p2-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p3/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p3-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p4/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p4-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p5/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p5-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p6-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p6/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p6-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p7-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p7/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p7-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p1/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p1-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p2/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p2-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p3/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p3-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p4/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p4-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p5/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p5-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p6-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p6/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p6-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p7-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p7/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p7-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p8-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p8/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p8-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p8-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p9-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v1_p9/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p9-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p9-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    ##Embedded
    'Run2012A-22Jan2013-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/Embed_mutau_2012A_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleRun2012A-22Jan2013-Embedded_MuTau.root',
    'sample'         : 'Run2012A-22Jan2013-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/Embed_mutau_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p1-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/Embed_mutau_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p2-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/Embed_mutau_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v2_p3/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p3-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/Embed_mutau_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p4-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/Embed_mutau_2012B_ReReco22Jan_HTT_09Oct13_Trees_MuTau_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p5-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p0-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012C_ReReco22Jan_HTT_09Oct13_Trees_v2_p0/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p0-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p0-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012C_ReReco22Jan_HTT_09Oct13_Trees_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p1-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012C_ReReco22Jan_HTT_09Oct13_Trees_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p2-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012C_ReReco22Jan_HTT_09Oct13_Trees_v2_p3/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p3-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012C_ReReco22Jan_HTT_09Oct13_Trees_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p4-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012C_ReReco22Jan_HTT_09Oct13_Trees_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p5-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p6-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012C_ReReco22Jan_HTT_09Oct13_Trees_v2_p6/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p6-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p7-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012C_ReReco22Jan_HTT_09Oct13_Trees_v2_p7/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p7-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012D_ReReco22Jan_HTT_09Oct13_Trees_v2_p1/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p1-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012D_ReReco22Jan_HTT_09Oct13_Trees_v2_p2/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p2-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012D_ReReco22Jan_HTT_09Oct13_Trees_v2_p3/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p3-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012D_ReReco22Jan_HTT_09Oct13_Trees_v2_p4/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p4-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012D_ReReco22Jan_HTT_09Oct13_Trees_v2_p5/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p5-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p6-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012D_ReReco22Jan_HTT_09Oct13_Trees_v2_p6/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p6-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p7-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012D_ReReco22Jan_HTT_09Oct13_Trees_v2_p7/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p7-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p8-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012D_ReReco22Jan_HTT_09Oct13_Trees_v2_p8/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p8-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p8-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p9-Embedded' : {
    'inputFilePath'  : WorkdirLoc+'EmbeddedPF/PFEmbed_mutau_2012D_ReReco22Jan_HTT_09Oct13_Trees_v2_p9/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p9-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p9-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    ##Bkg MC
    'DYJets_TauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets_TauTau_MuTau.root',
    'sample'         : 'DYJets_TauTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2716918./9669034,
    'iJson'          : -1
    } ,
    'DYJets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets_JetToTau_MuTau.root',
    'sample'         : 'DYJets_JetToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2716918./9669034,
    'iJson'          : -1
    } ,
    'DYJets_MuToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets_MuToTau_MuTau.root',
    'sample'         : 'DYJets_MuToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2716918./9669034,
    'iJson'          : -1
    } ,
    'DYJets1Jets_TauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets1Jets_TauTau_MuTau.root',
    'sample'         : 'DYJets1Jets_TauTau',
    'xSection'       : 561,
    'skimEff'        : 1.0 * 1.0 * 3814746./24045248 ,
    'iJson'          : -1
    } ,
    'DYJets1Jets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets1Jets_JetToTau_MuTau.root',
    'sample'         : 'DYJets1Jets_JetToTau',
    'xSection'       : 561,
    'skimEff'        : 1.0 * 1.0 * 3814746./24045248 ,
    'iJson'          : -1
    } ,
    'DYJets1Jets_MuToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets1Jets_MuToTau_MuTau.root',
    'sample'         : 'DYJets1Jets_MuToTau',
    'xSection'       : 561,
    'skimEff'        : 1.0 * 1.0 * 3814746./24045248 ,
    'iJson'          : -1
    } ,
    'DYJets2Jets_TauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets2Jets_TauTau_MuTau.root',
    'sample'         : 'DYJets2Jets_TauTau',
    'xSection'       : 181,
    'skimEff'        : 1.0 * 1.0 * 6111403./21837156,
    'iJson'          : -1
    } ,
    'DYJets2Jets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets2Jets_JetToTau_MuTau.root',
    'sample'         : 'DYJets2Jets_JetToTau',
    'xSection'       : 181,
    'skimEff'        : 1.0 * 1.0 * 6111403./21852156,
    'iJson'          : -1
    } ,
    'DYJets2Jets_MuToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets2Jets_MuToTau_MuTau.root',
    'sample'         : 'DYJets2Jets_MuToTau',
    'xSection'       : 181,
    'skimEff'        : 1.0 * 1.0 * 6111403./21852156,
    'iJson'          : -1
    } ,
    'DYJets3Jets_TauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets3Jets_TauTau_MuTau.root',
    'sample'         : 'DYJets3Jets_TauTau',
    'xSection'       : 51.1,
    'skimEff'        : 1.0 * 1.0 * 4195616./10980445,
    'iJson'          : -1
    } ,
    'DYJets3Jets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets3Jets_JetToTau_MuTau.root',
    'sample'         : 'DYJets3Jets_JetToTau',
    'xSection'       : 51.1,
    'skimEff'        : 1.0 * 1.0 * 4195616./10980445,
    'iJson'          : -1
    } ,
    'DYJets3Jets_MuToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets3Jets_MuToTau_MuTau.root',
    'sample'         : 'DYJets3Jets_MuToTau',
    'xSection'       : 51.1,
    'skimEff'        : 1.0 * 1.0 * 4195616./10980445,
    'iJson'          : -1
    } ,
    'DYJets4Jets_TauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets4Jets_TauTau_MuTau.root',
    'sample'         : 'DYJets4Jets_TauTau',
    'xSection'       : 23,
    'skimEff'        : 1.0 * 1.0 * 3125999./6402827,
    'iJson'          : -1
    } ,
    'DYJets4Jets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets4Jets_JetToTau_MuTau.root',
    'sample'         : 'DYJets4Jets_JetToTau',
    'xSection'       : 23,
    'skimEff'        : 1.0 * 1.0 * 3125999./6402827,
    'iJson'          : -1
    } ,
    'DYJets4Jets_MoToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets4Jets_MuToTau_MuTau.root',
    'sample'         : 'DYJets4Jets_MuToTau',
    'xSection'       : 23,
    'skimEff'        : 1.0 * 1.0 * 3125999./6402827,
    'iJson'          : -1
    } ,
    'TTJets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/TTJets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleTTJets_MuTau.root',
    'sample'         : 'TTJets',
    'xSection'       : 225.5,
    'skimEff'        : 1.0 * 0.503752 * 2660824./3487856,
    'iJson'          : -1
    },
    'WJets-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WJets-madgraph-PUS10-p1_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets-p1_MuTau.root',
    'sample'         : 'WJets-p1',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.108752 * 533250./2000279,
    'iJson'          : -1
    },
    'WJets-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WJets-madgraph-PUS10-p2_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets-p2_MuTau.root',
    'sample'         : 'WJets-p2',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.108710 * 1674658./6273656,
    'iJson'          : -1
    },
    'WJets1Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W1Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets1Jets_MuTau.root',
    'sample'         : 'WJets1Jets',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 1850955./22581598 ,
    'iJson'          : -1
    },
    'WJets2Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets2Jets_MuTau.root',
    'sample'         : 'WJets2Jets',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 5867536./34044921,
    'iJson'          : -1
    },
    'WJets3Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets3Jets_MuTau.root',
    'sample'         : 'WJets3Jets',
    'xSection'       : 36257,
    'skimEff'        :  1.0 * 3829211./15219503,
    'iJson'          : -1
    },
    'WJets4Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets4Jets_MuTau.root',
    'sample'         : 'WJets4Jets',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 4542508./13382803,##
    'iJson'          : -1
    },
    'T-tW' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/T-tW-powheg-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v7/',
    'outputFileName' : OutDir+'nTupleT-tW_MuTau.root',
    'sample'         : 'T-tW',
    'xSection'       : 11.1,
    #'skimEff'        : 0.437682 * 160607./490658,
    'skimEff'        : 1.0 * 160607./490658,
    'iJson'          : -1
    },
    'Tbar-tW' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/Tbar-tW-powheg-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v7/',
    'outputFileName' : OutDir+'nTupleTbar-tW_MuTau.root',
    'sample'         : 'Tbar-tW',
    'xSection'       : 11.1,
    #'skimEff'        : 0.437851 * 0.668091,
    'skimEff'        : 1.0 * 161536./493460,
    'iJson'          : -1
    },
    'WWJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WWJetsTo2L2Nu-madgraph-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v7/',
    'outputFileName' : OutDir+'nTupleWWJetsTo2L2Nu_MuTau.root',
    'sample'         : 'WWJetsTo2L2Nu',
    'xSection'       : 5.824,
    'skimEff'        : 0.535690 * 0.410962,
    'iJson'          : -1
    },
    'WZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WZJetsTo2L2Q-madgraph-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v7/',
    'outputFileName' : OutDir+'nTupleWZJetsTo2L2Q_MuTau.root',
    'sample'         : 'WZJetsTo2L2Q',
    'xSection'       : 2.207,
    #'skimEff'        : 0.471732 * 0.658115,
    'skimEff'        : 1.0 * 1135312./3215990,
    'iJson'          : -1
    },
    'WZJetsTo3LNu' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/WZJetsTo3LNu-madgraph-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v7/',
    'outputFileName' : OutDir+'nTupleWZJetsTo3LNu_MuTau.root',
    'sample'         : 'WZJetsTo3LNu',
    'xSection'       : 1.058,
    'skimEff'        : 1.0 * 0.618550 * 0.458578,
    'iJson'          : -1
    },
    'ZZJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/ZZJetsTo2L2Nu-madgraph-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v7/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Nu_MuTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    #'skimEff'        : 0.468227 * 0.322101,
    'skimEff'        : 1.0 * 170171./954911,
    'iJson'          : -1
    },
    'ZZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/ZZJetsTo2L2Q-madgraph-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v7/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Q_MuTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    'skimEff'        : 0.533349 * 0.648352,
    'iJson'          : -1
    },
    'ZZJetsTo4L' : {
    'inputFilePath'  : WorkdirLoc+'Backgrounds/ZZJetsTo4L-madgraph-PUS10_MC_Bkg_HTT_06Mar2013_Trees_MuTau_v7/',
    'outputFileName' : OutDir+'nTupleZZJetsTo4L_MuTau.root',
    'sample'         : 'ZZJetsTo4L',
    'xSection'       : 0.181,
    'skimEff'        : 0.577947 * 0.457893,
    'iJson'          : -1
    },
##     ##Higgs MC
    'GGFH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-90_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH90_MuTau.root',
    'sample'         : 'GGFH90',
    'xSection'       : 36.80 ,
    'skimEff'        : 8.33E-02 * 0.127154 * 0.544527,
    'iJson'          : -1
    },
    'GGFH95' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-95_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH95_MuTau.root',
    'sample'         : 'GGFH95',
    'xSection'       : 33.19 ,
    'skimEff'        : 8.32E-02 * 0.139953 * 0.552977,
    'iJson'          : -1
    },
    'GGFH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-100_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH100_MuTau.root',
    'sample'         : 'GGFH100',
    'xSection'       : 30.12 ,
    'skimEff'        : 8.28E-02 * 0.153989 * 0.557847,
    'iJson'          : -1
    },
    'GGFH105' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-105_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH105_MuTau.root',
    'sample'         : 'GGFH105',
    'xSection'       : 27.39 ,
    'skimEff'        : 8.17E-02 * 0.166444 * 0.562923,
    'iJson'          : -1
    },
    'GGFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-110_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH110_MuTau.root',
    'sample'         : 'GGFH110',
    'xSection'       : 25.04 ,
    'skimEff'        : 7.95E-02 * 0.180104 * 0.570791,
    'iJson'          : -1
    },
    'GGFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-115_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH115_MuTau.root',
    'sample'         : 'GGFH115',
    'xSection'       : 22.96 ,
    'skimEff'        : 7.58E-02 * 0.193332 * 0.570261,
    'iJson'          : -1
    },
    'GGFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-120_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH120_MuTau.root',
    'sample'         : 'GGFH120',
    'xSection'       : 21.13 ,
    'skimEff'        : 7.04E-02 * 0.205738 * 0.572362,
    'iJson'          : -1
    },
    'GGFH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-125_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH125_MuTau.root',
    'sample'         : 'GGFH125',
    'xSection'       : 19.52,
    'skimEff'        : 6.32E-02 * 0.218151 * 0.5758,
    'iJson'          : -1
    },
    'GGFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-130_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH130_MuTau.root',
    'sample'         : 'GGFH130',
    'xSection'       : 18.07 ,
    'skimEff'        : 5.45E-02 * 0.205738 * 0.577178,
    'iJson'          : -1
    },
    'GGFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-135_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH135_MuTau.root',
    'sample'         : 'GGFH135',
    'xSection'       : 16.79,
    'skimEff'        : 4.49E-02 * 0.242698 * 0.577621,
    'iJson'          : -1
    },
    'GGFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-140_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH140_MuTau.root',
    'sample'         : 'GGFH140',
    'xSection'       : 15.63,
    'skimEff'        : 3.52E-02 * 0.254905 * 0.580929,
    'iJson'          : -1
    },
    'GGFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-145_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH145_MuTau.root',
    'sample'         : 'GGFH145',
    'xSection'       : 14.59,
    'skimEff'        : 2.61E-02 * 0.266176 * 0.581166,
    'iJson'          : -1
    },
    'GGFH150' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-150_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH150_MuTau.root',
    'sample'         : 'GGFH150',
    'xSection'       : 13.65,
    'skimEff'        : 1.79E-02 * 0.276603 * 0.581997,
    'iJson'          : -1
    },
    'GGFH155' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-155_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH155_MuTau.root',
    'sample'         : 'GGFH155',
    'xSection'       : 12.79,
    'skimEff'        : 1.05E-02 * 0.286868 * 0.581456,
    'iJson'          : -1
    },
    'GGFH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-160_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleGGFH160_MuTau.root',
    'sample'         : 'GGFH160',
    'xSection'       : 11.95,
    'skimEff'        : 3.96E-03 * 0.297555 * 0.584683,
    'iJson'          : -1
    },
    'VBFH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-90_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH90_MuTau.root',
    'sample'         : 'VBFH90',
    'xSection'       : 2.191 ,
    'skimEff'        : 8.33E-02 * 0.288851 * 0.594209,
    'iJson'          : -1
    },
    'VBFH95' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-95_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH95_MuTau.root',
    'sample'         : 'VBFH95',
    'xSection'       : 2.084 ,
    'skimEff'        : 8.32E-02 * 0.298869 * 0.595243,
    'iJson'          : -1
    },
    'VBFH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-100_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH100_MuTau.root',
    'sample'         : 'VBFH100',
    'xSection'       : 1.988,
    'skimEff'        : 8.28E-02 * 0.308857 * 0.599498,
    'iJson'          : -1
    },
    'VBFH105' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-105_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH105_MuTau.root',
    'sample'         : 'VBFH105',
    'xSection'       : 1.897,
    'skimEff'        : 8.17E-02 * 0.318593 * 0.600782,
    'iJson'          : -1
    },
    'VBFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-110_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH110_MuTau.root',
    'sample'         : 'VBFH110',
    'xSection'       : 1.809,
    'skimEff'        : 7.95E-02 * 0.328027 * 0.605789,
    'iJson'          : -1
    },
    'VBFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-115_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH115_MuTau.root',
    'sample'         : 'VBFH115',
    'xSection'       : 1.729,
    'skimEff'        : 7.58E-02 * 0.336526 * 0.60583,
    'iJson'          : -1
    },
    'VBFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-120_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH120_MuTau.root',
    'sample'         : 'VBFH120',
    'xSection'       : 1.649,
    'skimEff'        : 7.04E-02 * 0.346365 * 0.607081,
    'iJson'          : -1
    },
    'VBFH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-125_MC_H_HTT_09Oct2013_Trees_MuTau_v1/',
    'outputFileName' : OutDir+'nTupleVBFH125_MuTau.root',
    'sample'         : 'VBFH125',
    'xSection'       : 1.578,
    'skimEff'        : 6.32E-02 * 0.356445 * 0.607902,
    'iJson'          : -1
    },
    'VBFH125-NoRecoil' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-125_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH125-NoRecoil_MuTau.root',
    'sample'         : 'VBFH125',
    'xSection'       : 1.578,
    'skimEff'        : 6.32E-02 * 0.356445 * 0.607902,
    'iJson'          : -1
    },
    'VBFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-130_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH130_MuTau.root',
    'sample'         : 'VBFH130',
    'xSection'       : 1.511,
    'skimEff'        : 5.45E-02 * 0.364656 * 0.608692,
    'iJson'          : -1
    },
    'VBFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-135_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH135_MuTau.root',
    'sample'         : 'VBFH135',
    'xSection'       : 1.448,
    'skimEff'        : 4.49E-02 * 0.372054 * 0.609374,
    'iJson'          : -1
    },
    'VBFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-140_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH140_MuTau.root',
    'sample'         : 'VBFH140',
    'xSection'       : 1.389,
    'skimEff'        : 3.52E-02 * 0.379645 * 0.611487,
    'iJson'          : -1
    },
    'VBFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-145_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH145_MuTau.root',
    'sample'         : 'VBFH145',
    'xSection'       : 1.333,
    'skimEff'        : 2.61E-02 * 0.388766 * 0.615292,
    'iJson'          : -1
    },
    'VBFH150' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-150_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH150_MuTau.root',
    'sample'         : 'VBFH150',
    'xSection'       : 1.280 ,
    'skimEff'        : 1.79E-02 * 0.397113 * 0.61599,
    'iJson'          : -1
    },
    'VBFH155' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-155_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH155_MuTau.root',
    'sample'         : 'VBFH155',
    'xSection'       : 1.231 ,
    'skimEff'        : 1.05E-02 * 0.402992 * 0.615836,
    'iJson'          : -1
    },
    'VBFH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-160_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVBFH160_MuTau.root',
    'sample'         : 'VBFH160',
    'xSection'       : 1.185 ,
    'skimEff'        : 3.96E-03 * 0.410462 * 0.617334,
    'iJson'          : -1
    },
    'VH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-90_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH90_MuTau.root',
    'sample'         : 'VH90',
    'xSection'       : (1.972 + 1.057 + 0.3233) ,
    'skimEff'        : 8.33E-02 * 0.3508 * 0.654786,
    'iJson'          : -1
    },
    'VH95' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-95_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH95_MuTau.root',
    'sample'         : 'VH95',
    'xSection'       : (1.676 + 0.9060 + 0.2812) ,
    'skimEff'        : 8.32E-02 * 0.367755 * 0.659576,
    'iJson'          : -1
    },
    'VH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-100_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH100_MuTau.root',
    'sample'         : 'VH100',
    'xSection'       : (1.432 + 0.7807 + 0.2454) ,
    'skimEff'        : 8.28E-02 * 0.3795 * 0.662,
    'iJson'          : -1
    },
    'VH105' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-105_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH105_MuTau.root',
    'sample'         : 'VH105',
    'xSection'       : (1.229 + 0.6750 + 0.2149) ,
    'skimEff'        : 8.17E-02 * 0.391685 * 0.665571,
    'iJson'          : -1
    },
    'VH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-110_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH110_MuTau.root',
    'sample'         : 'VH110',
    'xSection'       : (1.060+0.5869+0.1887) ,
    'skimEff'        : 7.95E-02 * 0.408058 * 0.666582,
    'iJson'          : -1
    },
    'VH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-115_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH115_MuTau.root',
    'sample'         : 'VH115',
    'xSection'       : (0.9165+0.5117+0.1663),
    'skimEff'        : 7.58E-02 * 0.419465 * 0.669919,
    'iJson'          : -1
    },
    'VH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-120_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH120_MuTau.root',
    'sample'         : 'VH120',
    'xSection'       : (0.7966+0.4483+0.1470),
    'skimEff'        : 7.04E-02 * 0.432465 * 0.67291,
    'iJson'          : -1
    },
    'VH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-125_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH125_MuTau.root',
    'sample'         : 'VH125',
    'xSection'       : (0.6966+0.3943+0.1302),
    'skimEff'        : 6.32E-02 * 0.444300 * 0.677099,
    'iJson'          : -1
    },
    'VH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-130_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH130_MuTau.root',
    'sample'         : 'VH130',
    'xSection'       : (0.6095+0.3473+0.1157),
    'skimEff'        : 5.45E-02 * 0.455723 * 0.676361,
    'iJson'          : -1
    },
    'VH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-135_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH135_MuTau.root',
    'sample'         : 'VH135',
    'xSection'       : (0.5351+0.3074+0.1031),
    'skimEff'        : 4.49E-02 * 0.466761 * 0.678825,
    'iJson'          : -1
    },
    'VH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-140_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH140_MuTau.root',
    'sample'         : 'VH140',
    'xSection'       : (0.4713+0.2728+0.09207),
    'skimEff'        : 3.52E-02 * 0.47803 * 0.680524,
    'iJson'          : -1
    },
    'VH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-145_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH145_MuTau.root',
    'sample'         : 'VH145',
    'xSection'       : (0.4164+0.2424+0.08246),
    'skimEff'        : 2.61E-02 * 0.486596 * 0.68425,
    'iJson'          : -1
    },
    'VH150' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-150_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH150_MuTau.root',
    'sample'         : 'VH150',
    'xSection'       : (0.3681 + 0.2159 + 0.07403),
    'skimEff'        : 1.79E-02 * 0.497987 * 0.683409,
    'iJson'          : -1
    },
    'VH155' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-155_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH155_MuTau.root',
    'sample'         : 'VH155',
    'xSection'       : (0.3252 + 0.1923 + 0.06664),
    'skimEff'        : 1.05E-02 * 0.507346 * 0.686742,
    'iJson'          : -1
    },
    'VH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-160_MC_H_HTT_06Mar2013_Trees_MuTau_v5/',
    'outputFileName' : OutDir+'nTupleVH160_MuTau.root',
    'sample'         : 'VH160',
    'xSection'       : (0.2817 + 0.1687 + 0.06013),
    'skimEff'        : 3.96E-03 * 0.515163 * 0.686238,
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
    for analysis in [ 'nominal', 'TauUp', 'TauDown','JetUp','JetDown']: 
        if  re.search("Data",sample)!=None and analysis != 'nominal':
            continue
        configOptions = option.copy()
        configOptions['analysis'] = analysis
        if  re.search("Data",sample)==None :
            configOptions['outputFileName'] = configOptions['outputFileName'].replace('.root', '_%s.root' % analysis)
        configFileName = "treeSkimmerMuTau_Winter13_%s_%s_cfg.py" % (sample,analysis)
        configFileName_full = os.path.join(submissionDirectory, configFileName)
        configFile = open(configFileName_full, 'w')
        configConfig = config_template.substitute(configOptions)
        configFile.write(configConfig)
        configFile.close()
