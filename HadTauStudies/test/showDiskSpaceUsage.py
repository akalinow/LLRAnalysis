#!/usr/bin/env python

import os
import shlex
import subprocess
import sys
import time

import LLRAnalysis.HadTauStudies.tools.eos as eos

#--------------------------------------------------------------------------------
# Show disk space usage on EOS,
# of directory given as command line argument (including all subdirectories)
#
# Author: Christian Veelken 
# 
# Examples:
# 
# ./showDiskSpaceUsage.py /store/user/veelken/CMSSW_5_3_x/
#
#--------------------------------------------------------------------------------

if not (len(sys.argv) == 2):
    raise ValueError("Usage: showDiskSpaceUsage.py filePath")

filePath = sys.argv[1]

scaleToGb = 1.e-9

def processFilePath(filePath):
    
    sumFileSizes = 0.

    if not filePath[len(filePath) - 1] == "/":
        filePath = filePath + "/"

    subdirectories = []

    source_file_infos = None
    try:
        source_file_infos = eos.lsl(filePath)
    except IOError:
        # filePath is not a subdirectory, but a file of zero size
        return sumFileSizes
    
    for source_file_info in source_file_infos:
        ##print source_file_info
        file_size = source_file_info['size']
        if file_size <= 1000: # directory
            subdirectories.append(source_file_info['path'])
        else: # actual file
            sumFileSizes += file_size

    for subdirectory in subdirectories:
        sumFileSizes_bak = sumFileSizes
        sumFileSizes_bak = sumFileSizes_bak + processFilePath(subdirectory)
        sumFileSizes = sumFileSizes_bak

    print " filePath = %s: usage = %1.3f" % (filePath, sumFileSizes*1.e-9)

    return sumFileSizes
        
processFilePath(filePath)
