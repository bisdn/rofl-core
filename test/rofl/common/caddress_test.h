/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/caddress.h"
#include "rofl/common/cmemory.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class caddress_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( caddress_test );
	CPPUNIT_TEST( testAddressLL );
	CPPUNIT_TEST( testAddressIn4 );
	CPPUNIT_TEST( testAddressIn6 );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testAddressLL();
	void testAddressIn4();
	void testAddressIn6();
};

