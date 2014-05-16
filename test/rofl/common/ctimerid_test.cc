/*
 * csocket_impl_test.cc
 *
 *  Created on: 07.04.2014
 *      Author: andreas
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "ctimerid_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( ctimerid_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
ctimerid_test::setUp()
{
#ifdef DEBUG
	rofl::logging::set_debug_level(7);
#endif
}



void
ctimerid_test::tearDown()
{

}



void
ctimerid_test::testDefaultConstructor()
{
	rofl::ctimerid tid;

	CPPUNIT_ASSERT(tid.get_tid() == 0);
}



void
ctimerid_test::testCopyConstructor()
{
	uint32_t tid = 0x12345678;
	rofl::ctimerid tid1(tid);
	rofl::ctimerid tid2(tid1);

	CPPUNIT_ASSERT(tid1.get_tid() == tid);
	CPPUNIT_ASSERT(tid2.get_tid() == tid);
}



void
ctimerid_test::testAssignmentOperator()
{
	uint32_t tid = 0x12345678;
	rofl::ctimerid tid1(tid);
	rofl::ctimerid tid2 = tid1;

	CPPUNIT_ASSERT(tid1.get_tid() == tid);
	CPPUNIT_ASSERT(tid2.get_tid() == tid);
}



void
ctimerid_test::testComparisonOperator()
{
	uint32_t tid = 0x12345678;
	rofl::ctimerid tid1(tid);
	rofl::ctimerid tid2(tid);

	CPPUNIT_ASSERT(tid1.get_tid() == tid);
	CPPUNIT_ASSERT(tid2.get_tid() == tid);
	CPPUNIT_ASSERT(tid1 == tid2);

	tid1.set_tid(0x55555555);

	CPPUNIT_ASSERT(not (tid1 == tid2));
}



void
ctimerid_test::testGetterAndSetter()
{
	uint32_t tid = 0x12345678;
	rofl::ctimerid tid1(tid);

	CPPUNIT_ASSERT(tid1.get_tid() == tid);

	tid = 0x55555555;
	tid1.set_tid(tid);

	CPPUNIT_ASSERT(tid1.get_tid() == tid);
}



void
ctimerid_test::testTimerIdWithMap()
{
	std::map<int, rofl::ctimerid> tids;

	for (int i = 0; i < 4; i++) {
		tids[i] = rofl::ctimerid((uint32_t)i);
	}

	for (int i = 0; i < 4; i++) {
		std::cerr << tids[i];
		CPPUNIT_ASSERT(tids[i].get_tid() == (uint32_t)i);
	}
}




