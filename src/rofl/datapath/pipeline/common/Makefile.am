MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = librofl_pipeline_common.la

librofl_pipeline_common_ladir = $(includedir)/rofl/datapath/pipeline/common

librofl_pipeline_common_la_HEADERS = \
	bitmap.h \
	datapacket.h \
	endianness.h \
	ipv6_exthdr.h \
	large_types.h \
	packet_matches.h \
	protocol_constants.h \
	ternary_fields.h \
	wrap_types.h \
	crc32cr.h

librofl_pipeline_common_la_SOURCES = \
	packet_matches.c \
	ternary_fields.c \
	crc32cr.c
