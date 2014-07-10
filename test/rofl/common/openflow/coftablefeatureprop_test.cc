#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coftablefeatureprop_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( coftablefeaturepropTest );

#if defined DEBUG
#undef DEBUG
#endif

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

#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "clone:" << std::endl << clone;
#endif

	clone = prop;

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "clone:" << std::endl << clone;

	std::cerr << "mprop:" << std::endl << mprop;
	std::cerr << "mclone:" << std::endl << mclone;
#endif



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

#ifdef DEBUG
	std::cerr << "mem:" << std::endl << mem;
	std::cerr << "prop:" << std::endl << prop;
#endif
}



void
coftablefeaturepropTest::testInstructionsClass()
{
	rofl::openflow::coftable_feature_prop_instructions prop(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPTFPT_INSTRUCTIONS);

	prop.add_instruction(rofl::openflow13::OFPIT_APPLY_ACTIONS, sizeof(struct rofl::openflow::ofp_instruction));
	prop.add_instruction(rofl::openflow13::OFPIT_WRITE_ACTIONS, sizeof(struct rofl::openflow::ofp_instruction));
	prop.add_instruction(rofl::openflow13::OFPIT_CLEAR_ACTIONS, sizeof(struct rofl::openflow::ofp_instruction));
	prop.add_instruction(rofl::openflow13::OFPIT_WRITE_METADATA, sizeof(struct rofl::openflow::ofp_instruction));
	prop.add_instruction(rofl::openflow13::OFPIT_GOTO_TABLE, sizeof(struct rofl::openflow::ofp_instruction));

	rofl::cmemory mem(prop.length());

	CPPUNIT_ASSERT(24 == prop.length()); // including padding

	prop.pack(mem.somem(), mem.memlen());

#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "mem:" << std::endl << mem;
#endif

	prop.clear();

#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "mem:" << std::endl << mem;
#endif

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

#ifdef DEBUG
	std::cerr << "mprop:" << std::endl << mprop;
#endif

	prop.unpack(mprop.somem(), mprop.memlen());

#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif
	CPPUNIT_ASSERT(rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS == prop.get_type());
	CPPUNIT_ASSERT(12 == prop.get_length());
	CPPUNIT_ASSERT(rofl::openflow13::OFPIT_APPLY_ACTIONS == prop.get_instruction_ids()[0].type);
	CPPUNIT_ASSERT(sizeof(struct rofl::openflow::ofp_instruction) == prop.get_instruction_ids()[0].len);
	CPPUNIT_ASSERT(rofl::openflow13::OFPIT_GOTO_TABLE == prop.get_instruction_ids()[1].type);
	CPPUNIT_ASSERT(sizeof(struct rofl::openflow::ofp_instruction) == prop.get_instruction_ids()[1].len);
}



void
coftablefeaturepropTest::testNextTablesClass()
{
	rofl::openflow::coftable_feature_prop_next_tables prop(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPTFPT_NEXT_TABLES);


	for (unsigned int i = 0; i < 26; i++) {
		prop.push_back(i);
	}

#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif
	CPPUNIT_ASSERT(32 == prop.length());

	rofl::cmemory mem(prop.length());

	prop.pack(mem.somem(), mem.memlen());

	for (unsigned int i = 0; i < 26; i++) {
		CPPUNIT_ASSERT(mem[4+i] == i);
	}
	CPPUNIT_ASSERT(30 == prop.get_length());
#ifdef DEBUG
	std::cerr << "mem:" << std::endl << mem;
#endif
	prop.clear();
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif
	prop.unpack(mem.somem(), mem.memlen());

	CPPUNIT_ASSERT(32 == prop.length());
	for (unsigned int i = 0; i < 26; i++) {
		CPPUNIT_ASSERT(mem[4+i] == i);
	}
	CPPUNIT_ASSERT(30 == prop.get_length());
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif


	mem[3] = 0x00; // force length field to 0x00 => unpack must fail with exception

	try {
		prop.unpack(mem.somem(), mem.memlen());
		// must not be reached due to exception
		CPPUNIT_ASSERT(0 == 1);
	} catch (rofl::openflow::eOFTableFeaturePropBase& e) {}
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif


	mem[3] = 0x04; // force length field to 0x04 => unpack won't fail, but no table-ids will be extracted

	prop.unpack(mem.somem(), mem.memlen());
	CPPUNIT_ASSERT(prop.size() == 0); // no tables should be extracted
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif
}



