#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmeterstatsarray_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmeterstatsarray_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmeterstatsarray_test::setUp()
{
}



void
cofmeterstatsarray_test::tearDown()
{
}



void
cofmeterstatsarray_test::testDefaultConstructor()
{
	rofl::openflow::cofmeterstatsarray mstats;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == mstats.get_version());
	CPPUNIT_ASSERT(0 == mstats.get_mstats().size());
}



void
cofmeterstatsarray_test::testCopyConstructor()
{
	unsigned int num_of_mstats	= 5;
	uint32_t meter_id			= 0xa1a2a3a4;
	uint32_t flow_count			= 0xb1b2b3b4;
	uint64_t packet_in_count	= 0xc1c2c3c4c5c6c7c8;
	uint64_t byte_in_count		= 0xd1d2d3d4d5d6d7d8;
	uint32_t duration_sec		= 0xe1e2e3e4;
	uint32_t duration_nsec		= 0xf1f2f3f4;

	rofl::openflow::cofmeterstatsarray mstats1(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < num_of_mstats; i++) {
		mstats1.set_meter_stats(i).set_meter_id(meter_id);
		mstats1.set_meter_stats(i).set_flow_count(flow_count);
		mstats1.set_meter_stats(i).set_packet_in_count(packet_in_count);
		mstats1.set_meter_stats(i).set_byte_in_count(byte_in_count);
		mstats1.set_meter_stats(i).set_duration_sec(duration_sec);
		mstats1.set_meter_stats(i).set_duration_nsec(duration_nsec);
	}


#ifdef DEBUG
	std::cerr << "mstats1:" << std::endl << mstats1;
#endif

	rofl::openflow::cofmeterstatsarray mstats2(mstats1);

#ifdef DEBUG
	std::cerr << "mstats2:" << std::endl << mstats2;
#endif

	for (unsigned int i = 0; i < num_of_mstats; i++) {
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_meter_id() 		== meter_id);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_flow_count() 		== flow_count);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_packet_in_count() == packet_in_count);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_byte_in_count() 	== byte_in_count);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_duration_sec() 	== duration_sec);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_duration_nsec() 	== duration_nsec);
	}

	CPPUNIT_ASSERT(mstats1 == mstats2);
}



void
cofmeterstatsarray_test::testPackUnpack()
{
	unsigned int num_of_mstats	= 5;
	uint32_t meter_id			= 0xa1a2a3a4;
	uint32_t flow_count			= 0xb1b2b3b4;
	uint64_t packet_in_count	= 0xc1c2c3c4c5c6c7c8;
	uint64_t byte_in_count		= 0xd1d2d3d4d5d6d7d8;
	uint32_t duration_sec		= 0xe1e2e3e4;
	uint32_t duration_nsec		= 0xf1f2f3f4;

	uint64_t packet_band_count	= 0x1112131415161718;
	uint64_t byte_band_count	= 0x2122232425262728;

	rofl::openflow::cofmeterstatsarray mstats1(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < num_of_mstats; i++) {
		mstats1.set_meter_stats(i).set_meter_id(meter_id);
		mstats1.set_meter_stats(i).set_flow_count(flow_count);
		mstats1.set_meter_stats(i).set_packet_in_count(packet_in_count);
		mstats1.set_meter_stats(i).set_byte_in_count(byte_in_count);
		mstats1.set_meter_stats(i).set_duration_sec(duration_sec);
		mstats1.set_meter_stats(i).set_duration_nsec(duration_nsec);
		for (unsigned int j = 0; j < 3; j++) {
			mstats1.set_meter_stats(i).set_meter_band_stats().set_mbstats(j).set_packet_band_count(packet_band_count);
			mstats1.set_meter_stats(i).set_meter_band_stats().set_mbstats(j).set_byte_band_count(byte_band_count);
		}
	}

	rofl::cmemory packed(mstats1.length());
	mstats1.pack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mstats1:" << std::endl << mstats1;
	std::cerr << "packed:" << std::endl << packed;
#endif

	rofl::openflow::cofmeterstatsarray mstats2(rofl::openflow13::OFP_VERSION);

	mstats2.unpack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mstats2:" << std::endl << mstats2;
#endif

	for (unsigned int i = 0; i < num_of_mstats; i++) {
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_meter_id() 		== meter_id);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_flow_count() 		== flow_count);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_packet_in_count() == packet_in_count);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_byte_in_count() 	== byte_in_count);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_duration_sec() 	== duration_sec);
		CPPUNIT_ASSERT(mstats2.get_meter_stats(i).get_duration_nsec() 	== duration_nsec);
		for (unsigned int j = 0; j < 3; j++) {
			CPPUNIT_ASSERT(mstats1.get_meter_stats(i).get_meter_band_stats().get_mbstats(j).get_packet_band_count() == packet_band_count);
			CPPUNIT_ASSERT(mstats1.get_meter_stats(i).get_meter_band_stats().get_mbstats(j).get_byte_band_count() == byte_band_count);
		}
	}

	CPPUNIT_ASSERT(mstats1 == mstats2);
}


