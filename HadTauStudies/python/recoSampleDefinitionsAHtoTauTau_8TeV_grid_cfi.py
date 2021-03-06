import FWCore.ParameterSet.Config as cms

import copy

from LLRAnalysis.HadTauStudies.recoSampleDefaults_cfi import SAMPLE_DEFAULTS

# List of samples to run in the analysis
SAMPLES_TO_ANALYZE = [
    ##'data_Tau_Run2012A_22Jan2013_v1',
    'data_TauParked_Run2012B_22Jan2013_v1',
    'data_TauParked_Run2012C_22Jan2013_v1',
    'data_TauParked_Run2012D_22Jan2013_v1',
    ##'data_Jet_Run2012A_22Jan2013ReReco_v1',
    'data_JetHT_Run2012B_22Jan2013ReReco_v1',
    'data_JetHT_Run2012C_22Jan2013ReReco_v1',
    'data_JetHT_Run2012D_22Jan2013ReReco_v1',
    ##'pfEmbed_Run2012A_22Jan2013_v1',
    'pfEmbed_Run2012B_22Jan2013_v2',
    'pfEmbed_Run2012C_22Jan2013_v2',
    'pfEmbed_Run2012D_22Jan2013_v2',
    'pfEmbed_TTJetsFullLept_v2',
    'DYJets',
    'DYJets_noTauPolarization',
    'DY1Jets',
    'DY2Jets',
    'DY2JetsExt',
    'DY3Jets',
    'DY4Jets',
    'WJets',
    'WJetsExt',
    'W1Jets',
    'W1JetsExt',
    'W2Jets',
    'W2JetsExt',
    'W3Jets',
    'W3JetsExt',
    'W4Jets',
    'TTJetsHadronic',
    'TTJetsSemiLept',
    'TTJetsSemiLept_tauola',
    'TTJetsFullLept',
    'TTJetsFullLept_tauola',
    'Tbar_tW',
    'T_tW',
    'WWJetsTo2L2Nu',
    'WZJetsTo3LNu',
    'WZJetsTo2L2Q',
    'ZZJetsTo4L',
    'ZZJetsTo2L2Nu',
    'ZZJetsTo2L2Q'
]

_millibarns = 1.0e+9
_picobarns =  1.0
_femtobarns = 1.0e-3

TARGET_LUMI = 18.3/_femtobarns # CV: estimated luminosity of 2012 run A+B+C+D data

