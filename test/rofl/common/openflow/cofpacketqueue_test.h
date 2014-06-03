#include "rofl/common/openflow/cofpacketqueue.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofpacketqueue_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofpacketqueue_test );
	CPPUNIT_TEST( testPacketQueue );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testPacketQueue();
};

