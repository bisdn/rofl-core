#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coftablefeatureprop_test.h"

CPPUNIT_TEST_SUITE_REGISTRATION( coftablefeaturepropTest );

void
coftablefeaturepropTest::setUp()
{
}



void
coftablefeaturepropTest::tearDown()
{
}



void
coftablefeaturepropTest::testDefaultConstructor()
{
	rofl::openflow::coftable_feature_prop prop;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == prop.get_version());
}



void
coftablefeaturepropTest::testCopyConstructor()
{
	rofl::openflow::coftable_feature_prop prop(rofl::openflow12::OFP_VERSION, 5, 18);
	rofl::cmemory mprop(prop.length());
	prop.pack(mprop.somem(), mprop.memlen());

	rofl::openflow::coftable_feature_prop clone;

	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "clone:" << std::endl << clone;

	clone = prop;

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "clone:" << std::endl << clone;


	std::cerr << "mprop:" << std::endl << mprop;
	std::cerr << "mclone:" << std::endl << mclone;



	CPPUNIT_ASSERT(rofl::openflow12::OFP_VERSION == clone.get_version());
	CPPUNIT_ASSERT(5 == clone.get_type());
	CPPUNIT_ASSERT(18 == clone.get_length());
}



void
coftablefeaturepropTest::testGetSetType()
{
	rofl::openflow::coftable_feature_prop prop(rofl::openflow12::OFP_VERSION, 5, 18);
	rofl::cmemory mem(prop.length());
	prop.pack(mem.somem(), mem.memlen());
	CPPUNIT_ASSERT(rofl::openflow12::OFP_VERSION == prop.get_version());
	CPPUNIT_ASSERT(5 == prop.get_type());

	prop.set_type(6);
	CPPUNIT_ASSERT(6 == prop.get_type());
}



void
coftablefeaturepropTest::testGetSetLength()
{
	rofl::openflow::coftable_feature_prop prop(rofl::openflow12::OFP_VERSION, 5, 18);
	rofl::cmemory mem(prop.length());
	prop.pack(mem.somem(), mem.memlen());
	CPPUNIT_ASSERT(rofl::openflow12::OFP_VERSION == prop.get_version());
	CPPUNIT_ASSERT(5 == prop.get_type());

	prop.set_length(10);
	CPPUNIT_ASSERT(10 == prop.get_length());
}



void
coftablefeaturepropTest::testPackUnpack()
{
	rofl::cmemory mem(12);
	for (unsigned int i = 4; i < 12; i++) {
		mem[i] = 0xaa;
	}
	// type
	mem[0] = 0x88;
	mem[1] = 0x8e;
	// length
	mem[2] = 0x00;
	mem[3] = 0x0c;

	rofl::openflow::coftable_feature_prop prop;
	prop.unpack(mem.somem(), mem.memlen());

	CPPUNIT_ASSERT(0x888e == prop.get_type());
	CPPUNIT_ASSERT(12 == prop.get_length());

	std::cerr << "mem:" << std::endl << mem;
	std::cerr << "prop:" << std::endl << prop;
}



void
coftablefeaturepropTest::testInstructionsClass()
{
	rofl::openflow::coftable_feature_prop_instructions prop;

	prop.push_back(rofl::cofinst(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPIT_APPLY_ACTIONS));
	prop.push_back(rofl::cofinst(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPIT_CLEAR_ACTIONS));
	prop.push_back(rofl::cofinst(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPIT_WRITE_ACTIONS));
	prop.push_back(rofl::cofinst(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPIT_WRITE_METADATA));
	prop.push_back(rofl::cofinst(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPIT_GOTO_TABLE));

	rofl::cmemory mem(prop.length());

	CPPUNIT_ASSERT(24 == prop.length()); // including padding

	prop.pack(mem.somem(), mem.memlen());

	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "mem:" << std::endl << mem;
}


