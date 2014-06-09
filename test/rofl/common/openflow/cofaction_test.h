#include "rofl/common/openflow/cofaction.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofaction_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofaction_test );
	CPPUNIT_TEST( testAction );
	CPPUNIT_TEST( testActionOutput );
	CPPUNIT_TEST( testActionOutput10 );
	CPPUNIT_TEST( testActionSetVlanVid );
	CPPUNIT_TEST( testActionSetVlanPcp );
	CPPUNIT_TEST( testActionStripVlan );
	CPPUNIT_TEST( testActionSetDlSrc );
	CPPUNIT_TEST( testActionSetDlDst );
	CPPUNIT_TEST( testActionSetNwSrc );
	CPPUNIT_TEST( testActionSetNwDst );
	CPPUNIT_TEST( testActionSetNwTos );
	CPPUNIT_TEST( testActionSetTpSrc );
	CPPUNIT_TEST( testActionSetTpDst );
	CPPUNIT_TEST( testActionEnqueue );
	CPPUNIT_TEST( testActionVendor );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testAction();
	void testActionOutput();
	void testActionOutput10();
	void testActionSetVlanVid();
	void testActionSetVlanPcp();
	void testActionStripVlan();
	void testActionSetDlSrc();
	void testActionSetDlDst();
	void testActionSetNwSrc();
	void testActionSetNwDst();
	void testActionSetNwTos();
	void testActionSetTpSrc();
	void testActionSetTpDst();
	void testActionEnqueue();
	void testActionVendor();
};

