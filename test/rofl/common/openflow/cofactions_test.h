#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofactions_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofactions_test );
	CPPUNIT_TEST( testActions );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testActions();

};

