#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofportstatsarray_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofportstatsarray_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofportstatsarray_test::setUp()
{
}



void
cofportstatsarray_test::tearDown()
{
}



void
cofportstatsarray_test::testDefaultConstructor()
{
	rofl::openflow::cofportstatsarray array;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == array.get_version());
}



void
cofportstatsarray_test::testCopyConstructor()
{
	rofl::openflow::cofportstatsarray array(rofl::openflow13::OFP_VERSION);

	array.set_port_stats(0).set_version(rofl::openflow13::OFP_VERSION);
	array.set_port_stats(0).set_port_no(0);
	array.set_port_stats(0).set_rx_packets	(0x1111111111111111);
	array.set_port_stats(0).set_tx_packets	(0x2222222222222222);
	array.set_port_stats(0).set_rx_bytes	(0x3333333333333333);
	array.set_port_stats(0).set_tx_bytes	(0x4444444444444444);
	array.set_port_stats(0).set_rx_dropped	(0x5555555555555555);
	array.set_port_stats(0).set_tx_dropped	(0x6666666666666666);
	array.set_port_stats(0).set_rx_errors	(0x7777777777777777);
	array.set_port_stats(0).set_tx_errors	(0x8888888888888888);
	array.set_port_stats(0).set_rx_frame_err(0x9999999999999999);
	array.set_port_stats(0).set_rx_over_err	(0xaaaaaaaaaaaaaaaa);
	array.set_port_stats(0).set_rx_crc_err	(0xbbbbbbbbbbbbbbbb);
	array.set_port_stats(0).set_collisions	(0xcccccccccccccccc);
	array.set_port_stats(0).set_duration_sec	(0x11111111);
	array.set_port_stats(0).set_duration_nsec	(0x22222222);

	array.set_port_stats(1).set_version(rofl::openflow13::OFP_VERSION);
	array.set_port_stats(1).set_port_no(0);
	array.set_port_stats(1).set_rx_packets	(0xa1a1a1a1a1a1a1a1);
	array.set_port_stats(1).set_tx_packets	(0xa2a2a2a2a2a2a2a2);
	array.set_port_stats(1).set_rx_bytes	(0xa3a3a3a3a3a3a3a3);
	array.set_port_stats(1).set_tx_bytes	(0xa4a4a4a4a4a4a4a4);
	array.set_port_stats(1).set_rx_dropped	(0xa5a5a5a5a5a5a5a5);
	array.set_port_stats(1).set_tx_dropped	(0xa6a6a6a6a6a6a6a6);
	array.set_port_stats(1).set_rx_errors	(0xa7a7a7a7a7a7a7a7);
	array.set_port_stats(1).set_tx_errors	(0xa8a8a8a8a8a8a8a8);
	array.set_port_stats(1).set_rx_frame_err(0xa9a9a9a9a9a9a9a9);
	array.set_port_stats(1).set_rx_over_err	(0xaaaaaaaaaaaaaaaa);
	array.set_port_stats(1).set_rx_crc_err	(0xabababababababab);
	array.set_port_stats(1).set_collisions	(0xacacacacacacacac);
	array.set_port_stats(1).set_duration_sec	(0xa1a1a1a1);
	array.set_port_stats(1).set_duration_nsec	(0xa2a2a2a2);

	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "marray:" << std::endl << marray;
#endif

	rofl::openflow::cofportstatsarray clone(array);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(marray == mclone);
}



