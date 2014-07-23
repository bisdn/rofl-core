/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofbucketcounter.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofbucketcounter_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofbucketcounter_test );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testCopyConstructor );
	CPPUNIT_TEST( testPackUnpack );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
	void testPackUnpack();
};

