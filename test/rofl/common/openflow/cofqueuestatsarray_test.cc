#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofqueuestatsarray_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofqueuestatsarray_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofqueuestatsarray_test::setUp()
{
}



void
cofqueuestatsarray_test::tearDown()
{
}



void
cofqueuestatsarray_test::testDefaultConstructor()
{
	rofl::openflow::cofqueuestatsarray array;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == array.get_version());
}



void
cofqueuestatsarray_test::testCopyConstructor()
{
	rofl::openflow::cofqueuestatsarray array(rofl::openflow13::OFP_VERSION);

	array.set_queue_stats(0, 0).set_version(rofl::openflow13::OFP_VERSION);
	array.set_queue_stats(0, 0).set_port_no(0);
	array.set_queue_stats(0, 0).set_queue_id(0);
	array.set_queue_stats(0, 0).set_tx_packets(0x1112131415161718);
	array.set_queue_stats(0, 0).set_tx_bytes(0x2122232425262728);
	array.set_queue_stats(0, 0).set_tx_errors(0x3132333435363738);
	array.set_queue_stats(0, 0).set_duration_sec(0x41424344);
	array.set_queue_stats(0, 0).set_duration_sec(0x51525354);

	array.set_queue_stats(0, 1).set_version(rofl::openflow13::OFP_VERSION);
	array.set_queue_stats(0, 1).set_port_no(0);
	array.set_queue_stats(0, 1).set_queue_id(1);
	array.set_queue_stats(0, 1).set_tx_packets(0x1112131415161718);
	array.set_queue_stats(0, 1).set_tx_bytes(0x2122232425262728);
	array.set_queue_stats(0, 1).set_tx_errors(0x3132333435363738);
	array.set_queue_stats(0, 1).set_duration_sec(0x41424344);
	array.set_queue_stats(0, 1).set_duration_sec(0x51525354);

	array.set_queue_stats(1, 0).set_version(rofl::openflow13::OFP_VERSION);
	array.set_queue_stats(1, 0).set_port_no(0);
	array.set_queue_stats(1, 0).set_queue_id(1);
	array.set_queue_stats(1, 0).set_tx_packets(0x1112131415161718);
	array.set_queue_stats(1, 0).set_tx_bytes(0x2122232425262728);
	array.set_queue_stats(1, 0).set_tx_errors(0x3132333435363738);
	array.set_queue_stats(1, 0).set_duration_sec(0x41424344);
	array.set_queue_stats(1, 0).set_duration_sec(0x51525354);


	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "marray:" << std::endl << marray;
#endif

	rofl::openflow::cofqueuestatsarray clone(array);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(marray == mclone);
}



