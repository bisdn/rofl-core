#include "rofl/common/cparams.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cparams_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cparams_test );
	CPPUNIT_TEST( testAll );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testAll();
};

