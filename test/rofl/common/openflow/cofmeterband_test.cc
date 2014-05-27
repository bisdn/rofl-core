#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmeterband_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmeterband_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmeterband_test::setUp()
{
}



void
cofmeterband_test::tearDown()
{
}



void
cofmeterband_test::testDefaultConstructor()
{
	rofl::openflow::cofmeter_band meterband;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == meterband.get_version());
	CPPUNIT_ASSERT(0 == meterband.get_type());
	CPPUNIT_ASSERT(sizeof(struct rofl::openflow13::ofp_meter_band_header) == meterband.get_length());
	CPPUNIT_ASSERT(0 == meterband.get_rate());
	CPPUNIT_ASSERT(0 == meterband.get_burst_size());
}



void
cofmeterband_test::testCopyConstructor()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;
	uint16_t type = 0xa1a2;
	uint16_t len = sizeof(struct rofl::openflow13::ofp_meter_band_header);
	uint32_t rate = 0xc1c2c3c4;
	uint32_t burst_size = 0xd1d2d3d4;
	rofl::cmemory body(6);
	for (unsigned int i = 0; i < 6; i++) {
		body[i] = i;
	}

	rofl::openflow::cofmeter_band mb1(of_version);
	mb1.set_type(type);
	mb1.set_rate(rate);
	mb1.set_burst_size(burst_size);
	mb1.set_body() = body;

	rofl::openflow::cofmeter_band mb2(mb1);

#ifdef DEBUG
	std::cerr << "mb1:" << std::endl << mb1;
	std::cerr << "mb2:" << std::endl << mb2;
#endif

	CPPUNIT_ASSERT(of_version == mb1.get_version());
	CPPUNIT_ASSERT(type == mb1.get_type());
	CPPUNIT_ASSERT(len == mb1.get_length());
	CPPUNIT_ASSERT(rate == mb1.get_rate());
	CPPUNIT_ASSERT(burst_size == mb1.get_burst_size());
	CPPUNIT_ASSERT(body == mb1.get_body());

	CPPUNIT_ASSERT(of_version == mb2.get_version());
	CPPUNIT_ASSERT(type == mb2.get_type());
	CPPUNIT_ASSERT(len == mb2.get_length());
	CPPUNIT_ASSERT(rate == mb2.get_rate());
	CPPUNIT_ASSERT(burst_size == mb2.get_burst_size());
	CPPUNIT_ASSERT(body == mb2.get_body());

	CPPUNIT_ASSERT(mb1 == mb2);
}




void
cofmeterband_test::testPackUnpack()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;
	uint16_t type = 0xa1a2;
	uint32_t rate = 0xc1c2c3c4;
	uint32_t burst_size = 0xd1d2d3d4;
	rofl::cmemory body(6);
	for (unsigned int i = 0; i < 6; i++) {
		body[i] = i;
	}
	uint16_t len = sizeof(struct rofl::openflow13::ofp_meter_band_header) + body.memlen();

	rofl::cmemory mem(18);
	mem[0] = 0xa1;
	mem[1] = 0xa2;
	mem[2] = 0x00;
	mem[3] = 0x12;
	mem[4] = 0xc1;
	mem[5] = 0xc2;
	mem[6] = 0xc3;
	mem[7] = 0xc4;
	mem[8] = 0xd1;
	mem[9] = 0xd2;
	mem[10] = 0xd3;
	mem[11] = 0xd4;
	mem[12] = 0x00;
	mem[13] = 0x01;
	mem[14] = 0x02;
	mem[15] = 0x03;
	mem[16] = 0x04;
	mem[17] = 0x05;

	rofl::openflow::cofmeter_band mb1(of_version);
	mb1.set_type(type);
	mb1.set_rate(rate);
	mb1.set_burst_size(burst_size);
	mb1.set_body() = body;

	rofl::cmemory packed(mb1.length());
	mb1.pack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mb1:" << std::endl << mb1;
	std::cerr << "packed:" << std::endl << packed;
	std::cerr << "mem:" << std::endl << mem;
#endif

	CPPUNIT_ASSERT(mem == packed);

	rofl::openflow::cofmeter_band mb2(of_version);

	mb2.unpack(mem.somem(), mem.memlen());

#ifdef DEBUG
	std::cerr << "mb1:" << std::endl << mb1;
	std::cerr << "mb2:" << std::endl << mb2;
	std::cerr << "mem:" << std::endl << mem;