void
cofqueuestatsarray_test::testOperatorPlus()
{
	std::vector<rofl::openflow::cofqueuestatsarray> array;
	array.push_back(rofl::openflow::cofqueuestatsarray(rofl::openflow13::OFP_VERSION));
	array.push_back(rofl::openflow::cofqueuestatsarray(rofl::openflow13::OFP_VERSION));

	array[0].set_queue_stats(0, 0).set_version(rofl::openflow13::OFP_VERSION);
	array[0].set_queue_stats(0, 0).set_port_no(0);
	array[0].set_queue_stats(0, 0).set_queue_id(0);
	array[0].set_queue_stats(0, 0).set_tx_packets(0x1112131415161718);
	array[0].set_queue_stats(0, 0).set_tx_bytes(0x2122232425262728);
	array[0].set_queue_stats(0, 0).set_tx_errors(0x3132333435363738);
	array[0].set_queue_stats(0, 0).set_duration_sec(0x41424344);
	array[0].set_queue_stats(0, 0).set_duration_nsec(0x51525354);

	array[0].set_queue_stats(0, 1).set_version(rofl::openflow13::OFP_VERSION);
	array[0].set_queue_stats(0, 1).set_port_no(0);
	array[0].set_queue_stats(0, 1).set_queue_id(1);
	array[0].set_queue_stats(0, 1).set_tx_packets(0x1112131415161718);
	array[0].set_queue_stats(0, 1).set_tx_bytes(0x2122232425262728);
	array[0].set_queue_stats(0, 1).set_tx_errors(0x3132333435363738);
	array[0].set_queue_stats(0, 1).set_duration_sec(0x41424344);
	array[0].set_queue_stats(0, 1).set_duration_nsec(0x51525354);

	array[0].set_queue_stats(1, 0).set_version(rofl::openflow13::OFP_VERSION);
	array[0].set_queue_stats(1, 0).set_port_no(1);
	array[0].set_queue_stats(1, 0).set_queue_id(0);
	array[0].set_queue_stats(1, 0).set_tx_packets(0x1112131415161718);
	array[0].set_queue_stats(1, 0).set_tx_bytes(0x2122232425262728);
	array[0].set_queue_stats(1, 0).set_tx_errors(0x3132333435363738);
	array[0].set_queue_stats(1, 0).set_duration_sec(0x41424344);
	array[0].set_queue_stats(1, 0).set_duration_nsec(0x51525354);

	array[1].set_queue_stats(4, 0).set_version(rofl::openflow13::OFP_VERSION);
	array[1].set_queue_stats(4, 0).set_port_no(4);
	array[1].set_queue_stats(4, 0).set_queue_id(0);
	array[1].set_queue_stats(4, 0).set_tx_packets(0x1112131415161718);
	array[1].set_queue_stats(4, 0).set_tx_bytes(0x2122232425262728);
	array[1].set_queue_stats(4, 0).set_tx_errors(0x3132333435363738);
	array[1].set_queue_stats(4, 0).set_duration_sec(0x41424344);
	array[1].set_queue_stats(4, 0).set_duration_nsec(0x51525354);

	array[1].set_queue_stats(5, 0).set_version(rofl::openflow13::OFP_VERSION);
	array[1].set_queue_stats(5, 0).set_port_no(5);
	array[1].set_queue_stats(5, 0).set_queue_id(0);
	array[1].set_queue_stats(5, 0).set_tx_packets(0x1112131415161718);
	array[1].set_queue_stats(5, 0).set_tx_bytes(0x2122232425262728);
	array[1].set_queue_stats(5, 0).set_tx_errors(0x3132333435363738);
	array[1].set_queue_stats(5, 0).set_duration_sec(0x41424344);
	array[1].set_queue_stats(5, 0).set_duration_nsec(0x51525354);

	array[1].set_queue_stats(5, 1).set_version(rofl::openflow13::OFP_VERSION);
	array[1].set_queue_stats(5, 1).set_port_no(5);
	array[1].set_queue_stats(5, 1).set_queue_id(1);
	array[1].set_queue_stats(5, 1).set_tx_packets(0x1112131415161718);
	array[1].set_queue_stats(5, 1).set_tx_bytes(0x2122232425262728);
	array[1].set_queue_stats(5, 1).set_tx_errors(0x3132333435363738);
	array[1].set_queue_stats(5, 1).set_duration_sec(0x41424344);
	array[1].set_queue_stats(5, 1).set_duration_nsec(0x51525354);




#ifdef DEBUG
	std::cerr << "array[0]:" << std::endl << array[0];
	std::cerr << "array[1]:" << std::endl << array[1];
#endif

	rofl::openflow::cofqueuestatsarray merge(array[0]);
	merge += array[1];

#ifdef DEBUG
	std::cerr << "array[0]+array[1]:" << std::endl << merge;
	std::cerr << "merge.set_queue_stats(0, 0) == array[0].get_queue_stats(0, 0): " << (merge.set_queue_stats(0, 0) == array[0].get_queue_stats(0, 0)) << std::endl;
	std::cerr << "merge.set_queue_stats(0, 1) == array[0].get_queue_stats(0, 1): " << (merge.set_queue_stats(0, 1) == array[0].get_queue_stats(0, 1)) << std::endl;
	std::cerr << "merge.set_queue_stats(1, 0) == array[0].get_queue_stats(1, 0): " << (merge.set_queue_stats(1, 0) == array[0].get_queue_stats(1, 0)) << std::endl;
	std::cerr << "merge.set_queue_stats(4, 0) == array[1].get_queue_stats(4, 0): " << (merge.set_queue_stats(4, 0) == array[1].get_queue_stats(4, 0)) << std::endl;
	std::cerr << "merge.set_queue_stats(5, 0) == array[1].get_queue_stats(5, 0): " << (merge.set_queue_stats(5, 0) == array[1].get_queue_stats(5, 0)) << std::endl;
	std::cerr << "merge.set_queue_stats(5, 1) == array[1].get_queue_stats(5, 1): " << (merge.set_queue_stats(5, 1) == array[1].get_queue_stats(5, 1)) << std::endl;
#endif

	CPPUNIT_ASSERT(merge.set_queue_stats(0, 0) == array[0].get_queue_stats(0, 0));
	CPPUNIT_ASSERT(merge.set_queue_stats(0, 1) == array[0].get_queue_stats(0, 1));
	CPPUNIT_ASSERT(merge.set_queue_stats(1, 0) == array[0].get_queue_stats(1, 0));
	CPPUNIT_ASSERT(merge.set_queue_stats(4, 0) == array[1].get_queue_stats(4, 0));
	CPPUNIT_ASSERT(merge.set_queue_stats(5, 0) == array[1].get_queue_stats(5, 0));
	CPPUNIT_ASSERT(merge.set_queue_stats(5, 1) == array[1].get_queue_stats(5, 1));
}