RECO_SAMPLES = {
    ##'data_Tau_Run2012A_22Jan2013_v1' : {
    ##    'datasetpath'      : '/Tau/Run2012A-22Jan2013-v1/AOD',
    ##    'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
    ##    'events_processed' : 4316637, # 2013/11/30
    ##    'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
    ##    'runselection'     : "190456-193621",
    ##    'conditions'       : 'FT_53_V6_AN2::All',
    ##    'lumis_per_job'    : 20,
    ##    'type'             : 'Data',
    ##    'trigger'          : 'Tau'
    ##},
    'data_TauParked_Run2012B_22Jan2013_v1' : {
        'datasetpath'      : '/TauParked/Run2012B-22Jan2013-v1/AOD',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 46187183, # 2013/11/30
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "193833-196531",
        'conditions'       : 'FT_53_V6_AN2::All',
        'lumis_per_job'    : 15,
        'type'             : 'Data',
        'trigger'          : 'Tau'
    },
    'data_TauParked_Run2012C_22Jan2013_v1' : {
        'datasetpath'      : '/TauParked/Run2012C-22Jan2013-v1/AOD',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 58371814, # 2013/11/30
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "198022-203742",
        'conditions'       : 'GR_P_V40::All',
        'lumis_per_job'    : 15,
        'type'             : 'Data',
        'trigger'          : 'Tau'
    },
    'data_TauParked_Run2012D_22Jan2013_v1' : {
        'datasetpath'      : '/TauParked/Run2012D-22Jan2013-v1/AOD',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 63220920, # 2013/11/30
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "203777-208686",
        'conditions'       : 'GR_P_V40::All',
        'lumis_per_job'    : 15,
        'type'             : 'Data',
        'trigger'          : 'Tau'
    },
    ##'data_Jet_Run2012A_22Jan2013ReReco_v1' : {
    ##    'datasetpath'      : '/Jet/Run2012A-22Jan2013-v1/AOD',
    ##    'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
    ##    'events_processed' : 9400333, # 2014/06/08
    ##    'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
    ##    'runselection'     : "190456-193621",
    ##    'conditions'       : 'FT_53_V6_AN2::All',
    ##    'lumis_per_job'    : 20,
    ##    'type'             : 'Data',
    ##    'trigger'          : 'Jet'
    ##},
    'data_JetHT_Run2012B_22Jan2013ReReco_v1' : {
        'datasetpath'      : '/JetHT/Run2012B-22Jan2013-v1/AOD',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 15513284, # 2014/06/08
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "193833-196531",
        'conditions'       : 'FT_53_V6_AN2::All',
        'lumis_per_job'    : 15,
        'type'             : 'Data',
        'trigger'          : 'Jet'
    },
    'data_JetHT_Run2012C_22Jan2013ReReco_v1' : {
        'datasetpath'      : '/JetHT/Run2012C-22Jan2013-v1/AOD',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 25789161, # 2014/06/08
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "198022-203742",
        'conditions'       : 'GR_P_V40::All',
        'lumis_per_job'    : 15,
        'type'             : 'Data',
        'trigger'          : 'Jet'
    },
    'data_JetHT_Run2012D_22Jan2013ReReco_v1' : {
        'datasetpath'      : '/JetHT/Run2012D-22Jan2013-v1/AOD',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 27572330, # 2014/06/08
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "203777-208686",
        'conditions'       : 'GR_P_V40::All',
        'lumis_per_job'    : 15,
        'type'             : 'Data',
        'trigger'          : 'Jet'
    },
    ##'pfEmbed_Run2012A_22Jan2013_v1' : {
    ##    'datasetpath'      : '/DoubleMu/StoreResults-Run2012A_22Jan2013_v1_PFembedded_trans1_tau132_pthad1_30had2_30_v1-5ef1c0fd428eb740081f19333520fdc8/USER',
    ##    'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
    ##    'events_processed' : 328647, # 2013/12/05
    ##    'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
    ##    'runselection'     : "190456-193621",
    ##    'conditions'       : 'FT_53_V6_AN2::All',
    ##    'lumis_per_job'    : 20,
    ##    'type'             : 'pfEmbeddedData'
    ##},
    'pfEmbed_Run2012B_22Jan2013_v1' : {
        'datasetpath'      : '/DoubleMuParked/StoreResults-Run2012B_22Jan2013_v1_PFembedded_trans1_tau132_pthad1_30had2_30_v1-5ef1c0fd428eb740081f19333520fdc8/USER',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 1545816, # 2013/12/05
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "193833-196531",
        'conditions'       : 'FT_53_V6_AN2::All',
        'lumis_per_job'    : 15,
        'type'             : 'pfEmbeddedData'
    },
    'pfEmbed_Run2012B_22Jan2013_v2' : {
        'datasetpath'      : '/DoubleMuParked/StoreResults-Run2012B_22Jan2013_v1_PFembedded_trans1_tau132_pthad1_30had2_30_v1-5ef1c0fd428eb740081f19333520fdc8/USER',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 1545816, # 2013/12/05
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "193833-196531",
        'conditions'       : 'FT_53_V6_AN2::All',
        'lumis_per_job'    : 15,
        'type'             : 'pfEmbeddedData'
    },
    'pfEmbed_Run2012C_22Jan2013_v1' : {
        'datasetpath'      : '/DoubleMuParked/StoreResults-Run2012C_22Jan2013_v1_PFembedded_trans1_tau132_pthad1_30had2_30_v1-5ef1c0fd428eb740081f19333520fdc8/USER',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 2358463, # 2013/12/05
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "198022-203742",
        'conditions'       : 'GR_P_V40::All',
        'lumis_per_job'    : 15,
        'type'             : 'pfEmbeddedData'
    },
    'pfEmbed_Run2012C_22Jan2013_v2' : {
        'datasetpath'      : '/DoubleMuParked/StoreResults-Run2012C_22Jan2013_v1_PFembedded_trans1_tau132_pthad1_30had2_30_v1-5ef1c0fd428eb740081f19333520fdc8/USER',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 2358463, # 2013/12/05
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "198022-203742",
        'conditions'       : 'GR_P_V40::All',
        'lumis_per_job'    : 15,
        'type'             : 'pfEmbeddedData'
    },
    'pfEmbed_Run2012D_22Jan2013_v1' : {
        'datasetpath'      : '/DoubleMuParked/StoreResults-Run2012D_22Jan2013_v1_PFembedded_trans1_tau132_pthad1_30had2_30_v1-5ef1c0fd428eb740081f19333520fdc8/USER',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 2460546, # 2013/12/05
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "203777-208686",
        'conditions'       : 'GR_P_V40::All',
        'lumis_per_job'    : 15,
        'type'             : 'pfEmbeddedData'
    },
    'pfEmbed_Run2012D_22Jan2013_v2' : {
        'datasetpath'      : '/DoubleMuParked/StoreResults-Run2012D_22Jan2013_v1_PFembedded_trans1_tau132_pthad1_30had2_30_v1-5ef1c0fd428eb740081f19333520fdc8/USER',
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",        
        'events_processed' : 2460546, # 2013/12/05
        'lumi_mask'        : "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt",
        'runselection'     : "203777-208686",
        'conditions'       : 'GR_P_V40::All',
        'lumis_per_job'    : 15,
        'type'             : 'pfEmbeddedData'
    },
    'pfEmbed_TTJetsFullLept' : {
        'datasetpath'      : "/TTJets_FullLeptMGDecays_8TeV-madgraph-tauola/StoreResults-Summer12_TTJets_FullLeptMGDecays_DR53X_PU_S10_START53_V7C_v2_PFembedded_trans1_tau132_pthad1_30had2_30_v1-5ef1c0fd428eb740081f19333520fdc8/USER",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 468406,
        'events_per_job'   : 20000,
        'x_sec'            : 241.5*_picobarns*0.1050, # CV: taken from TOP-14-016, no need to multiply by branching fraction for both taus to decay hadronically when using addBackgroundZTT2
        'type'             : 'pfEmbeddedMC'
    },
    'pfEmbed_TTJetsFullLept_v2' : {
        'datasetpath'      : "/TTJets_FullLeptMGDecays_8TeV-madgraph-tauola/StoreResults-Summer12_TTJets_FullLeptMGDecays_DR53X_PU_S10_START53_V7C_v2_PFembedded_trans1_tau132_pthad1_30had2_30_v1-5ef1c0fd428eb740081f19333520fdc8/USER",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 468406,
        'events_per_job'   : 20000,
        'x_sec'            : 241.5*_picobarns*0.1050, # CV: taken from TOP-14-016, no need to multiply by branching fraction for both taus to decay hadronically when using addBackgroundZTT2
        'type'             : 'pfEmbeddedMC'
    },
    'DYJets' : {
        'datasetpath'      : "/DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 30459503,
        'events_per_job'   : 40000,
        'x_sec'            : 3503.71*_picobarns, # Note: Madgraph samples are generated for M > 50 GeV      
        'type'             : 'smMC'
    },
    'DYJets_noTauPolarization' : {
        'datasetpath'      : "/DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball-tauola-tauPolarOff/Summer12_DR53X-PU_S10_START53_V19-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 29426492,
        'events_per_job'   : 40000,
        'x_sec'            : 3503.71*_picobarns, # Note: Madgraph samples are generated for M > 50 GeV      
        'type'             : 'smMC'
    },
    'DY1Jets' : {
        'datasetpath'      : "/DY1JetsToLL_M-50_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 24045248,
        'events_per_job'   : 40000,
        'x_sec'            : 666.3*_picobarns,
        'type'             : 'smMC'
    },
    'DY2Jets' : {
        'datasetpath'      : "/DY2JetsToLL_M-50_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 2352304,
        'events_per_job'   : 30000,
        'x_sec'            : 215.0*_picobarns,
        'type'             : 'smMC'
    },
    'DY2JetsExt' : {
        'datasetpath'      : "/DY2JetsToLL_M-50_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7C-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 21852156,
        'events_per_job'   : 30000,
        'x_sec'            : 215.0*_picobarns,
        'type'             : 'smMC'
    },
    'DY3Jets' : {
        'datasetpath'      : "/DY3JetsToLL_M-50_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 11015445,
        'events_per_job'   : 30000,
        'x_sec'            : 60.7*_picobarns,
        'type'             : 'smMC'
    },
    'DY4Jets' : {
        'datasetpath'      : "/DY4JetsToLL_M-50_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 6402827,
        'events_per_job'   : 30000,
        'x_sec'            : 27.3*_picobarns,
        'type'             : 'smMC'
    },        
    'WJets' : {
        'datasetpath'      : "/WJetsToLNu_TuneZ2Star_8TeV-madgraph-tarball/Summer12_DR53X-PU_S10_START53_V7A-v2/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 57709905,
        'events_per_job'   : 50000,
        'x_sec'            : 36257.2*_picobarns, # NLO cross-section @ 8 TeV,
                                                 # taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV
        'type'             : 'smMC'
    },
    'WJetsExt' : {
        'datasetpath'      : "/WJetsToLNu_TuneZ2Star_8TeV-madgraph-tarball/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 18393090,
        'events_per_job'   : 50000,
        'x_sec'            : 36257.2*_picobarns, # NLO cross-section @ 8 TeV,
                                                 # taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV
        'type'             : 'smMC'
    },
    'W1Jets' : {
        'datasetpath'      : "/W1JetsToLNu_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 23141598,
        'events_per_job'   : 40000,
        'x_sec'            : 6381.2*_picobarns,
        'type'             : 'smMC'
    },
    'W1JetsExt' : {
        'datasetpath'      : "/W1JetsToLNu_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V19-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 29784800,
        'events_per_job'   : 40000,
        'x_sec'            : 6381.2*_picobarns,
        'type'             : 'smMC'
    },
    'W2Jets' : {
        'datasetpath'      : "/W2JetsToLNu_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 34044921,
        'events_per_job'   : 40000,
        'x_sec'            : 2039.8*_picobarns,
        'type'             : 'smMC'
    },
    'W2JetsExt' : {
        'datasetpath'      : "/W2JetsToLNu_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V19-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 30693853,
        'events_per_job'   : 40000,
        'x_sec'            : 2039.8*_picobarns,
        'type'             : 'smMC'
    },
    'W3Jets' : {
        'datasetpath'      : "/W3JetsToLNu_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 15539503,
        'events_per_job'   : 30000,
        'x_sec'            : 612.5*_picobarns,
        'type'             : 'smMC'
    },
    'W3JetsExt' : {
        'datasetpath'      : "/W3JetsToLNu_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V19-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 15241144,
        'events_per_job'   : 30000,
        'x_sec'            : 612.5*_picobarns,
        'type'             : 'smMC'
    },
    'W4Jets' : {
        'datasetpath'      : "/W4JetsToLNu_TuneZ2Star_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 13382803,
        'events_per_job'   : 30000,
        'x_sec'            : 251.0*_picobarns,
        'type'             : 'smMC'
    },
    'TTJetsHadronic' : {
        'datasetpath'      : "/TTJets_HadronicMGDecays_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A_ext-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 31223821,
        'events_per_job'   : 20000,
        'x_sec'            : 241.5*_picobarns*0.4570, # CV: taken from TOP-14-016
        'type'             : 'smMC'
    },
    'TTJetsSemiLept' : {
        'datasetpath'      : "/TTJets_SemiLeptMGDecays_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A_ext-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 25424818,
        'events_per_job'   : 20000,
        'x_sec'            : 241.5*_picobarns*0.4380, # CV: taken from TOP-14-016
        'type'             : 'smMC'
    },
    'TTJetsSemiLept_tauola' : {
        'datasetpath'      : "/TTJets_SemiLeptMGDecays_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7C-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 24953451,
        'events_per_job'   : 20000,
        'x_sec'            : 241.5*_picobarns*0.4380, # CV: taken from TOP-14-016
        'type'             : 'smMC'
    },
    'TTJetsFullLept' : {
        'datasetpath'      : "/TTJets_FullLeptMGDecays_8TeV-madgraph/Summer12_DR53X-PU_S10_START53_V7A-v2/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 12119013,
        'events_per_job'   : 20000,
        'x_sec'            : 241.5*_picobarns*0.1050, # CV: taken from TOP-14-016
        'type'             : 'smMC'
    },
    'TTJetsFullLept_tauola' : {
        'datasetpath'      : "/TTJets_FullLeptMGDecays_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7C-v2/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 12011428,
        'events_per_job'   : 20000,
        'x_sec'            : 241.5*_picobarns*0.1050, # CV: taken from TOP-14-016
        'type'             : 'smMC'
    },
    'Tbar_tW' : {
        'datasetpath'      : "/Tbar_tW-channel-DR_TuneZ2star_8TeV-powheg-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 493460,
        'events_per_job'   : 20000,
        'x_sec'            : 11.1*_picobarns, # NLO cross-section @ 8 TeV,
                                              # taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV
        'type'             : 'smMC'
    },
    'T_tW' : {
        'datasetpath'      : "/T_tW-channel-DR_TuneZ2star_8TeV-powheg-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 497658,
        'events_per_job'   : 20000,
        'x_sec'            : 11.1*_picobarns, # NLO cross-section @ 8 TeV,
                                              # taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV
        'type'             : 'smMC'
    },
    'WWJetsTo2L2Nu' : {
        'datasetpath'      : "/WWJetsTo2L2Nu_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          :  "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 1933235,
        'x_sec'            : 5.8*_picobarns, # CV: taken from AN-13/171
        'type'             : 'smMC'
    },    
    'WZJetsTo3LNu' : {
        'datasetpath'      : "/WZJetsTo3LNu_TuneZ2_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          :  "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 2017979,
        'events_per_job'   : 20000,
        'x_sec'            : 1.1*_picobarns, # CV: taken from AN-13/171
        'type'             : 'smMC'
    },
    'WZJetsTo2L2Q' : {
        'datasetpath'      : "/WZJetsTo2L2Q_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          :  "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 3215990,
        'events_per_job'   : 20000,
        'x_sec'            : 2.2*_picobarns, # CV: taken from AN-13/171
        'type'             : 'smMC'
    },    
    'ZZJetsTo4L' : {
        'datasetpath'      : "/ZZJetsTo4L_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          :  "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 4807893,
        'events_per_job'   : 20000,
        'x_sec'            : 0.2*_picobarns, # CV: taken from AN-13/171
        'type'             : 'smMC'
    },
    'ZZJetsTo2L2Nu' : {
        'datasetpath'      : "/ZZJetsTo2L2Nu_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v3/AODSIM",
        'dbs_url'          :  "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 954911,
        'events_per_job'   : 20000,
        'x_sec'            : 0.7*_picobarns, # CV: taken from AN-13/171
        'type'             : 'smMC'
    },
    'ZZJetsTo2L2Q' : {
        'datasetpath'      : "/ZZJetsTo2L2Q_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM",
        'dbs_url'          :  "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
        'events_processed' : 1936727,
        'events_per_job'   : 20000,
        'x_sec'            : 2.5*_picobarns, # CV: taken from AN-13/171
        'type'             : 'smMC'
    }
}

