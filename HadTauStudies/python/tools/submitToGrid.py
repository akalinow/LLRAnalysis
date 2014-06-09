import copy
import os
import re
import shlex
import string
import subprocess
import time

_CRAB_TEMPLATE = string.Template('''
[CRAB]
jobtype = cmssw
use_server = $use_server
scheduler = $scheduler

[CMSSW]
datasetpath = $datasetpath
dbs_url = $dbs_url
pset = $pset
total_number_of_$split_type = $total_number
$split_job_option

output_file = $output_file
get_edm_output = $get_edm_output
$lumi_mask
$runselection

[USER]
ui_working_dir = $ui_working_dir
return_data = $return_data
copy_data = $copy_data
publish_data = $publish_data
publish_data_name = $publish_data_name
dbs_url_for_publication = $dbs_url_for_publication
$storage_options

[GRID]
rb = CERN
maxtarballsize = 200
$SE_white_list
$SE_black_list
''')

_CRAB_DEFAULTS = {
    ##'number_of_jobs' : 300,
    'total_number' : -1,
    'return_data' : 0,
    'copy_data' : 1,
    'use_server' : 0,
    'get_edm_output' : 0,
    'scheduler' : 'remoteGlidein',
    #------------------------------------
    # for storing output of crab job on eos @ CERN    
    'storage_element' : 'T2_CH_CERN',
    'publish_data' : 0,
    'publish_data_name' : "",
    'dbs_url_for_publication' : "",
    #------------------------------------
    #------------------------------------
    # for publishing output in DBS
    #'storage_element' : 'T2_FR_GRIF_LLR',
    #'publish_data' : 1,
    #'publish_data_name' : 'ZmumuRecoilSelection',
    #'dbs_url_for_publication' : 'https://cmsdbsprod.cern.ch:8443/cms_dbs_ph_analysis_01_writer/servlet/DBSServlet',
    #------------------------------------
    'lumi_mask' : '',
    'runselection' : ''
}

