#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofgroupstatsarray_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofgroupstatsarray_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofgroupstatsarray_test::setUp()
{
}



void
cofgroupstatsarray_test::tearDown()
{
}



void
cofgroupstatsarray_test::testDefaultConstructor()
{
	rofl::openflow::cofgroupstatsarray array;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == array.get_version());
}



void
cofgroupstatsarray_test::testCopyConstructor()
{
	rofl::openflow::cofgroupstatsarray array(rofl::openflow13::OFP_VERSION);

	array.set_group_stats(0).set_version(rofl::openflow13::OFP_VERSION);
	array.set_group_stats(0).set_group_id(0);
	array.set_group_stats(0).set_packet_count(0xaabbccdd);
	array.set_group_stats(0).set_byte_count(0x11223344);
	array.set_group_stats(0).set_ref_count(0x55667788);
	array.set_group_stats(0).set_duration_sec(0x77777777);
	array.set_group_stats(0).set_duration_nsec(0x88888888);
	array.set_group_stats(0).set_bucket_counters().set_bucket_counter(0).set_packet_count(0x1111111111111111);
	array.set_group_stats(0).set_bucket_counters().set_bucket_counter(0).set_byte_count  (0x2222222222222222);
	array.set_group_stats(0).set_bucket_counters().set_bucket_counter(1).set_packet_count(0x3333333333333333);
	array.set_group_stats(0).set_bucket_counters().set_bucket_counter(1).set_byte_count  (0x4444444444444444);

	array.set_group_stats(1).set_version(rofl::openflow13::OFP_VERSION);
	array.set_group_stats(1).set_group_id(1);
	array.set_group_stats(1).set_packet_count(0xbbccddee);
	array.set_group_stats(1).set_byte_count(0x22334455);
	array.set_group_stats(1).set_ref_count(0x778899aa);
	array.set_group_stats(1).set_duration_sec(0xaaaaaaaa);
	array.set_group_stats(1).set_duration_nsec(0xbbbbbbbb);


	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "marray:" << std::endl << marray;
#endif

	rofl::openflow::cofgroupstatsarray clone(array);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(marray == mclone);
}



void
cofgroupstatsarray_test::testOperatorPlus()
{
	std::vector<rofl::openflow::cofgroupstatsarray> array;
	array.push_back(rofl::openflow::cofgroupstatsarray(rofl::openflow13::OFP_VERSION));
	array.push_back(rofl::openflow::cofgroupstatsarray(rofl::openflow13::OFP_VERSION));

	array[0].set_group_stats(0).set_version(rofl::openflow13::OFP_VERSION);
	array[0].set_group_stats(0).set_group_id(0);
	array[0].set_group_stats(0).set_packet_count(0xaabbccdd);
	array[0].set_group_stats(0).set_byte_count(0x11223344);
	array[0].set_group_stats(0).set_ref_count(0x10101010);
	array[0].set_group_stats(0).set_duration_sec(0x77777777);
	array[0].set_group_stats(0).set_duration_nsec(0x88888888);
	array[0].set_group_stats(0).set_bucket_counters().set_bucket_counter(0).set_packet_count(0x1111111111111111);
	array[0].set_group_stats(0).set_bucket_counters().set_bucket_counter(0).set_byte_count  (0x2222222222222222);
	array[0].set_group_stats(0).set_bucket_counters().set_bucket_counter(1).set_packet_count(0x3333333333333333);
	array[0].set_group_stats(0).set_bucket_counters().set_bucket_counter(1).set_byte_count  (0x4444444444444444);

	array[0].set_group_stats(1).set_version(rofl::openflow13::OFP_VERSION);
	array[0].set_group_stats(1).set_group_id(1);
	array[0].set_group_stats(1).set_packet_count(0xbbccddee);
	array[0].set_group_stats(1).set_byte_count(0x22334455);
	array[0].set_group_stats(1).set_ref_count(0x20202020);
	array[0].set_group_stats(1).set_duration_sec(0xaaaaaaaa);
	array[0].set_group_stats(1).set_duration_nsec(0xbbbbbbbb);

	array[1].set_group_stats(2).set_version(rofl::openflow13::OFP_VERSION);
	array[1].set_group_stats(2).set_group_id(2);
	array[1].set_group_stats(2).set_packet_count(0xbbccddee);
	array[1].set_group_stats(2).set_byte_count(0x22334455);
	array[1].set_group_stats(2).set_ref_count(0x30303030);
	array[1].set_group_stats(2).set_duration_sec(0xaaaaaaaa);
	array[1].set_group_stats(2).set_duration_nsec(0xbbbbbbbb);



#ifdef DEBUG
	std::cerr << "array[0]:" << std::endl << array[0];
	std::cerr << "array[1]:" << std::endl << array[1];
#endif

	rofl::openflow::cofgroupstatsarray merge(array[0]);
	merge += array[1];

#ifdef DEBUG
	std::cerr << "array[0]+array[1]:" << std::endl << merge;
	std::cerr << "merge.set_group_stats(0) == array[0].get_group_stats(0): " << (merge.set_group_stats(0) == array[0].get_group_stats(0)) << std::endl;
	std::cerr << "merge.set_group_stats(1) == array[0].get_group_stats(1): " << (merge.set_group_stats(1) == array[0].get_group_stats(1)) << std::endl;
	std::cerr << "merge.set_group_stats(2) == array[1].get_group_stats(2): " << (merge.set_group_stats(2) == array[1].get_group_stats(2)) << std::endl;
#endif

	CPPUNIT_ASSERT(merge.set_group_stats(0) == array[0].get_group_stats(0));
	CPPUNIT_ASSERT(merge.set_group_stats(1) == array[0].get_group_stats(1));
	CPPUNIT_ASSERT(merge.set_group_stats(2) == array[1].get_group_stats(2));
}



