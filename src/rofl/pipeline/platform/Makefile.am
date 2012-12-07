MAINTAINERCLEANFILES = Makefile.in

# build this before anything else
BUILT_SOURCES = platform_lock.h

noinst_LTLIBRARIES = librofl_pipeline_platform_thread.la librofl_pipeline_platform.la

# todo to configure.ac
#if USE_MALLOC
MALLOC_SOURCES = posix/memory.c
#endif

# todo to configure.ac
#if USE_PTHREAD
librofl_pipeline_platform_thread_ladir = $(includedir)/rofl/pipeline/platform/posix

librofl_pipeline_platform_thread_la_HEADERS = \
	posix/lock_types.h

librofl_pipeline_platform_thread_la_SOURCES = \
	posix/pthread_atomic_operations.c \
	posix/pthread_lock.c \
	posix/lock_types.h
#endif

# todo to configure.ac
PLATFORM=posix

librofl_pipeline_platform_ladir = $(includedir)/rofl/pipeline/platform

librofl_pipeline_platform_la_HEADERS = \
	cutil.h \
	atomic_operations.h \
	lock.h \
	platform_lock.h \
	platform_hooks.h \
	memory.h

librofl_pipeline_platform_la_SOURCES = \
	cutil.c \
	cutil.h \
	atomic_operations.h \
	lock.h \
	platform_lock.h \
	platform_hooks.h \
	$(MALLOC_SOURCES)
	
librofl_pipeline_platform_la_LIBADD = librofl_pipeline_platform_thread.la

platform_lock.h: platform_lock.sh Makefile
	$(SHELL) $(srcdir)/platform_lock.sh $(PLATFORM) > $(srcdir)/platform_lock.h
