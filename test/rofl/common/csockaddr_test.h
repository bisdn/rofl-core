#include "rofl/common/csockaddr.h"
#include "rofl/common/cmemory.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class csockaddr_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( csockaddr_test );
	CPPUNIT_TEST( testSockAddr );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testSockAddr();
};

