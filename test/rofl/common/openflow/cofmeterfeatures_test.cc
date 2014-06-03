#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmeterfeatures_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmeterfeatures_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmeterfeatures_test::setUp()
{
}



void
cofmeterfeatures_test::tearDown()
{
}



void
cofmeterfeatures_test::testDefaultConstructor()
{
	rofl::openflow::cofmeter_features_reply msrep;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == msrep.get_version());
	CPPUNIT_ASSERT(0 == msrep.get_max_meter());
	CPPUNIT_ASSERT(0 == msrep.get_band_types());
	CPPUNIT_ASSERT(0 == msrep.get_capabilities());
	CPPUNIT_ASSERT(0 == msrep.get_max_bands());
	CPPUNIT_ASSERT(0 == msrep.get_max_color());
}



void
cofmeterfeatures_test::testCopyConstructor()
{
	uint32_t max_meter			= 0xa1a2a3a4;
	uint32_t band_types			= 0xb1b2b3b4;
	uint32_t capabilities		= 0xc1c2c3c4;
	uint8_t	max_bands			= 0xd1;
	uint8_t max_color			= 0xe1;



	rofl::openflow::cofmeter_features_reply msrep1(rofl::openflow13::OFP_VERSION);
	msrep1.set_max_meter(max_meter);
	msrep1.set_band_types(band_types);
	msrep1.set_capabilities(capabilities);
	msrep1.set_max_bands(max_bands);
	msrep1.set_max_color(max_color);


	rofl::openflow::cofmeter_features_reply msrep2(msrep1);

#ifdef DEBUG
	std::cerr << "msrep1:" << std::endl << msrep1;
	std::cerr << "msrep2:" << std::endl << msrep2;
#endif

	CPPUNIT_ASSERT(msrep1.get_max_meter()		== max_meter);
	CPPUNIT_ASSERT(msrep1.get_band_types()		== band_types);
	CPPUNIT_ASSERT(msrep1.get_capabilities()	== capabilities);
	CPPUNIT_ASSERT(msrep1.get_max_bands()		== max_bands);
	CPPUNIT_ASSERT(msrep1.get_max_color()		== max_color);

	CPPUNIT_ASSERT(msrep2.get_max_meter()		== max_meter);
	CPPUNIT_ASSERT(msrep2.get_band_types()		== band_types);
	CPPUNIT_ASSERT(msrep2.get_capabilities()	== capabilities);
	CPPUNIT_ASSERT(msrep2.get_max_bands()		== max_bands);
	CPPUNIT_ASSERT(msrep2.get_max_color()		== max_color);

	CPPUNIT_ASSERT(msrep1 == msrep2);
}



void
cofmeterfeatures_test::testPackUnpack()
{
	uint32_t max_meter			= 0xa1a2a3a4;
	uint32_t band_types			= 0xb1b2b3b4;
	uint32_t capabilities		= 0xc1c2c3c4;
	uint8_t	max_bands			= 0xd1;
	uint8_t max_color			= 0xe1;



	rofl::openflow::cofmeter_features_reply msrep1(rofl::openflow13::OFP_VERSION);
	msrep1.set_max_meter(max_meter);
	msrep1.set_band_types(band_types);
	msrep1.set_capabilities(capabilities);
	msrep1.set_max_bands(max_bands);
	msrep1.set_max_color(max_color);

	rofl::cmemory packed1(msrep1.length());
	msrep1.pack(packed1.somem(), packed1.memlen());

#ifdef DEBUG
	std::cerr << "msrep1:" << std::endl << msrep1;
	std::cerr << "packed1:" << std::endl << packed1;
#endif

	rofl::openflow::cofmeter_features_reply msrep2(rofl::openflow13::OFP_VERSION);
	msrep2.unpack(packed1.somem(), packed1.memlen());

#ifdef DEBUG
	std::cerr << "msrep2:" << std::endl << msrep2;
#endif

	CPPUNIT_ASSERT(msrep2.get_max_meter()		== max_meter);
	CPPUNIT_ASSERT(msrep2.get_band_types()		== band_types);
	CPPUNIT_ASSERT(msrep2.get_capabilities()	== capabilities);
	CPPUNIT_ASSERT(msrep2.get_max_bands()		== max_bands);
	CPPUNIT_ASSERT(msrep2.get_max_color()		== max_color);
	CPPUNIT_ASSERT(msrep1 == msrep2);
}


