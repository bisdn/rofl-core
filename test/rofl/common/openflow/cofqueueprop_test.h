#include "rofl/common/openflow/cofqueueprop.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofqueueprop_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofqueueprop_test );
	CPPUNIT_TEST( testQueueProp );
	CPPUNIT_TEST( testQueuePropMinRate );
	CPPUNIT_TEST( testQueuePropMaxRate );
	CPPUNIT_TEST( testQueuePropExperimenter );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testQueueProp();
	void testQueuePropMinRate();
	void testQueuePropMaxRate();
	void testQueuePropExperimenter();
};

