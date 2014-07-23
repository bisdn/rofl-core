/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
}



void
ctimerid_test::testCopyConstructor()
{
	rofl::ctimerid tid1;
	rofl::ctimerid tid2(tid1);

	CPPUNIT_ASSERT(tid1.get_tid() == tid2.get_tid());
}



void
ctimerid_test::testAssignmentOperator()
{
	rofl::ctimerid tid1;
	rofl::ctimerid tid2 = tid1;

	CPPUNIT_ASSERT(tid1.get_tid() == tid2.get_tid());
}



void
ctimerid_test::testGetterAndSetter()
{
	rofl::ctimerid tid1;

	uint32_t tid = 0x55555555;
	tid1.set_tid(tid);

	CPPUNIT_ASSERT(tid1.get_tid() == tid);
}




