#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmeterbandstats_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmeterbandstats_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmeterbandstats_test::setUp()
{
}



void
cofmeterbandstats_test::tearDown()
{
}



void
cofmeterbandstats_test::testDefaultConstructor()
{
	rofl::openflow::cofmeter_band_stats mbstats;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == mbstats.get_version());
	CPPUNIT_ASSERT(0 == mbstats.get_packet_band_count());
	CPPUNIT_ASSERT(0 == mbstats.get_byte_band_count());
}



void
cofmeterbandstats_test::testCopyConstructor()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;
	uint64_t packet_band_count = 0xa1a2a3a4a5a6a7a8;
	uint64_t byte_band_count = 0xb1b2b3b4b5b6b7b8;

	rofl::openflow::cofmeter_band_stats mbs1(of_version);

	mbs1.set_packet_band_count(packet_band_count);
	mbs1.set_byte_band_count(byte_band_count);

	rofl::openflow::cofmeter_band_stats mbs2(mbs1);

#ifdef DEBUG
	std::cerr << "mbs1:" << std::endl << mbs1;
	std::cerr << "mbs2:" << std::endl << mbs2;
#endif

	CPPUNIT_ASSERT(packet_band_count == mbs2.get_packet_band_count());
	CPPUNIT_ASSERT(byte_band_count == mbs2.get_byte_band_count());

	CPPUNIT_ASSERT(mbs1 == mbs2);
}




void
cofmeterbandstats_test::testPackUnpack()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;
	uint64_t packet_band_count = 0xa1a2a3a4a5a6a7a8;
	uint64_t byte_band_count = 0xb1b2b3b4b5b6b7b8;

	rofl::openflow::cofmeter_band_stats mbs1(of_version);

	mbs1.set_packet_band_count(packet_band_count);
	mbs1.set_byte_band_count(byte_band_count);

	rofl::cmemory packed(mbs1.length());
	mbs1.pack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mbs1:" << std::endl << mbs1;
	std::cerr << "packed:" << std::endl << packed;
#endif

	CPPUNIT_ASSERT(16 == mbs1.length());
	for (unsigned int i = 0; i < 8; i++) {
		CPPUNIT_ASSERT(0xa1 + i == packed[i]);
	}
	for (unsigned int i = 0; i < 8; i++) {
		CPPUNIT_ASSERT(0xb1 + i == packed[i+8]);
	}

	rofl::openflow::cofmeter_band_stats mbs2(of_version);

	mbs2.unpack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mbs2:" << std::endl << mbs2;
#endif

	CPPUNIT_ASSERT(packet_band_count == mbs2.get_packet_band_count());
	CPPUNIT_ASSERT(byte_band_count == mbs2.get_byte_band_count());

	CPPUNIT_ASSERT(mbs1 == mbs2);
}