# CV: add SM Higgs Monte Carlo samples
smHiggsMassPoints = [ 120, 125, 130 ]
x_sec_SM_ggHiggs = {}
x_sec_SM_ggHiggs[120] = 20.86*_picobarns*7.04E-02  # cross-section times BR(Higgs -> tautau)
x_sec_SM_ggHiggs[125] = 19.27*_picobarns*6.32E-02
x_sec_SM_ggHiggs[130] = 17.85*_picobarns*5.45E-02
x_sec_SM_vbfHiggs = {}
x_sec_SM_vbfHiggs[120] = 1.649*_picobarns*7.04E-02 # cross-section times BR(Higgs -> tautau)
x_sec_SM_vbfHiggs[125] = 1.578*_picobarns*6.32E-02
x_sec_SM_vbfHiggs[130] = 1.511*_picobarns*5.45E-02
eventsProcessed = {}
##eventsProcessed["ggHiggs_120"]  = 4999561
eventsProcessed["ggHiggs_120"]  = 2000000 # CV: only 2 million events analyzed
eventsProcessed["ggHiggs_125"]  =  968134
eventsProcessed["ggHiggs_130"]  =  999921
##eventsProcessed["vbfHiggs_120"] = 4799659
eventsProcessed["vbfHiggs_120"] = 2000000 # CV: only 2 million events analyzed
eventsProcessed["vbfHiggs_125"] =  998836
eventsProcessed["vbfHiggs_130"] =  998790
for massPoint in smHiggsMassPoints:
    ggSampleName = "HiggsGGH%1.0f" % massPoint
    RECO_SAMPLES[ggSampleName] = {
        'datasetpath'      : '/GluGluToHToTauTau_M-%1.0f_8TeV-powheg-pythia6/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM' % massPoint,
        'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
        'events_processed' : eventsProcessed["ggHiggs_%1.0f" % massPoint],
        'events_per_job'   : 20000,
        'x_sec'            : x_sec_SM_ggHiggs[massPoint],
        'type'             : 'smMC'
    }
    SAMPLES_TO_ANALYZE.append(ggSampleName)
    vbfSampleName = "HiggsVBF%1.0f" % massPoint
    RECO_SAMPLES[vbfSampleName] = {
        'datasetpath'      : '/VBF_HToTauTau_M-%1.0f_8TeV-powheg-pythia6/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM' % massPoint,
        'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
        'events_processed' : eventsProcessed["vbfHiggs_%1.0f" % massPoint],
        'events_per_job'   : 20000,
        'x_sec'            : x_sec_SM_vbfHiggs[massPoint],
        'type'             : 'smMC'
    }
    SAMPLES_TO_ANALYZE.append(vbfSampleName)
