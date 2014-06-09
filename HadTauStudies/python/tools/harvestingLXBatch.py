
import hashlib
import time
import re
import os

from collections import defaultdict

from LLRAnalysis.HadTauStudies.tools.harvesting import eos_source
import LLRAnalysis.HadTauStudies.tools.jobtools as jobtools
import LLRAnalysis.HadTauStudies.tools.eos as eos
import LLRAnalysis.HadTauStudies.tools.crab as crab

_HARVESTER_CMD = '$CMSSW_BASE/src/LLRAnalysis/HadTauStudies/python/tools/genericHarvester.py'
_MERGER_CMD = '$CMSSW_BASE/src/LLRAnalysis/HadTauStudies/python/tools/genericSkimMerger.py'

def write_comment_header(file_to_write, text):
    " Write a visible comment header block to a shell script file "
    file_to_write.write("###########################################\n")
    file_to_write.write("#            %s\n" % text)
    file_to_write.write("###########################################\n")

def make_harvest_scripts(plot_regex, skim_regex,
                         channel = "",
                         sampleToAnalyze = "",
                         job_id = None,
                         # An iterable that gives the input files
                         input_source = None,
                         # Pass input_files_info explicitely in case input files
                         # do not yet exist an time when make_harvest_scripts is called
                         input_files_info = None,
                         # Allow to switch between 'genericHarvester.py' and 'hadd'
                         harvester_command = _HARVESTER_CMD,
                         # Abort harvesting/merging scripts in case input files fail to get copied
                         abort_on_rfcp_error = True,
                         # Where to put the output
                         castor_output_directory = None,
                         script_directory = None,
                         merge_script_name = None,
                         local_copy_mapper = None,
                         chunk_size = 1e9, # 1 GB
                         max_input_files_per_chunk = 50,
                         run_harvesting = True,
                         run_merging = True,
                         check_old_files = True,
                         max_bsub_concurrent_file_access = 2000,
                         verbosity = 1):

    # Get the jobId from the user registry
    if job_id is None:
        raise ValueError("Missing parameter 'jobId' !!")
    if script_directory is None:
        raise ValueError("Missing parameter 'script_directory' !!")

    # Create the directory where we store the scripts if it doesn't exist
    if not os.path.exists(script_directory):
        os.mkdir(script_directory)

    if input_files_info is None:
        # Get all files with nonzero size in the input castor directory
        print "Getting files to harvest from input, input_source = %s" % input_source
        input_files_info = [ x for x in input_source if x['size'] ]
    #print "input_files_info = %s" % input_files_info

    print "Getting files from destination"
    # Get all the tmp files (that are non-zero)
    local_output_directory = os.getcwd()
    if castor_output_directory.find("/store") != -1:
        tmp_files_info = [ x for x in eos_source(castor_output_directory) if x['size'] ]
    else:
        if castor_output_directory.find(":") != -1:
            local_output_directory = castor_output_directory[castor_output_directory.find(":") + 1:]
        else:
            local_output_directory = castor_output_directory
        tmp_files_info = []
        for x in os.listdir(local_output_directory):
            file_info = {
                'path'        : os.path.join(local_output_directory, os.path.basename(x)),
                'size'        : 1,           # dummy
                'time'        : time.localtime(),
                'file'        : os.path.basename(x),
                'permissions' : 'mrw-r--r--' # "ordinary" file access permissions
            }
            tmp_files_info.append(file_info)
    tmp_files = set(x['file'] for x in tmp_files_info)
    #print "tmp_files_info = %s" % tmp_files_info

    # Make a repository of info about our files
    all_files_dict = {}
    for file_info in input_files_info + tmp_files_info:
        all_files_dict[file_info['path']] = file_info
    #print "all_files_dict = %s" % all_files_dict

    # Keep track of files that we put in tmp with these jobs and that we care
    # about.  We can stop caring about old files if after adding new files (i.e.
    # crab jobs finishing), the merge block they were in contains a different
    # set of files.
    relevant_tmp_files = set([])

    # Keep track of the final harvested output
    final_harvest_files = []

    harvest_script_name = "_".join(['submit', channel, sampleToAnalyze, job_id, 'harvest']) + '.sh'
        
    harvest_log = open('_'.join(('harvest', channel, sampleToAnalyze, job_id, 'log')) + '.txt', 'a')

    # Keep track of the names of lxbatch jobs
    bsub_job_names = []

    if run_harvesting:
        # Select those that match our given regex for a sample
        plot_file_map = defaultdict(list)
        # Keep track of a hash of
        plot_matcher = re.compile(plot_regex)
        plot_source_hashes = defaultdict(hashlib.md5)
        for file in input_files_info:
            match = plot_matcher.match(file['file'])
            if match:
                full_file = file['path']
                sample = None
                if sampleToAnalyze != "":
                    sample = sampleToAnalyze
                else:
                    sample = match.group('sample')
                plot_file_map[sample].append(full_file)
                plot_source_hashes[sample].update(full_file)

        harvest_log.write('=========== %s ================' % time.asctime())
        for sample in sorted(plot_source_hashes.keys()):
            # Write down the hash of the input files
            harvest_log.write(
                " ".join([channel, sample, job_id, '<harvest>' 'input[%s]' %
                          plot_source_hashes[sample].hexdigest()])+'\n')

        submit_file = open(harvest_script_name, 'w')
        # Make the bsub scripts
        submit_file.write("#!/bin/bash\n")
        # Now build mergers for each of the samples
        # Count how many bsubs we have created, so we can put pauses in to
        # thwart the rate limit.
        bsub_file_access_counter = 0
        for sample in plot_file_map.keys():
            #if sample.find('NoPU') == -1:
            #    continue
            # Add helpful comments
            write_comment_header(submit_file, "Harvesting channel %s, sample %s" % (channel, sample))
            print " Building harvesting for channel %s, sample %s" % (channel, sample)
            print " -- Found %i files to harvest" % len(plot_file_map[sample])
            # Build merge tree.  We add the source has to the sample name.
            split = 6
            if 'PPmuX' in sample:
                print "High yield sample %s detected, setting split to 4" % sample
                split = 4
            merge_jobs = jobtools.make_merge_dependency_tree(
                "_".join([channel, sample, job_id]), plot_file_map[sample],
                local_output_directory, split = split)
            #print "merge_jobs = %s" % merge_jobs
            # Only do work that hasn't been done before.  We can check and see
            # if the output of a given merge layer is already in the temp
            # directory.  As the filenames contain a suffix with the hash of the
            # input file names, we can be sure that if a file is out of date we
            # will notice.
            merge_jobs_needed = []
            files_to_build = set([])
            print " --- Generated %i harvest layers:" % len(merge_jobs)
            for i, layer in enumerate(merge_jobs):
                # Figure out how many files we need to build
                layer_jobs_needed = []
                for layer_job in layer:
                    #print "layer_job = ", layer_job
                    # Check if we've already built this output file in the tmp
                    file_base_name = os.path.basename(layer_job[0])
                    needed = True
                    # Check if we are rebuilding a dependency
                    building_a_dependency = any(
                        file in files_to_build for file in layer_job[1])
                    if not building_a_dependency and file_base_name in tmp_files:
                        output_m_time = all_files_dict[layer_job[0]]['time']
                        out_of_date = False
                        for input_file in layer_job[1]:
                            #print "input_file = ", input_file
                            if not input_file in all_files_dict.keys() or all_files_dict[input_file]['time'] > output_m_time:
                                print "File: %s is older than its dependency %s, rebuilding!" % (
                                    file_base_name, input_file)
                                # Check if it's out of date
                                out_of_date = True
                                break
                        if not out_of_date:
                            needed = False
                    if needed:
                        layer_jobs_needed.append(layer_job)
                    # Keep track of the relevant files, so we can delete old
                    # cruft
                    relevant_tmp_files.add(file_base_name)
                # Check if this is the final output layer
                if len(layer) == 1:
                    final_harvest_files.append((sample, layer[0][0]))
                print " ---- layer %i has %i jobs, of which %i not done" % (
                    i, len(layer), len(layer_jobs_needed))
                merge_jobs_needed.append(layer_jobs_needed)

            # Keep track of what jobId was used for a paticular output file
            job_registry = {}
            # If a file is not produced by a job (already exists in CASTOR/EOS),
            # then the job ID returned is none.
            get_job_name = lambda x : x in job_registry and job_registry[x] or None

            for ilayer, layer in enumerate(merge_jobs_needed):
                write_comment_header(submit_file, "Layer %i" % ilayer)
                submit_file.write("echo Submitting layer %i of channel %s, sample %s\n"
                                  % (ilayer, channel, sample))
                for ijob, (output_file, input_files) in enumerate(layer):
                    # Get the job name (if it exists) and file name for the
                    # input files.
                    input_files_and_jobs = [
                        (get_job_name(file), file) for file in input_files]

                    # Build a function that constructs our log file name given the
                    # job file hash.
                    # Build a function that constructs our log file name given the
                    # job file hash.
                    if not os.path.exists('lxbatch_log'):
                        os.makedirs('lxbatch_log')
                    def log_file_maker(job_hash):
                        log_fileName = os.path.join(
                            'lxbatch_log', "_".join(
                                ['harvest', job_hash, 'layer_%i' % ilayer]) + '.log')
                        # CV: delete log-files from previous job submissions
                        os.system("rm -f %s" % log_fileName)
                        return log_fileName

                    # Build the script                    
                    job_name, script = jobtools.make_bsub_script(
                        output_file, input_files_and_jobs,
                        log_file_maker,
                        harvester_command,
                        abort_on_rfcp_error = abort_on_rfcp_error,
                        label = "_".join([
                            "harvest",
                            channel, sample,
                            "layer", str(ilayer),
                            "job", str(ijob)]))
                    #print "job_name = %s" % job_name
                    bsub_job_names.append(job_name)
                    
                    # Register our job
                    #print "--> registering channel %s, sample %s, jobId %s" % (channel, sample, job_id)
                    #print " script_directory = %s" % script_directory
                    job_registry[output_file] = job_name
                    #print " job_registry[%s] = %s" % (output_file, job_registry[output_file])
                    script_file = os.path.join(
                        script_directory, "_".join([
                            "harvest",
                            channel, sample, job_id,
                            "layer", str(ilayer),
                            "job", str(ijob)]
                        ) + ".sh")
                    submit_file.write("bsub < %s\n" % script_file)
                    # Keep track of how many files we access
                    bsub_file_access_counter += split
                    if bsub_file_access_counter > max_bsub_concurrent_file_access:
                        bsub_file_access_counter = 0
                        submit_file.write("# thwart rate limit\n")
                        submit_file.write(
                            "echo Sleeping for 500 seconds, it is now:\n")
                        submit_file.write(
                            "date\n")
                        submit_file.write("sleep 500\n")
                    with open(script_file, 'w') as script_file:
                        script_file.write(script)
        submit_file.close()

    if run_merging:
        # Now build skim merging file
        # Select those that match our given regex for a sample
        skim_file_map = defaultdict(list)
        skim_fileinhash_map = defaultdict(hashlib.md5)
        skim_fileouthash_map = defaultdict(hashlib.md5)
        skim_matcher = re.compile(skim_regex)
        for file in input_files_info:
            match = skim_matcher.match(file['file'])
            if match:
                full_file = file['path']
                # Parse the sample from the regex
                sample = None
                if sampleToAnalyze != "":
                    sample = sampleToAnalyze                    
                else:
                    sample = match.group('sample')
                # For the skims, keep track of the file size well, since we use it
                # to group the jobs.
                skim_file_map[sample].append(
                    (file['time'], file['size'], full_file))
                # Keep track of the hash of all input files so we know what went
                # into our output
                skim_fileinhash_map[sample].update(full_file)

        def make_skim_name(sample, chunk, hash):
            " Generate a nice name for an output skim "
            return "_".join(["skim", sample, "chunk", str(chunk), hash]) + ".root"

        if merge_script_name is None:
            merge_script_name = "_".join(['submit', channel, sampleToAnalyze, job_id, 'merge']) + '.sh'
        with open(merge_script_name, 'w') as merge_script:
            merge_jobs_counter = 0
            bsub_file_access_counter = 0
            for sample in skim_file_map.keys():
                write_comment_header(merge_script, " Merging " + sample)
                print "Merging channel %s, sample %s" % (channel, sample)
                files = skim_file_map[sample]
                num_files = len(files)
                total_file_size =  sum(map(lambda x: x[1], files))/1e6
                # Divide the job up into chunks that are about 1 GB each in size
                chunks = list(jobtools.split(files, chunk_size, max_input_files_per_chunk, lambda x: x[1]))
                print " Total sample size: %i files, %i MB - splitting into %i chunks" % (
                    num_files, total_file_size, len(chunks)),
                # Keep track of jobs we are actually running
                skim_merge_jobs = []
                for ichunk, input_files in enumerate(chunks):
                    # Figure out the name for our file.  It contains a hash of its
                    # inputs.  We don't add the time, as we never have any LXBatch
                    # job dependencies.
                    just_the_files = [x[2] for x in input_files]
                    output_file = make_skim_name(
                        "_".join([channel, sample, job_id]), ichunk, jobtools.hash_files(
                            just_the_files, add_time=False))
                    skim_fileouthash_map[sample].update(output_file)

                    relevant_tmp_files.add(output_file)
                    if output_file not in tmp_files:
                        output_file_full_path = os.path.join(
                            castor_output_directory, output_file)
                        # Add "None" as the job id of the input files to indicate we
                        # don't care about any dependencies.  The index on x takes
                        # out only the filename, not the size or the time.
                        skim_merge_jobs.append(
                            (output_file_full_path,
                             map(lambda x: (None, x[2]), input_files))
                        )

                print " -- %i chunks are already done, skipping" % (
                    len(chunks) - len(skim_merge_jobs))
 
                for ijob, (output_file, input_files) in enumerate(skim_merge_jobs):
                    def merge_log_file_maker(job_hash):
                        log_fileName = os.path.join(
                            'lxbatch_log', "_".join(
                                ['merge', job_hash, 'job_%i' % ijob]) + '.log')
                        # CV: delete log-files from previous job submissions
                        os.system("rm -f %s" % log_fileName)
                        return log_fileName

                    # Generate script contents
                    job_name, script = jobtools.make_bsub_script(
                        output_file,
                        input_files,
                        merge_log_file_maker,
                        _MERGER_CMD,
                        abort_on_rfcp_error = abort_on_rfcp_error,
                        label = "_".join([
                            "merge",
                            channel, sample, 
                            "chunk", str(ijob)]))
                    script_file = os.path.join(
                        script_directory, "_".join([
                            "merge",
                            channel, sample, job_id,
                            "chunk", str(ijob)])
                        + ".sh")
                    
                    # Add our bsub command
                    merge_script.write("bsub < %s\n" % script_file)
                    merge_jobs_counter += 1
                    bsub_file_access_counter += len(input_files)
                    if bsub_file_access_counter > max_bsub_concurrent_file_access:
                        bsub_file_access_counter = 0
                        merge_script.write("# thwart rate limit\n")
                        merge_script.write(
                            "echo Sleeping for 500 seconds, it is now:\n")
                        merge_script.write('date\n')
                        merge_script.write("sleep 500\n")
                    with open(script_file, 'w') as script_file:
                        script_file.write(script)

            print " Built %i merge jobs in %s" % (merge_jobs_counter,
                                                  merge_script_name)

        for sample in sorted(skim_fileinhash_map.keys()):
            # Write down the hash of the input files
            harvest_log.write(
                " ".join([job_id, sample, '<merge>'
                          'input[%s]' % skim_fileinhash_map[sample].hexdigest(),
                          'output[%s]' % skim_fileouthash_map[sample].hexdigest(),
                          ])+'\n')

    if check_old_files:
        # Compute all the extra tmp files
        extra_crap = tmp_files - relevant_tmp_files
        if extra_crap:
            print " Found %i extra files from previous harvest jobs"%len(extra_crap)
            print " Writing these files to file garbage.txt. To delete, please run: "
            print " cat garbage.txt | xargs -P 10 -n 1 rfrm "
            with open('garbage.txt', 'w') as garbage_file:
                for file in extra_crap:
                    full_path = os.path.join(castor_output_directory, file)
                    garbage_file.write(full_path + '\n')

    local_copy_script = 'copy_harvest_local_%s.sh' % job_id
    with open(local_copy_script, 'w') as copy_script:
        for sample, file in final_harvest_files:
            copy_script.write('rfcp %s %s &\n' % (
                file, local_copy_mapper(sample)))
        copy_script.write('wait\n')

        if verbosity > 0:
            if run_harvesting:
                print "To harvest plots, run %s" % harvest_script_name
            if run_merging:
                print "To merge skims, run %s" % merge_script_name 
            print "After harvesting is done, run %s to copy files locally" % local_copy_script

    # Make all our stuff executable
    os.chmod(local_copy_script, 0755)
    if run_harvesting:
        os.chmod(harvest_script_name, 0755)
    if run_merging:    
        os.chmod(merge_script_name, 0755)

    # Return name of scripts
    retVal = {}
    retVal['harvest_script_name'] = harvest_script_name
    retVal['bsub_job_names']      = bsub_job_names
    retVal['final_harvest_files'] = final_harvest_files
    retVal['merge_script_name']   = merge_script_name
    retVal['local_copy_script']   = local_copy_script
    return retVal

