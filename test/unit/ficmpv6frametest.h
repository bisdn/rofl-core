#include "rofl/common/protocols/ficmpv6frame.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace rofl;

class ficmpv6frameTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( ficmpv6frameTest );
	CPPUNIT_TEST( testICMPv6Type );
	CPPUNIT_TEST( testICMPv6Code );
	CPPUNIT_TEST_SUITE_END();

private:

	cmemory 		*mem;
	ficmpv6frame 	*icmpv6;

public:
	void setUp();
	void tearDown();

	void testICMPv6Type();
	void testICMPv6Code();
};

