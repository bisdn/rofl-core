#include "rofl/common/openflow/cofmeterconfigarray.h"
#include "rofl/common/cmemory.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofmeterconfigarray_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofmeterconfigarray_test );
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

