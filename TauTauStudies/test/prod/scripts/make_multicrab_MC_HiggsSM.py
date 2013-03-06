import string
import sys

samples = {}
names   = {}
n = 0

ifile = open(sys.argv[1])
for line in ifile:
	fields = line.split()
	if len(fields)==0:
		continue
	samples[n] = str(fields[0])
	name = samples[n]
	names[n]=""
	i=0
	j=0
	while i<len(name) :
		if name[i:i+4]=="8TeV" :
			names[n]=name[1:i-1]
			break
		i += 1
	n += 1
ifile.close()

print "[MULTICRAB]"
print "cfg=crab_pat_06Mar13_MC.cfg"
print "[COMMON]"
print "USER.storage_element = T2_FR_GRIF_LLR"
print ""
print ""

for k in range(0,n):
	print "["+names[k]+"_MC_HTT_06Mar2013_PAT_v1]"
	print "CMSSW.datasetpath="+samples[k]
	print "CMSSW.total_number_of_events = -1"
	print "CMSSW.events_per_job = 5000"
	print "USER.publish_data_name = "+names[k]+"_MC_HTT_06Mar2013_PAT_v1"
	print ""

