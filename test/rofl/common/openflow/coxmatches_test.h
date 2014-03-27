#include "rofl/common/openflow/coxmatches.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class coxmatches_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( coxmatches_test );
	CPPUNIT_TEST( testPack );
	CPPUNIT_TEST( testUnPack );
	CPPUNIT_TEST( testAddMatch );
	CPPUNIT_TEST( testDropMatch );
	CPPUNIT_TEST( testSetMatch );
	CPPUNIT_TEST( testGetMatch );
	CPPUNIT_TEST( testHasMatch );
	CPPUNIT_TEST( testStrictMatching );
	CPPUNIT_TEST( testNonStrictMatching );
	CPPUNIT_TEST( testOxmVlanVidUnpack );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testPack();
	void testUnPack();
	void testAddMatch();
	void testDropMatch();
	void testSetMatch();
	void testGetMatch();
	void testHasMatch();
	void testStrictMatching();
	void testNonStrictMatching();

	void testOxmVlanVidUnpack();
};

