MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = tranny morpheus

morpheus_SOURCES = \
	cportvlan_mapper.cc \
	morpheus_main.cc \
	morpheus.cc \
	morpheus_nested.cc \
	csh_aggregate_stats.cc \
	csh_barrier.cc \
	csh_desc_stats.cc \
	csh_features_request.cc \
	csh_flow_mod.cc \
	csh_flow_stats.cc \
	csh_get_config.cc \
	csh_packet_in.cc \
	csh_packet_out.cc \
	csh_port_mod.cc \
	csh_set_config.cc \
	csh_table_mod.cc \
	csh_table_stats.cc

morpheus_LDADD = ../../src/rofl/librofl.la \
	-lpthread \
	-lrt 

tranny_SOURCES = \
	tranny.cc \
	tranny.h \
	main.cc
tranny_LDADD = ../../src/rofl/librofl.la \
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
