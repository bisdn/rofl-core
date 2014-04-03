#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "caddress_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( caddress_test );

#if defined DEBUG
#undef DEBUG
#endif

void
caddress_test::setUp()
{
}



void
caddress_test::tearDown()
{
}



void
caddress_test::testGetAddrInfo()
{
	{
		std::string node("www.bisdn.de");
		std::string service("http");
		rofl::caddress addr1(node, service);
#ifdef DEBUG
		std::cerr << std::endl << addr1;
		std::cerr << std::endl << addr1.addr_c_str();
#endif

		rofl::caddress addr2(AF_INET, "212.91.241.169", 80);
		CPPUNIT_ASSERT(addr1 == addr2);
	}

	{
		std::string node("www.bisdn.de");
		std::string service("http");
		try {
			rofl::caddress addr(node, service, 0, AF_INET6);
#ifdef DEBUG
			std::cerr << std::endl << addr;
			std::cerr << std::endl << addr.addr_c_str();
#endif
			CPPUNIT_ASSERT(false);
		} catch (rofl::eSysCall& e) {
#ifdef DEBUG
			std::cerr << e;
#endif
		}
	}

	{
		std::string node("localhost");
		std::string service("https");
		rofl::caddress addr1(node, service, 0, AF_INET6);
#ifdef DEBUG
		std::cerr << std::endl << addr1;
		std::cerr << std::endl << addr1.addr_c_str();
#endif

		rofl::caddress addr2(AF_INET6, "::1", 443);
		CPPUNIT_ASSERT(addr1 == addr2);
	}
}



