import FWCore.ParameterSet.Config as cms

SAMPLE_DEFAULTS = {
    'dbs_url'                 : "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet",
    'conditions'              : 'START53_V23::All',
    'hlt_paths'               : [],
    'lumi_mask'               : '',
    'runselection'            : '',
    'SE_white_list'           : '',
    'SE_black_list'           : 'T2_BE_IIHE,T3_CH_PSI',
    'skim_eff'                : 1.,
    'parameters_to_overwrite' : {}   
}