void
cofportstatsarray_test::testOperatorPlus()
{
	std::vector<rofl::openflow::cofportstatsarray> array;
	array.push_back(rofl::openflow::cofportstatsarray(rofl::openflow13::OFP_VERSION));
	array.push_back(rofl::openflow::cofportstatsarray(rofl::openflow13::OFP_VERSION));

	array[0].set_port_stats(0).set_version(rofl::openflow13::OFP_VERSION);
	array[0].set_port_stats(0).set_port_no(0);
	array[0].set_port_stats(0).set_rx_packets	(0x1111111111111111);
	array[0].set_port_stats(0).set_tx_packets	(0x2222222222222222);
	array[0].set_port_stats(0).set_rx_bytes		(0x3333333333333333);
	array[0].set_port_stats(0).set_tx_bytes		(0x4444444444444444);
	array[0].set_port_stats(0).set_rx_dropped	(0x5555555555555555);
	array[0].set_port_stats(0).set_tx_dropped	(0x6666666666666666);
	array[0].set_port_stats(0).set_rx_errors	(0x7777777777777777);
	array[0].set_port_stats(0).set_tx_errors	(0x8888888888888888);
	array[0].set_port_stats(0).set_rx_frame_err	(0x9999999999999999);
	array[0].set_port_stats(0).set_rx_over_err	(0xaaaaaaaaaaaaaaaa);
	array[0].set_port_stats(0).set_rx_crc_err	(0xbbbbbbbbbbbbbbbb);
	array[0].set_port_stats(0).set_collisions	(0xcccccccccccccccc);
	array[0].set_port_stats(0).set_duration_sec	(0x11111111);
	array[0].set_port_stats(0).set_duration_nsec(0x22222222);

	array[0].set_port_stats(1).set_version(rofl::openflow13::OFP_VERSION);
	array[0].set_port_stats(1).set_port_no(1);
	array[0].set_port_stats(1).set_rx_packets	(0xa1a1a1a1a1a1a1a1);
	array[0].set_port_stats(1).set_tx_packets	(0xa2a2a2a2a2a2a2a2);
	array[0].set_port_stats(1).set_rx_bytes		(0xa3a3a3a3a3a3a3a3);
	array[0].set_port_stats(1).set_tx_bytes		(0xa4a4a4a4a4a4a4a4);
	array[0].set_port_stats(1).set_rx_dropped	(0xa5a5a5a5a5a5a5a5);
	array[0].set_port_stats(1).set_tx_dropped	(0xa6a6a6a6a6a6a6a6);
	array[0].set_port_stats(1).set_rx_errors	(0xa7a7a7a7a7a7a7a7);
	array[0].set_port_stats(1).set_tx_errors	(0xa8a8a8a8a8a8a8a8);
	array[0].set_port_stats(1).set_rx_frame_err	(0xa9a9a9a9a9a9a9a9);
	array[0].set_port_stats(1).set_rx_over_err	(0xaaaaaaaaaaaaaaaa);
	array[0].set_port_stats(1).set_rx_crc_err	(0xabababababababab);
	array[0].set_port_stats(1).set_collisions	(0xacacacacacacacac);
	array[0].set_port_stats(1).set_duration_sec	(0xa1a1a1a1);
	array[0].set_port_stats(1).set_duration_nsec(0xa2a2a2a2);

	array[1].set_port_stats(2).set_version(rofl::openflow13::OFP_VERSION);
	array[1].set_port_stats(2).set_port_no(2);
	array[1].set_port_stats(2).set_rx_packets	(0x1111111111111111);
	array[1].set_port_stats(2).set_tx_packets	(0x2222222222222222);
	array[1].set_port_stats(2).set_rx_bytes		(0x3333333333333333);
	array[1].set_port_stats(2).set_tx_bytes		(0x4444444444444444);
	array[1].set_port_stats(2).set_rx_dropped	(0x5555555555555555);
	array[1].set_port_stats(2).set_tx_dropped	(0x6666666666666666);
	array[1].set_port_stats(2).set_rx_errors	(0x7777777777777777);
	array[1].set_port_stats(2).set_tx_errors	(0x8888888888888888);
	array[1].set_port_stats(2).set_rx_frame_err	(0x9999999999999999);
	array[1].set_port_stats(2).set_rx_over_err	(0xaaaaaaaaaaaaaaaa);
	array[1].set_port_stats(2).set_rx_crc_err	(0xbbbbbbbbbbbbbbbb);
	array[1].set_port_stats(2).set_collisions	(0xcccccccccccccccc);
	array[1].set_port_stats(2).set_duration_sec	(0x11111111);
	array[1].set_port_stats(2).set_duration_nsec(0x22222222);


#ifdef DEBUG
	std::cerr << "array[0]:" << std::endl << array[0];
	std::cerr << "array[1]:" << std::endl << array[1];
#endif

	rofl::openflow::cofportstatsarray merge(array[0]);
	merge += array[1];

#ifdef DEBUG
	std::cerr << "array[0]+array[1]:" << std::endl << merge;
	std::cerr << "merge.set_port_stats(0) == array[0].get_port_stats(0): " << (merge.set_port_stats(0) == array[0].get_port_stats(0)) << std::endl;
	std::cerr << "merge.set_port_stats(1) == array[0].get_port_stats(1): " << (merge.set_port_stats(1) == array[0].get_port_stats(1)) << std::endl;
	std::cerr << "merge.set_port_stats(2) == array[1].get_port_stats(2): " << (merge.set_port_stats(2) == array[1].get_port_stats(2)) << std::endl;
#endif

	CPPUNIT_ASSERT(merge.set_port_stats(0) == array[0].get_port_stats(0));
	CPPUNIT_ASSERT(merge.set_port_stats(1) == array[0].get_port_stats(1));
	CPPUNIT_ASSERT(merge.set_port_stats(2) == array[1].get_port_stats(2));
}



