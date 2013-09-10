MAINTAINERCLEANFILES = Makefile.in

if ONLY_C_CODE
SUBDIRS = rofl/datapath/pipeline/openflow/openflow1x/pipeline
else
SUBDIRS = unit rofl/datapath/pipeline/openflow/openflow1x/pipeline
endif

export INCLUDES += -I$(abs_srcdir)/../src/

