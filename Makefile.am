MAINTAINERCLEANFILES = aclocal.m4 \
	ar-lib \
	config.guess \
	config.sub \
	configure \
	depcomp \
	install-sh \
	ltmain.sh \
	Makefile.in \
	missing 

export INCLUDES += -I$(abs_srcdir)/src/
SUBDIRS = src test examples tools doc 
export LIBROFL=$(PWD)/src/rofl/librofl.la

install-exec-hook:

uninstall-hook:

maintainer-clean-local:
	rm -rf m4
