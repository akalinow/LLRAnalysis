import hashlib
import os
import sys
import time
import string
sys.setrecursionlimit(int(1e4))

'''

Tools for splitting up jobs into chunks.

Author: Evan K. Friis, UC Davis

'''


bsub_template = string.Template(
'''#!/bin/csh
#BSUB -J $jobName
#BSUB -q 1nd
#BSUB -o $logFile
$dependencies

echo "Increasing vmem"
limit vmem unlim
echo "Changing directory"
cd $dir
pwd
setenv SCRAM_ARCH $arch
echo "Setting up runtime"
cmsenv
cd -
pwd
echo "Copying input files"
$copy_command
echo "Checking copies succeeded"
set expectedFiles = ( $inputFileList )
set existingFiles
foreach inputFile ( $$expectedFiles )
    ls $$inputFile
    if ( "$$status" == 0 ) then
        set existingFiles = ( $$existingFiles $$inputFile )
    else    
        echo "Failed to copy $$inputFile to local area!"
        $exit_command
    endif
end
echo "Current time:"
date
echo "Running script"
$merger_comand $outputFileCommand $$existingFiles
setenv result $$status
echo Job finished with exit code: $$result
if ( "$$result" == 0 ) then
    echo "Removing existing file"
    $rfrm $outputFile
    echo "Copying output"
    $rfcp $outputFileName $outputFile
$rfcp_add_output_files
endif
exit 0
''')

def hash_files(files, add_time=True):
    " Get the hash of a set of file names "
    hash = hashlib.md5()
    map(hash.update, files)
    if add_time:
        hash.update(time.asctime())
    return hash.hexdigest()[:4]

def make_bsub_script(output_file, input_jobs_and_files,
                     log_file_maker, merge_method, pass_io_files = True, abort_on_rfcp_error = True, label = "",
                     add_output_files = []):
    # Create a unique job name for our output file.  We leave the add_time
    # option set to true so that lxbatch jobs always have a unique name.
    # Otherwise when we add dependencies LXB gets confused if there were
    # previous (finished/failed) jobs with the same name.
    job_name = "_".join([label, hash_files([output_file])])
    job_dependencies = [ jobname for jobname, file in input_jobs_and_files
                        if jobname is not None ]
    dependencies = ''
    if job_dependencies:
        dependencies = "#BSUB -w '"
        # Make sure there is no space in the exit command
        dependencies += " && ".join('done("%s")' % job_name for
                                    job_name in job_dependencies)
        dependencies += "'"

    arch = os.environ['SCRAM_ARCH']
    dir = os.environ['CMSSW_BASE']
    input_files = [ file for job, file in input_jobs_and_files ]

    # Build log file name
    log_file = log_file_maker(job_name)

    # Copy all the files locally
    copy_command = ""
    for input_file in input_files:
        if input_file.find("/castor") != -1:
            copy_command += 'prfcp.py %s .\n' % input_file
            #copy_command += "wait\n"
        elif input_file.find("/store") != -1:
            copy_command += 'cmsStage %s .\n' % input_file
        else:
            inputFileName = None
            if input_file.find(":") != -1:
                inputFileName = os.path.basename(input_file[input_file.find(":") + 1:])
            else:
                inputFileName = os.path.basename(input_file)
            copy_command += 'cp %s .\n' % inputFileName

    exit_command = ""
    if abort_on_rfcp_error:
        exit_command = "exit 100"

    rfrm_command = ""
    rfcp_command = None
    outputFileName = None
    if output_file.find("/castor") != -1:
        rfrm_command = "rfrm"
        rfcp_command = "rfcp"
        outputFileName = os.path.basename(output_file)
    elif output_file.find("/store") != -1:
        rfrm_command = "cmsRm"
        rfcp_command = "cmsStage"
        outputFileName = os.path.basename(output_file)
    else:
        rfcp_command = "scp"
        if output_file.find(":") != -1:
            outputFileName = os.path.basename(output_file[output_file.find(":") + 1:])
        else:
            outputFileName = os.path.basename(output_file)

    inputFileCommand = " ".join(map(os.path.basename, input_files))
    inputFileList = inputFileCommand # just a list of the files
    outputFileCommand = outputFileName

    # If we customize the cfg to auto load the correct names, we don't need to
    # pass any command line options
    if not pass_io_files:
        inputFileCommand = ""
        outputFileCommand = ""

    # Copy additional output files
    rfcp_add_output_files = ""
    for add_output_file in add_output_files:
        if output_file.find("/castor") != -1:
            rfcp_add_output_files += "    %s %s\n" % (rfrm_command, os.path.join(os.path.dirname(output_file), add_output_file))
            rfcp_add_output_files += "    %s %s %s\n" % (rfcp_command, (add_output_file, os.path.join(os.path.dirname(output_file), add_output_file)))
        elif output_file.find("/store") != -1:
            rfcp_add_output_files += "    %s %s\n" % (rfrm_command, os.path.join(os.path.dirname(output_file), add_output_file))
            rfcp_add_output_files += "    %s %s %s\n" % (rfcp_command, (add_output_file, os.path.join(os.path.dirname(output_file), add_output_file)))
        else:
            if output_file.find(":") != -1:
                output_host = output_file[:output_file.find(":")]
                rfcp_add_output_files += "    %s %s %s:%s\n" % (rfcp_command, add_output_file, output_host, os.path.join(os.path.dirname(output_file[output_file.find(":") + 1:]), add_output_file))
            else:
                raise ValueError("Machine hosting output file path = %s must be specified !!" % os.path.dirname(output_file))

    # Return a tuple containing the output job name and the text of the script
    # file
    return (job_name, bsub_template.substitute(
        dir=dir,
        jobName=job_name,
        dependencies=dependencies,
        logFile = log_file,
        rfrm = rfrm_command,
        outputFile = output_file,
        inputFileCommand = inputFileCommand,
        inputFileList = inputFileList,
        outputFileCommand = outputFileCommand,
        outputFileName = outputFileName,
        rfcp = rfcp_command,
        rfcp_add_output_files = rfcp_add_output_files,
        copy_command = copy_command,
        exit_command = exit_command,
        merger_comand = merge_method,
        arch = arch,
    ))

