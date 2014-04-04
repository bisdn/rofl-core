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
		crandom.h \
		crandom.cc \
		cpipe.h \
		cpipe.cc \
		cclock.h \
		cclock.cc \
		cfsm.h \
		cfsm.cc \
		ctimer.h \
		ctimer.cc \
		ctimers.h \
		ctimers.cc \
		cevent.h \
		cevents.h \
		cevents.cc \
		ciosrv.h \
		ciosrv.cc \
		cmemory.h \
		cmemory.cc \
		coflist.h \
		coflist.cc \
		csocket.h \
		csocket.cc \
		csocket_impl.h \
		csocket_impl.cc \
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
		crofshim.h \
		crofshim.cc \
		logging.h \
		logging.cc \
		csegmentation.h \
		csegmentation.cc \
		csegmsg.h \
		csegmsg.cc \
		cparam.h \
		cparam.cc \
		cparams.h \
		cparams.cc
		
if ROFL_HAVE_OPENSSL
librofl_common_la_SOURCES += \
		csocket_openssl.h \
		csocket_openssl.cc \
		ssl_lib.cc \
		ssl_lib.h \
		ssl_connection.cc \
		ssl_connection.h
endif

librofl_common_la_LIBADD=openflow/libopenflow.la protocols/libprotocols.la utils/librofl_common_utils.la -lrt

library_includedir=$(includedir)/rofl/common
library_include_HEADERS= \
		caddress.h \
		cmacaddr.h \
		cpacket.h \
		croflexp.h \
		crandom.h \
		cpipe.h \
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
		crofshim.h \
		cclock.h \
		croflexception.h \
		cfsm.h \
		ctimer.h \
		ctimers.h \
		cevent.h \
		cevents.h \
		ciosrv.h \
		cmemory.h \
		coflist.h \
		csocket.h \
		csocket_impl.h \
		cvastring.h \
		fframe.h \
		thread_helper.h \
		ctransaction.h \
		ctransactions.h \
		logging.h \
		csegmentation.h \
		csegmsg.h \
		cparam.h \
		cparams.h

if ROFL_HAVE_OPENSSL
library_include_HEADERS += \
		csocket_openssl.h \
		ssl_lib.h \
		ssl_connection.h
endif


AM_CPPFLAGS=-fPIC
