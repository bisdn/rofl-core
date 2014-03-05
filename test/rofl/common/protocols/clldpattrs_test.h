#include "rofl/common/protocols/clldpattrs.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class clldpattrsTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( clldpattrsTest );
	CPPUNIT_TEST( testLLDPAttrs );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testLLDPAttrs();
};

