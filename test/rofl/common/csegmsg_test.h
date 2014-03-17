#include "rofl/common/csegmsg.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class csegmsg_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( csegmsg_test );
	CPPUNIT_TEST( testStoreAndMerge );
	CPPUNIT_TEST( testRetrieveAndDetach );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testStoreAndMerge();
	void testRetrieveAndDetach();
};

