#include "rofl/common/openflow/cofhelloelems.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofhelloelemsTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofhelloelemsTest );
	CPPUNIT_TEST( testPack );
	CPPUNIT_TEST( testUnPack );
	CPPUNIT_TEST_SUITE_END();

private:

	rofl::openflow::cofhelloelems *elems;
	rofl::cmemory *mem;

public:
	void setUp();
	void tearDown();

	void testPack();
	void testUnPack();
};

