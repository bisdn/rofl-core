#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmeterconfig_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmeterconfig_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmeterconfig_test::setUp()
{
}



void
cofmeterconfig_test::tearDown()
{
}



void
cofmeterconfig_test::testDefaultConstructor()
{
	rofl::openflow::cofmeter_config_request msreq;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == msreq.get_version());
	CPPUNIT_ASSERT(0 == msreq.get_meter_id());

	rofl::openflow::cofmeter_config_reply msrep;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == msrep.get_version());
	CPPUNIT_ASSERT(0 == msrep.get_flags());
	CPPUNIT_ASSERT(0 == msrep.get_meter_id());
}



void
cofmeterconfig_test::testCopyConstructor()
{
	uint16_t flags 				= 0xb1b2;
	uint32_t meter_id 			= 0xa1a2a3a4;



	rofl::openflow::cofmeter_config_request msreq1(rofl::openflow13::OFP_VERSION);
	msreq1.set_meter_id(meter_id);

	rofl::openflow::cofmeter_config_request msreq2(msreq1);

#ifdef DEBUG
	std::cerr << "msreq1:" << std::endl << msreq1;
	std::cerr << "msreq2:" << std::endl << msreq2;
#endif

	CPPUNIT_ASSERT(msreq1.get_meter_id() == meter_id);

	CPPUNIT_ASSERT(msreq2.get_meter_id() == meter_id);
	CPPUNIT_ASSERT(msreq1 == msreq2);





	rofl::openflow::cofmeter_config_reply msrep1(rofl::openflow13::OFP_VERSION);
	msrep1.set_flags(flags);
	msrep1.set_meter_id(meter_id);


	rofl::openflow::cofmeter_config_reply msrep2(msrep1);

#ifdef DEBUG
	std::cerr << "msrep1:" << std::endl << msrep1;
	std::cerr << "msrep2:" << std::endl << msrep2;
#endif

	CPPUNIT_ASSERT(msrep1.get_flags()	 		== flags);
	CPPUNIT_ASSERT(msrep1.get_meter_id() 		== meter_id);

	CPPUNIT_ASSERT(msrep2.get_flags()	 		== flags);
	CPPUNIT_ASSERT(msrep2.get_meter_id() 		== meter_id);

	CPPUNIT_ASSERT(msrep1 == msrep2);
}



void
cofmeterconfig_test::testPackUnpack()
{
	uint16_t flags				= 0xb1b2;
	uint32_t meter_id 			= 0xa1a2a3a4;
	uint32_t rate				= 0xc1c2c3c4;
	uint32_t burst_size			= 0xd1d2d3d4;

	rofl::openflow::cofmeter_config_request msreq1(rofl::openflow13::OFP_VERSION);
	msreq1.set_meter_id(meter_id);

	rofl::cmemory packed1(msreq1.length());
	msreq1.pack(packed1.somem(), packed1.memlen());

#ifdef DEBUG
	std::cerr << "msreq1:" << std::endl << msreq1;
	std::cerr << "packed1:" << std::endl << packed1;
#endif

	rofl::openflow::cofmeter_config_request msreq2(rofl::openflow13::OFP_VERSION);
	msreq2.unpack(packed1.somem(), packed1.memlen());

#ifdef DEBUG
	std::cerr << "msreq2:" << std::endl << msreq2;
#endif

	CPPUNIT_ASSERT(msreq2.get_meter_id() == meter_id);
	CPPUNIT_ASSERT(msreq1 == msreq2);



	rofl::openflow::cofmeter_config_reply msrep1(rofl::openflow13::OFP_VERSION);
	msrep1.set_flags(flags);
	msrep1.set_meter_id(meter_id);
	msrep1.set_meter_bands().set_meter_band_drop(0).set_rate(rate);
	msrep1.set_meter_bands().set_meter_band_drop(0).set_burst_size(burst_size);
	msrep1.set_meter_bands().set_meter_band_drop(1).set_rate(rate);
	msrep1.set_meter_bands().set_meter_band_drop(1).set_burst_size(burst_size);

	rofl::cmemory packed2(msrep1.length());
	msrep1.pack(packed2.somem(), packed2.memlen());

#ifdef DEBUG
	std::cerr << "msrep1:" << std::endl << msrep1;
	std::cerr << "packed2:" << std::endl << packed2;
#endif

	rofl::openflow::cofmeter_config_reply msrep2(rofl::openflow13::OFP_VERSION);
	msrep2.unpack(packed2.somem(), packed2.memlen());

#ifdef DEBUG
	std::cerr << "msrep2:" << std::endl << msrep2;
#endif

	CPPUNIT_ASSERT(msrep2.get_meter_bands().get_num_of_mbs() == 2);
	CPPUNIT_ASSERT(msrep2.get_flags()	 		== flags);
	CPPUNIT_ASSERT(msrep2.get_meter_id() 		== meter_id);
	CPPUNIT_ASSERT(msrep1 == msrep2);
}


