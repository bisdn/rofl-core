#include "rofl/common/caddrinfo.h"
#include "rofl/common/cmemory.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class caddrinfo_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( caddrinfo_test );
	CPPUNIT_TEST( testAddrInfo );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testAddrInfo();
};

