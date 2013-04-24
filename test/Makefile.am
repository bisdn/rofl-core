MAINTAINERCLEANFILES = Makefile.in

if ONLY_C_CODE
SUBDIRS = rofl/datapath/pipeline/openflow/openflow12/pipeline
else
SUBDIRS = unit rofl/datapath/pipeline/openflow/openflow12/pipeline
endif

export INCLUDES += -I$(abs_srcdir)/../src/

