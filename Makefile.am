MAINTAINERCLEANFILES = $(top_srcdir)/aclocal.m4 \
	$(top_srcdir)/ar-lib \
	$(top_srcdir)/config.guess \
	$(top_srcdir)/config.sub \
	$(top_srcdir)/configure \
	$(top_srcdir)/depcomp \
	$(top_srcdir)/install-sh \
	$(top_srcdir)/ltmain.sh \
	$(top_srcdir)/Makefile.in \
	$(top_srcdir)/missing \
	$(top_srcdir)/compile \
	$(top_srcdir)/test-driver

export INCLUDES += -I$(abs_srcdir)/src/ -I$(abs_builddir)/src/
SUBDIRS = src test examples tools doc 
export LIBROFL=$(PWD)/src/rofl/librofl.la

#Could be improved.. 
.PHONY: doc
doc:
	@cd doc/ && make doc		

install-exec-hook:

uninstall-hook:

maintainer-clean-local:
	rm -rf $(top_srcdir)/m4

ACLOCAL_AMFLAGS = -I m4
