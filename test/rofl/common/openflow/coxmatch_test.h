/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class coxmatch_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( coxmatch_test );
	CPPUNIT_TEST( test1Byte );
	CPPUNIT_TEST( test2Bytes );
	CPPUNIT_TEST( test4Bytes );
	CPPUNIT_TEST( test6Bytes );
	CPPUNIT_TEST( test8Bytes );
	CPPUNIT_TEST( test1ByteHasMask );
	CPPUNIT_TEST( test2BytesHasMask );
	CPPUNIT_TEST( test4BytesHasMask );
	CPPUNIT_TEST( test6BytesHasMask );
	CPPUNIT_TEST( test8BytesHasMask );
	CPPUNIT_TEST( testOxmIPv4Src );
	CPPUNIT_TEST( testOxmIPv4Dst );
	CPPUNIT_TEST( testOxmIPv6Src );
	CPPUNIT_TEST( testOxmIPv6Dst );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void test1Byte();
	void test2Bytes();
	void test4Bytes();
	void test6Bytes();
	void test8Bytes();
	void test1ByteHasMask();
	void test2BytesHasMask();
	void test4BytesHasMask();
	void test6BytesHasMask();
	void test8BytesHasMask();

	void testOxmIPv4Src();
	void testOxmIPv4Dst();
	void testOxmIPv6Src();
	void testOxmIPv6Dst();
};