def split(input_files, threshold, max_input_files_per_chunk, function):
    current_value = 0
    num_input_files_per_chunk = 0
    current_list = []
    while input_files:
        current_list.append(input_files.pop(0))
        current_value += function(current_list[-1])
        num_input_files_per_chunk += 1
        if current_value > threshold or num_input_files_per_chunk > max_input_files_per_chunk or not input_files:
            yield current_list
            current_value = 0
            num_input_files_per_chunk = 0
            current_list = []


def make_merger(name, split, output_dir, filetype=".root"):
    def name_function(njob, nlayer, current_merge):
        " Get a unique name containing a hash of the parent files "
        hash = hashlib.md5()
        map(hash.update, current_merge)
        return "_".join(
            [name, "layer", str(nlayer), "job",
             str(njob), hash.hexdigest()[:3]]) + filetype

    def merge(current_layer, current_merge, merge_stack, output_layers):
        if len(current_merge) < split and merge_stack:
            current_merge.append(merge_stack.pop(0))
            return merge(current_layer, current_merge,
                         merge_stack, output_layers)
        # Check if the current merge is done or we have run out of things to merge
        else:
            output_name = os.path.join(output_dir, name_function(
                len(current_layer), len(output_layers), current_merge))
            current_layer.append((output_name, current_merge))
            # Check if we have more merging to do in this layer
            if merge_stack:
                return merge(current_layer, [], merge_stack, output_layers)
            else:
                # Add the layer to the output layers
                output_layers.append(current_layer)
                # Use the outputs of the latest layer as the new merge stack
                if len(current_layer) == 1:
                    # Final exit
                    return output_layers
                else:
                    return merge(
                        [], [], [out for out, infiles in current_layer],
                        output_layers)
    return merge

def make_merge_dependency_tree(name, input_files, output_dir, split=5):
    ''' Build a merge tree, where each branch merges [split] input files.
    You must specify the last modified times of all input files.
    Note that this function is path indpendent.
    '''
    merger= make_merger(name, split, output_dir)
    result = merger([], [], input_files, [])
    return result

if __name__ == "__main__":
    listicle = [ (i, "doot_%i.root" % i) for i in range(60) ]
    output = make_merge_dependency_tree("blah", listicle, "/data1000/friis")
    for i, layer in enumerate(output):
        print "layer", i
        print layer


