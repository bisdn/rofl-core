#include "rofl/common/openflow/messages/cofmsg_meter_features_stats.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofmsgmeterfeaturesTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofmsgmeterfeaturesTest );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testPackUnpack );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testPackUnpack();
};

