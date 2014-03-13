#include "rofl/common/protocols/clldpmsg.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class clldpmsgTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( clldpmsgTest );
	CPPUNIT_TEST( testLLDPMsg );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testLLDPMsg();
};

