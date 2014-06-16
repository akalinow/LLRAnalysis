import FWCore.ParameterSet.Config as cms

import copy

from LLRAnalysis.HadTauStudies.tools.submitToGrid import submitToGrid

def _number_of_jobs(sample_info, preferred = 10000, max_jobs = 300):
    """
    Try to run on <preferred> events per job, unless it would create to many jobs.
    """
    # Check if explicitly specified
    if 'number_of_jobs' in sample_info:
        return sample_info['number_of_jobs']
    # Otherwise try and run 10k events per job, with a max of 300 jobs
    to_process = sample_info['events_processed']*sample_info['skim_eff']
    desired_njobs = to_process/preferred
    if desired_njobs < 0:
        return max_jobs
    output = (desired_njobs > max_jobs) and max_jobs or desired_njobs
    return int(output)

def submitAnalysisToGrid(configFile = None,
                         channel = None, sample = None, jobId = None,
                         sample_info = None,
                         crabFilePath = 'crab',
                         outputFilePath = None, outputFiles = None):
    """
    Submit analysis job via crab
    """

    # check that configFile, channel, samples, outputFilePath and jobId
    # parameters are defined and non-empty
    if configFile is None:
        raise ValueError("Configuration Parameter 'configFile' not defined !!")
    if channel is None:
        raise ValueError("Configuration Parameter 'channel' not defined !!")
    if sample is None:
        raise ValueError("Configuration Parameter 'sample' not defined !!")
    if jobId is None:
        raise ValueError("Configuration Parameter 'jobId' not defined !!")
    if outputFilePath is None:
        raise ValueError("Configuration Parameter 'outputFilePath' not defined !!")
    if outputFiles is None:
        raise ValueError("Configuration Parameter 'outputFiles' not defined !!")

    # Make job info
    jobInfo = {
        'channel' : channel,
        'sample'  : sample,
        'id'      : jobId
    }

    # Build crab options
    crabOptions = {
        ##'number_of_jobs'  : _number_of_jobs(sample_info),
        'datasetpath'     : sample_info['datasetpath'],
        'dbs_url'         : sample_info['dbs_url'],
        'user_remote_dir' : outputFilePath,
        'output_file'     : ", ".join(outputFiles),
        # copy output to EOS disks at CERN
        'return_data'     : 0,
        'copy_data'       : 1,
        'storage_element' : "T2_CH_CERN",
        'total_number'    : (sample_info['type'].find('MC') != -1 and 'events_subset' in sample_info.keys()) and sample_info['events_subset'] or -1,
        'split_type'      : (sample_info['type'] == 'Data' or sample_info['type'].find('EmbeddedData') != -1) and 'lumis' or 'events',
        'lumi_mask'       : sample_info['lumi_mask'],
        'runselection'    : sample_info['runselection'],
        'SE_white_list'   : sample_info['SE_white_list'],
        'SE_black_list'   : sample_info['SE_black_list']
    }
    
    split_job_option = None
    print sample_info
    if sample_info['type'].find('MC') != -1 and 'events_per_job' in sample_info.keys():
        split_job_option = "events_per_job = %i" % sample_info['events_per_job']
    elif (sample_info['type'] == 'Data' or sample_info['type'].find('EmbeddedData') != -1) and 'lumis_per_job' in sample_info.keys():
        split_job_option = "lumis_per_job = %i" % sample_info['lumis_per_job']
    else:
        split_job_option = "number_of_jobs = %i" % _number_of_jobs(sample_info)
    crabOptions['split_job_option'] = split_job_option

    ##submitToGrid(configFile, jobInfo, crabOptions, create = True, submit = True, cfgdir = crabFilePath)
    submitToGrid(configFile, jobInfo, crabOptions, create = True, submit = False, cfgdir = crabFilePath) # CV: only for testing
