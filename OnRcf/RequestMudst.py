#!/opt/star/sl64_gcc482/bin/python

# 2016.01.14	Li Yi
# main script to call those matching code

#from sys import argv

#script, filename = argv

def MapOne(particle):

    filename = "embedpp%s200.txt" % particle

    txt = open(filename,"r")
    
    print "Read in %r:" % filename
    
    data = txt.read()
    
    inputlist = data.split("\n")
    
    runidmap = []
    
    for row in inputlist:
        ilist = row.split()
        runidmap.append(ilist)
    
    runidmap = runidmap[:-1]		# delete the last one which is empty
    print(runidmap)
    
    
    
    import subprocess
    
    for irun in runidmap:
        icmd="get_file_list.pl -keys \'path,filename\' -cond \'runnumber=%s,filetype=daq_reco_MuDst,events!=0,trgsetupname=pp200_production_2012,filename~st_physics,production=P12id,storage!=hpss,sanity=1\' -limit 0 -delim \'/\'| grep %s" % (irun[0], irun[1])
        #print(icmd)
        ioutput = subprocess.Popen(icmd, stdout=subprocess.PIPE, shell=True)
        (ipath, err) = ioutput.communicate()
        #print " --> "
        #print(ipath)
        #print "..."
        ifullpath = "root://xrdstar.rcf.bnl.gov:1095/%s" % ipath 
        ifullpath = ifullpath[:-1]
        eventlistpath = "list/%s_%s_%s_%s.txt" % (irun[0], irun[1],particle,irun[2])
        rootcmd = "root4star -b -q MapEmbed2Read.C\'(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")\'" % (ifullpath, eventlistpath,irun[0],irun[1],irun[2],particle)
        print(rootcmd)
        subprocess.call(rootcmd,shell=True)
    
    return 
    
particles=['AntiProton','Proton','Kplus','Kminus','Piplus','Piminus'];

for particle in particles:
    MapOne(particle)



