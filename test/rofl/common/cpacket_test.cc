#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cpacket_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cpacket_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cpacket_test::setUp()
{
}



void
cpacket_test::tearDown()
{
}



void
cpacket_test::test_push()
{
	rofl::cpacket p(32);

	for (unsigned int i = 0; i < 32; i++) {
		p[i] = i;
	}

	rofl::cpacket pclone(p);

	std::cerr << p;
	p.push(14, 4);
	std::cerr << p;
	p.pop(14, 4);
	std::cerr << p;

	CPPUNIT_ASSERT(p == pclone);
}



void
cpacket_test::test_pop()
{
}

