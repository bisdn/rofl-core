#include <rofl/common/protocols/fipv6frame.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>



class fipv6frameTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( fipv6frameTest );
	CPPUNIT_TEST( testVersion );
	CPPUNIT_TEST( testTrafficClass );
	CPPUNIT_TEST_SUITE_END();

private:

	cmemory *mem_A;
	cmemory *mem_B;
	fipv6frame *ipv6_A;
	fipv6frame *ipv6_B;

public:
	void setUp();
	void tearDown();

	void testVersion();
	void testTrafficClass();
};

