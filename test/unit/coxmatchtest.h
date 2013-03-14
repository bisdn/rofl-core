#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace rofl;

class coxmatchTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( coxmatchTest );
	CPPUNIT_TEST( testInPort );
	CPPUNIT_TEST( testIPv4Src );
	CPPUNIT_TEST( testIPv4SrcWithMask );
	CPPUNIT_TEST( testEthType );
	CPPUNIT_TEST_SUITE_END();

private:

public:
	void setUp();
	void tearDown();

	void testInPort();
	void testIPv4Src();
	void testIPv4SrcWithMask();
	void testEthType();
};

