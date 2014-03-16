#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coftablefeatures_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( coftablefeaturesTest );

#if defined DEBUG
#undef DEBUG
#endif

void
coftablefeaturesTest::setUp()
{
}



void
coftablefeaturesTest::tearDown()
{
}



void
coftablefeaturesTest::testDefaultConstructor()
{
	rofl::openflow::coftable_features table;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == table.get_version());
}



void
coftablefeaturesTest::testCopyConstructor()
{
	rofl::openflow::coftable_features table(rofl::openflow13::OFP_VERSION);
	table.set_config(0xc1c2c3c4);
	table.set_max_entries(0xa1a2a3a4);
	table.set_metadata_match(0xe1e2e3e4e5e6e7e8);
	table.set_metadata_write(0xf1f2f3f4f5f6f7f8);
	table.set_name("table0");
	table.set_table_id(0x0a);
	table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow13::OFPAT_SET_FIELD);

	rofl::cmemory mtable(table.length());
	table.pack(mtable.somem(), mtable.memlen());

#ifdef DEBUG
	std::cerr << "table:" << std::endl << table;
	std::cerr << "mtable:" << std::endl << mtable;
#endif

	rofl::openflow::coftable_features clone(rofl::openflow13::OFP_VERSION);
#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif

	clone.unpack(mtable.somem(), mtable.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif
}



void
coftablefeaturesTest::testPackUnpack()
{
	rofl::openflow::coftable_features table(rofl::openflow13::OFP_VERSION);

	rofl::cmemory mem(sizeof(struct rofl::openflow13::ofp_table_features));
	mem[1] = 0x50;

	try {
		table.unpack(mem.somem(), mem.memlen());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eTableFeaturesReqBadLen& e) {}

#ifdef DEBUG
	std::cerr << "table:" << std::endl << table;
#endif
}



