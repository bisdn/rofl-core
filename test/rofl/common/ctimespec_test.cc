/*
 * csocket_impl_test.cc
 *
 *  Created on: 07.04.2014
 *      Author: andreas
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "ctimespec_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( ctimespec_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
ctimespec_test::setUp()
{
#ifdef DEBUG
	rofl::logging::set_debug_level(7);
#endif
}



void
ctimespec_test::tearDown()
{

}



void
ctimespec_test::testTimeSpec()
{
	rofl::ctimespec ts1(1, 100);
	rofl::ctimespec ts2(1, 200);
	rofl::ctimespec ts3(1, 300);


	CPPUNIT_ASSERT(ts1 < ts2);
	CPPUNIT_ASSERT(ts1 < ts3);
	CPPUNIT_ASSERT(ts2 < ts3);

	CPPUNIT_ASSERT(ts2 > ts1);
	CPPUNIT_ASSERT(ts3 > ts1);
	CPPUNIT_ASSERT(ts3 > ts2);

	ts1 = ts3;

	CPPUNIT_ASSERT(ts1.set_timespec().tv_sec = 1);
	CPPUNIT_ASSERT(ts1.set_timespec().tv_nsec = 300);
}



