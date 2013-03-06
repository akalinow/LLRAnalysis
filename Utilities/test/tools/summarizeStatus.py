import string
import sys

ifile = open(sys.argv[1])
task  = str(sys.argv[2])

# List of numbers of jobs
jobsDone    = {}
jobsNotDone = {}

# List of status of jobs
statusDone    = {}
statusNotDone = {}

# Lenghts of the lists
nDone    = 0
nNotDone = 0

nTotal=0
nRetrieved=0
nExit=0

for line in ifile:
	fields = line.split()

	if len(fields)==0:
		continue

	elif len(fields)==9:
		if str(fields[6])=='Code':
			jobsDone[nDone]  = str(fields[1] )
			statusDone[nDone]= str(fields[-1])

			nExit+=int(jobsDone[nDone])
			nDone+=1

	elif len(fields)==4:
		if str(fields[2])=='Jobs':

			if str(fields[-1])=='Retrieved':
				nRetrieved=int(fields[1])
			else:
				jobsNotDone[nNotDone]   = str(fields[1])
				statusNotDone[nNotDone] = str(fields[-1])
				nNotDone+=1

		elif str(fields[2])=='Total':
			nTotal=int(fields[1])
		
phrase  = 'Task '+task+' : '
phrase += str(nTotal)+' jobs ; Done : '

for i in range(0,nDone):
	phrase += jobsDone[i]+'['+statusDone[i]+']'
	if i<nDone-1:
		phrase += ' , '

phrase += ' ; Not Done : '

for i in range(0,nNotDone):
	phrase += jobsNotDone[i]+' '+statusNotDone[i]
	if i<nNotDone-1:
		phrase += ', '

if nExit!=nRetrieved:
	phrase += ' !ERR='+str(nRetrieved)

print phrase
ifile.close()
