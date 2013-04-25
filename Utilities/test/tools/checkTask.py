import string
import sys

print 'echo "launch script"'

ifile = open(sys.argv[1])
task  = str(sys.argv[2])
force = str(sys.argv[3])
pub   = str(sys.argv[4])

jobsResubmit={}
n=0

jobsCreated={}
nC=0

jobsForce={}
nF=0

jobsToGetServer={}
nGetS=0

jobsToGet=""
jobsAbort=""
jobsSubmitted=""

seen_resubmit=0
seen_abort=0
seen_done=0
seen_submitted=0
seen_force=0
seen_done_server=0

nSuccess=0
nTotal=0

isServer=0

for line in ifile:
	fields = line.split()
	if len(fields)==0:
		continue

	msg=""

	# DECIDE TREATMENT TYPE
	if len(fields)==2 and ((str(fields[0])=="scheduler" and str(fields[1])=="remoteGlidein") or (str(fields[0])=="server" and str(fields[1])=="ON")):
		isServer=1
		print "## isServer"

	if len(fields)==7 and str(fields[1])=="Y" and str(fields[3])=="Terminated":
		#print "## seen Terminated job"
		if str(fields[2])=="Done":
			seen_done_server=1
			jobsToGetServer[nGetS]=str(fields[0])
			nGetS += 1
			#print "## added job #"+str(fields[0])

	if (isServer==1 and seen_done_server==0) or isServer==0:
		#print "## normal treatment : server w/o Done jobs or !server"
		
		# Jobs to resubmit
		if seen_resubmit==1:
			jobsResubmit[n]=str(fields[3])
			seen_resubmit=0
			n+=1

		# Aborted
		if seen_abort==2:
			jobsAbort=str(fields[3])
			seen_abort=0

		elif seen_abort==1:
			seen_abort=2

		# Submitted
		if seen_submitted==1:
			jobsSubmitted=str(fields[4])
			seen_submitted=0

		# Jobs to force
		if seen_force==1:
			jobsForce[nF]=str(fields[4])
			nF+=1
			seen_force=0

		# Done
		if seen_done==2:
			jobsToGet=str(fields[3])
			seen_done=0

		elif seen_done==1:
			seen_done=2

		# Look file content
		if len(fields)==9:
			if str(fields[6])=="Code":
				if int(fields[-1])!=0:
					seen_resubmit=1
				else:
					nSuccess=int(fields[1])
					#print nSuccess

		elif len(fields)==4 :
			if str(fields[-2])=="Jobs":
				msg=str(fields[-1])
				if msg=="Aborted":
					seen_abort=1
				if msg=="Done":
					seen_done=1
				if msg=="Submitted":
					seen_submitted=1
					seen_force=1
				if msg=="Scheduled" or msg=="Ready":
					seen_force=1

			if str(fields[-2])=="Total":
				nTotal=int(fields[1])

			if str(fields[2])=="Created" and str(fields[3])=="Created":
				jobsCreated[nC]=str(fields[0])
				nC+=1

# PRINT INSTRUCTION #

if "NoISB" in force:
	instruction="crab -GRID.additional_jdl_parameters='AllowZippedISB = false' -c "+task+" "
else:
	instruction="crab -c "+task+" "

# Submit
joblineC=""
if len(jobsCreated)>0:
	joblineC=jobsCreated[0]
	for i in range(1,nC):
		joblineC += ","+str(jobsCreated[i])

	instruction += " -submit "+joblineC

# Getoutput
if (isServer==1 and seen_done_server==1 and nGetS>0):
	#print "## case isServer w/ jobs Done"
	#print str(nGetS)
	joblineGetS=str(jobsToGetServer[0])
	for i in range(1,nGetS):
		joblineGetS += ","+str(jobsToGetServer[i])
	#print "## jobslineGetS="+joblineGetS
	instruction += " -get "+joblineGetS

elif jobsToGet!="" :
	instruction += " -get "+jobsToGet+" "

# Force Resubmit
if jobsSubmitted!="" and force=="force":
	instruction += " -forceResubmit "+jobsSubmitted+" "

joblineF=""
if len(jobsForce)>0 and force=="forceAll":
	joblineF=jobsForce[0]
	for i in range(1,nF):
		joblineF += ","+str(jobsForce[i])

	instruction += " -forceResubmit "+joblineF

# Resubmit

## Status Code != 0
jobline=""
if len(jobsResubmit)>0:
	jobline=jobsResubmit[0]
	for i in range(1,n):
		jobline += ","+str(jobsResubmit[i])

if jobline!="" or jobsAbort!="":
	instruction += " -resubmit "
	if jobline!="":
		instruction += jobline+","

	if jobsAbort!="":
		instruction += jobsAbort

instruction1=''
instruction2=''
#print nSuccess
#print nTotal

if pub=="pub" :	
	if (nSuccess==nTotal and isServer==0) or (isServer==1 and nGetS==0 and nC==0 and nF==0 and n==0) :
		instruction="crab -c "+task+" -publish > "+task+"/log/publish.txt"
		instruction += " ; crab -c "+task+" -report > "+task+"/log/report.txt"

#if nTotal!=0:
if nSuccess==nTotal and pub=="pub":
	print 'echo "Publish"'

print instruction

print 'echo "end script"'

ifile.close()
