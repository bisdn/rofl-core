#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "caddrinfos_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( caddrinfos_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
caddrinfos_test::setUp()
{
}



void
caddrinfos_test::tearDown()
{
}



void
caddrinfos_test::testAddrInfos()
{
	rofl::caddrinfos ai;

	ai.set_node("localhost");
	ai.set_ai_hints().set_ai_family(AF_INET);
	ai.set_ai_hints().set_ai_socktype(SOCK_STREAM);
	ai.resolve();

	std::cerr << "addrinfos:" << std::endl << ai;

	ai.set_node("www.bisdn.de");
	ai.set_service("http");
	ai.resolve();

	std::cerr << "addrinfos:" << std::endl << ai;

	ai.set_node("127.0.0.1");
	ai.set_ai_hints().set_ai_family(AF_INET);
	ai.set_ai_hints().set_ai_socktype(SOCK_STREAM);
	ai.resolve();

	std::cerr << "addrinfos:" << std::endl << ai;
}


