#include "rofl/common/openflow/cofportstatsarray.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofportstatsarray_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofportstatsarray_test );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testCopyConstructor );
	CPPUNIT_TEST( testOperatorPlus );
	CPPUNIT_TEST( testPackUnpack );
	CPPUNIT_TEST( testAddDropSetGetHas );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
	void testOperatorPlus();
	void testPackUnpack();
	void testAddDropSetGetHas();
};

