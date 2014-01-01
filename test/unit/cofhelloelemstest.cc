#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "cofhelloelemstest.h"
#include <stdlib.h>

CPPUNIT_TEST_SUITE_REGISTRATION( cofhelloelemsTest );

void
cofhelloelemsTest::setUp()
{
	elems = new rofl::openflow::cofhelloelems();
}



void
cofhelloelemsTest::tearDown()
{
	delete elems;
}



void
cofhelloelemsTest::testPack()
{

	//CPPUNIT_ASSERT(*rule_nonstrict == oxl);
}



void
cofhelloelemsTest::testUnpack()
{

	//CPPUNIT_ASSERT(*rule_nonstrict == oxl);
}

