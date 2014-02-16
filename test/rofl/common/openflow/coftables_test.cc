#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coftables_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( coftablesTest );

#if defined DEBUG
//#undef DEBUG
#endif

void
coftablesTest::setUp()
{
}



void
coftablesTest::tearDown()
{
}



void
coftablesTest::testDefaultConstructor()
{
	rofl::openflow::coftables tables;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == tables.get_version());
}



void
coftablesTest::testCopyConstructor()
{
	rofl::openflow::coftables tables(rofl::openflow13::OFP_VERSION);

	tables.add_table(0).set_max_entries(1024);
	tables.add_table(1).set_max_entries(1024);

	rofl::cmemory mtables(tables.length());
	tables.pack(mtables.somem(), mtables.memlen());

#ifdef DEBUG
	std::cerr << "tables:" << std::endl << tables;
	std::cerr << "mtables:" << std::endl << mtables;
#endif

	rofl::openflow::coftables clone(tables);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(mtables == mclone);
}



void
coftablesTest::testPackUnpack()
{
	rofl::openflow::coftables tables(rofl::openflow13::OFP_VERSION);

	tables.add_table(0).set_config(0xc1c2c3c4);
	tables.set_table(0).set_max_entries(1024);
	tables.set_table(0).set_metadata_match(0xe1e2e3e4e5e6e7e8);
	tables.set_table(0).set_metadata_write(0xf1f2f3f4f5f6f7f8);
	tables.set_table(0).set_name("table0");
	tables.set_table(0).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_WRITE_ACTIONS);
	tables.set_table(0).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_APPLY_ACTIONS);
	tables.set_table(0).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_CLEAR_ACTIONS);
	tables.set_table(0).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_WRITE_METADATA);
	tables.set_table(0).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_GOTO_TABLE);
	tables.set_table(0).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_METER);
	tables.set_table(0).set_properties().set_tfp_next_tables().add_table_id(1);
	tables.set_table(0).set_properties().set_tfp_next_tables().add_table_id(5);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PORT);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_SRC);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_SRC_MASK);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_VID);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_OP);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SHA);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SHA_MASK);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_THA);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_THA_MASK);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SPA);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SPA_MASK);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_TPA);
	tables.set_table(0).set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_TPA_MASK);
	tables.set_table(0).set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_OUTPUT);
	tables.set_table(0).set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_COPY_TTL_IN);
	tables.set_table(0).set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_COPY_TTL_OUT);

	tables.add_table(1).set_config(0xc1c2c3c4);
	tables.set_table(1).set_max_entries(1024);
	tables.set_table(1).set_metadata_match(0xe1e2e3e4e5e6e7e8);
	tables.set_table(1).set_metadata_write(0xf1f2f3f4f5f6f7f8);
	tables.set_table(1).set_name("table1");
	tables.set_table(1).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_WRITE_ACTIONS);
	tables.set_table(1).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_APPLY_ACTIONS);
	tables.set_table(1).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_CLEAR_ACTIONS);
	tables.set_table(1).set_properties().set_tfp_next_tables().add_table_id(5);

	tables.add_table(5).set_config(0xc1c2c3c4);
	tables.set_table(5).set_max_entries(1024);
	tables.set_table(5).set_metadata_match(0xe1e2e3e4e5e6e7e8);
	tables.set_table(5).set_metadata_write(0xf1f2f3f4f5f6f7f8);
	tables.set_table(5).set_name("table5");
	tables.set_table(5).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_WRITE_ACTIONS);
	tables.set_table(5).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_APPLY_ACTIONS);
	tables.set_table(5).set_properties().set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_CLEAR_ACTIONS);

	rofl::cmemory mtables(tables.length());
	tables.pack(mtables.somem(), mtables.memlen());

#ifdef DEBUG
	std::cerr << "tables:" << std::endl << tables;
	std::cerr << "mtables:" << std::endl << mtables;
#endif
}



void
coftablesTest::testAddDropSetGetHas()
{
	rofl::openflow::coftables tables(rofl::openflow13::OFP_VERSION);

	try {
		tables.get_table(1);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTablesNotFound& e) {};

	if (tables.has_table(1)) {
		CPPUNIT_ASSERT(false);
	}
	tables.add_table(1);

	try {
		tables.get_table(1);
	} catch (rofl::openflow::eOFTablesNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		tables.set_table(1);
	} catch (rofl::openflow::eOFTablesNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not tables.has_table(1)) {
		CPPUNIT_ASSERT(false);
	}

	tables.drop_table(1);

	try {
		tables.get_table(1);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTablesNotFound& e) {};

	if (tables.has_table(1)) {
		CPPUNIT_ASSERT(false);
	}
	tables.add_table(1);
}