#endif

	CPPUNIT_ASSERT(of_version == mb2.get_version());
	CPPUNIT_ASSERT(type == mb2.get_type());
	CPPUNIT_ASSERT(len == mb2.get_length());
	CPPUNIT_ASSERT(rate == mb2.get_rate());
	CPPUNIT_ASSERT(burst_size == mb2.get_burst_size());
	CPPUNIT_ASSERT(body == mb2.get_body());

	CPPUNIT_ASSERT(mb1 == mb2);
}



void
cofmeterband_test::testInvalidLength()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;

	rofl::cmemory mem(18);
	mem[0] = 0xa1;
	mem[1] = 0xa2;
	mem[2] = 0x00;
	mem[3] = 0x12;
	mem[4] = 0xc1;
	mem[5] = 0xc2;
	mem[6] = 0xc3;
	mem[7] = 0xc4;
	mem[8] = 0xd1;
	mem[9] = 0xd2;
	mem[10] = 0xd3;
	mem[11] = 0xd4;
	mem[12] = 0x00;
	mem[13] = 0x01;
	mem[14] = 0x02;
	mem[15] = 0x03;
	mem[16] = 0x04;
	mem[17] = 0x05;

	rofl::openflow::cofmeter_band mb1(of_version);

	try {
		mb1.unpack(mem.somem(), 12); // without body
		CPPUNIT_ASSERT(false);
	} catch (...) {}

	try {
		mb1.unpack(mem.somem(), 2); // even without length field
		CPPUNIT_ASSERT(false);
	} catch (...) {}

	mb1.unpack(mem.somem(), mem.memlen());

	rofl::cmemory packed(18);

	try {
		mb1.pack(packed.somem(), 2);
		CPPUNIT_ASSERT(false);
	} catch (...) {}

	try {
		mb1.pack(packed.somem(), 12);
		CPPUNIT_ASSERT(false);
	} catch (...) {}
}



void
cofmeterband_test::testDscpRemark()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;
	uint16_t type = 0xa1a2;
	uint32_t rate = 0xc1c2c3c4;
	uint32_t burst_size = 0xd1d2d3d4;
	//uint16_t len = sizeof(struct rofl::openflow13::ofp_meter_band_header) + body.memlen();
	uint8_t prec_level = 0xf1;

	rofl::openflow::cofmeter_band_dscp_remark mb1(of_version);
	mb1.set_type(type);
	mb1.set_rate(rate);
	mb1.set_burst_size(burst_size);
	mb1.set_prec_level(prec_level);

	CPPUNIT_ASSERT(prec_level == mb1.get_prec_level());

	rofl::cmemory packed(mb1.length());
	mb1.pack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mb1:" << std::endl << mb1;
	std::cerr << "packed:" << std::endl << packed;
#endif

	CPPUNIT_ASSERT(packed[12] == prec_level);

	rofl::openflow::cofmeter_band_dscp_remark mb2(of_version);
	mb2.unpack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mb2:" << std::endl << mb2;
#endif

	CPPUNIT_ASSERT(mb1 == mb2);
}



void
cofmeterband_test::testExperimenter()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;
	uint16_t type = 0xa1a2;
	uint32_t rate = 0xc1c2c3c4;
	uint32_t burst_size = 0xd1d2d3d4;
	uint32_t exp_id = 0xe1e2e3e4;
	//uint16_t len = sizeof(struct rofl::openflow13::ofp_meter_band_header) + body.memlen();

	rofl::openflow::cofmeter_band_experimenter mb1(of_version);
	mb1.set_type(type);
	mb1.set_rate(rate);
	mb1.set_burst_size(burst_size);
	mb1.set_exp_id(exp_id);
	mb1.set_exp_body().resize(6);
	for (unsigned int i = 0; i < 6; i++) {
		mb1.set_exp_body()[i] = i;
	}

	CPPUNIT_ASSERT(exp_id == mb1.get_exp_id());

	rofl::cmemory packed(mb1.length());
	mb1.pack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mb1:" << std::endl << mb1;
	std::cerr << "packed:" << std::endl << packed;
#endif

	for (unsigned int i = 0; i < 6; i++) {
		CPPUNIT_ASSERT(packed[16+i] == i);
	}
}



