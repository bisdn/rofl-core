#include "rofl/common/protocols/fipv6frame.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
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
};

