#include "rofl/common/openflow/cofaction.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofaction_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofaction_test );
	CPPUNIT_TEST( testAction );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testAction();
};

