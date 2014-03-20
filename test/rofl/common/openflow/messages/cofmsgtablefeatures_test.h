#include "rofl/common/openflow/messages/cofmsg_table_features_stats.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofmsgtablefeaturesTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofmsgtablefeaturesTest );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testCopyConstructor );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
};

