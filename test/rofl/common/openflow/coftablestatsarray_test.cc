#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coftablestatsarray_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( coftablestatsarray_test );

#if defined DEBUG
#undef DEBUG
#endif

void
coftablestatsarray_test::setUp()
{
}



void
coftablestatsarray_test::tearDown()
{
}



void
coftablestatsarray_test::testDefaultConstructor()
{
	rofl::openflow::coftablestatsarray array;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == array.get_version());
}



void
coftablestatsarray_test::testCopyConstructor()
{
	rofl::openflow::coftablestatsarray array(rofl::openflow13::OFP_VERSION);

	array.set_table_stats(0).set_version(rofl::openflow13::OFP_VERSION);
	array.set_table_stats(0).set_table_id(0);
	array.set_table_stats(0).set_active_count(0x11121314);
	array.set_table_stats(0).set_lookup_count(0x2122232425262728);
	array.set_table_stats(0).set_matched_count(0x3132333435363738);

	array.set_table_stats(1).set_version(rofl::openflow13::OFP_VERSION);
	array.set_table_stats(1).set_table_id(1);
	array.set_table_stats(1).set_active_count(0x11121314);
	array.set_table_stats(1).set_lookup_count(0x2122232425262728);
	array.set_table_stats(1).set_matched_count(0x3132333435363738);

	array.set_table_stats(4).set_version(rofl::openflow13::OFP_VERSION);
	array.set_table_stats(4).set_table_id(4);
	array.set_table_stats(4).set_active_count(0x11121314);
	array.set_table_stats(4).set_lookup_count(0x2122232425262728);
	array.set_table_stats(4).set_matched_count(0x3132333435363738);

	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "marray:" << std::endl << marray;
#endif

	rofl::openflow::coftablestatsarray clone(array);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(marray == mclone);
}



void
coftablestatsarray_test::testOperatorPlus()
{
	std::vector<rofl::openflow::coftablestatsarray> array;
	array.push_back(rofl::openflow::coftablestatsarray(rofl::openflow13::OFP_VERSION));
	array.push_back(rofl::openflow::coftablestatsarray(rofl::openflow13::OFP_VERSION));

	array[0].set_table_stats(0).set_version(rofl::openflow13::OFP_VERSION);
	array[0].set_table_stats(0).set_table_id(0);
	array[0].set_table_stats(0).set_active_count(0x11121314);
	array[0].set_table_stats(0).set_lookup_count(0x2122232425262728);
	array[0].set_table_stats(0).set_matched_count(0x3132333435363738);

	array[0].set_table_stats(1).set_version(rofl::openflow13::OFP_VERSION);
	array[0].set_table_stats(1).set_table_id(1);
	array[0].set_table_stats(1).set_active_count(0x11121314);
	array[0].set_table_stats(1).set_lookup_count(0x2122232425262728);
	array[0].set_table_stats(1).set_matched_count(0x3132333435363738);

	array[1].set_table_stats(4).set_version(rofl::openflow13::OFP_VERSION);
	array[1].set_table_stats(4).set_table_id(4);
	array[1].set_table_stats(4).set_active_count(0x11121314);
	array[1].set_table_stats(4).set_lookup_count(0x2122232425262728);
	array[1].set_table_stats(4).set_matched_count(0x3132333435363738);


#ifdef DEBUG
	std::cerr << "array[0]:" << std::endl << array[0];
	std::cerr << "array[1]:" << std::endl << array[1];
#endif

	rofl::openflow::coftablestatsarray merge(array[0]);
	merge += array[1];

#ifdef DEBUG
	std::cerr << "array[0]+array[1]:" << std::endl << merge;
	std::cerr << "merge.set_table_stats(0) == array[0].get_table_stats(0): " << (merge.set_table_stats(0) == array[0].get_table_stats(0)) << std::endl;
	std::cerr << "merge.set_table_stats(1) == array[0].get_table_stats(1): " << (merge.set_table_stats(1) == array[0].get_table_stats(1)) << std::endl;
	std::cerr << "merge.set_table_stats(4) == array[1].get_table_stats(4): " << (merge.set_table_stats(4) == array[1].get_table_stats(4)) << std::endl;
#endif

	CPPUNIT_ASSERT(merge.set_table_stats(0) == array[0].get_table_stats(0));
	CPPUNIT_ASSERT(merge.set_table_stats(1) == array[0].get_table_stats(1));
	CPPUNIT_ASSERT(merge.set_table_stats(4) == array[1].get_table_stats(4));
}



void
coftablestatsarray_test::testPackUnpack()
{
	/*
	 * unpack
	 */
	rofl::cmemory mem(sizeof(struct rofl::openflow13::ofp_table_stats));
	struct rofl::openflow13::ofp_table_stats *table = (struct rofl::openflow13::ofp_table_stats*)mem.somem();

	table->table_id = 8;
	table->active_count = htobe32(0x11121314);
	table->lookup_count = htobe64(0x2122232425262728);
	table->matched_count = htobe64(0x3132333435363738);

	rofl::openflow::coftablestatsarray array(rofl::openflow13::OFP_VERSION);

	array.unpack(mem.somem(), mem.memlen());

#ifdef DEBUG
	std::cerr << "mem:" << std::endl << mem;
	std::cerr << "unpacked mem:" << std::endl << array;
#endif

	CPPUNIT_ASSERT(array.get_table_stats(8).get_table_id() == 8);
	CPPUNIT_ASSERT(array.get_table_stats(8).get_active_count() == 0x11121314);
	CPPUNIT_ASSERT(array.get_table_stats(8).get_lookup_count() == 0x2122232425262728);
	CPPUNIT_ASSERT(array.get_table_stats(8).get_matched_count() == 0x3132333435363738);

	/*
	 * pack
	 */

	array.clear();

	try {
		array.get_table_stats(8);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eTableStatsNotFound& e) {};

	array.set_table_stats(8).set_table_id(8);
	array.set_table_stats(8).set_active_count(0x11121314);
	array.set_table_stats(8).set_lookup_count(0x2122232425262728);
	array.set_table_stats(8).set_matched_count(0x3132333435363738);

	rofl::cmemory marray(array.length());
	array.pack(marray.somem(), marray.memlen());

	CPPUNIT_ASSERT(marray == mem);

	table = (struct rofl::openflow13::ofp_table_stats*)marray.somem();

#ifdef DEBUG
	std::cerr << "array:" << std::endl << array;
	std::cerr << "packed array:" << std::endl << marray;
#endif

	CPPUNIT_ASSERT(table->table_id == 8);
	CPPUNIT_ASSERT(be32toh(table->active_count) == 0x11121314);
	CPPUNIT_ASSERT(be64toh(table->lookup_count) == 0x2122232425262728);
	CPPUNIT_ASSERT(be64toh(table->matched_count) == 0x3132333435363738);
}



void
coftablestatsarray_test::testAddDropSetGetHas()
{
	rofl::openflow::coftablestatsarray array(rofl::openflow13::OFP_VERSION);

	try {
		array.get_table_stats(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eTableStatsNotFound& e) {};

	if (array.has_table_stats(0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_table_stats(0);

	try {
		array.get_table_stats(0);
	} catch (rofl::openflow::eTableStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		array.set_table_stats(0);
	} catch (rofl::openflow::eTableStatsNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not array.has_table_stats(0)) {
		CPPUNIT_ASSERT(false);
	}

	array.drop_table_stats(0);

	try {
		array.get_table_stats(0);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eTableStatsNotFound& e) {};

	if (array.has_table_stats(0)) {
		CPPUNIT_ASSERT(false);
	}
	array.add_table_stats(0);
}


