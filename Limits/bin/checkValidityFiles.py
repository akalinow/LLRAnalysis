#!/usr/bin/env python

import sys, os, re, shlex, string, subprocess, ROOT
from ROOT import TFile
from ROOT import gDirectory

inputFilePath = str(sys.argv[1])
inputFiles = os.listdir(inputFilePath)

for inputFile in inputFiles:
    if not ".root" in inputFile:
        continue

    file = TFile(inputFilePath+'/'+inputFile)
