#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "csegmsg_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( csegmsg_test );

#if defined DEBUG
#undef DEBUG
#endif

void
csegmsg_test::setUp()
{
}



void
csegmsg_test::tearDown()
{
}



void
csegmsg_test::testStoreAndMerge()
{
	rofl::csegmsg segmsg(0x11121314);

	/*
	 * first message
	 */
	rofl::openflow::cofmsg_table_stats_reply msg1(rofl::openflow13::OFP_VERSION);
	msg1.set_xid(0x11121314);
	msg1.set_stats_flags(rofl::openflow13::OFPMPF_REPLY_MORE);

	msg1.set_table_stats_array().set_table_stats(0).set_table_id(0);
	msg1.set_table_stats_array().set_table_stats(0).set_active_count(0x11121314);
	msg1.set_table_stats_array().set_table_stats(0).set_lookup_count(0x2122232425262728);
	msg1.set_table_stats_array().set_table_stats(0).set_matched_count(0x3132333435363738);

	msg1.set_table_stats_array().set_table_stats(1).set_table_id(1);
	msg1.set_table_stats_array().set_table_stats(1).set_active_count(0x11121314);
	msg1.set_table_stats_array().set_table_stats(1).set_lookup_count(0x2122232425262728);
	msg1.set_table_stats_array().set_table_stats(1).set_matched_count(0x3132333435363738);


	/*
	 * second message (to be appended)
	 */
	rofl::openflow::cofmsg_table_stats_reply msg2(rofl::openflow13::OFP_VERSION);
	msg2.set_xid(0x11121314);
	msg2.set_stats_flags(rofl::openflow13::OFPMPF_REPLY_MORE);

	msg2.set_table_stats_array().set_table_stats(4).set_table_id(4);
	msg2.set_table_stats_array().set_table_stats(4).set_active_count(0x11121314);
	msg2.set_table_stats_array().set_table_stats(4).set_lookup_count(0x2122232425262728);
	msg2.set_table_stats_array().set_table_stats(4).set_matched_count(0x3132333435363738);


	/*
	 * third message (replaces content for table-id 4 of previous message)
	 */
	rofl::openflow::cofmsg_table_stats_reply msg3(rofl::openflow13::OFP_VERSION);
	msg3.set_xid(0x11121314);
	msg3.set_stats_flags(0);

	msg3.set_table_stats_array().set_table_stats(4).set_table_id(4);
	msg3.set_table_stats_array().set_table_stats(4).set_active_count(0x51525354);
	msg3.set_table_stats_array().set_table_stats(4).set_lookup_count(0x6162636465666768);
	msg3.set_table_stats_array().set_table_stats(4).set_matched_count(0x7172737475767778);

	msg3.set_table_stats_array().set_table_stats(8).set_table_id(8);
	msg3.set_table_stats_array().set_table_stats(8).set_active_count(0xa1a2a3a4);
	msg3.set_table_stats_array().set_table_stats(8).set_lookup_count(0xb1b2b3b4b5b6b7b8);
	msg3.set_table_stats_array().set_table_stats(8).set_matched_count(0xc1c2c3c4c5c6c7c8);





	segmsg.store_and_merge_msg(msg1);

#ifdef DEBUG
	std::cerr << "segmsg:" << std::endl << segmsg;
#endif

	segmsg.store_and_merge_msg(msg2);

#ifdef DEBUG
	std::cerr << "segmsg:" << std::endl << segmsg;
#endif

	segmsg.store_and_merge_msg(msg3);

#ifdef DEBUG
	std::cerr << "segmsg:" << std::endl << segmsg;
#endif

	rofl::openflow::cofmsg_table_stats_reply merged = dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const&>( segmsg.get_msg() );

	CPPUNIT_ASSERT(merged.get_table_stats_array().size() == 4);
	CPPUNIT_ASSERT(merged.set_table_stats_array().set_table_stats(0) == msg1.get_table_stats_array().get_table_stats(0));
	CPPUNIT_ASSERT(merged.set_table_stats_array().set_table_stats(1) == msg1.get_table_stats_array().get_table_stats(1));
	CPPUNIT_ASSERT(not (merged.set_table_stats_array().set_table_stats(4) == msg2.get_table_stats_array().get_table_stats(4)));
	CPPUNIT_ASSERT(merged.set_table_stats_array().set_table_stats(4) == msg3.get_table_stats_array().get_table_stats(4));
	CPPUNIT_ASSERT(merged.set_table_stats_array().set_table_stats(8) == msg3.get_table_stats_array().get_table_stats(8));
}