vhSampleName = "HiggsVH125"
RECO_SAMPLES[vhSampleName] = {
    'datasetpath'      : '/WH_ZH_TTH_HToTauTau_M-125_8TeV-pythia6-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM',
    'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
    'events_processed' : 200124,
    'events_per_job'   : 20000,
    'x_sec'            : (0.69669 + 0.3943 + 0.1302)*0.0632*_picobarns, # taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#53X_MC_Samples
    'type'             : 'smMC'
}
SAMPLES_TO_ANALYZE.append(vhSampleName)
bbhSampleName = "HiggsBB125"
RECO_SAMPLES[bbhSampleName] = {
    'datasetpath'      : '/BBH_HToTauTau_M_125_TuneZ2star_8TeV_pythia6_tauola/Summer12_DR53X-PU_S10_START53_V19-v1/AODSIM',
    'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
    'events_processed' : 300300,
    'events_per_job'   : 20000,
    'x_sec'            : 0.2035*0.0632*_picobarns, # taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#53X_MC_Samples
    'type'             : 'smMC'
}
SAMPLES_TO_ANALYZE.append(bbhSampleName)
whbbSampleName = "HiggsWHbb125"
RECO_SAMPLES[whbbSampleName] = {
    'datasetpath'      : '/WH_WToLNu_HToBB_M-125_8TeV-powheg-herwigpp/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM',
    'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
    'events_processed' : 1000000,
    'events_per_job'   : 20000,
    'x_sec'            : 0.7046*0.326*0.577*_picobarns,
    'type'             : 'smMC'
}
SAMPLES_TO_ANALYZE.append(whbbSampleName)
zhbbSampleName = "HiggsZHbb125"
RECO_SAMPLES[zhbbSampleName] = {
    'datasetpath'      : '/ZH_ZToLL_HToBB_M-125_8TeV-powheg-herwigpp/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM',
    'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
    'events_processed' : 999462,
    'events_per_job'   : 20000,
    'x_sec'            : 0.4153*0.101*0.577*_picobarns,
    'type'             : 'smMC'
}
SAMPLES_TO_ANALYZE.append(zhbbSampleName)

