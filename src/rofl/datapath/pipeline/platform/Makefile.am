MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = librofl_pipeline_platform.la

librofl_pipeline_platform_ladir = $(includedir)/rofl/datapath/pipeline/platform

if LOCK_INLINE 
LOCK_INL_HDR=lock_inline.h
else 
LOCK_INL_HDR=
endif

if ATOMIC_OPS_INLINE 
ATOMIC_OPS_INL_HDR=atomic_ops_inline.h
else 
ATOMIC_OPS_INL_HDR=
endif

librofl_pipeline_platform_la_HEADERS = \
	atomic_operations.h \
	lock.h \
	$(LOCK_INL_HDR) \
	$(ATOMIC_OPS_INL_HDR) \
	memory.h \
	packet.h

librofl_pipeline_platform_la_SOURCES = \
	atomic_operations.h \
	lock.h \
	$(LOCK_INL_HDR) \
	$(ATOMIC_OPS_INL_HDR) \
	memory.h \
	packet.h