void
csegmsg_test::testRetrieveAndDetach()
{
	rofl::csegmsg segmsg(0x11121314);

	/*
	 * first message
	 */
	rofl::openflow::cofmsg_table_stats_reply msg1(rofl::openflow13::OFP_VERSION);
	msg1.set_xid(0x11121314);
	msg1.set_stats_flags(rofl::openflow13::OFPMPF_REPLY_MORE);

	msg1.set_table_stats_array().set_table_stats(0).set_table_id(0);
	msg1.set_table_stats_array().set_table_stats(0).set_active_count(0x11121314);
	msg1.set_table_stats_array().set_table_stats(0).set_lookup_count(0x2122232425262728);
	msg1.set_table_stats_array().set_table_stats(0).set_matched_count(0x3132333435363738);

	msg1.set_table_stats_array().set_table_stats(1).set_table_id(1);
	msg1.set_table_stats_array().set_table_stats(1).set_active_count(0x11121314);
	msg1.set_table_stats_array().set_table_stats(1).set_lookup_count(0x2122232425262728);
	msg1.set_table_stats_array().set_table_stats(1).set_matched_count(0x3132333435363738);


	/*
	 * second message (to be appended)
	 */
	rofl::openflow::cofmsg_table_stats_reply msg2(rofl::openflow13::OFP_VERSION);
	msg2.set_xid(0x11121314);
	msg2.set_stats_flags(rofl::openflow13::OFPMPF_REPLY_MORE);

	msg2.set_table_stats_array().set_table_stats(4).set_table_id(4);
	msg2.set_table_stats_array().set_table_stats(4).set_active_count(0x11121314);
	msg2.set_table_stats_array().set_table_stats(4).set_lookup_count(0x2122232425262728);
	msg2.set_table_stats_array().set_table_stats(4).set_matched_count(0x3132333435363738);


	/*
	 * third message (replaces content for table-id 4 of previous message)
	 */
	rofl::openflow::cofmsg_table_stats_reply msg3(rofl::openflow13::OFP_VERSION);
	msg3.set_xid(0x11121314);
	msg3.set_stats_flags(0);

	msg3.set_table_stats_array().set_table_stats(4).set_table_id(4);
	msg3.set_table_stats_array().set_table_stats(4).set_active_count(0x51525354);
	msg3.set_table_stats_array().set_table_stats(4).set_lookup_count(0x6162636465666768);
	msg3.set_table_stats_array().set_table_stats(4).set_matched_count(0x7172737475767778);

	msg3.set_table_stats_array().set_table_stats(8).set_table_id(8);
	msg3.set_table_stats_array().set_table_stats(8).set_active_count(0xa1a2a3a4);
	msg3.set_table_stats_array().set_table_stats(8).set_lookup_count(0xb1b2b3b4b5b6b7b8);
	msg3.set_table_stats_array().set_table_stats(8).set_matched_count(0xc1c2c3c4c5c6c7c8);


	segmsg.store_and_merge_msg(msg1);

#ifdef DEBUG
	std::cerr << "segmsg:" << std::endl << segmsg;
#endif

	segmsg.store_and_merge_msg(msg2);

#ifdef DEBUG
	std::cerr << "segmsg:" << std::endl << segmsg;
#endif

	segmsg.store_and_merge_msg(msg3);

#ifdef DEBUG
	std::cerr << "segmsg:" << std::endl << segmsg;
#endif


	rofl::openflow::cofmsg_table_stats_reply* merged = dynamic_cast<rofl::openflow::cofmsg_table_stats_reply*>( segmsg.retrieve_and_detach_msg() );

	try {
		segmsg.get_msg();
		CPPUNIT_ASSERT(false);
	} catch(rofl::eInval& e) {};

	delete merged;
}


