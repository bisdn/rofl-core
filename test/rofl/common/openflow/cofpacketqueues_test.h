#include "rofl/common/openflow/cofpacketqueues.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofpacketqueues_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofpacketqueues_test );
	CPPUNIT_TEST( testPacketQueues );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testPacketQueues();
};

