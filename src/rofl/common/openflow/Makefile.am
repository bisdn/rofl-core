MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = extensions 

noinst_LTLIBRARIES = libopenflow.la

libopenflow_la_SOURCES= cflowentry.h cfttable.h cgtentry.h cofaclist.h cofactionset.h cofbclist.h cofctl.h cofinlist.h cofmatch.h cofport.h cofrpc.h coxmlist.h openflow12.h cftentry.h cgroupentry.h cgttable.h cofaction.h cofbase.h cofbucket.h cofdpt.h cofinst.h cofpacket.h cofportlist.h coxmatch.h openflow11.h openflow_rofl.h cofstats.h \
	 cflowentry.cc cfttable.cc cgtentry.cc cofaclist.cc cofactionset.cc cofbclist.cc cofctl.cc cofinlist.cc cofmatch.cc cofport.cc cofrpc.cc coxmlist.cc cftentry.cc cgroupentry.cc cgttable.cc cofaction.cc cofbase.cc cofbucket.cc cofdpt.cc cofinst.cc cofpacket.cc cofportlist.cc coxmatch.cc cofstats.cc

libopenflow_la_LIBADD= extensions/libopenflow_extensions.la

library_includedir=$(includedir)/rofl/common/openflow
library_include_HEADERS = cflowentry.h  cfttable.h     cgtentry.h  cofaclist.h  cofactionset.h  cofbclist.h  cofctl.h   cofinlist.h  cofmatch.h   cofport.h      cofrpc.h    coxmlist.h    openflow12.h\
		  cftentry.h    cgroupentry.h  cgttable.h  cofaction.h  cofbase.h       cofbucket.h  cofdpt.h  cofinst.h    cofpacket.h  cofportlist.h  coxmatch.h  openflow11.h  openflow_rofl.h cofstats.h
 
