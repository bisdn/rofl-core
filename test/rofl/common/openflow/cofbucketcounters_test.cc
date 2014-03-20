#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofbucketcounters_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofbucket_counters_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofbucket_counters_test::setUp()
{
}



void
cofbucket_counters_test::tearDown()
{
}



void
cofbucket_counters_test::testDefaultConstructor()
{
	rofl::openflow::cofbucket_counters bcs;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == bcs.get_version());
}



void
cofbucket_counters_test::testCopyConstructor()
{
	rofl::openflow::cofbucket_counters bcs(rofl::openflow13::OFP_VERSION);

	uint32_t bucket_counter_id = 0;

	bcs.set_bucket_counter(bucket_counter_id  ).set_packet_count(0x1111111111111111);
	bcs.set_bucket_counter(bucket_counter_id  ).set_byte_count(  0x2222222222222222);
	bcs.set_bucket_counter(++bucket_counter_id).set_packet_count(0x3333333333333333);
	bcs.set_bucket_counter(bucket_counter_id  ).set_byte_count(  0x4444444444444444);
	bcs.set_bucket_counter(++bucket_counter_id).set_packet_count(0x5555555555555555);
	bcs.set_bucket_counter(bucket_counter_id  ).set_byte_count(  0x6666666666666666);
	bcs.set_bucket_counter(++bucket_counter_id).set_packet_count(0x7777777777777777);
	bcs.set_bucket_counter(bucket_counter_id  ).set_byte_count(  0x8888888888888888);

	rofl::cmemory mbcs(bcs.length());
	bcs.pack(mbcs.somem(), mbcs.memlen());

#ifdef DEBUG
	std::cerr << "bcs:" << std::endl << bcs;
	std::cerr << "mbcs:" << std::endl << mbcs;
#endif

	rofl::openflow::cofbucket_counters clone(bcs);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(bcs.length() == clone.length());
	CPPUNIT_ASSERT(bcs.length() == 4*sizeof(struct rofl::openflow13::ofp_bucket_counter));
	CPPUNIT_ASSERT(mbcs == mclone);
}






void
cofbucket_counters_test::testPackUnpack()
{
	uint64_t mem[8];

	/*
	 * pack
	 */
	rofl::openflow::cofbucket_counters bcs(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < 4; i++) {
		bcs.set_bucket_counter(i).set_packet_count(2*i+0);
		bcs.set_bucket_counter(i).set_byte_count(2*i+1);
	}

	rofl::cmemory mbcs(bcs.length());
	bcs.pack(mbcs.somem(), mbcs.memlen());

#ifdef DEBUG
	std::cerr << "bcs:" << std::endl << bcs;
	std::cerr << "mbcs:" << std::endl << mbcs;
#endif

	for (unsigned int i = 0; i < 8; i++) {
#ifdef DEBUG
		std::cerr << "mbcs[" << (int)(8*i+7) << "]=" << (int)mbcs[8*i+7] << " i=" << i << std::endl;
#endif
		CPPUNIT_ASSERT(mbcs[8*i+7] == i);
	}




	/*
	 * unpack
	 */
	for (unsigned int i = 0; i < 8; i++) {
		mem[i] = htobe64(i);
		mem[i] = htobe64(i);
	}

	bcs.clear();
	bcs.unpack((uint8_t*)mem, sizeof(mem));

	for (unsigned int i = 0; i < 4; i++) {
		CPPUNIT_ASSERT(bcs.get_bucket_counter(i).get_packet_count() == 2*i+0);
		CPPUNIT_ASSERT(bcs.get_bucket_counter(i).get_byte_count() == 2*i+1);
	}
}



void
cofbucket_counters_test::testAddDropSetGetHas()
{
	rofl::openflow::cofbucket_counters bcs(rofl::openflow13::OFP_VERSION);

	try {
		bcs.get_bucket_counter(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eBucketCounterNotFound& e) {};

	if (bcs.has_bucket_counter(0)) {
		CPPUNIT_ASSERT(false);
	}
	bcs.add_bucket_counter(0);

	try {
		bcs.get_bucket_counter(0);
	} catch (rofl::openflow::eBucketCounterNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		bcs.set_bucket_counter(0);
	} catch (rofl::openflow::eBucketCounterNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not bcs.has_bucket_counter(0)) {
		CPPUNIT_ASSERT(false);
	}

	bcs.drop_bucket_counter(0);

	try {
		bcs.get_bucket_counter(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eBucketCounterNotFound& e) {};

	if (bcs.has_bucket_counter(0)) {
		CPPUNIT_ASSERT(false);
	}
	bcs.add_bucket_counter(0);
}