# CV: add MSSM Higgs signal Monte Carlo samples
mssmHiggsMassPoints = [ 80, 90, 100, 110, 120, 130, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 ]
eventsProcessed["ggPhi_80"]   =  988416
eventsProcessed["ggPhi_90"]   = 1000350
eventsProcessed["ggPhi_100"]  = 1000080
eventsProcessed["ggPhi_110"]  = 1000348
eventsProcessed["ggPhi_120"]  = 1000620
eventsProcessed["ggPhi_130"]  =  995840
eventsProcessed["ggPhi_140"]  =  999680
eventsProcessed["ggPhi_160"]  =  988800
eventsProcessed["ggPhi_180"]  =  997200
eventsProcessed["ggPhi_200"]  =  985855
eventsProcessed["ggPhi_250"]  = 1000441
eventsProcessed["ggPhi_300"]  =  990976
eventsProcessed["ggPhi_350"]  = 1000395
eventsProcessed["ggPhi_400"]  =  995864
eventsProcessed["ggPhi_450"]  =  999440
eventsProcessed["ggPhi_500"]  =  981688
eventsProcessed["ggPhi_600"]  =  980316
eventsProcessed["ggPhi_700"]  =  985800
eventsProcessed["ggPhi_800"]  =  987600
eventsProcessed["ggPhi_900"]  =  975744
eventsProcessed["ggPhi_1000"] = 1000377
eventsProcessed["bbPhi_80"]   =  996592
eventsProcessed["bbPhi_90"]   =  995840
eventsProcessed["bbPhi_100"]  = 1000320
eventsProcessed["bbPhi_110"]  =  999600
eventsProcessed["bbPhi_120"]  =  991913
eventsProcessed["bbPhi_130"]  = 1000008
eventsProcessed["bbPhi_140"]  =  993600
eventsProcessed["bbPhi_160"]  =  975000
eventsProcessed["bbPhi_180"]  =  981022
eventsProcessed["bbPhi_200"]  =  999408
eventsProcessed["bbPhi_250"]  =  988200
eventsProcessed["bbPhi_300"]  =  999900 # CV: number of events contained in sample 'HiggsSUSYBB300v2'
eventsProcessed["bbPhi_350"]  =  982080
eventsProcessed["bbPhi_400"]  =  998702
eventsProcessed["bbPhi_450"]  =  973830
eventsProcessed["bbPhi_500"]  =  999976
eventsProcessed["bbPhi_600"]  =  993061
eventsProcessed["bbPhi_700"]  =  996960
eventsProcessed["bbPhi_800"]  = 1000432
eventsProcessed["bbPhi_900"]  =  997674
eventsProcessed["bbPhi_1000"] =  987947
for massPoint in mssmHiggsMassPoints:
    ggSampleName = "HiggsSUSYGluGlu%1.0f" % massPoint
    RECO_SAMPLES[ggSampleName] = {
        'datasetpath'      : '/SUSYGluGluToHToTauTau_M-%1.0f_8TeV-pythia6-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM' % massPoint,
        'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
        'events_processed' : eventsProcessed["ggPhi_%1.0f" % massPoint],
        'events_per_job'   : 20000,
        'type'             : 'bsmMC'
    }
    SAMPLES_TO_ANALYZE.append(ggSampleName)
    bbSampleName = "HiggsSUSYBB%1.0f" % massPoint
    RECO_SAMPLES[bbSampleName] = {
        'datasetpath'      : '/SUSYBBHToTauTau_M-%1.0f_8TeV-pythia6-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM' % massPoint,
        'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
        'events_processed' : eventsProcessed["bbPhi_%1.0f" % massPoint],
        'events_per_job'   : 20000,
        'type'             : 'bsmMC'
    }
    SAMPLES_TO_ANALYZE.append(bbSampleName)
