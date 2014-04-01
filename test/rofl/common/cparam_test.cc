#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cparam_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cparam_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
cparam_test::setUp()
{
}



void
cparam_test::tearDown()
{
}



void
cparam_test::testAll()
{
	std::string s_test = std::string("a test string ...");

	rofl::cparam p;

	p.set_string() = s_test;

	rofl::cparam pp(s_test);

	rofl::cparam clone(p);

	CPPUNIT_ASSERT(p == clone);
	CPPUNIT_ASSERT(pp.get_string() == s_test);
	CPPUNIT_ASSERT(clone.get_string() == s_test);
}


