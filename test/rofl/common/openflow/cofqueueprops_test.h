#include "rofl/common/openflow/cofqueueprops.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofqueueprops_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofqueueprops_test );
	CPPUNIT_TEST( testQueueProps );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testQueueProps();
};

