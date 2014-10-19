#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "csockaddr_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( csockaddr_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
csockaddr_test::setUp()
{
}



void
csockaddr_test::tearDown()
{
}



void
csockaddr_test::testSockAddr()
{
	rofl::csockaddr saddr;

	saddr = rofl::csockaddr(rofl::caddress_in4("127.0.0.1"), 6633);

	std::cerr << "saddr:" << std::endl << saddr;

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0xa4a3a2a1;
	sin.sin_port = htobe16(6633);

	saddr.unpack((uint8_t*)&sin, sizeof(sin));

	std::cerr << "saddr:" << std::endl << saddr;
}