bbSampleName = "HiggsSUSYBB300v2" 
RECO_SAMPLES[bbSampleName] = {
    'datasetpath'      : '/SUSYBBHToTauTau_M-300_8TeV-pythia6-tauola/Summer12_DR53X-PU_S10_START53_V7A-v2/AODSIM',
    'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
    'events_processed' : eventsProcessed["bbPhi_300"],
    'events_per_job'   : 20000,
    'type'             : 'bsmMC'
}
SAMPLES_TO_ANALYZE.append(bbSampleName)

#--------------------------------------------------------------------------------
# CV: additional MC samples for di-Higgs production study

hhTo2b2tauSampleName = "hhTo2b2tau_v2" 
RECO_SAMPLES[hhTo2b2tauSampleName] = {
    'datasetpath'      : '/GluGluToHHTo2B2Tau_M-125_8TeV-madgraph-pythia6/Summer12_DR53X-PU_S10_START53_V19-v1/AODSIM',
    'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
    'events_processed' : 94889,
    'events_per_job'   : 20000,
    'type'             : 'bsmMC'
}
SAMPLES_TO_ANALYZE.append(hhTo2b2tauSampleName)

mssmHtohhMassPoints = [ 260, 300, 350 ]
eventsProcessed["mssmHtohh_260"] = 464960
eventsProcessed["mssmHtohh_300"] = 457250
eventsProcessed["mssmHtohh_350"] = 570356
for massPoint in mssmHtohhMassPoints:
    mssmHtohhSampleName = "mssmH%1.0ftohh" % massPoint
    RECO_SAMPLES[mssmHtohhSampleName] = {
        'datasetpath'      : '/GluGluToHTohhTo2Tau2B_mH-%1.0f_mh-125_8TeV-pythia6-tauola/Summer12_DR53X-PU_S10_START53_V19-v1/AODSIM' % massPoint,
        'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
        'events_processed' : eventsProcessed["mssmHtohh_%1.0f" % massPoint],
        'events_per_job'   : 20000,
        'type'             : 'bsmMC'
    }
    SAMPLES_TO_ANALYZE.append(mssmHtohhSampleName)

