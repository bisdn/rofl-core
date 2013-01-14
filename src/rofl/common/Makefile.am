MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow protocols 

noinst_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES = caddress.h  cmacaddr.h cpacket.h croflexp.h endian_conversion.h rofcommon.h cclock.h cerror.h cfsm.h ciosrv.h cmemory.h coflist.h cphyport.h cport.h csocket.h cvastring.h fframe.h thread_helper.h cxidstore.h crofbase.h cvalue.h \
caddress.cc cclock.cc cerror.cc cfsm.cc   ciosrv.cc cmacaddr.cc cmemory.cc coflist.cc cpacket.cc cphyport.cc cport.cc croflexp.cc csocket.cc fframe.cc cxidstore.cc  crofbase.cc

libcommon_la_LIBADD= openflow/libopenflow.la protocols/libprotocols.la -lrt

library_includedir=$(includedir)/rofl/common
library_include_HEADERS= caddress.h      cmacaddr.h  cpacket.h    croflexp.h    endian_conversion.h  rofcommon.h crofbase.h cvalue.h \
		cclock.h    cerror.h    cfsm.h        ciosrv.h     cmemory.h   coflist.h   cphyport.h  cport.h    csocket.h   cvastring.h  fframe.h             thread_helper.h  cxidstore.h

