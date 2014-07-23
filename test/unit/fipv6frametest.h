/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/protocols/fipv6frame.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/ciosrv.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace rofl;

class fipv6frameTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( fipv6frameTest );
	CPPUNIT_TEST( testVersion );
	CPPUNIT_TEST( testTrafficClass );
	CPPUNIT_TEST( testFlowLabel );
	CPPUNIT_TEST( testPayloadLength );
	CPPUNIT_TEST( testNextHeader );
	CPPUNIT_TEST( testHopLimit );
	CPPUNIT_TEST( testSrcAddress );
	CPPUNIT_TEST( testDstAddress );
	CPPUNIT_TEST( testAllHeaders );
	CPPUNIT_TEST_SUITE_END();

private:

	cmemory 	*mem;
	fipv6frame 	*ipv6;
	caddress 	*ipv6_src;
	caddress 	*ipv6_dst;

public:
	void setUp();
	void tearDown();

	void testVersion();
	void testTrafficClass();
	void testFlowLabel();
	void testPayloadLength();
	void testNextHeader();
	void testHopLimit();
	void testSrcAddress();
	void testDstAddress();
	void testAllHeaders();
};