void
cofportstatsarray_test::testPackUnpack()
{
	/*
	 * unpack
	 */
	rofl::cmemory mem(sizeof(struct rofl::openflow13::ofp_port_stats));
	struct rofl::openflow13::ofp_port_stats *port = (struct rofl::openflow13::ofp_port_stats*)mem.somem();

	port->port_no = htobe32(8);
	port->rx_packets = htobe64(0x1112131415161718);
	port->tx_packets = htobe64(0x2122232425262728);
	port->rx_bytes = htobe64(0x3132333435363738);
	port->tx_bytes = htobe64(0x4142434445464748);
	port->rx_dropped = htobe64(0x5152535455565758);
	port->tx_dropped = htobe64(0x6162636465666768);
	port->rx_errors = htobe64(0x7172737475767778);
	port->tx_errors = htobe64(0x8182838485868788);
	port->rx_frame_err = htobe64(0x9192939495969798);
	port->rx_over_err = htobe64(0xa1a2a3a4a5a6a7a8);
	port->rx_crc_err = htobe64(0xb1b2b3b4b5b6b7b8);
	port->collisions = htobe64(0xc1c2c3c4c5c6c7c8);
	port->duration_sec = htobe32(0xd1d2d3d4);
	port->duration_nsec = htobe32(0xe1e2e3e4);


	rofl::openflow::cofportstatsarray array(rofl::openflow13::OFP_VERSION);

	array.unpack(mem.somem(), mem.memlen());

#ifdef DEBUG
	std::cerr << "mem:" << std::endl << mem;
	std::cerr << "unpacked mem:" << std::endl << array;
#endif

	CPPUNIT_ASSERT(array.get_port_stats(8).get_port_no() == 8);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_rx_packets() == 0x1112131415161718);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_tx_packets() == 0x2122232425262728);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_rx_bytes() == 0x3132333435363738);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_tx_bytes() == 0x4142434445464748);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_rx_dropped() == 0x5152535455565758);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_tx_dropped() == 0x6162636465666768);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_rx_errors() == 0x7172737475767778);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_tx_errors() == 0x8182838485868788);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_rx_frame_err() == 0x9192939495969798);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_rx_over_err() == 0xa1a2a3a4a5a6a7a8);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_rx_crc_err() == 0xb1b2b3b4b5b6b7b8);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_collisions() == 0xc1c2c3c4c5c6c7c8);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_duration_sec() == 0xd1d2d3d4);
	CPPUNIT_ASSERT(array.get_port_stats(8).get_duration_nsec() == 0xe1e2e3e4);

	/*
	 * pack
	 */

	array.clear();

	try {
		array.get_port_stats(8);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::ePortStatsNotFound& e) {};

	array.set_port_stats(8).set_port_no(8);
	array.set_port_stats(8).set_rx_packets(0x1112131415161718);
	array.set_port_stats(8).set_tx_packets(0x2122232425262728);
	array.set_port_stats(8).set_rx_bytes(0x3132333435363738);
	array.set_port_stats(8).set_tx_bytes(0x4142434445464748);
	array.set_port_stats(8).set_rx_dropped(0x5152535455565758);
	array.set_port_stats(8).set_tx_dropped(0x6162636465666768);
	array.set_port_stats(8).set_rx_errors(0x7172737475767778);
	array.set_port_stats(8).set_tx_errors(0x8182838485868788);
	array.set_port_stats(8).set_rx_frame_err(0x9192939495969798);
	array.set_port_stats(8).set_rx_over_err(0xa1a2a3a4a5a6a7a8);
	array.set_port_stats(8).set_rx_crc_err(0xb1b2b3b4b5b6b7b8);
	array.set_port_stats(8).set_collisions(0xc1c2c3c4c5c6c7c8);
	array.set_port_stats(8).set_duration_sec(0xd1d2d3d4);
	array.set_port_stats(8).set_duration_nsec(0xe1e2e3e4);

	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

	CPPUNIT_ASSERT(marray == mem);

	port = (struct rofl::openflow13::ofp_port_stats*)marray.somem();

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "packed array:" << std::endl << marray;
#endif

	CPPUNIT_ASSERT(be32toh(port->port_no) == 8);
	CPPUNIT_ASSERT(be64toh(port->rx_packets) == 0x1112131415161718);
	CPPUNIT_ASSERT(be64toh(port->tx_packets) == 0x2122232425262728);
	CPPUNIT_ASSERT(be64toh(port->rx_bytes) == 0x3132333435363738);
	CPPUNIT_ASSERT(be64toh(port->tx_bytes) == 0x4142434445464748);
	CPPUNIT_ASSERT(be64toh(port->rx_dropped) == 0x5152535455565758);
	CPPUNIT_ASSERT(be64toh(port->tx_dropped) == 0x6162636465666768);
	CPPUNIT_ASSERT(be64toh(port->rx_errors) == 0x7172737475767778);
	CPPUNIT_ASSERT(be64toh(port->tx_errors) == 0x8182838485868788);
	CPPUNIT_ASSERT(be64toh(port->rx_frame_err) == 0x9192939495969798);
	CPPUNIT_ASSERT(be64toh(port->rx_over_err) == 0xa1a2a3a4a5a6a7a8);
	CPPUNIT_ASSERT(be64toh(port->rx_crc_err) == 0xb1b2b3b4b5b6b7b8);
	CPPUNIT_ASSERT(be64toh(port->collisions) == 0xc1c2c3c4c5c6c7c8);
	CPPUNIT_ASSERT(be32toh(port->duration_sec) == 0xd1d2d3d4);
	CPPUNIT_ASSERT(be32toh(port->duration_nsec) == 0xe1e2e3e4);
}



void
cofportstatsarray_test::testAddDropSetGetHas()
{
	rofl::openflow::cofportstatsarray array(rofl::openflow13::OFP_VERSION);

	try {
		array.get_port_stats(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::ePortStatsNotFound& e) {};

	if (array.has_port_stats(0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_port_stats(0);

	try {
		array.get_port_stats(0);
	} catch (rofl::openflow::ePortStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		array.set_port_stats(0);
	} catch (rofl::openflow::ePortStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not array.has_port_stats(0)) {
		CPPUNIT_ASSERT(false);
	}

	array.drop_port_stats(0);

	try {
		array.get_port_stats(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::ePortStatsNotFound& e) {};

	if (array.has_port_stats(0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_port_stats(0);
}


