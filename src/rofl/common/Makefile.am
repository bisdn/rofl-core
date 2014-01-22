MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow protocols utils 

noinst_LTLIBRARIES = librofl_common.la

librofl_common_la_SOURCES = \
		endian_conversion.h \
		rofcommon.h \
		croflexception.h \
		thread_helper.h \
		caddress.h \
		caddress.cc \
		cmacaddr.h \
		cmacaddr.cc \
		cpacket.h \
		cpacket.cc \
		croflexp.h \
		croflexp.cc \
		cclock.h \
		cclock.cc \
		cfsm.h \
		cfsm.cc \
		ciosrv.h \
		ciosrv.cc \
		cmemory.h \
		cmemory.cc \
		coflist.h \
		coflist.cc \
		csocket.h \
		csocket.cc \
		cvastring.h \
		fframe.h \
		fframe.cc \
		ctransaction.h \
		ctransaction.cc \
		ctransactions.h \
		ctransactions.cc \
		crofbase.h \
		crofbase.cc \
		crofctl.h \
		crofctl.cc \
		crofctl_impl.h \
		crofctl_impl.cc \
		crofdpt.h \
		crofdpt.cc \
		crofdpt_impl.h \
		crofdpt_impl.cc \
		crofsock.h \
		crofsock.cc \
		crofconn.h \
		crofconn.cc \
		crofchan.h \
		crofchan.cc \
		logging.h \
		logging.cc

librofl_common_la_LIBADD=openflow/libopenflow.la protocols/libprotocols.la utils/librofl_common_utils.la -lrt

library_includedir=$(includedir)/rofl/common
library_include_HEADERS= \
		caddress.h \
		cmacaddr.h \
		cpacket.h \
		croflexp.h \
		endian_conversion.h \
		rofcommon.h \
		crofbase.h \
		crofctl.h \
		crofctl_impl.h \
		crofdpt.h \
		crofdpt_impl.h \
		crofsock.h \
		crofconn.h \
		crofchan.h \
		cclock.h \
		croflexception.h \
		cfsm.h \
		ciosrv.h \
		cmemory.h \
		coflist.h \
		csocket.h \
		cvastring.h \
		fframe.h \
		thread_helper.h \
		ctransaction.h \
		ctransactions.h \
		logging.h


AM_CPPFLAGS=-fPIC
