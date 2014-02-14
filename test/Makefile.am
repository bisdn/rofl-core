MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = rofl/datapath/pipeline/openflow/openflow1x/pipeline\
	rofl/datapath/pipeline/monitoring

SUBDIRS += rofl

if !ONLY_C_CODE
SUBDIRS += unit 
endif

export INCLUDES += -I$(abs_srcdir)/../src/