void
coftablefeaturepropTest::testActionsClass()
{
	rofl::openflow::coftable_feature_prop_actions prop(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPTFPT_APPLY_ACTIONS);

	prop.add_action(rofl::openflow13::OFPAT_GROUP, sizeof(struct rofl::openflow13::ofp_action_group));
	prop.add_action(rofl::openflow13::OFPAT_OUTPUT, sizeof(struct rofl::openflow13::ofp_action_output));
	prop.add_action(rofl::openflow13::OFPAT_SET_FIELD, sizeof(struct rofl::openflow13::ofp_action_set_field));
	prop.add_action(rofl::openflow13::OFPAT_DEC_NW_TTL, sizeof(struct rofl::openflow13::ofp_action_header));
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif
	rofl::cmemory mem(prop.length());

	prop.pack(mem.somem(), mem.memlen());
#ifdef DEBUG
	std::cerr << "mem:" << std::endl << mem;
#endif
	CPPUNIT_ASSERT(24 == prop.length());
	CPPUNIT_ASSERT(20 == prop.get_length());

	prop.clear();
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif
	prop.unpack(mem.somem(), mem.memlen());
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif


	mem[3] = 0x00; // force length field to 0x00 => unpack must fail with exception

	try {
		prop.unpack(mem.somem(), mem.memlen());
		// must not be reached due to exception
		CPPUNIT_ASSERT(0 == 1);
	} catch (rofl::openflow::eOFTableFeaturePropBase& e) {}
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif


	mem[3] = 0x04; // force length field to 0x04 => unpack won't fail, but no table-ids will be extracted

	prop.unpack(mem.somem(), mem.memlen());
	CPPUNIT_ASSERT(prop.get_action_ids().size() == 0); // no tables should be extracted
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif
}



void
coftablefeaturepropTest::testOxmClass()
{
	rofl::openflow::coftable_feature_prop_oxm prop(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPTFPT_MATCH);

	prop.set_oxm_ids().push_back(rofl::openflow::OXM_TLV_BASIC_ARP_OP);
	prop.set_oxm_ids().push_back(rofl::openflow::OXM_TLV_BASIC_IPV4_DST);
	prop.set_oxm_ids().push_back(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL);
	prop.set_oxm_ids().push_back(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK);
	prop.set_oxm_ids().push_back(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK);
	prop.set_oxm_ids().push_back(rofl::openflow::OXM_TLV_BASIC_MPLS_TC);

	prop.set_oxm_ids_exp().push_back(((uint64_t)rofl::openflow::OFPXMC_EXPERIMENTER << 48) | 0x0000a3a4b1b2b3b4);
	prop.set_oxm_ids_exp().push_back(((uint64_t)rofl::openflow::OFPXMC_EXPERIMENTER << 48) | 0x0000c3c4d1d2d3d4);

	rofl::cmemory mem(prop.length());

	prop.pack(mem.somem(), mem.memlen());
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "mem:" << std::endl << mem;
#endif
	prop.clear();
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif
	prop.unpack(mem.somem(), mem.memlen());
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
	std::cerr << "mem:" << std::endl << mem << std::endl;
#endif
	CPPUNIT_ASSERT(6 == prop.get_oxm_ids().size());
	CPPUNIT_ASSERT(2 == prop.get_oxm_ids_exp().size());

	CPPUNIT_ASSERT((uint32_t)rofl::openflow::OXM_TLV_BASIC_ARP_OP 			== prop.get_oxm_ids()[0]);
	CPPUNIT_ASSERT((uint32_t)rofl::openflow::OXM_TLV_BASIC_IPV4_DST 		== prop.get_oxm_ids()[1]);
	CPPUNIT_ASSERT((uint32_t)rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL 		== prop.get_oxm_ids()[2]);
	CPPUNIT_ASSERT((uint32_t)rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK 	== prop.get_oxm_ids()[3]);
	CPPUNIT_ASSERT((uint32_t)rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK 	== prop.get_oxm_ids()[4]);
	CPPUNIT_ASSERT((uint32_t)rofl::openflow::OXM_TLV_BASIC_MPLS_TC 			== prop.get_oxm_ids()[5]);
	CPPUNIT_ASSERT((((uint64_t)rofl::openflow::OFPXMC_EXPERIMENTER << 48) | 0x0000a3a4b1b2b3b4) == prop.get_oxm_ids_exp()[0]);
	CPPUNIT_ASSERT((((uint64_t)rofl::openflow::OFPXMC_EXPERIMENTER << 48) | 0x0000c3c4d1d2d3d4) == prop.get_oxm_ids_exp()[1]);

	prop.clear();



	mem[3] = 0x00; // force length to 0x00

	try {
		prop.unpack(mem.somem(), mem.memlen());
		CPPUNIT_ASSERT(0 == 1);
	} catch (rofl::openflow::eOFTableFeaturePropInval& e) {}
#ifdef DEBUG
	std::cerr << "prop:" << std::endl << prop;
#endif
	CPPUNIT_ASSERT(0 == prop.get_oxm_ids().size());
	CPPUNIT_ASSERT(0 == prop.get_oxm_ids_exp().size());




	mem[3] = 0x04; // force length to 0x04

	try {
		prop.unpack(mem.somem(), mem.memlen());
	} catch (rofl::openflow::eOFTableFeaturePropInval& e) {
		CPPUNIT_ASSERT(0 == 1);
	}
#ifdef DEBUG
	std::cerr << "mem:" << std::endl << mem;
	std::cerr << "get_oxm_ids().size() " << prop.get_oxm_ids().size() << std::endl;
	std::cerr << "prop:" << std::endl << prop;
#endif
	CPPUNIT_ASSERT(0 == prop.get_oxm_ids().size());
	CPPUNIT_ASSERT(0 == prop.get_oxm_ids_exp().size());
}


