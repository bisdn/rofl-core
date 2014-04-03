#include "rofl/common/caddress.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class caddress_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( caddress_test );
	CPPUNIT_TEST( testGetAddrInfo );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testGetAddrInfo();
};

