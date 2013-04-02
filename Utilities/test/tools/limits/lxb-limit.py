#!/usr/bin/env python
from optparse import OptionParser, OptionGroup

## set up the option parser
parser = OptionParser(usage="usage: %prog [options] ARGs", description="This is a script for simple job submission of limit.py processes to lxb/lxq. It is e.g. called from submit.py, when running this script in batch mode for several main options. The arguments ARGs correspond to the directories that are meant to be processed by limit.py")
##
## MAIN OPTIONS
##
parser.add_option("--name", dest="name", default="batch", type="string",
                  help="Add the job name here. [Default: \"batch\"]")
parser.add_option("--jobdir", dest="jobdir", default="LIMITS/", type="string",
                  help="Directory to contain the jobs. [Default: \"batch\"]")
parser.add_option("--limit-options", dest="limit", default="", type="string",
                  help="Add all options you want to pass to limit.py encapsulated by quotation marks '\"'. [Default: \"\"]")
parser.add_option("--batch-options", dest="batch", default="", type="string",
                  help="Add all options you want to pass to lxb/lxq encapsulated by quotation marks '\"'. [Default: \"\"]")
parser.add_option("--lxq", dest="lxq", default=False, action="store_true",
                  help="Specify this option when running on lxq instead of lxb. [Default: False]")
parser.add_option("--llr", dest="llr", default=False, action="store_true",
                  help="Specify this option when running on LLR T3. [Default: False]")
parser.add_option("--condor", dest="condor", default=False, action="store_true",
                  help="Specify this option when running on condor instead of lxb. [Default: False]")
## check number of arguments; in case print usage
(options, args) = parser.parse_args()
if len(args) < 1 :
    parser.print_usage()
    exit(1)

import os
import sys
import glob

import logging
log = logging.getLogger("lxb-limit")
logging.basicConfig(stream=sys.stderr, level=logging.INFO)

#dirglob = args[0]
jobdir = options.jobdir
name = options.name
bsubargs = options.batch
option_str = options.limit

# If we are using CONDOR, make sure any card creation happens in a writable
# directory.
if options.condor:
    option_str = " ".join([option_str, "--working-dir=${_CONDOR_SCRATCH_DIR}"])

## prepare log file directory
os.system("mkdir -p log")

script_template = '''#!/bin/bash

cd {working_dir}
eval `scram runtime -sh`

echo "Running limit.py:"
echo "with options {options}"
echo "in directory {directory}"

echo "Running"
$CMSSW_BASE/src/HiggsAnalysis/HiggsToTauTau/scripts/limit.py {options} {directory}

'''

lxq_fragment = '''#!/bin/bash
export SCRAM_ARCH=$scram_arch
ini cmssw
ini autoproxy
'''

condor_sub_template = '''
log = condor.log
notification = never
getenv = true
## make sure AFS is accessible and suppress the default FilesystemDomain requirements of Condor
requirements = HasAFS_OSG && TARGET.FilesystemDomain =!= UNDEFINED && TARGET.UWCMS_CVMFS_Revision >= 0

'''

#if not glob.glob(dirglob):
#    print "No limit directories found in glob %s" % glob
#    sys.exit(1)

if options.lxq :
    script_template = script_template.replace('#!/bin/bash', lxq_fragment)

if not os.path.exists(jobdir+'/'+name):
    os.system("mkdir -p %s/%s" % (jobdir,name))

submit_name = '%s/%s/%s_submit.sh' % (jobdir,name,name)
with open(submit_name, 'w') as submit_script:
    if options.condor:
        submit_script.write(condor_sub_template)
    if options.lxq or options.llr:
        submit_script.write('export scram_arch=$SCRAM_ARCH\n')
        submit_script.write('export cmssw_base=$CMSSW_BASE\n')
##     if not os.path.exists(jobdir+'/'+name):
##         os.system("mkdir -p %s/%s" % (jobdir,name))
    for i, dir in enumerate(args):
    #for i, dir in enumerate(glob.glob(dirglob)):
        ## don't submit jobs on old LSF output
        if 'LSFJOB' in dir:
            continue
        if 'common' in dir:
            continue
        log.info("Generating submission script for %s", dir)
        script_file_name = '%s/%s/%s_%i.sh' % (jobdir, name, name, i)
        with open(script_file_name, 'w') as script:
            script.write(
                script_template.format(
                working_dir=os.getcwd(),
                options=option_str,
                directory=dir
                ))
        os.system('chmod a+x %s' % script_file_name)
        if options.condor :
            submit_script.write("\n")
            submit_script.write(
                "executable = %s/%s\n" % (os.getcwd(), script_file_name))
            submit_script.write(
                "output = %s/%s\n" % (
                    os.getcwd(), script_file_name.replace('.sh', '.stdout')))
            submit_script.write(
                "error = %s/%s\n"
                % (os.getcwd(), script_file_name.replace('.sh', '.stderr')))
            submit_script.write("queue\n")
        elif options.lxq :
            submit_script.write('qsub -l site=hh -l h_vmem=4000M %s -v scram_arch -v cmssw_base %s\n'
                                % (bsubargs, script_file_name))
        elif options.llr :
            submit_script.write('/opt/exp_soft/cms/t3/t3submit -V %s_%i.sh\n'
                                % (name, i) )
        else :
            os.system('touch {PWD}/log/{LOG}'.format(
                PWD=os.getcwd(), LOG=script_file_name[script_file_name.rfind('/')+1:].replace('.sh', '.log')))
            submit_script.write('bsub {QUEUE} -oo {PATH}/log/{LOG} {PATH}/{FILE}\n'.format(
                QUEUE=bsubargs, LOG=script_file_name[script_file_name.rfind('/')+1:].replace('.sh', '.log'), PATH=os.getcwd(), FILE=script_file_name))
os.system('chmod a+x %s' % submit_name)
