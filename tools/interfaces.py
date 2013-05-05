#!/usr/bin/python
import subprocess
import sys

s_linkadd = "/sbin/ip link add type veth";
s_linkrem = "/sbin/ip link del veth";
s_linkup  = "/sbin/ip link set up dev veth";

if len(sys.argv) == 2:
    if sys.argv[1] == "start":
        for i in range(4):
            #print (s_linkadd.split(" "));
            subprocess.call(s_linkadd.split(" "));
    
        for i in range(8):
            #print (s_linkup[:] + repr(i)).split(" ");
            subprocess.call((s_linkup[:] + repr(i)).split(" "));
    else:
        for i in range(4):
            #print (s_linkrem[:] + repr(2*i)).split(" ");
            subprocess.call((s_linkrem[:] + repr(2*i)).split(" "));
else:
    print sys.argv[0] + " [start|stop]";
    
