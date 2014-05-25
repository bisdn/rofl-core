#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmeterbandstatsarray_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmeterbandstatsarray_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmeterbandstatsarray_test::setUp()
{
}



void
cofmeterbandstatsarray_test::tearDown()
{
}



void
cofmeterbandstatsarray_test::testDefaultConstructor()
{
	rofl::openflow::cofmeter_band_stats_array mbstats;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == mbstats.get_version());
	CPPUNIT_ASSERT(0 == mbstats.get_mbs_array().size());
}



void
cofmeterbandstatsarray_test::testCopyConstructor()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;
	uint64_t packet_band_count = 0xa1a2a3a4a5a6a7a8;
	uint64_t byte_band_count = 0xb1b2b3b4b5b6b7b8;

	rofl::openflow::cofmeter_band_stats_array mbs1(of_version);

	mbs1.add_mbstats();
	mbs1.add_mbstats(0);
	mbs1.add_mbstats(1);

	CPPUNIT_ASSERT(2 == mbs1.get_mbs_array().size());

	mbs1.clear();

	for (unsigned int i = 0; i < 4; i++) {
		mbs1.set_mbstats(i).set_packet_band_count(packet_band_count);
		mbs1.set_mbstats(i).set_byte_band_count(byte_band_count);
	}

	CPPUNIT_ASSERT(4 == mbs1.get_mbs_array().size());

	rofl::openflow::cofmeter_band_stats_array mbs2(mbs1);

#ifdef DEBUG
	std::cerr << "mbs1:" << std::endl << mbs1;
	std::cerr << "mbs2:" << std::endl << mbs2;
#endif

	CPPUNIT_ASSERT(4 == mbs2.get_mbs_array().size());

	for (unsigned int i = 0; i < 4; i++) {
		CPPUNIT_ASSERT(mbs2.get_mbstats(i).get_packet_band_count() == packet_band_count);
		CPPUNIT_ASSERT(mbs2.set_mbstats(i).get_byte_band_count() == byte_band_count);
	}
}




void
cofmeterbandstatsarray_test::testPackUnpack()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;
	uint64_t packet_band_count = 0xa1a2a3a4a5a6a7a8;
	uint64_t byte_band_count = 0xb1b2b3b4b5b6b7b8;

	rofl::openflow::cofmeter_band_stats_array mbs1(of_version);

	for (unsigned int i = 0; i < 4; i++) {
		mbs1.set_mbstats(i).set_packet_band_count(packet_band_count);
		mbs1.set_mbstats(i).set_byte_band_count(byte_band_count);
	}

	rofl::cmemory packed(mbs1.length());
	mbs1.pack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mbs1:" << std::endl << mbs1;
	std::cerr << "packed:" << std::endl << packed;
#endif

	for (unsigned int j = 0; j < 4; j++) {
		for (unsigned int i = 0; i < 8; i++) {
			CPPUNIT_ASSERT(0xa1 + i == packed[16*j+i+0]);
		}
		for (unsigned int i = 0; i < 8; i++) {
			CPPUNIT_ASSERT(0xb1 + i == packed[16*j+i+8]);
		}
	}

	rofl::openflow::cofmeter_band_stats_array mbs2(of_version);

	mbs2.unpack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mbs2:" << std::endl << mbs2;
#endif

	CPPUNIT_ASSERT(4 == mbs2.get_mbs_array().size());

	for (unsigned int i = 0; i < 4; i++) {
		CPPUNIT_ASSERT(mbs2.get_mbstats(i).get_packet_band_count() == packet_band_count);
		CPPUNIT_ASSERT(mbs2.set_mbstats(i).get_byte_band_count() == byte_band_count);
	}
}