void
cofqueuestatsarray_test::testPackUnpack()
{
	/*
	 * unpack
	 */
	rofl::cmemory mem(sizeof(struct rofl::openflow13::ofp_queue_stats));
	struct rofl::openflow13::ofp_queue_stats *queue = (struct rofl::openflow13::ofp_queue_stats*)mem.somem();

	queue->port_no = htobe32(8);
	queue->queue_id = htobe32(1);
	queue->tx_packets = htobe64(0x1112131415161718);
	queue->tx_bytes = htobe64(0x2122232425262728);
	queue->tx_errors = htobe64(0x3132333435363738);
	queue->duration_sec = htobe32(0x11111111);
	queue->duration_nsec = htobe32(0x22222222);

	rofl::openflow::cofqueuestatsarray array(rofl::openflow13::OFP_VERSION);
	array.unpack(mem.somem(), mem.memlen());

#ifdef DEBUG
	std::cerr << "mem:" << std::endl << mem;
	std::cerr << "unpacked mem:" << std::endl << array;
#endif

	CPPUNIT_ASSERT(array.get_queue_stats(8, 1).get_port_no() == 8);
	CPPUNIT_ASSERT(array.get_queue_stats(8, 1).get_queue_id() == 1);
	CPPUNIT_ASSERT(array.get_queue_stats(8, 1).get_tx_packets() == 0x1112131415161718);
	CPPUNIT_ASSERT(array.get_queue_stats(8, 1).get_tx_bytes() == 0x2122232425262728);
	CPPUNIT_ASSERT(array.get_queue_stats(8, 1).get_tx_errors() == 0x3132333435363738);
	CPPUNIT_ASSERT(array.get_queue_stats(8, 1).get_duration_sec() == 0x11111111);
	CPPUNIT_ASSERT(array.get_queue_stats(8, 1).get_duration_nsec() == 0x22222222);

	/*
	 * pack
	 */

	array.clear();

	try {
		array.get_queue_stats(8, 1);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eQueueStatsNotFound& e) {};

	array.set_queue_stats(8, 1).set_port_no(8);
	array.set_queue_stats(8, 1).set_queue_id(1);
	array.set_queue_stats(8, 1).set_tx_packets(0x1112131415161718);
	array.set_queue_stats(8, 1).set_tx_bytes(0x2122232425262728);
	array.set_queue_stats(8, 1).set_tx_errors(0x3132333435363738);
	array.set_queue_stats(8, 1).set_duration_sec(0x11111111);
	array.set_queue_stats(8, 1).set_duration_nsec(0x22222222);


	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

	CPPUNIT_ASSERT(marray == mem);

	queue = (struct rofl::openflow13::ofp_queue_stats*)marray.somem();

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "packed array:" << std::endl << marray;
#endif

	CPPUNIT_ASSERT(be32toh(queue->port_no) == 8);
	CPPUNIT_ASSERT(be32toh(queue->queue_id) == 1);
	CPPUNIT_ASSERT(be64toh(queue->tx_packets) == 0x1112131415161718);
	CPPUNIT_ASSERT(be64toh(queue->tx_bytes) == 0x2122232425262728);
	CPPUNIT_ASSERT(be64toh(queue->tx_errors) == 0x3132333435363738);
	CPPUNIT_ASSERT(be32toh(queue->duration_sec) == 0x11111111);
	CPPUNIT_ASSERT(be32toh(queue->duration_nsec) == 0x22222222);
}



void
cofqueuestatsarray_test::testAddDropSetGetHas()
{
	rofl::openflow::cofqueuestatsarray array(rofl::openflow13::OFP_VERSION);

	try {
		array.get_queue_stats(0, 0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eQueueStatsNotFound& e) {};

	if (array.has_queue_stats(0, 0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_queue_stats(0, 0);

	try {
		array.get_queue_stats(0, 0);
	} catch (rofl::openflow::eQueueStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		array.set_queue_stats(0, 0);
	} catch (rofl::openflow::eQueueStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not array.has_queue_stats(0, 0)) {
		CPPUNIT_ASSERT(false);
	}

	array.drop_queue_stats(0, 0);

	try {
		array.get_queue_stats(0, 0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eQueueStatsNotFound& e) {};

	if (array.has_queue_stats(0, 0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_queue_stats(0, 0);
}


