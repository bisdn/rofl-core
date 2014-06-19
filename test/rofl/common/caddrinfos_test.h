#include "rofl/common/caddrinfos.h"
#include "rofl/common/cmemory.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class caddrinfos_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( caddrinfos_test );
	CPPUNIT_TEST( testAddrInfos );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testAddrInfos();
};

