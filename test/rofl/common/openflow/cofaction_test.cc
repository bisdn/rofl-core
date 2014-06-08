#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofaction_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofaction_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofaction_test::setUp()
{
}



void
cofaction_test::tearDown()
{
}



void
cofaction_test::testAction()
{
	rofl::openflow::cofaction action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());
}





