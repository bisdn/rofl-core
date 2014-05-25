#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmeterbands_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmeterbands_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmeterbands_test::setUp()
{
}



void
cofmeterbands_test::tearDown()
{
}



void
cofmeterbands_test::testDefaultConstructor()
{
	rofl::openflow::cofmeter_bands mbs;
#ifdef DEBUG
	std::cerr << "mbs: " << std::endl << mbs;
#endif
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == mbs.get_version());
	CPPUNIT_ASSERT(0 == mbs.get_num_of_mbs());
}



void
cofmeterbands_test::testCopyConstructor()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;

	rofl::openflow::cofmeter_bands mbs1(of_version);

	mbs1.add_meter_band_drop();
	mbs1.add_meter_band_drop();
	mbs1.add_meter_band_drop();

	CPPUNIT_ASSERT(1 == mbs1.get_mbs_drop().size());

	mbs1.add_meter_band_drop(0);
	mbs1.add_meter_band_drop(1);
	mbs1.add_meter_band_drop(2);

	CPPUNIT_ASSERT(3 == mbs1.get_mbs_drop().size());

	mbs1.add_meter_band_dscp_remark();
	mbs1.add_meter_band_dscp_remark();
	mbs1.add_meter_band_dscp_remark();

	CPPUNIT_ASSERT(1 == mbs1.get_mbs_dscp_remark().size());

	mbs1.add_meter_band_dscp_remark(0);
	mbs1.add_meter_band_dscp_remark(1);
	mbs1.add_meter_band_dscp_remark(2);

	CPPUNIT_ASSERT(3 == mbs1.get_mbs_dscp_remark().size());

	mbs1.add_meter_band_experimenter();
	mbs1.add_meter_band_experimenter();
	mbs1.add_meter_band_experimenter();

	CPPUNIT_ASSERT(1 == mbs1.get_mbs_experimenter().size());

	mbs1.add_meter_band_experimenter(0);
	mbs1.add_meter_band_experimenter(1);
	mbs1.add_meter_band_experimenter(2);

	CPPUNIT_ASSERT(3 == mbs1.get_mbs_experimenter().size());


	rofl::openflow::cofmeter_bands mbs2(mbs1);

#ifdef DEBUG
	std::cerr << "mbs1:" << std::endl << mbs1;
	std::cerr << "mbs2:" << std::endl << mbs2;
#endif

	CPPUNIT_ASSERT(3 == mbs2.get_mbs_drop().size());
	CPPUNIT_ASSERT(3 == mbs2.get_mbs_dscp_remark().size());
	CPPUNIT_ASSERT(3 == mbs2.get_mbs_experimenter().size());
}




void
cofmeterbands_test::testPackUnpack()
{
	uint8_t	of_version = rofl::openflow13::OFP_VERSION;

	rofl::openflow::cofmeter_bands mbs(of_version);

	uint32_t rate = 0xb1b2b3b4;
	uint32_t burst_size = 0xc1c2c3c4;

	mbs.set_meter_band_drop();
	mbs.set_meter_band_drop().set_rate(rate);
	mbs.set_meter_band_drop().set_burst_size(burst_size);
	mbs.set_meter_band_drop(3);
	mbs.set_meter_band_drop(3).set_rate(rate);
	mbs.set_meter_band_drop(3).set_burst_size(burst_size);

	for (unsigned int i = 0; i < 4; i++) {
		mbs.set_meter_band_dscp_remark(i).set_rate(rate);
		mbs.set_meter_band_dscp_remark(i).set_burst_size(burst_size);
		mbs.set_meter_band_dscp_remark(i).set_prec_level(i);
	}

	mbs.set_meter_band_experimenter().set_rate(rate);
	mbs.set_meter_band_experimenter().set_burst_size(burst_size);
	mbs.set_meter_band_experimenter().set_exp_id(0xe1e2e3e4);
	mbs.set_meter_band_experimenter().set_exp_body().resize(6);
	for (unsigned int i = 0; i < 6; i++) {
		mbs.set_meter_band_experimenter().set_body()[i] = i;
	}

	rofl::cmemory packed(mbs.length());
	mbs.pack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mbs:" << std::endl << mbs;
	std::cerr << "packed:" << std::endl << packed;
#endif

	rofl::openflow::cofmeter_bands clone(of_version);

	clone.unpack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif

	CPPUNIT_ASSERT(2 == clone.get_mbs_drop().size());
	CPPUNIT_ASSERT(4 == clone.get_mbs_dscp_remark().size());
	CPPUNIT_ASSERT(1 == clone.get_mbs_experimenter().size());

	CPPUNIT_ASSERT(0xe1e2e3e4 == clone.get_meter_band_experimenter(0).get_exp_id());
	for (unsigned int i = 0; i < 6; i++) {
		CPPUNIT_ASSERT(i == clone.get_meter_band_experimenter().get_body()[i]);
	}
}



void
cofmeterbands_test::testInvalidLength()
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