void
cofgroupstatsarray_test::testPackUnpack()
{
#if 0
	/*
	 * test memory
	 */
	std::vector<rofl::openflow::cofgroup_stats_reply> stats;
	stats.push_back(rofl::openflow::cofgroup_stats_reply(rofl::openflow13::OFP_VERSION));
	stats.push_back(rofl::openflow::cofgroup_stats_reply(rofl::openflow13::OFP_VERSION));
	stats.push_back(rofl::openflow::cofgroup_stats_reply(rofl::openflow13::OFP_VERSION));

	stats[0].set_version(rofl::openflow13::OFP_VERSION);
	stats[0].set_group_id(0);
	stats[0].set_group_type(rofl::openflow13::OFPGT_ALL);
	stats[0].set_buckets().set_bucket(0).set_packet_count(0x0a);
	stats[0].set_buckets().set_bucket(0).set_byte_count(0x88);
	stats[0].set_buckets().set_bucket(0).set_watch_port(0xc3c3c4c4);
	stats[0].set_buckets().set_bucket(0).set_watch_group(0xd3d3d4d4);

	stats[1].set_version(rofl::openflow13::OFP_VERSION);
	stats[1].set_group_id(0);
	stats[1].set_group_type(rofl::openflow13::OFPGT_ALL);
	stats[1].set_buckets().set_bucket(0).set_packet_count(0x0a);
	stats[1].set_buckets().set_bucket(0).set_byte_count(0x88);
	stats[1].set_buckets().set_bucket(0).set_watch_port(0xc3c3c4c4);
	stats[1].set_buckets().set_bucket(0).set_watch_group(0xd3d3d4d4);
	stats[1].set_buckets().set_bucket(0).set_actions().append_action_dec_mpls_ttl();

	stats[2].set_version(rofl::openflow13::OFP_VERSION);
	stats[2].set_group_id(0);
	stats[2].set_group_type(rofl::openflow13::OFPGT_ALL);
	stats[2].set_buckets().set_bucket(0).set_packet_count(0x0a);
	stats[2].set_buckets().set_bucket(0).set_byte_count(0x88);
	stats[2].set_buckets().set_bucket(0).set_watch_port(0xc3c3c4c4);
	stats[2].set_buckets().set_bucket(0).set_watch_group(0xd3d3d4d4);
	stats[2].set_buckets().set_bucket(0).set_actions().append_action_dec_mpls_ttl();
	stats[2].set_buckets().set_bucket(1).set_packet_count(0x1a);
	stats[2].set_buckets().set_bucket(1).set_byte_count(0x99);
	stats[2].set_buckets().set_bucket(1).set_watch_port(0xe3e3e4e4);
	stats[2].set_buckets().set_bucket(1).set_watch_group(0xf3f3f4f4);
	stats[2].set_buckets().set_bucket(1).set_actions().append_action_group(0xee);


	size_t len = 0;
	for (unsigned int i = 0; i < 3; i++) {
		len += stats[i].length();
	}
	rofl::cmemory mem(len);

	uint8_t *buf = mem.somem();
	for (unsigned int i = 0; i < 3; i++) {
#ifdef DEBUG
		std::cerr << "stats[" << i << "]=" << stats[i] << std::endl;
		std::cerr << "stats[" << i << "].length()=" << (int)stats[i].length() << std::endl;
#endif

		stats[i].pack(buf, stats[i].length());
		buf += stats[i].length();
	}

	/*
	 * array under test
	 */
	rofl::openflow::cofgroupstatsarray array(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < 3; i++) {
		array.set_group_stats(i) = stats[i];
	}

	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "marray:" << std::endl << marray;
	std::cerr << "mem:" << std::endl << mem;
#endif

	CPPUNIT_ASSERT(marray == mem);

	rofl::openflow::cofgroupstatsarray clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(marray.somem(), marray.memlen());
#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif

#endif
}



void
cofgroupstatsarray_test::testAddDropSetGetHas()
{
	rofl::openflow::cofgroupstatsarray array(rofl::openflow13::OFP_VERSION);

	try {
		array.get_group_stats(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eGroupStatsNotFound& e) {};

	if (array.has_group_stats(0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_group_stats(0);

	try {
		array.get_group_stats(0);
	} catch (rofl::openflow::eGroupStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		array.set_group_stats(0);
	} catch (rofl::openflow::eGroupStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not array.has_group_stats(0)) {
		CPPUNIT_ASSERT(false);
	}

	array.drop_group_stats(0);

	try {
		array.get_group_stats(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eGroupStatsNotFound& e) {};

	if (array.has_group_stats(0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_group_stats(0);
}


