MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow protocols 

noinst_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES = caddress.h cctlelem.h cfibentry.h cfwdelem.h cfwdtable.h cmacaddr.h cmempool.h cpacket.h croflexp.h cutbase.h endian_conversion.h rofcommon.h cclock.h cerror.h cfsm.h cfwdentry.h ciosrv.h cmemory.h coflist.h cport.h csocket.h cvastring.h fframe.h thread_helper.h \
caddress.cc cclock.cc cctlelem.cc cerror.cc cfibentry.cc cfsm.cc cfwdelem.cc cfwdentry.cc cfwdtable.cc ciosrv.cc cmacaddr.cc cmemory.cc cmempool.cc coflist.cc cpacket.cc cport.cc croflexp.cc csocket.cc cutbase.cc fframe.cc

libcommon_la_LIBADD= openflow/libopenflow.la protocols/libprotocols.la -lrt

library_includedir=$(includedir)/rofl/common
library_include_HEADERS= caddress.h  cctlelem.h  cfibentry.h  cfwdelem.h   cfwdtable.h  cmacaddr.h  cmempool.h  cpacket.h  croflexp.h  cutbase.h    endian_conversion.h  rofcommon.h \
		cclock.h    cerror.h    cfsm.h       cfwdentry.h  ciosrv.h     cmemory.h   coflist.h   cport.h    csocket.h   cvastring.h  fframe.h             thread_helper.h	