abelianZprimeTohhMassPoints = [ 300, 500, 700 ]
eventsProcessed["abelianZprimeTohh_300"] = 19996
eventsProcessed["abelianZprimeTohh_500"] = 19996
eventsProcessed["abelianZprimeTohh_700"] = 19997
for massPoint in abelianZprimeTohhMassPoints:
    abelianZprimeTohhSampleName = "abelianZprime%1.0ftohh" % massPoint
    RECO_SAMPLES[abelianZprimeTohhSampleName] = {
        'datasetpath'      : '/AbelianZprime_hh_TauTauBB_LHC8_M%1.0f_GENSIM_29082012/caber-Radion_hh_TauTauBB_LHC8_M%1.0f_31082013_AODSIM-c8f8ed334db8a7d6f56c62266b1dfa5b/USER' % (massPoint, massPoint),
        'dbs_url'          : 'phys02',
        'events_processed' : eventsProcessed["abelianZprimeTohh_%1.0f" % massPoint],
        'events_per_job'   : 20000,
        'type'             : 'bsmMC'
    }
    SAMPLES_TO_ANALYZE.append(abelianZprimeTohhSampleName)

gravitonTohhMassPoints = [ 270, 300, 500, 700, 1000 ]
eventsProcessed["gravitonTohh_270"]  = 99800
eventsProcessed["gravitonTohh_300"]  = 99863
eventsProcessed["gravitonTohh_500"]  = 99959
eventsProcessed["gravitonTohh_700"]  = 99957
eventsProcessed["gravitonTohh_1000"] = 99975
for massPoint in gravitonTohhMassPoints:
    gravitonTohhSampleName = "graviton%1.0fTohh" % massPoint
    RECO_SAMPLES[gravitonTohhSampleName] = {
        'datasetpath'      : '/GravitonToHH_2Tau_2b_M-%1.0f_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_S10_START53_V19-v2/AODSIM' % massPoint,
        'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
        'events_processed' : eventsProcessed["gravitonTohh_%1.0f" % massPoint],
        'events_per_job'   : 20000,
        'type'             : 'bsmMC'
    }
    SAMPLES_TO_ANALYZE.append(gravitonTohhSampleName)
    
