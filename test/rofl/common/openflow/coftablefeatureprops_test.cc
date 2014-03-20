#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coftablefeatureprops_test.h"

CPPUNIT_TEST_SUITE_REGISTRATION( coftablefeaturepropsTest );

#ifdef DEBUG
#undef DEBUG
#endif

void
coftablefeaturepropsTest::setUp()
{
}



void
coftablefeaturepropsTest::tearDown()
{
}



void
coftablefeaturepropsTest::testDefaultConstructor()
{
	rofl::openflow::coftable_feature_props props;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == props.get_version());
#ifdef DEBUG
	std::cerr << "props:" << std::endl << props;
#endif
}



void
coftablefeaturepropsTest::testCopyConstructor()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	props.add_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_APPLY_ACTIONS);
	props.set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_WRITE_ACTIONS);
	props.set_tfp_instructions().add_instruction(rofl::openflow13::OFPIT_GOTO_TABLE);
	props.add_tfp_instructions_miss().add_instruction(rofl::openflow13::OFPIT_GOTO_TABLE);
	props.set_tfp_instructions_miss().add_instruction(rofl::openflow13::OFPIT_APPLY_ACTIONS);
	props.add_tfp_next_tables().add_table_id(0);
	props.set_tfp_next_tables().add_table_id(1);
	props.set_tfp_next_tables().add_table_id(2);
	props.add_tfp_next_tables_miss();
	props.add_tfp_write_actions().add_action(rofl::openflow::OFPAT_DEC_NW_TTL);
	props.set_tfp_write_actions().add_action(rofl::openflow::OFPAT_OUTPUT);
	props.add_tfp_write_actions_miss();
	props.add_tfp_apply_actions();
	props.add_tfp_apply_actions_miss();
	props.add_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_OP);
	props.set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SHA);
	props.add_tfp_wildcards();
	props.add_tfp_write_setfield();
	props.add_tfp_write_setfield_miss();
	props.add_tfp_apply_setfield();
	props.add_tfp_apply_setfield_miss();

	rofl::openflow::coftable_feature_props clone;

	rofl::cmemory mprops(props.length());
	props.pack(mprops.somem(), mprops.memlen());

	clone = props;

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "props:" << std::endl << props;
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mprops:" << std::endl << mprops;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(rofl::openflow13::OFP_VERSION == clone.get_version());
	CPPUNIT_ASSERT(mprops == mclone);

	clone.clear();

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif

	clone.unpack(mprops.somem(), mprops.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif

	CPPUNIT_ASSERT(clone.get_tfp_instructions().get_instruction_ids().size() == 3);
	CPPUNIT_ASSERT(clone.get_tfp_instructions_miss().get_instruction_ids().size() == 2);
}





void
coftablefeaturepropsTest::testAddDropSetGetHasInstructions()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_instructions();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_instructions()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_instructions();

	try {
		props.get_tfp_instructions();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_instructions();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_instructions()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_instructions();

	try {
		props.get_tfp_instructions();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_instructions()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_instructions();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasInstructionsMiss()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_instructions_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_instructions_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_instructions_miss();

	try {
		props.get_tfp_instructions_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_instructions_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_instructions_miss()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_instructions_miss();

	try {
		props.get_tfp_instructions_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_instructions_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_instructions_miss();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasNextTables()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_next_tables();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_next_tables()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_next_tables();

	try {
		props.get_tfp_next_tables();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_next_tables();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_next_tables()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_next_tables();

	try {
		props.get_tfp_next_tables();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_next_tables()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_next_tables();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasNextTablesMiss()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_next_tables_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_next_tables_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_next_tables_miss();

	try {
		props.get_tfp_next_tables_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_next_tables_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_next_tables_miss()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_next_tables_miss();

	try {
		props.get_tfp_next_tables_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_next_tables_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_next_tables_miss();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasWriteActions()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_write_actions();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_write_actions()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_write_actions();

	try {
		props.get_tfp_write_actions();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_write_actions();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_write_actions()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_write_actions();

	try {
		props.get_tfp_write_actions();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_write_actions()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_write_actions();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasWriteActionsMiss()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_write_actions_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_write_actions_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_write_actions_miss();

	try {
		props.get_tfp_write_actions_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_write_actions_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_write_actions_miss()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_write_actions_miss();

	try {
		props.get_tfp_write_actions_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_write_actions_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_write_actions_miss();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasApplyActions()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_apply_actions();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_apply_actions()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_apply_actions();

	try {
		props.get_tfp_apply_actions();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_apply_actions();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_apply_actions()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_apply_actions();

	try {
		props.get_tfp_apply_actions();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_apply_actions()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_apply_actions();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasApplyActionsMiss()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_apply_actions_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_apply_actions_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_apply_actions_miss();

	try {
		props.get_tfp_apply_actions_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_apply_actions_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_apply_actions_miss()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_apply_actions_miss();

	try {
		props.get_tfp_apply_actions_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_apply_actions_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_apply_actions_miss();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasMatch()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_match();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_match()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_match();

	try {
		props.get_tfp_match();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_match();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_match()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_match();

	try {
		props.get_tfp_match();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_match()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_match();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasWildcards()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_wildcards();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_wildcards()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_wildcards();

	try {
		props.get_tfp_wildcards();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_wildcards();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_wildcards()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_wildcards();

	try {
		props.get_tfp_wildcards();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_wildcards()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_wildcards();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasWriteSetField()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_write_setfield();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_write_setfield()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_write_setfield();

	try {
		props.get_tfp_write_setfield();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_write_setfield();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_write_setfield()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_write_setfield();

	try {
		props.get_tfp_write_setfield();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_write_setfield()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_write_setfield();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasWriteSetFieldMiss()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_write_setfield_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_write_setfield_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_write_setfield_miss();

	try {
		props.get_tfp_write_setfield_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_write_setfield_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_write_setfield_miss()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_write_setfield_miss();

	try {
		props.get_tfp_write_setfield_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_write_setfield_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_write_setfield_miss();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasApplySetField()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_apply_setfield();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_apply_setfield()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_apply_setfield();

	try {
		props.get_tfp_apply_setfield();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_apply_setfield();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_apply_setfield()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_apply_setfield();

	try {
		props.get_tfp_apply_setfield();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_apply_setfield()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_apply_setfield();
}



void
coftablefeaturepropsTest::testAddDropSetGetHasApplySetFieldMiss()
{
	rofl::openflow::coftable_feature_props props(rofl::openflow13::OFP_VERSION);

	try {
		props.get_tfp_apply_setfield_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_apply_setfield_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_apply_setfield_miss();

	try {
		props.get_tfp_apply_setfield_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		props.set_tfp_apply_setfield_miss();
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not props.has_tfp_apply_setfield_miss()) {
		CPPUNIT_ASSERT(false);
	}

	props.drop_tfp_apply_setfield_miss();

	try {
		props.get_tfp_apply_setfield_miss();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOFTableFeaturePropsNotFound& e) {};

	if (props.has_tfp_apply_setfield_miss()) {
		CPPUNIT_ASSERT(false);
	}
	props.add_tfp_apply_setfield_miss();
}



