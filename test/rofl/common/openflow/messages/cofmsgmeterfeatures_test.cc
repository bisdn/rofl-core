#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgmeterfeatures_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgmeterfeaturesTest );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmsgmeterfeaturesTest::setUp()
{
}



void
cofmsgmeterfeaturesTest::tearDown()
{
}



void
cofmsgmeterfeaturesTest::testDefaultConstructor()
{
	rofl::openflow::cofmsg_meter_features_stats_request request;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == request.get_version());
	rofl::openflow::cofmsg_meter_features_stats_reply reply;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == reply.get_version());
}



void
cofmsgmeterfeaturesTest::testPackUnpack()
{
	uint32_t xid = 0xa1a2a3a4;
	uint16_t stats_flags = 0xb1b2;


	rofl::openflow::cofmsg_meter_features_stats_request req1(rofl::openflow13::OFP_VERSION, xid, stats_flags);

	rofl::cmemory packed1(req1.length());
	req1.pack(packed1.somem(), packed1.memlen());

#ifdef DEBUG
	std::cerr << "req1:" << std::endl << req1;
	std::cerr << "packed1:" << std::endl << packed1;
#endif

	rofl::openflow::cofmsg_meter_features_stats_request req2(rofl::openflow13::OFP_VERSION);

	req2.unpack(packed1.somem(), packed1.memlen());

#ifdef DEBUG
	std::cerr << "req2:" << std::endl << req2;
#endif





	rofl::openflow::cofmsg_meter_features_stats_reply rep1(rofl::openflow13::OFP_VERSION, xid, stats_flags);

	rofl::cmemory packed2(rep1.length());
	rep1.pack(packed2.somem(), packed2.memlen());

#ifdef DEBUG
	std::cerr << "rep1:" << std::endl << rep1;
	std::cerr << "packed2:" << std::endl << packed2;
#endif

	rofl::openflow::cofmsg_meter_features_stats_reply rep2(rofl::openflow13::OFP_VERSION);

	rep2.unpack(packed2.somem(), packed2.memlen());

#ifdef DEBUG
	std::cerr << "rep2:" << std::endl << rep2;
#endif

}