radionTohhMassPoints = [ 300, 500, 700, 1000 ]
eventsProcessed["radionTohh_300"]  = 99836
eventsProcessed["radionTohh_500"]  = 99947
eventsProcessed["radionTohh_700"]  = 99957
eventsProcessed["radionTohh_1000"] = 99569
for massPoint in radionTohhMassPoints:
    radionTohhSampleName = "radion%1.0fTohh" % massPoint
    RECO_SAMPLES[radionTohhSampleName] = {
        'datasetpath'      : '/RadionToHH_2Tau_2b_M-%1.0f_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_S10_START53_V19-v2/AODSIM' % massPoint,
        'dbs_url'          : 'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet',
        'events_processed' : eventsProcessed["radionTohh_%1.0f" % massPoint],
        'events_per_job'   : 20000,
        'type'             : 'bsmMC'
    }
    SAMPLES_TO_ANALYZE.append(radionTohhSampleName)

#--------------------------------------------------------------------------------    

# Update to use the defaults if necessary
for sample in RECO_SAMPLES.keys():
    defaults = copy.copy(SAMPLE_DEFAULTS)
    defaults.update(RECO_SAMPLES[sample])
    RECO_SAMPLES[sample] = defaults
    parameters_to_overwrite = {}
    if RECO_SAMPLES[sample]['type'] == "Data":
        parameters_to_overwrite['runOnMC']    = False
        parameters_to_overwrite['runOnEmbed'] = False
        parameters_to_overwrite['embedType']  = ""
        parameters_to_overwrite['trigger']    = RECO_SAMPLES[sample]['trigger']
    elif RECO_SAMPLES[sample]['type'] == "pfEmbeddedData":
        parameters_to_overwrite['runOnMC']    = False
        parameters_to_overwrite['runOnEmbed'] = True
        parameters_to_overwrite['embedType']  = "PfEmbed"
        parameters_to_overwrite['trigger']    = "TauPlusJet"
    elif RECO_SAMPLES[sample]['type'] == "rhEmbeddedData":
        parameters_to_overwrite['runOnMC']    = False
        parameters_to_overwrite['runOnEmbed'] = True
        parameters_to_overwrite['embedType']  = "RhEmbed"
        parameters_to_overwrite['trigger']    = "TauPlusJet"
    elif RECO_SAMPLES[sample]['type'] == "smMC" or RECO_SAMPLES[sample]['type'] == "bsmMC":
        parameters_to_overwrite['runOnMC']    = True
        parameters_to_overwrite['runOnEmbed'] = False
        parameters_to_overwrite['embedType']  = ""
        parameters_to_overwrite['trigger']    = "TauPlusJet"
    elif RECO_SAMPLES[sample]['type'] == "pfEmbeddedMC":
        parameters_to_overwrite['runOnMC']    = True
        parameters_to_overwrite['runOnEmbed'] = True
        parameters_to_overwrite['embedType']  = "PfEmbed"
        parameters_to_overwrite['trigger']    = "TauPlusJet"
    elif RECO_SAMPLES[sample]['type'] == "rhEmbeddedMC":
        parameters_to_overwrite['runOnMC']    = True
        parameters_to_overwrite['runOnEmbed'] = True
        parameters_to_overwrite['embedType']  = "RhEmbed"
        parameters_to_overwrite['trigger']    = "TauPlusJet"
    else:
        raise ValueError("Invalid Configuration Parameter 'type' = %s for sample = %s !!" % (RECO_SAMPLES[sample]['type'], sample))
    RECO_SAMPLES[sample]['parameters_to_overwrite'] = parameters_to_overwrite
    
recoSampleDefinitionsAHtoTauTau_8TeV = {
    'SAMPLES_TO_ANALYZE' : SAMPLES_TO_ANALYZE,
    'TARGET_LUMI'        : TARGET_LUMI,
    'RECO_SAMPLES'       : RECO_SAMPLES
}
