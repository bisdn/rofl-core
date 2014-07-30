#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "caddrinfo_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( caddrinfo_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
caddrinfo_test::setUp()
{
}



void
caddrinfo_test::tearDown()
{
}



void
caddrinfo_test::testAddrInfo()
{
	rofl::caddrinfo ainfo;

	std::cerr << "ainfo:" << std::endl << ainfo;
}


