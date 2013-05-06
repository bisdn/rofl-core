#include "rofl/common/crofbase.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace rofl;

class crofbaseTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( crofbaseTest );
#if 1
	CPPUNIT_TEST( testConnect );
#endif
	CPPUNIT_TEST_SUITE_END();

private:

	crofbase *ctl;
	crofbase *dpt;

public:
	void setUp();
	void tearDown();

	void testConnect();
};

