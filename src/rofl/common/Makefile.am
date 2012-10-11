MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow protocols 

noinst_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES = caddress.h cfibentry.h cfwdelem.h cfwdtable.h cmacaddr.h cpacket.h croflexp.h endian_conversion.h rofcommon.h cclock.h cerror.h cfsm.h cfwdentry.h ciosrv.h cmemory.h coflist.h cphyport.h cport.h csocket.h ctlbase.h cvastring.h fframe.h thread_helper.h cadapt.h cxidstore.h \
caddress.cc cclock.cc cerror.cc cfibentry.cc cfsm.cc cfwdelem.cc cfwdentry.cc cfwdtable.cc ciosrv.cc cmacaddr.cc cmemory.cc coflist.cc cpacket.cc cphyport.cc cport.cc croflexp.cc csocket.cc ctlbase.cc fframe.cc cadapt.cc cxidstore.cc

libcommon_la_LIBADD= openflow/libopenflow.la protocols/libprotocols.la -lrt

library_includedir=$(includedir)/rofl/common
library_include_HEADERS= caddress.h  cfibentry.h  cfwdelem.h   cfwdtable.h  cmacaddr.h  cpacket.h  ctlbase.h  cadapt.h  croflexp.h    endian_conversion.h  rofcommon.h \
		cclock.h    cerror.h    cfsm.h       cfwdentry.h  ciosrv.h     cmemory.h   coflist.h   cphyport.h  cport.h    csocket.h   cvastring.h  fframe.h             thread_helper.h  cxidstore.h
