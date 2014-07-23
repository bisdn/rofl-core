/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/coftablefeatures.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class coftablefeaturesTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( coftablefeaturesTest );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testCopyConstructor );
	CPPUNIT_TEST( testPackUnpack );
#if 0
	CPPUNIT_TEST( testInstructionsClass );
	CPPUNIT_TEST( testNextTablesClass );
	CPPUNIT_TEST( testActionsClass );
	CPPUNIT_TEST( testOxmClass );
#endif
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
	void testPackUnpack();
	void testInstructionsClass();
	void testNextTablesClass();
	void testActionsClass();
	void testOxmClass();
};

