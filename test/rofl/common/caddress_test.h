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