void
cofmeterstatsarray_test::testOperatorPlus()
{
	unsigned int num_of_mstats	= 5;
	uint32_t meter_id			= 0xa1a2a3a4;
	uint32_t flow_count			= 0xb1b2b3b4;
	uint64_t packet_in_count	= 0xc1c2c3c4c5c6c7c8;
	uint64_t byte_in_count		= 0xd1d2d3d4d5d6d7d8;
	uint32_t duration_sec		= 0xe1e2e3e4;
	uint32_t duration_nsec		= 0xf1f2f3f4;

	rofl::openflow::cofmeterstatsarray mstats1(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < num_of_mstats; i++) {
		mstats1.set_meter_stats(i).set_meter_id(meter_id);
		mstats1.set_meter_stats(i).set_flow_count(flow_count);
		mstats1.set_meter_stats(i).set_packet_in_count(packet_in_count);
		mstats1.set_meter_stats(i).set_byte_in_count(byte_in_count);
		mstats1.set_meter_stats(i).set_duration_sec(duration_sec);
		mstats1.set_meter_stats(i).set_duration_nsec(duration_nsec);
	}

	rofl::openflow::cofmeterstatsarray mstats2(rofl::openflow13::OFP_VERSION);
	for (unsigned int i = 0; i < num_of_mstats; i++) {
		mstats2.set_meter_stats(i).set_meter_id(meter_id);
		mstats2.set_meter_stats(i).set_flow_count(flow_count);
		mstats2.set_meter_stats(i).set_packet_in_count(packet_in_count);
		mstats2.set_meter_stats(i).set_byte_in_count(byte_in_count);
		mstats2.set_meter_stats(i).set_duration_sec(duration_sec);
		mstats2.set_meter_stats(i).set_duration_nsec(duration_nsec);
	}


#ifdef DEBUG
	std::cerr << "mstats1:" << std::endl << mstats1;
	std::cerr << "mstats2:" << std::endl << mstats2;
#endif

	rofl::openflow::cofmeterstatsarray mstats3(rofl::openflow13::OFP_VERSION);

	mstats3 += mstats1;
	mstats3 += mstats2;

#ifdef DEBUG
	std::cerr << "mstats3:" << std::endl << mstats3;
#endif

	for (unsigned int i = 0; i < 2*num_of_mstats; i++) {
		CPPUNIT_ASSERT(mstats3.get_meter_stats(i).get_meter_id() 		== meter_id);
		CPPUNIT_ASSERT(mstats3.get_meter_stats(i).get_flow_count() 		== flow_count);
		CPPUNIT_ASSERT(mstats3.get_meter_stats(i).get_packet_in_count() == packet_in_count);
		CPPUNIT_ASSERT(mstats3.get_meter_stats(i).get_byte_in_count() 	== byte_in_count);
		CPPUNIT_ASSERT(mstats3.get_meter_stats(i).get_duration_sec() 	== duration_sec);
		CPPUNIT_ASSERT(mstats3.get_meter_stats(i).get_duration_nsec() 	== duration_nsec);
	}
}



void
cofmeterstatsarray_test::testAddDropSetGetHas()
{
	rofl::openflow::cofmeterstatsarray mstats(rofl::openflow13::OFP_VERSION);

	try {
		mstats.get_meter_stats();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eRofMeterStatsNotFound& e) {};

	if (mstats.has_meter_stats()) {
		CPPUNIT_ASSERT(false);
	}
	mstats.set_meter_stats();

	try {
		mstats.get_meter_stats();
	} catch (rofl::openflow::eRofMeterStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		mstats.set_meter_stats();
	} catch (rofl::openflow::eRofMeterStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not mstats.has_meter_stats()) {
		CPPUNIT_ASSERT(false);
	}

	mstats.drop_meter_stats();

	try {
		mstats.get_meter_stats();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eRofMeterStatsNotFound& e) {};

	if (mstats.has_meter_stats()) {
		CPPUNIT_ASSERT(false);
	}
	mstats.set_meter_stats();
}


