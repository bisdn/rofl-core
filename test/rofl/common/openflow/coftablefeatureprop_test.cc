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
	rofl::openflow::coftable_feature_prop_instructions prop(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPTFPT_INSTRUCTIONS);

	prop.get_instruction_ids().push_back(std::pair<uint16_t, uint16_t>(rofl::openflow13::OFPIT_APPLY_ACTIONS, sizeof(struct rofl::openflow::ofp_instruction)));
	prop.get_instruction_ids().push_back(std::pair<uint16_t, uint16_t>(rofl::openflow13::OFPIT_WRITE_ACTIONS, sizeof(struct rofl::openflow::ofp_instruction)));
	prop.get_instruction_ids().push_back(std::pair<uint16_t, uint16_t>(rofl::openflow13::OFPIT_CLEAR_ACTIONS, sizeof(struct rofl::openflow::ofp_instruction)));
	prop.get_instruction_ids().push_back(std::pair<uint16_t, uint16_t>(rofl::openflow13::OFPIT_WRITE_METADATA, sizeof(struct rofl::openflow::ofp_instruction)));
	prop.get_instruction_ids().push_back(std::pair<uint16_t, uint16_t>(rofl::openflow13::OFPIT_GOTO_TABLE, sizeof(struct rofl::openflow::ofp_instruction)));

	rofl::cmemory mem(prop.length());

	CPPUNIT_ASSERT(24 == prop.length()); // including padding

	prop.pack(mem.somem(), mem.memlen());

	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "mem:" << std::endl << mem;

	prop.clear();

	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "mem:" << std::endl << mem;

	rofl::cmemory mprop(16);
	mprop[0] = 0x00; // OFPTFPT_INSTRUCTIONS_MISS
	mprop[1] = rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS; // OFPTFPT_INSTRUCTIONS_MISS
	mprop[2] = 0x00; // length: 12 bytes (+ 4 bytes padding yielding 16 bytes)
	mprop[3] = 0x0c;
	// instruction APPLY_ACTIONS
	mprop[4] = 0x00;
	mprop[5] = rofl::openflow13::OFPIT_APPLY_ACTIONS;
	mprop[6] = 0x00;
	mprop[7] = sizeof(struct rofl::openflow::ofp_instruction);
	// instructions GOTO_TABLE
	mprop[8] = 0x00;
	mprop[9] = rofl::openflow13::OFPIT_GOTO_TABLE;
	mprop[10] = 0x00;
	mprop[11] = sizeof(struct rofl::openflow::ofp_instruction);

	std::cerr << "mprop:" << std::endl << mprop;

	prop.unpack(mprop.somem(), mprop.memlen());

	std::cerr << "prop:" << std::endl << prop;
	CPPUNIT_ASSERT(rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS == prop.get_type());
	CPPUNIT_ASSERT(12 == prop.get_length());
	CPPUNIT_ASSERT(rofl::openflow13::OFPIT_APPLY_ACTIONS == prop.get_instruction_ids()[0].first);
	CPPUNIT_ASSERT(sizeof(struct rofl::openflow::ofp_instruction) == prop.get_instruction_ids()[0].second);
	CPPUNIT_ASSERT(rofl::openflow13::OFPIT_GOTO_TABLE == prop.get_instruction_ids()[1].first);
	CPPUNIT_ASSERT(sizeof(struct rofl::openflow::ofp_instruction) == prop.get_instruction_ids()[1].second);
}



void
coftablefeaturepropTest::testNextTablesClass()
{
	rofl::openflow::coftable_feature_prop_next_tables prop(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPTFPT_NEXT_TABLES);

}




