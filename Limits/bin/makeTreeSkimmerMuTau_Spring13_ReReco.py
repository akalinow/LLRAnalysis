#!/usr/bin/env python

import os
import re
import shlex
import string
import subprocess

WorkdirLoc       = '/data_CMS/cms/htautau/PostMoriond/TREES_NewRecoil/MuTau/'
OutDir           = '/data_CMS/cms/htautau/PostMoriond/NTUPLES_NewRecoil/MuTau/'

WorkdirLocNewPat = '/data_CMS/cms/htautau/PostMoriond/TREES_NewNadir/MuTau/'
OutDirNewPat     = '/data_CMS/cms/htautau/PostMoriond/NTUPLES_NewNadir/MuTau/'

options = {
    ###mu+tau samples

    ##DATA
    'Run2012A-22Jan2013-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012A_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4/',
    'outputFileName' : OutDir+'nTupleRun2012A-22Jan2013-Data_MuTau.root',
    'sample'         : 'Run2012A-22Jan2013-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p1/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p1-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p2/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p2-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p3/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p3-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p4/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p4-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012B_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p5/',
    'outputFileName' : OutDir+'nTupleRun2012B-22Jan2013-p5-Data_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },   
    'Run2012C-22Jan2013-p0-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p0/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p0-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p0-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p1/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p1-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p2/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p2-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p3/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p3-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p4/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p4-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p5/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p5-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p6-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p6/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p6-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p7-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012C_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p7/',
    'outputFileName' : OutDir+'nTupleRun2012C-22Jan2013-p7-Data_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p1-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p1/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p1-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p1-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p2-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p2/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p2-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p2-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p3-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p3/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p3-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p3-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p4-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p4/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p4-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p4-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p5-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p5/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p5-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p5-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p6-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p6/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p6-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p6-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p7-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p7/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p7-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p7-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p8-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p8/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p8-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p8-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p9-Data' : {
    'inputFilePath'  : WorkdirLoc+'Data/Data_2012D_ReReco22Jan_HTT_06Mar2013_Trees_MuTau_v4_p9/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p9-Data_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p9-Data',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    ##Embedded
    'Run2012A-22Jan2013-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012A_ReReco22Jan_HTT_06Mar13_Trees_v10/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012A-22Jan2013-Embedded_MuTau.root',
    'sample'         : 'Run2012A-22Jan2013-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012B_ReReco22Jan_HTT_06Mar13_Trees_v10_p1/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012B-22Jan2013-p1-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012B_ReReco22Jan_HTT_06Mar13_Trees_v10_p2/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012B-22Jan2013-p2-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012B_ReReco22Jan_HTT_06Mar13_Trees_v10_p3/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012B-22Jan2013-p3-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012B_ReReco22Jan_HTT_06Mar13_Trees_v10_p4/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012B-22Jan2013-p4-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012B-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012B_ReReco22Jan_HTT_06Mar13_Trees_v10_p5/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012B-22Jan2013-p5-Embedded_MuTau.root',
    'sample'         : 'Run2012B-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p0-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012C_ReReco22Jan_HTT_06Mar13_Trees_v10_p0/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012C-22Jan2013-p0-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p0-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012C_ReReco22Jan_HTT_06Mar13_Trees_v10_p1/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012C-22Jan2013-p1-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012C_ReReco22Jan_HTT_06Mar13_Trees_v10_p2/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012C-22Jan2013-p2-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012C_ReReco22Jan_HTT_06Mar13_Trees_v10_p3/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012C-22Jan2013-p3-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012C_ReReco22Jan_HTT_06Mar13_Trees_v10_p4/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012C-22Jan2013-p4-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012C_ReReco22Jan_HTT_06Mar13_Trees_v10_p5/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012C-22Jan2013-p5-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p6-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012C_ReReco22Jan_HTT_06Mar13_Trees_v10_p6/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012C-22Jan2013-p6-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012C-22Jan2013-p7-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012C_ReReco22Jan_HTT_06Mar13_Trees_v10_p7/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012C-22Jan2013-p7-Embedded_MuTau.root',
    'sample'         : 'Run2012C-22Jan2013-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p1-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v10_p1/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012D-22Jan2013-p1-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p1-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p2-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v10_p2/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012D-22Jan2013-p2-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p2-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p3-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v10_p3/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012D-22Jan2013-p3-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p3-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p4-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v10_p4/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012D-22Jan2013-p4-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p4-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p5-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v10_p5/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012D-22Jan2013-p5-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p5-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p6-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v10_p6/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012D-22Jan2013-p6-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p6-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p7-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v10_p7/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012D-22Jan2013-p7-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p7-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p8-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v10_p8/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012D-22Jan2013-p8-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p8-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p9-Embedded' : {
    'inputFilePath'  : WorkdirLocNewPat+'Embedded/Embed_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v10_p9/',
    'outputFileName' : OutDirNewPat+'nTupleRun2012D-22Jan2013-p9-Embedded_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p9-Embedded',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p1-EmbeddedLowPt' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/EmbedLowPt_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v8_p1/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p1-EmbeddedLowPt_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p1-EmbeddedLowPt',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p2-EmbeddedLowPt' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/EmbedLowPt_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v8_p2/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p2-EmbeddedLowPt_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p2-EmbeddedLowPt',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p3-EmbeddedLowPt' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/EmbedLowPt_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v8_p3/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p3-EmbeddedLowPt_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p3-EmbeddedLowPt',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p4-EmbeddedLowPt' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/EmbedLowPt_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v8_p4/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p4-EmbeddedLowPt_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p4-EmbeddedLowPt',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p5-EmbeddedLowPt' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/EmbedLowPt_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v8_p5/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p5-EmbeddedLowPt_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p5-EmbeddedLowPt',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p6-EmbeddedLowPt' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/EmbedLowPt_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v8_p6/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p6-EmbeddedLowPt_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p6-EmbeddedLowPt',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p7-EmbeddedLowPt' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/EmbedLowPt_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v8_p7/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p7-EmbeddedLowPt_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p7-EmbeddedLowPt',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p8-EmbeddedLowPt' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/EmbedLowPt_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v8_p8/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p8-EmbeddedLowPt_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p8-EmbeddedLowPt',
    'xSection'       : 0,
    'skimEff'        : 0,
    'iJson'          : 7
    },
    'Run2012D-22Jan2013-p9-EmbeddedLowPt' : {
    'inputFilePath'  : WorkdirLoc+'Embedded/EmbedLowPt_mutau_2012D_ReReco22Jan_HTT_06Mar13_Trees_v8_p9/',
    'outputFileName' : OutDir+'nTupleRun2012D-22Jan2013-p9-EmbeddedLowPt_MuTau.root',
    'sample'         : 'Run2012D-22Jan2013-p9-EmbeddedLowPt',
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
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1
    } ,
    'DYJets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets_JetToTau_MuTau.root',
    'sample'         : 'DYJets_JetToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1
    } ,
    'DYJets_MuToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DYJets-50-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets_MuToTau_MuTau.root',
    'sample'         : 'DYJets_MuToTau',
    'xSection'       : 3504,
    'skimEff'        : 1.0 * 0.317439 * 2474447./9669034,
    'iJson'          : -1
    } ,
    'DYJets1Jets_TauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets1Jets_TauTau_MuTau.root',
    'sample'         : 'DYJets1Jets_TauTau',
    'xSection'       : 561,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1
    } ,
    'DYJets1Jets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets1Jets_JetToTau_MuTau.root',
    'sample'         : 'DYJets1Jets_JetToTau',
    'xSection'       : 561,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1
    } ,
    'DYJets1Jets_MuToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY1Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets1Jets_MuToTau_MuTau.root',
    'sample'         : 'DYJets1Jets_MuToTau',
    'xSection'       : 561,
    'skimEff'        : 1.0 * 1.0 * 3308290./24045248 ,
    'iJson'          : -1
    } ,
    'DYJets2Jets_TauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets2Jets_TauTau_MuTau.root',
    'sample'         : 'DYJets2Jets_TauTau',
    'xSection'       : 181,
    'skimEff'        : 1.0 * 1.0 * 5354820./21852156,
    'iJson'          : -1
    } ,
    'DYJets2Jets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets2Jets_JetToTau_MuTau.root',
    'sample'         : 'DYJets2Jets_JetToTau',
    'xSection'       : 181,
    'skimEff'        : 1.0 * 1.0 * 5354820./21852156,
    'iJson'          : -1
    } ,
    'DYJets2Jets_MuToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY2Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets2Jets_MuToTau_MuTau.root',
    'sample'         : 'DYJets2Jets_MuToTau',
    'xSection'       : 181,
    'skimEff'        : 1.0 * 1.0 * 5354820./21852156,
    'iJson'          : -1
    } ,
    'DYJets3Jets_TauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets3Jets_TauTau_MuTau.root',
    'sample'         : 'DYJets3Jets_TauTau',
    'xSection'       : 51.1,
    'skimEff'        : 1.0 * 1.0 * 3736521./10980445,
    'iJson'          : -1
    } ,
    'DYJets3Jets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets3Jets_JetToTau_MuTau.root',
    'sample'         : 'DYJets3Jets_JetToTau',
    'xSection'       : 51.1,
    'skimEff'        : 1.0 * 1.0 * 3736521./10980445,
    'iJson'          : -1
    } ,
    'DYJets3Jets_MuToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY3Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets3Jets_MuToTau_MuTau.root',
    'sample'         : 'DYJets3Jets_MuToTau',
    'xSection'       : 51.1,
    'skimEff'        : 1.0 * 1.0 * 3736521./10980445,
    'iJson'          : -1
    } ,
    'DYJets4Jets_TauTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets4Jets_TauTau_MuTau.root',
    'sample'         : 'DYJets4Jets_TauTau',
    'xSection'       : 23,
    'skimEff'        : 1.0 * 1.0 * 2859057./6402827,
    'iJson'          : -1
    } ,
    'DYJets4Jets_JetToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets4Jets_JetToTau_MuTau.root',
    'sample'         : 'DYJets4Jets_JetToTau',
    'xSection'       : 23,
    'skimEff'        : 1.0 * 1.0 * 2859057./6402827,
    'iJson'          : -1
    } ,
    'DYJets4Jets_MoToTau' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/DY4Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleDYJets4Jets_MuToTau_MuTau.root',
    'sample'         : 'DYJets4Jets_MuToTau',
    'xSection'       : 23,
    'skimEff'        : 1.0 * 1.0 * 2859057./6402827,
    'iJson'          : -1
    } ,
    'TTJets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/TTJets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleTTJets_MuTau.root',
    'sample'         : 'TTJets',
    'xSection'       : 225.5,
    'skimEff'        : 1.0 * 0.503752 * 2553164./3487856,
    'iJson'          : -1
    },
    'WJets-p1' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WJets-madgraph-PUS10-p1_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets-p1_MuTau.root',
    'sample'         : 'WJets-p1',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.108752 * 450650./2000279,
    'iJson'          : -1
    },
    'WJets-p2' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WJets-madgraph-PUS10-p2_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets-p2_MuTau.root',
    'sample'         : 'WJets-p2',
    'xSection'       : 36257,
    'skimEff'        : 1.0 * 0.108710 * 1415230./6273656,
    'iJson'          : -1
    },
    'WJets1Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W1Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets1Jets_MuTau.root',
    'sample'         : 'WJets1Jets',
    'xSection'       : 36257,
    ##'skimEff'        :  1.0 * 1490832./23141598,##
    ##'skimEff'        :  1.0 * 1477081./22441598,
    'skimEff'        :  1.0 * 1486190./22581598 ,
    'iJson'          : -1
    },
    'WJets2Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W2Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets2Jets_MuTau.root',
    'sample'         : 'WJets2Jets',
    'xSection'       : 36257,
    ##'skimEff'        : 1.0 * 1034822./34044921,##
    ##'skimEff'        : 1.0 * 1477081./22441598,
    'skimEff'        : 1.0 * 5021879./34044921,
    'iJson'          : -1
    },
    'WJets3Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W3Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets3Jets_MuTau.root',
    'sample'         : 'WJets3Jets',
    'xSection'       : 36257,
    ##'skimEff'        :  1.0 * 3353306./15539503,##
    ##'skimEff'        :  1.0 * 3309349./14939503,
    'skimEff'        :  1.0 * 3371272./15219503,
    'iJson'          : -1
    },
    'WJets4Jets' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/W4Jets-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWJets4Jets_MuTau.root',
    'sample'         : 'WJets4Jets',
    'xSection'       : 36257,
    ##'skimEff'        : 1.0 * 3993529./13382803,##
    ##'skimEff'        : 1.0 * 2836319./9224356,##
    'skimEff'        : 1.0 * 4114405./13382803,##
    'iJson'          : -1
    },
    'T-tW' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/T-tW-powheg-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleT-tW_MuTau.root',
    'sample'         : 'T-tW',
    'xSection'       : 11.1,
    'skimEff'        : 0.437682 * 150203./217816,
    'iJson'          : -1
    },
    'Tbar-tW' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/Tbar-tW-powheg-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleTbar-tW_MuTau.root',
    'sample'         : 'Tbar-tW',
    'xSection'       : 11.1,
    'skimEff'        : 0.437851 * 148847./216062,
    'iJson'          : -1
    },
    'WWJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WWJetsTo2L2Nu-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWWJetsTo2L2Nu_MuTau.root',
    'sample'         : 'WWJetsTo2L2Nu',
    'xSection'       : 5.824,
    'skimEff'        : 0.535690 * 423373./1014187,
    'iJson'          : -1
    },
    'WZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WZJetsTo2L2Q-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWZJetsTo2L2Q_MuTau.root',
    'sample'         : 'WZJetsTo2L2Q',
    'xSection'       : 2.207,
    'skimEff'        : 0.471732 * 1011659./1517085,
    'iJson'          : -1
    },
    'WZJetsTo3LNu' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/WZJetsTo3LNu-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleWZJetsTo3LNu_MuTau.root',
    'sample'         : 'WZJetsTo3LNu',
    'xSection'       : 1.058,
    'skimEff'        : 0.618550 * 609046./1248321,
    'iJson'          : -1
    },
    'ZZJetsTo2L2Nu' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/ZZJetsTo2L2Nu-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Nu_MuTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    'skimEff'        : 0.468227 * 144262./437751,
    'iJson'          : -1
    },
    'ZZJetsTo2L2Q' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/ZZJetsTo2L2Q-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleZZJetsTo2L2Q_MuTau.root',
    'sample'         : 'ZZJetsTo2L2Q',
    'xSection'       : 2.502,
    'skimEff'        : 0.533349 * 673294./1025952,
    'iJson'          : -1
    },
    'ZZJetsTo4L' : {
    'inputFilePath'  : WorkdirLoc+'BackgroundsMC/ZZJetsTo4L-madgraph-PUS10_MC_Bkg_HTT_10May2013_Trees_MuTau_v3/',
    'outputFileName' : OutDir+'nTupleZZJetsTo4L_MuTau.root',
    'sample'         : 'ZZJetsTo4L',
    'xSection'       : 0.181,
    'skimEff'        : 0.577947 *  1028639./2225997,
    'iJson'          : -1
    },
    ##Higgs MC
    'GGFH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-90_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH90_MuTau.root',
    'sample'         : 'GGFH90',
    'xSection'       : 36.80 ,
    'skimEff'        : 8.33E-02 * 0.127154 * 70530./127141,
    'iJson'          : -1
    },
    'GGFH95' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-95_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH95_MuTau.root',
    'sample'         : 'GGFH95',
    'xSection'       : 33.19 ,
    'skimEff'        : 8.32E-02 * 0.139953 * 78879./139942,
    'iJson'          : -1
    },
    'GGFH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-100_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH100_MuTau.root',
    'sample'         : 'GGFH100',
    'xSection'       : 30.12 ,
    'skimEff'        : 8.28E-02 * 0.153989 * 87528./153974,
    'iJson'          : -1
    },
    'GGFH105' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-105_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH105_MuTau.root',
    'sample'         : 'GGFH105',
    'xSection'       : 27.39 ,
    'skimEff'        : 8.17E-02 * 0.166444 * 95701./166428,
    'iJson'          : -1
    },
    'GGFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-110_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH110_MuTau.root',
    'sample'         : 'GGFH110',
    'xSection'       : 25.04 ,
    'skimEff'        : 7.95E-02 * 0.180104 * 104781./180087,
    'iJson'          : -1
    },
    'GGFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-115_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH115_MuTau.root',
    'sample'         : 'GGFH115',
    'xSection'       : 22.96 ,
    'skimEff'        : 7.58E-02 * 0.193332 * 103342./177445,
    'iJson'          : -1
    },
    'GGFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-120_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH120_MuTau.root',
    'sample'         : 'GGFH120',
    'xSection'       : 21.13 ,
    'skimEff'        : 7.04E-02 * 0.205738 * 601400./1028601,
    'iJson'          : -1
    },
    'GGFH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-125_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH125_MuTau.root',
    'sample'         : 'GGFH125',
    'xSection'       : 19.52,
    'skimEff'        : 6.32E-02 * 0.218151 * 124232./211199,
    'iJson'          : -1
    },
    'GGFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-130_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH130_MuTau.root',
    'sample'         : 'GGFH130',
    'xSection'       : 18.07 ,
    'skimEff'        : 5.45E-02 * 0.205738 * 135906./230102,
    'iJson'          : -1
    },
    'GGFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-135_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH135_MuTau.root',
    'sample'         : 'GGFH135',
    'xSection'       : 16.79,
    'skimEff'        : 4.49E-02 * 0.242698 * 123500./208897,
    'iJson'          : -1
    },
    'GGFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-140_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH140_MuTau.root',
    'sample'         : 'GGFH140',
    'xSection'       : 15.63,
    'skimEff'        : 3.52E-02 * 0.254905 * 151488./254888,
    'iJson'          : -1
    },
    'GGFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-145_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH145_MuTau.root',
    'sample'         : 'GGFH145',
    'xSection'       : 14.59,
    'skimEff'        : 2.61E-02 * 0.266176 * 152486./256093,
    'iJson'          : -1
    },
    'GGFH150' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-150_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH150_MuTau.root',
    'sample'         : 'GGFH150',
    'xSection'       : 13.65,
    'skimEff'        : 1.79E-02 * 0.276603 * 165126./276412,
    'iJson'          : -1
    },
    'GGFH155' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-155_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH155_MuTau.root',
    'sample'         : 'GGFH155',
    'xSection'       : 12.79,
    'skimEff'        : 1.05E-02 * 0.286868 * 155229./260422,
    'iJson'          : -1
    },
    'GGFH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/GluGluToHToTauTau_M-160_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleGGFH160_MuTau.root',
    'sample'         : 'GGFH160',
    'xSection'       : 11.95,
    'skimEff'        : 3.96E-03 * 0.297555 * 178456./297529,
    'iJson'          : -1
    },
    'VBFH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-90_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH90_MuTau.root',
    'sample'         : 'VBFH90',
    'xSection'       : 2.191 ,
    'skimEff'        : 8.33E-02 * 0.288851 * 176816./288419,
    'iJson'          : -1
    },
    'VBFH95' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-95_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH95_MuTau.root',
    'sample'         : 'VBFH95',
    'xSection'       : 2.084 ,
    'skimEff'        : 8.32E-02 * 0.298869 * 183431./298399,
    'iJson'          : -1
    },
    'VBFH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-100_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH100_MuTau.root',
    'sample'         : 'VBFH100',
    'xSection'       : 1.988,
    'skimEff'        : 8.28E-02 * 0.308857 * 190861./308342,
    'iJson'          : -1
    },
    'VBFH105' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-105_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH105_MuTau.root',
    'sample'         : 'VBFH105',
    'xSection'       : 1.897,
    'skimEff'        : 8.17E-02 * 0.318593 * 197395./318051,
    'iJson'          : -1
    },
    'VBFH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-110_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH110_MuTau.root',
    'sample'         : 'VBFH110',
    'xSection'       : 1.809,
    'skimEff'        : 7.95E-02 * 0.328027 * 102290./ 163812,
    'iJson'          : -1
    },
    'VBFH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-115_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH115_MuTau.root',
    'sample'         : 'VBFH115',
    'xSection'       : 1.729,
    'skimEff'        : 7.58E-02 * 0.336526 * 210279./336146,
    'iJson'          : -1
    },
    'VBFH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-120_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH120_MuTau.root',
    'sample'         : 'VBFH120',
    'xSection'       : 1.649,
    'skimEff'        : 7.04E-02 * 0.346365 * 1042932./1662432,
    'iJson'          : -1
    },
    'VBFH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-125_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVBFH125_MuTau.root',
    'sample'         : 'VBFH125',
    'xSection'       : 1.578,
    'skimEff'        : 6.32E-02 * 0.356445 * 223334./356030,
    'iJson'          : -1
    },
    'VBFH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-130_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH130_MuTau.root',
    'sample'         : 'VBFH130',
    'xSection'       : 1.511,
    'skimEff'        : 5.45E-02 * 0.364656 * 229034./364215 ,
    'iJson'          : -1
    },
    'VBFH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-135_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH135_MuTau.root',
    'sample'         : 'VBFH135',
    'xSection'       : 1.448,
    'skimEff'        : 4.49E-02 * 0.372054 * 234075./371614,
    'iJson'          : -1
    },
    'VBFH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-140_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH140_MuTau.root',
    'sample'         : 'VBFH140',
    'xSection'       : 1.389,
    'skimEff'        : 3.52E-02 * 0.379645 * 239700./379225,
    'iJson'          : -1
    },
    'VBFH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-145_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH145_MuTau.root',
    'sample'         : 'VBFH145',
    'xSection'       : 1.333,
    'skimEff'        : 2.61E-02 * 0.388766 * 246840./388332,
    'iJson'          : -1
    },
    'VBFH150' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-150_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH150_MuTau.root',
    'sample'         : 'VBFH150',
    'xSection'       : 1.280 ,
    'skimEff'        : 1.79E-02 * 0.397113 * 252492./396316,
    'iJson'          : -1
    },
    'VBFH155' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-155_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH155_MuTau.root',
    'sample'         : 'VBFH155',
    'xSection'       : 1.231 ,
    'skimEff'        : 1.05E-02 * 0.402992 * 128195./201281,
    'iJson'          : -1
    },
    'VBFH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/VBF_HToTauTau_M-160_MC_H_HTT_30May2013_Trees_MuTau_v2',
    'outputFileName' : OutDir+'nTupleVBFH160_MuTau.root',
    'sample'         : 'VBFH160',
    'xSection'       : 1.185 ,
    'skimEff'        : 3.96E-03 * 0.410462 * 130961./204975,
    'iJson'          : -1
    },
    'VH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-90_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH90_MuTau.root',
    'sample'         : 'VH90',
    'xSection'       : (1.972 + 1.057 + 0.3233) ,
    'skimEff'        : 8.33E-02 * 0.3508 * 47117./70160,
    'iJson'          : -1
    },
    'VH95' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-95_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH95_MuTau.root',
    'sample'         : 'VH95',
    'xSection'       : (1.676 + 0.9060 + 0.2812) ,
    'skimEff'        : 8.32E-02 * 0.367755 * 49776./73551,
    'iJson'          : -1
    },
    'VH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-100_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH100_MuTau.root',
    'sample'         : 'VH100',
    'xSection'       : (1.432 + 0.7807 + 0.2454) ,
    'skimEff'        : 8.28E-02 * 0.3795 * 51501./75900,
    'iJson'          : -1
    },
    'VH105' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-105_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH105_MuTau.root',
    'sample'         : 'VH105',
    'xSection'       : (1.229 + 0.6750 + 0.2149) ,
    'skimEff'        : 8.17E-02 * 0.391685 * 53618./78337,
    'iJson'          : -1
    },
    'VH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-110_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH110_MuTau.root',
    'sample'         : 'VH110',
    'xSection'       : (1.060+0.5869+0.1887) ,
    'skimEff'        : 7.95E-02 * 0.408058 * 55990./81778,
    'iJson'          : -1
    },
    'VH115' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-115_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH115_MuTau.root',
    'sample'         : 'VH115',
    'xSection'       : (0.9165+0.5117+0.1663),
    'skimEff'        : 7.58E-02 * 0.419465 * 57925./84086,
    'iJson'          : -1
    },
    'VH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-120_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH120_MuTau.root',
    'sample'         : 'VH120',
    'xSection'       : (0.7966+0.4483+0.1470),
    'skimEff'        : 7.04E-02 * 0.432465 * 59517./85884,
    'iJson'          : -1
    },
    'VH125' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-125_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH125_MuTau.root',
    'sample'         : 'VH125',
    'xSection'       : (0.6966+0.3943+0.1302),
    'skimEff'        : 6.32E-02 * 0.444300 * 61842./88915,
    'iJson'          : -1
    },
    'VH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-130_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH130_MuTau.root',
    'sample'         : 'VH130',
    'xSection'       : (0.6095+0.3473+0.1157),
    'skimEff'        : 5.45E-02 * 0.455723 * 63369./91181,
    'iJson'          : -1
    },
    'VH135' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-135_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH135_MuTau.root',
    'sample'         : 'VH135',
    'xSection'       : (0.5351+0.3074+0.1031),
    'skimEff'        : 4.49E-02 * 0.466761 * 65248./93412,
    'iJson'          : -1
    },
    'VH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-140_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH140_MuTau.root',
    'sample'         : 'VH140',
    'xSection'       : (0.4713+0.2728+0.09207),
    'skimEff'        : 3.52E-02 * 0.47803 * 66990./95695,
    'iJson'          : -1
    },
    'VH145' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-145_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH145_MuTau.root',
    'sample'         : 'VH145',
    'xSection'       : (0.4164+0.2424+0.08246),
    'skimEff'        : 2.61E-02 * 0.486596 * 68646./97506,
    'iJson'          : -1
    },
    'VH150' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-150_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH150_MuTau.root',
    'sample'         : 'VH150',
    'xSection'       : (0.3681 + 0.2159 + 0.07403),
    'skimEff'        : 1.79E-02 * 0.497987 * 70167./99839,
    'iJson'          : -1
    },
    'VH155' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-155_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH155_MuTau.root',
    'sample'         : 'VH155',
    'xSection'       : (0.3252 + 0.1923 + 0.06664),
    'skimEff'        : 1.05E-02 * 0.507346 * 71905./101664,
    'iJson'          : -1
    },
    'VH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsSM/WH_ZH_TTH_HToTauTau_M-160_MC_H_HTT_30May2013_Trees_MuTau_v2/',
    'outputFileName' : OutDir+'nTupleVH160_MuTau.root',
    'sample'         : 'VH160',
    'xSection'       : (0.2817 + 0.1687 + 0.06013),
    'skimEff'        : 3.96E-03 * 0.515163 * 72871./103115,
    'iJson'          : -1
    },
    ######MSSM
    'SUSYGGH80' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH80-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH80_MuTau.root',
    'sample'         : 'SUSYGGH80',
    'xSection'       : 1.0,
    'skimEff'        : 0.104221299534 * 55755./103014,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH90-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH90_MuTau.root',
    'sample'         : 'SUSYGGH90',
    'xSection'       : 1.0,
    'skimEff'        : 0.130604288499 * 73465./130650,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH100-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH100_MuTau.root',
    'sample'         : 'SUSYGGH100',
    'xSection'       : 1.0,
    'skimEff'        : 0.156415486761 * 90127./156428,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH110-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH110_MuTau.root',
    'sample'         : 'SUSYGGH110',
    'xSection'       : 1.0,
    'skimEff'        : 0.182624446693 * 106926./182688,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH120-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH120_MuTau.root',
    'sample'         : 'SUSYGGH120',
    'xSection'       : 1.0,
    'skimEff'        : 0.208174931542 * 123290./208304,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH130-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH130_MuTau.root',
    'sample'         : 'SUSYGGH130',
    'xSection'       : 1.0,
    'skimEff'        : 0.232187901671 * 137499./231222,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH140-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH140_MuTau.root',
    'sample'         : 'SUSYGGH140',
    'xSection'       : 1.0,
    'skimEff'        : 0.256235995519 * 153919./256154,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH160-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH160_MuTau.root',
    'sample'         : 'SUSYGGH160',
    'xSection'       : 1.0,
    'skimEff'        : 0.298341423948 * 178301./295000,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH180' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH180-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH180_MuTau.root',
    'sample'         : 'SUSYGGH180',
    'xSection'       : 1.0,
    'skimEff'        : 0.336519253911 * 204775./335577,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH200' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH200-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH200_MuTau.root',
    'sample'         : 'SUSYGGH200',
    'xSection'       : 1.0,
    'skimEff'        : 0.368996454854 * 222769./363777,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH250' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH250-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH250_MuTau.root',
    'sample'         : 'SUSYGGH250',
    'xSection'       : 1.0,
    'skimEff'        : 0.43499716625 * 268579./435189,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH300' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH300-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH300_MuTau.root',
    'sample'         : 'SUSYGGH300',
    'xSection'       : 1.0,
    'skimEff'        : 0.48091174761 * 296334./476572,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH350' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH350-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH350_MuTau.root',
    'sample'         : 'SUSYGGH350',
    'xSection'       : 1.0,
    'skimEff'        : 0.499360752503 * 312599./499558,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH400' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH400-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH400_MuTau.root',
    'sample'         : 'SUSYGGH400',
    'xSection'       : 1.0,
    'skimEff'        : 0.518683274021 * 326585./516538,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH450' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH450-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH450_MuTau.root',
    'sample'         : 'SUSYGGH450',
    'xSection'       : 1.0,
    'skimEff'        : 0.521099815897 * 331358./520808,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH500' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH500-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH500_MuTau.root',
    'sample'         : 'SUSYGGH500',
    'xSection'       : 1.0,
    'skimEff'        : 0.500466543342 * 314766./491302,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH600' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH600-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH600_MuTau.root',
    'sample'         : 'SUSYGGH600',
    'xSection'       : 1.0,
    'skimEff'        : 0.276240518363 * 174464./270803,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH700' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH700-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH700_MuTau.root',
    'sample'         : 'SUSYGGH700',
    'xSection'       : 1.0,
    'skimEff'        : 0.222982349361 * 142697./219816,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH800' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH800-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH800_MuTau.root',
    'sample'         : 'SUSYGGH800',
    'xSection'       : 1.0,
    'skimEff'        : 0.18059437019 * 115548./178355,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH900' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH900-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH900_MuTau.root',
    'sample'         : 'SUSYGGH900',
    'xSection'       : 1.0,
    'skimEff'        : 0.14921024367 * 94040./145591,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYGGH1000' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYGGH1000-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYGGH1000_MuTau.root',
    'sample'         : 'SUSYGGH1000',
    'xSection'       : 1.0,
    'skimEff'        : 0.126561286395 * 81354./126609,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH80' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH80-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH80_MuTau.root',
    'sample'         : 'SUSYBBH80',
    'xSection'       : 1.0,
    'skimEff'        : 0.0925855314913 * 47010./92270,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH90' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH90-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH90_MuTau.root',
    'sample'         : 'SUSYBBH90',
    'xSection'       : 1.0,
    'skimEff'        : 0.118757029242 * 63712./118263,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH100' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH100-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH100_MuTau.root',
    'sample'         : 'SUSYBBH100',
    'xSection'       : 1.0,
    'skimEff'        : 0.146602087332 * 81635./146649,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH110' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH110-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH110_MuTau.root',
    'sample'         : 'SUSYBBH110',
    'xSection'       : 1.0,
    'skimEff'        : 0.173093237295 * 98647./173024,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH120' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH120-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH120_MuTau.root',
    'sample'         : 'SUSYBBH120',
    'xSection'       : 1.0,
    'skimEff'        : 0.200630498844 * 114705./199008,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH130' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH130-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH130_MuTau.root',
    'sample'         : 'SUSYBBH130',
    'xSection'       : 1.0,
    'skimEff'        : 0.22481120151 * 131242./224813,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH140' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH140-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH140_MuTau.root',
    'sample'         : 'SUSYBBH140',
    'xSection'       : 1.0,
    'skimEff'        : 0.25027979066 * 146177./248678,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH160' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH160-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH160_MuTau.root',
    'sample'         : 'SUSYBBH160',
    'xSection'       : 1.0,
    'skimEff'        : 0.295152820513 * 170912./287774,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH180' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH180-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH180_MuTau.root',
    'sample'         : 'SUSYBBH180',
    'xSection'       : 1.0,
    'skimEff'        : 0.333586810489 * 195229./327256,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH200' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH200-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH200_MuTau.root',
    'sample'         : 'SUSYBBH200',
    'xSection'       : 1.0,
    'skimEff'        : 0.370115108144 * 221906./369896,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH250' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH250-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH250_MuTau.root',
    'sample'         : 'SUSYBBH250',
    'xSection'       : 1.0,
    'skimEff'        : 0.437450920866 * 262237./432289,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH300' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH300-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH300_MuTau.root',
    'sample'         : 'SUSYBBH300',
    'xSection'       : 1.0,
    'skimEff'        : 0.487159715972 * 297096./487111,
   'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH350' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH350-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH350_MuTau.root',
    'sample'         : 'SUSYBBH350',
    'xSection'       : 1.0,
    'skimEff'        : 0.524930759205 * 316045./515524,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH400' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH400-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH400_MuTau.root',
    'sample'         : 'SUSYBBH400',
    'xSection'       : 1.0,
    'skimEff'        : 0.554657946014 * 341667./553938,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH450' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH450-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH450_MuTau.root',
    'sample'         : 'SUSYBBH450',
    'xSection'       : 1.0,
    'skimEff'        : 0.578385344465 * 349676./563249,
   'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH500' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH500-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH500_MuTau.root',
    'sample'         : 'SUSYBBH500',
    'xSection'       : 1.0,
    'skimEff'        : 0.595235285647 * 372346./595221,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH600' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH600-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH600_MuTau.root',
    'sample'         : 'SUSYBBH600',
    'xSection'       : 1.0,
    'skimEff'        : 0.614847426291 * 387790./610581,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH700' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH700-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH700_MuTau.root',
    'sample'         : 'SUSYBBH700',
    'xSection'       : 1.0,
    'skimEff'        : 0.62296681913 * 399540./621073,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH800' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH800-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH800_MuTau.root',
    'sample'         : 'SUSYBBH800',
    'xSection'       : 1.0,
    'skimEff'        : 0.625660317922 * 402974./617674,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH900' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH900-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH900_MuTau.root',
    'sample'         : 'SUSYBBH900',
    'xSection'       : 1.0,
    'skimEff'        : 0.622572102711 * 409592./621124,
    'iJson'          : -1,
    'iDiv'           : 0,
    'nDiv'           : 1
    },
    'SUSYBBH1000' : {
    'inputFilePath'  : WorkdirLoc+'HiggsMSSM/SUSYBBH1000-pythia-PUS10_MuTau_Trees_v3/',
    'outputFileName' : OutDir+'nTupleSUSYBBH1000_MuTau.root',
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

process.treeSkimmerMuTau = cms.PSet(
    sample = cms.string('$sample'),
    analysis = cms.string('$analysis'),
    xSection = cms.double($xSection),
    skimEff = cms.double($skimEff),
    iJson = cms.int32($iJson)
)
''')

currentDirectory    = os.getcwd()
submissionDirectory = os.path.join(currentDirectory, "ConfigsReReco")

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
