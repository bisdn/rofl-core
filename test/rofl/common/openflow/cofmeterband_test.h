#include "rofl/common/openflow/cofmeterband.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofmeterband_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofmeterband_test );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testCopyConstructor );
	CPPUNIT_TEST( testPackUnpack );
	CPPUNIT_TEST( testInvalidLength );
	CPPUNIT_TEST( testDscpRemark );
	CPPUNIT_TEST( testExperimenter );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
	void testPackUnpack();
	void testInvalidLength();
	void testDscpRemark();
	void testExperimenter();
};

