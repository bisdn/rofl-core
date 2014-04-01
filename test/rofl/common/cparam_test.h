#include "rofl/common/cparam.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cparam_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cparam_test );
	CPPUNIT_TEST( testAll );
	CPPUNIT_TEST( testInt );
	CPPUNIT_TEST( testUnsignedInt );
	CPPUNIT_TEST( testBool );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testAll();
	void testInt();
	void testUnsignedInt();
	void testBool();
};