def submitToGrid(configFile, jobInfo, crabOptions, crabFileName_full = None, ui_working_dir = None,
                 create = True, submit = True, cfgdir='crab'):

    # CV: if crabFileName has been passed as function argument
    #     assume crab config file already exists,
    #     else create it
    if not crabFileName_full:
        # Update the default crab options with our options
        fullCrabOptions = copy.copy(_CRAB_DEFAULTS)
        # Point crab to our PSET
        fullCrabOptions['pset'] = configFile
        workingDirectory = os.getcwd()
        submissionDirectory = os.path.join(workingDirectory, cfgdir)
        jobName = configFile
        if jobName.rfind('/') != -1:
            jobName = jobName[jobName.rfind('/') + 1:]
        jobName = jobName.replace('_cfg.py', '')
        #print("jobName = %s" % jobName)
        ui_working_dir = os.path.join(
            submissionDirectory, 'crabdir_%s' % jobName)
        fullCrabOptions['ui_working_dir'] = ui_working_dir
        fullCrabOptions.update(crabOptions)

        # For these cases we need some additional processing
        if fullCrabOptions['lumi_mask']:
            fullCrabOptions['lumi_mask'] = (
                'lumi_mask = ' + fullCrabOptions['lumi_mask'])
        if fullCrabOptions['runselection']:
            fullCrabOptions['runselection'] = (
                'runselection = ' + fullCrabOptions['runselection'])

        # CV: Generate commands to tell crab where to store the output file.
        #     A work-around is needed in order to write to EOS group space,
        #     as described in https://hypernews.cern.ch/HyperNews/CMS/get/crabFeedback/7094/1/1.html
        #
        #     The following setting are used:
        #      - storage_element = srm-eoscms.cern.ch
        #       (taken from section "SEs" of https://cmsweb.cern.ch/sitedb/prod/sites/T2_CH_CERN )
        #      - storage_path = /srm/v2/server?SFN=/eos/cms
        #       (following section "Stage out and publication to a 'non Official CMS site'" of https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideCrabFaq
        #        and reverse engineering of the log-file that is created by crab when writing the crab output to /store/user)
        #      - user_remote_dir = /store/group/...
        #       (following section "Stage out and publication to a 'non Official CMS site'" of https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideCrabFaq )
        #
        storage_options = ""
        if crabOptions['storage_element'] == 'T2_CH_CERN' and crabOptions['user_remote_dir'].find("/store/group/") != -1:            
            storage_options += "storage_element = srm-eoscms.cern.ch\n"
            storage_options += "storage_path = /srm/v2/server?SFN=/eos/cms\n" 
            storage_options += "user_remote_dir = %s\n" % crabOptions['user_remote_dir']
        else:
            storage_options += "storage_element = %s\n" % crabOptions['storage_element']
            storage_options += "user_remote_dir = %s\n" % crabOptions['user_remote_dir']
            ##storage_options += "check_user_remote_dir = 0\n"
        print "storage_options:"
        print storage_options
        fullCrabOptions['storage_options'] = storage_options

        # Add SE_white_list/SE_back_list commands if specified
        if fullCrabOptions['SE_white_list'] and fullCrabOptions['SE_white_list'] != '':
            fullCrabOptions['SE_white_list'] = (
                'SE_white_list = ' + fullCrabOptions['SE_white_list'])
        elif fullCrabOptions['SE_black_list'] and fullCrabOptions['SE_black_list'] != '':
            fullCrabOptions['SE_black_list'] = (
                'SE_black_list = ' + fullCrabOptions['SE_black_list'])

        # Create the crab file
        crabFileName = "crab_" + jobName + ".cfg"
        crabFileName_full = os.path.join(submissionDirectory, crabFileName)
        crabFile = open(crabFileName_full, 'w')
        crabConfig = _CRAB_TEMPLATE.substitute(fullCrabOptions)
        # CV: remove empty lines
        #    (referring to unused options)
        regExpr_comment_line = re.compile("#+[a-zA-Z0-9_=\\s]*")
        regExpr_empty_line = re.compile("\w+\s*=\s*$")
        crabConfig_cleaned = ""
        for line in crabConfig.split('\n'):
            if not regExpr_comment_line.match(line) and not regExpr_empty_line.match(line):
                crabConfig_cleaned += "%s\n" % line
        crabFile.write(crabConfig_cleaned)
        crabFile.close()
    elif ui_working_dir is None:
        raise ValueError('Undefined ui_working_dir !!')

    numJobsCreated = None

    if create:
        crabCreateCommand = "crab -create -cfg " + crabFileName_full
        print crabCreateCommand
        subprocess.call(crabCreateCommand, shell = True)        
        crabStatusCommand = "crab -status -c %s" % ui_working_dir
        print crabStatusCommand
        subprocess.call(crabStatusCommand, shell = True)  

    if submit:
        # Check number of jobs created by crab
        crabStatusCommand = "crab -status -c %s" % ui_working_dir
        retval = subprocess.Popen(shlex.split(crabStatusCommand), stdout = subprocess.PIPE)
        #retval.wait()
        # CV: retval.wait() sometimes makes computer wait forever...
        #     wait fixed time (30 seconds) instead and hope that 'crab -status' has finished by then
        time.sleep(30)
        lines = retval.stdout.readlines()
        regExpr = re.compile("Log file is (?P<logFileName>\S*)")
        logFileName = None
        for line in lines:
            regExprMatch = regExpr.match(line)
            if regExprMatch is not None:
                logFileName = str(regExprMatch.group('logFileName'))
        print "logFileName = %s" % logFileName
        if not logFileName:
            raise ValueError('Failed to find crab log-file !!')
        logFile = open(logFileName, "r")
        lines = logFile.readlines()
        regExpr = re.compile("\s*>>>>>>>>> (?P<numJobsCreated>\d*) Jobs Created\s*")
        for line in lines:
            regExprMatch = regExpr.match(line)
            if regExprMatch is not None:
                numJobsCreated = int(regExprMatch.group('numJobsCreated'))
        print "numJobsCreated = %i" % numJobsCreated
        if numJobsCreated is None:
            raise ValueError('Failed to determine number of crab jobs created !!')
        
        # Submit crab jobs in groups of 500 jobs
        # (500 = maximum number of jobs crab can handle in case jobs are submitted without using crab server)
        numJobsPerSubmit = 500
        for jobIndex in range(1 + (numJobsCreated - 1)/numJobsPerSubmit):
            crabSubmitCommand = "crab -submit %i-%i -c %s" % \
              (1 + jobIndex*numJobsPerSubmit, min((jobIndex + 1)*numJobsPerSubmit, numJobsCreated), ui_working_dir)
            print crabSubmitCommand
            subprocess.call(crabSubmitCommand, shell = True)
            crabStatusCommand = "crab -status %i-%i -c %s" % \
              (1 + jobIndex*numJobsPerSubmit, min((jobIndex + 1)*numJobsPerSubmit, numJobsCreated), ui_working_dir)
            print crabStatusCommand
            subprocess.call(crabStatusCommand, shell = True)

