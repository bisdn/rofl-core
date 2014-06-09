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
	CPPUNIT_TEST( testActionCopyTtlOut );
	CPPUNIT_TEST( testActionCopyTtlIn );
	CPPUNIT_TEST( testActionSetMplsTtl );
	CPPUNIT_TEST( testActionDecMplsTtl );
	CPPUNIT_TEST( testActionPushVlan );
	CPPUNIT_TEST( testActionPopVlan );
	CPPUNIT_TEST( testActionPushMpls );
	CPPUNIT_TEST( testActionPopMpls );
	CPPUNIT_TEST( testActionGroup );
	CPPUNIT_TEST( testActionSetNwTtl );
	CPPUNIT_TEST( testActionDecNwTtl );
	CPPUNIT_TEST( testActionSetQueue );
	CPPUNIT_TEST( testActionSetField );
	CPPUNIT_TEST( testActionExperimenter );
	CPPUNIT_TEST( testActionPushPbb );
	CPPUNIT_TEST( testActionPopPbb );
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
	void testActionCopyTtlOut();
	void testActionCopyTtlIn();
	void testActionSetMplsTtl();
	void testActionDecMplsTtl();
	void testActionPushVlan();
	void testActionPopVlan();
	void testActionPushMpls();
	void testActionPopMpls();
	void testActionGroup();
	void testActionSetNwTtl();
	void testActionDecNwTtl();
	void testActionSetQueue();
	void testActionSetField();
	void testActionExperimenter();
	void testActionPushPbb();
	void testActionPopPbb();

};

