MAINTAINERCLEANFILES = aclocal.m4 config.guess config.sub configure \
depcomp install-sh ltmain.sh Makefile.in missing

SUBDIRS = src 

install-exec-hook:
	@echo -n "Running ldconfig for you..."
	@ldconfig
	@echo "Done"

uninstall-hook:
	@echo "Cleaning the house... Done!"

maintainer-clean-local:
	rm -rf m4
