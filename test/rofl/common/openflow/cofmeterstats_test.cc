#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmeterstats_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmeterstats_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmeterstats_test::setUp()
{
}



void
cofmeterstats_test::tearDown()
{
}



void
cofmeterstats_test::testDefaultConstructor()
{
	rofl::openflow::cofmeter_stats_request msreq;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == msreq.get_version());
	CPPUNIT_ASSERT(0 == msreq.get_meter_id());

	rofl::openflow::cofmeter_stats_reply msrep;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == msrep.get_version());
	CPPUNIT_ASSERT(0 == msrep.get_meter_id());
	CPPUNIT_ASSERT(0 == msrep.get_flow_count());
	CPPUNIT_ASSERT(0 == msrep.get_packet_in_count());
	CPPUNIT_ASSERT(0 == msrep.get_byte_in_count());
	CPPUNIT_ASSERT(0 == msrep.get_duration_sec());
	CPPUNIT_ASSERT(0 == msrep.get_duration_nsec());
}



void
cofmeterstats_test::testCopyConstructor()
{
	uint32_t meter_id 			= 0xa1a2a3a4;
	uint32_t flow_count 		= 0xb1b2b3b4;
	uint64_t packet_in_count 	= 0xc1c2c3c4c5c6c7c8;
	uint64_t byte_in_count 		= 0xd1d2d3d4d5d6d7d8;
	uint32_t duration_sec 		= 0xe1e2e3e4;
	uint32_t duration_nsec 		= 0xf1f2f3f4;



	rofl::openflow::cofmeter_stats_request msreq1(rofl::openflow13::OFP_VERSION);
	msreq1.set_meter_id(meter_id);

	rofl::openflow::cofmeter_stats_request msreq2(msreq1);

#ifdef DEBUG
	std::cerr << "msreq1:" << std::endl << msreq1;
	std::cerr << "msreq2:" << std::endl << msreq2;
#endif

	CPPUNIT_ASSERT(msreq1.get_meter_id() == meter_id);

	CPPUNIT_ASSERT(msreq2.get_meter_id() == meter_id);
	CPPUNIT_ASSERT(msreq1 == msreq2);





	rofl::openflow::cofmeter_stats_reply msrep1(rofl::openflow13::OFP_VERSION);
	msrep1.set_meter_id(meter_id);
	msrep1.set_flow_count(flow_count);
	msrep1.set_packet_in_count(packet_in_count);
	msrep1.set_byte_in_count(byte_in_count);
	msrep1.set_duration_sec(duration_sec);
	msrep1.set_duration_nsec(duration_nsec);

	rofl::openflow::cofmeter_stats_reply msrep2(msrep1);

#ifdef DEBUG
	std::cerr << "msrep1:" << std::endl << msrep1;
	std::cerr << "msrep2:" << std::endl << msrep2;
#endif

	CPPUNIT_ASSERT(msrep1.get_meter_id() 		== meter_id);
	CPPUNIT_ASSERT(msrep1.get_flow_count() 		== flow_count);
	CPPUNIT_ASSERT(msrep1.get_packet_in_count() == packet_in_count);
	CPPUNIT_ASSERT(msrep1.get_byte_in_count() 	== byte_in_count);
	CPPUNIT_ASSERT(msrep1.get_duration_sec() 	== duration_sec);
	CPPUNIT_ASSERT(msrep1.get_duration_nsec() 	== duration_nsec);

	CPPUNIT_ASSERT(msrep2.get_meter_id() 		== meter_id);
	CPPUNIT_ASSERT(msrep2.get_flow_count() 		== flow_count);
	CPPUNIT_ASSERT(msrep2.get_packet_in_count() == packet_in_count);
	CPPUNIT_ASSERT(msrep2.get_byte_in_count() 	== byte_in_count);
	CPPUNIT_ASSERT(msrep2.get_duration_sec() 	== duration_sec);
	CPPUNIT_ASSERT(msrep2.get_duration_nsec() 	== duration_nsec);

	CPPUNIT_ASSERT(msrep1 == msrep2);
}



void
cofmeterstats_test::testPackUnpack()
{
	uint32_t meter_id 			= 0xa1a2a3a4;
	uint32_t flow_count 		= 0xb1b2b3b4;
	uint64_t packet_in_count 	= 0xc1c2c3c4c5c6c7c8;
	uint64_t byte_in_count 		= 0xd1d2d3d4d5d6d7d8;
	uint32_t duration_sec 		= 0xe1e2e3e4;
	uint32_t duration_nsec 		= 0xf1f2f3f4;

	rofl::openflow::cofmeter_stats_request msreq1(rofl::openflow13::OFP_VERSION);
	msreq1.set_meter_id(meter_id);

	rofl::cmemory packed1(msreq1.length());
	msreq1.pack(packed1.somem(), packed1.memlen());

#ifdef DEBUG
	std::cerr << "msreq1:" << std::endl << msreq1;
	std::cerr << "packed1:" << std::endl << packed1;
#endif

	rofl::openflow::cofmeter_stats_request msreq2(rofl::openflow13::OFP_VERSION);
	msreq2.unpack(packed1.somem(), packed1.memlen());

#ifdef DEBUG
	std::cerr << "msreq2:" << std::endl << msreq2;
#endif

	CPPUNIT_ASSERT(msreq2.get_meter_id() == meter_id);
	CPPUNIT_ASSERT(msreq1 == msreq2);



	rofl::openflow::cofmeter_stats_reply msrep1(rofl::openflow13::OFP_VERSION);
	msrep1.set_meter_id(meter_id);
	msrep1.set_flow_count(flow_count);
	msrep1.set_packet_in_count(packet_in_count);
	msrep1.set_byte_in_count(byte_in_count);
	msrep1.set_duration_sec(duration_sec);
	msrep1.set_duration_nsec(duration_nsec);

	rofl::cmemory packed2(msrep1.length());
	msrep1.pack(packed2.somem(), packed2.memlen());

#ifdef DEBUG
	std::cerr << "msrep1:" << std::endl << msrep1;
	std::cerr << "packed2:" << std::endl << packed2;
#endif

	rofl::openflow::cofmeter_stats_reply msrep2(rofl::openflow13::OFP_VERSION);
	msrep2.unpack(packed2.somem(), packed2.memlen());

#ifdef DEBUG
	std::cerr << "msrep2:" << std::endl << msrep2;
#endif

	CPPUNIT_ASSERT(msrep2.get_meter_id() 		== meter_id);
	CPPUNIT_ASSERT(msrep2.get_flow_count() 		== flow_count);
	CPPUNIT_ASSERT(msrep2.get_packet_in_count() == packet_in_count);
	CPPUNIT_ASSERT(msrep2.get_byte_in_count() 	== byte_in_count);
	CPPUNIT_ASSERT(msrep2.get_duration_sec() 	== duration_sec);
	CPPUNIT_ASSERT(msrep2.get_duration_nsec() 	== duration_nsec);
	CPPUNIT_ASSERT(msrep1 == msrep2);
}


