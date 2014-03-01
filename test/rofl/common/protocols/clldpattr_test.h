#include "rofl/common/protocols/clldpattr.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class clldpattrTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( clldpattrTest );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testCopyConstructor );
	CPPUNIT_TEST( testPackUnpack );
	CPPUNIT_TEST( testId );
	CPPUNIT_TEST( testTTL );
	CPPUNIT_TEST( testDesc );
	CPPUNIT_TEST( testSysCaps );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
	void testPackUnpack();
	void testId();
	void testTTL();
	void testDesc();
	void testSysCaps();
};

