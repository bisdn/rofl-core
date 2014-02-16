#include "rofl/common/openflow/coftablefeatureprops.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class coftablefeaturepropsTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( coftablefeaturepropsTest );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testCopyConstructor );
	CPPUNIT_TEST( testAddDropSetGetHasInstructions );
	CPPUNIT_TEST( testAddDropSetGetHasInstructionsMiss );
	CPPUNIT_TEST( testAddDropSetGetHasNextTables );
	CPPUNIT_TEST( testAddDropSetGetHasNextTablesMiss );
	CPPUNIT_TEST( testAddDropSetGetHasWriteActions );
	CPPUNIT_TEST( testAddDropSetGetHasWriteActionsMiss );
	CPPUNIT_TEST( testAddDropSetGetHasApplyActions );
	CPPUNIT_TEST( testAddDropSetGetHasApplyActionsMiss );
	CPPUNIT_TEST( testAddDropSetGetHasMatch );
	CPPUNIT_TEST( testAddDropSetGetHasWildcards );
	CPPUNIT_TEST( testAddDropSetGetHasWriteSetField );
	CPPUNIT_TEST( testAddDropSetGetHasWriteSetFieldMiss );
	CPPUNIT_TEST( testAddDropSetGetHasApplySetField );
	CPPUNIT_TEST( testAddDropSetGetHasApplySetFieldMiss );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
	void testAddDropSetGetHasInstructions();
	void testAddDropSetGetHasInstructionsMiss();
	void testAddDropSetGetHasNextTables();
	void testAddDropSetGetHasNextTablesMiss();
	void testAddDropSetGetHasWriteActions();
	void testAddDropSetGetHasWriteActionsMiss();
	void testAddDropSetGetHasApplyActions();
	void testAddDropSetGetHasApplyActionsMiss();
	void testAddDropSetGetHasMatch();
	void testAddDropSetGetHasWildcards();
	void testAddDropSetGetHasWriteSetField();
	void testAddDropSetGetHasWriteSetFieldMiss();
	void testAddDropSetGetHasApplySetField();
	void testAddDropSetGetHasApplySetFieldMiss();
};

