MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow protocols utils 

noinst_LTLIBRARIES = librofl_common.la

librofl_common_la_SOURCES = \
		endian_conversion.h \
		rofcommon.h \
		croflexception.h \
		thread_helper.h \
		cthread.h \
		caddress.h \
		caddress.cc \
		cpacket.h \
		cpacket.cc \
		crandom.h \
		crandom.cc \
		cpipe.h \
		cpipe.cc \
		cclock.h \
		cclock.cc \
		ctimespec.h \
		ctimespec.cc \
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
		crofdpt.h \
		crofdpt.cc \
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
		cparams.cc \
		ctimerid.h \
		ctimerid.cc \
		cauxid.h \
		cctlid.h \
		cdptid.h \
		csockaddr.h \
		csockaddr.cc \
		caddrinfo.h \
		caddrinfo.cc \
		caddrinfos.h \
		caddrinfos.cc \
		cindex.h \
		cdpid.h \
		crofqueue.h
		
if ROFL_HAVE_OPENSSL
librofl_common_la_SOURCES += \
		csocket_openssl.h \
		csocket_openssl.cc
endif

librofl_common_la_LIBADD=openflow/libopenflow.la protocols/libprotocols.la utils/librofl_common_utils.la -lrt

library_includedir=$(includedir)/rofl/common
library_include_HEADERS= \
		caddress.h \
		cpacket.h \
		crandom.h \
		cpipe.h \
		endian_conversion.h \
		rofcommon.h \
		crofbase.h \
		crofctl.h \
		crofdpt.h \
		crofsock.h \
		crofconn.h \
		crofchan.h \
		crofshim.h \
		cclock.h \
		croflexception.h \
		ctimespec.h \
		ctimer.h \
		ctimers.h \
		cevent.h \
		cevents.h \
		ciosrv.h \
		cmemory.h \
		csocket.h \
		csocket_impl.h \
		cvastring.h \
		fframe.h \
		thread_helper.h \
		cthread.h \
		ctransaction.h \
		ctransactions.h \
		logging.h \
		csegmentation.h \
		csegmsg.h \
		cparam.h \
		cparams.h \
		ctimerid.h \
		cauxid.h \
		cctlid.h \
		cdptid.h \
		csockaddr.h \
		caddrinfo.h \
		caddrinfos.h \
		cindex.h \
		cdpid.h \
		crofqueue.h

if ROFL_HAVE_OPENSSL
library_include_HEADERS += \
		csocket_openssl.h
endif


AM_CPPFLAGS=-fPIC
