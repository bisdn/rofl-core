#include "rofl/common/openflow/messages/cofmsg_flow_mod.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofmsgflowmodTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofmsgflowmodTest );
	CPPUNIT_TEST( testMsgFlowMod );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testMsgFlowMod();
};

