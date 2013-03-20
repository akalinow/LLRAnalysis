# arg 1 : list of files already on data_CMS
# arg 2 : number of "_" signs before NumberOfJobs in a file name
# arg 3 : idx of element to look in the line (str(fields[n]))
# arg 4 : dpm directory
# arg 5 : local directory

import string
import sys

ifile = open(sys.argv[1])
N_    = int(sys.argv[2])
idx   = int(sys.argv[3])
dpm   = str(sys.argv[4])
local = str(sys.argv[5])
copy  = str(sys.argv[6]) ## rfcp / cp

def detJobNum( name , N_ ):
	chain = str(name)
	n = int(len(chain))
	n_ = int(N_)
        num=-1
	counter = 0
	ijob1 = ijob2 = ijob3 = 0
	
	for i in range(0,n):
		if chain[i] == "_":
			counter += 1
		if counter==n_ and ijob1==0:
			ijob1 = i
		elif counter==n_+1 and ijob2==0:
			ijob2 = i
		elif counter==n_+2 and ijob3==0:
			ijob3 = i

	num=int(chain[ijob1+1:ijob2])
	ver=int(chain[ijob2+1:ijob3])

	lis=[num,ver]

	return lis


filesLoc = {}
countLoc = 0

fileDoublon = {}
countDoublon = 0

avoid = {}
countAvoid = 0

for line in ifile:
	fields = line.split()
	filesLoc[countLoc] = str( fields[idx] )
	countLoc += 1
ifile.close()

#print filesLoc[countLoc-1]

start=0
end=countLoc
for i in range(start,end) :

	if i in avoid:
		continue

	lis1 = detJobNum( filesLoc[i] , N_ )
	iLast = i

	for j in range(start,end) :

		if j in avoid:
			continue
		
		lis2 = detJobNum( filesLoc[j] , N_ )

		if lis1[0]==lis2[0]:
			if lis2[1]>lis1[1]:
				iLast = j
			avoid[countAvoid] = j
			countAvoid += 1

	fileDoublon[countDoublon] = filesLoc[iLast]
	countDoublon += 1

for j in range(0,countDoublon) :
	print copy+' '+dpm+'/'+fileDoublon[j]+' '+local
