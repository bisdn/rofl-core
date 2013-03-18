MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = extensions 

noinst_LTLIBRARIES = libopenflow.la

libopenflow_la_SOURCES= cflowentry.h cofaclist.h cofactionset.h cofbclist.h cofctl.h cofinlist.h cofmatch.h cofport.h coxmlist.h openflow.h openflow13.h openflow12.h openflow11.h openflow10.h cgroupentry.h cofaction.h cofbucket.h cofdpt.h cofinst.h cofpacket.h cofportlist.h coxmatch.h openflow_rofl.h openflow_rofl_exceptions.h cofstats.h cofdescstats.h cofflowstats.h cofaggrstats.h coftablestats.h cofportstats.h cofqueuestats.h cofgroupstats.h \
	 cflowentry.cc cofaclist.cc cofactionset.cc cofbclist.cc cofctl.cc cofinlist.cc cofmatch.cc cofport.cc coxmlist.cc cgroupentry.cc cofaction.cc cofbucket.cc cofdpt.cc cofinst.cc cofpacket.cc cofportlist.cc coxmatch.cc cofstats.cc cofdescstats.cc cofflowstats.cc cofaggrstats.cc coftablestats.cc cofportstats.cc cofqueuestats.cc cofgroupstats.cc

libopenflow_la_LIBADD= extensions/libopenflow_extensions.la

library_includedir=$(includedir)/rofl/common/openflow
library_include_HEADERS = cflowentry.h cofaclist.h cofactionset.h cofbclist.h cofctl.h cofinlist.h \
			  cofmatch.h cofport.h coxmlist.h openflow.h openflow13.h openflow12.h openflow11.h openflow10.h \
		   cgroupentry.h  cofaction.h       cofbucket.h  cofdpt.h  cofinst.h    cofpacket.h  cofportlist.h  coxmatch.h   openflow_rofl.h openflow_rofl_exceptions.h cofstats.h cofdescstats.h cofflowstats.h cofaggrstats.h coftablestats.h cofportstats.h cofqueuestats.h cofgroupstats.h
 
