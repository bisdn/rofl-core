#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofflowstatsarray_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofflowstatsarray_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
cofflowstatsarray_test::setUp()
{
}



void
cofflowstatsarray_test::tearDown()
{
}



void
cofflowstatsarray_test::testDefaultConstructor()
{
	rofl::openflow::cofflowstatsarray array;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == array.get_version());
}



void
cofflowstatsarray_test::testCopyConstructor()
{
	rofl::openflow::cofflowstatsarray array(rofl::openflow13::OFP_VERSION);

	array.set_flow_stats(0).set_idle_timeout(0xc1);
	array.set_flow_stats(0).set_hard_timeout(0xd1);
	array.set_flow_stats(0).set_byte_count(0xa1a2);
	array.set_flow_stats(0).set_packet_count(0xb1b2);

	array.set_flow_stats(1).set_idle_timeout(0xe1);
	array.set_flow_stats(1).set_hard_timeout(0xf1);
	array.set_flow_stats(1).set_byte_count(0x8182);
	array.set_flow_stats(1).set_packet_count(0x9192);

	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "marray:" << std::endl << marray;
#endif

	rofl::openflow::cofflowstatsarray clone(array);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(marray == mclone);
}



void
cofflowstatsarray_test::testPackUnpack()
{
	/*
	 * test memory
	 */
	rofl::openflow::cofflow_stats_reply stats[2];

	stats[0].set_version(rofl::openflow13::OFP_VERSION);
	stats[0].set_byte_count(0xa1a2);
	stats[0].set_packet_count(0xb1b2);
	stats[0].set_duration_sec(0xc1c2);
	stats[0].set_duration_nsec(0xd1d2);
	stats[0].set_idle_timeout(0x10);
	stats[0].set_hard_timeout(0x20);
	stats[0].set_match().set_arp_opcode(0x5555);
	stats[0].set_match().set_eth_dst(rofl::cmacaddr("11:22:33:44:55:66"));
	stats[0].set_match().set_eth_src(rofl::cmacaddr("22:22:22:22:22:22"));
	stats[0].set_match().set_eth_type(0x3333);
	stats[0].set_instructions().set_inst_apply_actions().actions.append_action_copy_ttl_in();
	stats[0].set_instructions().set_inst_apply_actions().actions.append_action_output(6);

	stats[1].set_version(rofl::openflow13::OFP_VERSION);
	stats[1].set_byte_count(0xa3a4);
	stats[1].set_packet_count(0xb3b4);
	stats[1].set_duration_sec(0xc3c4);
	stats[1].set_duration_nsec(0xd3d4);
	stats[1].set_idle_timeout(0x30);
	stats[1].set_hard_timeout(0x40);
	stats[1].set_match().set_arp_opcode(0x8888);
	stats[1].set_match().set_eth_dst(rofl::cmacaddr("44:44:44:44:44:44"));

	std::cerr << "stats[0]=" << stats[0] << std::endl;
	std::cerr << "stats[0].length()=" << (int)stats[0].length() << std::endl;
	std::cerr << "stats[1]=" << stats[1] << std::endl;
	std::cerr << "stats[1].length()=" << (int)stats[1].length() << std::endl;

	rofl::cmemory mem(stats[0].length() + stats[1].length());

	stats[0].pack(mem.somem(), stats[0].length());
	stats[1].pack(mem.somem() + stats[0].length(), stats[1].length());


	/*
	 * array under test
	 */
	rofl::openflow::cofflowstatsarray array(rofl::openflow13::OFP_VERSION);

	array.set_flow_stats(0) = stats[0];
	array.set_flow_stats(1) = stats[1];

	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "marray:" << std::endl << marray;
	std::cerr << "mem:" << std::endl << mem;
#endif

	CPPUNIT_ASSERT(marray == mem);

	rofl::openflow::cofflowstatsarray clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(marray.somem(), marray.memlen());
#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif
}



void
cofflowstatsarray_test::testAddDropSetGetHas()
{
	rofl::openflow::cofflowstatsarray array(rofl::openflow13::OFP_VERSION);

	try {
		array.get_flow_stats(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eFlowStatsNotFound& e) {};

	if (array.has_flow_stats(0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_flow_stats(0);

	try {
		array.get_flow_stats(0);
	} catch (rofl::openflow::eFlowStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		array.set_flow_stats(0);
	} catch (rofl::openflow::eFlowStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not array.has_flow_stats(0)) {
		CPPUNIT_ASSERT(false);
	}

	array.drop_flow_stats(0);

	try {
		array.get_flow_stats(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eFlowStatsNotFound& e) {};

	if (array.has_flow_stats(0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_flow_stats(0);
}


