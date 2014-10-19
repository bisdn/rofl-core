#include "rofl/common/cpacket.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cpacket_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cpacket_test );
	CPPUNIT_TEST( test_push );
	CPPUNIT_TEST( test_pop );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void test_push();
	void test_pop();
};

