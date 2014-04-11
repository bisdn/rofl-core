#include "rofl/common/openflow/coftables.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class coftablesTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( coftablesTest );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testCopyConstructor );
	CPPUNIT_TEST( testPackUnpack );
	CPPUNIT_TEST( testAddDropSetGetHas );
	CPPUNIT_TEST( testMappingTableStatsArray );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
	void testPackUnpack();
	void testAddDropSetGetHas();
	void testMappingTableStatsArray();
};

