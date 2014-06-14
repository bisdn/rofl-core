#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "caddress_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( caddress_test );

#if defined DEBUG
//#undef DEBUG
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
caddress_test::testAddressLL()
{
	rofl::caddress_ll addr;
	CPPUNIT_ASSERT(addr.is_null());

	addr = rofl::caddress_ll("a0:a1:a2:a3:a4:a5");

	rofl::cmemory mem(6);
	for (unsigned int i = 0; i < 6; i++) {
		mem[i] = 0xa0 + i;
	}

	//std::cerr << "addr: " << std::endl << addr;
	//std::cerr << "mem: " << std::endl << mem;

	CPPUNIT_ASSERT(mem == addr);

	rofl::caddress_ll mask("ff:f0:0f:00:ff:00");
	rofl::caddress_ll masked = addr & mask;

	//std::cerr << "mask: " << std::endl << mask;
	//std::cerr << "masked: " << std::endl << masked;

	rofl::caddress_ll result("a0:a0:02:00:a4:00");

	CPPUNIT_ASSERT(masked == result);
	CPPUNIT_ASSERT(masked < addr);
	CPPUNIT_ASSERT(addr > masked);
}



void
caddress_test::testAddressIn4()
{
	rofl::caddress_in4 addr;

	addr = rofl::caddress_in4("161.162.163.164");

	rofl::cmemory mem(4);
	for (unsigned int i = 0; i < 4; i++) {
		mem[i] = 0xa1 + i;
	}

	//std::cerr << "addr: " << std::endl << addr;
	//std::cerr << "mem: " << std::endl << mem;

	CPPUNIT_ASSERT(mem == addr);

	rofl::caddress_in4 mask("255.255.240.0");
	rofl::caddress_in4 masked = addr & mask;

	//std::cerr << "mask: " << std::endl << mask;
	//std::cerr << "masked: " << std::endl << masked;

	rofl::caddress_in4 result("161.162.160.0");

	CPPUNIT_ASSERT(masked == result);
	CPPUNIT_ASSERT(masked < addr);
	CPPUNIT_ASSERT(addr > masked);
}



void
caddress_test::testAddressIn6()
{
	rofl::caddress_in6 addr;

	addr = rofl::caddress_in6("a0a1:a2a3:a4a5:a6a7:a8a9:aaab:acad:aeaf");

	rofl::cmemory mem(16);
	for (unsigned int i = 0; i < 16; i++) {
		mem[i] = 0xa0 + i;
	}

	//std::cerr << "addr: " << std::endl << addr;
	//std::cerr << "mem: " << std::endl << mem;

	CPPUNIT_ASSERT(mem == addr);

	rofl::caddress_in6 mask("ffff:ff00:f00f:00ff:0ff0:000f:00f0:0f00");
	rofl::caddress_in6 masked = addr & mask;

	//std::cerr << "mask: " << std::endl << mask;
	//std::cerr << "masked: " << std::endl << masked;

	rofl::caddress_in6 result("a0a1:a200:a005:00a7:08a0:000b:00a0:0e00");

	CPPUNIT_ASSERT(masked == result);
	CPPUNIT_ASSERT(masked < addr);
	CPPUNIT_ASSERT(addr > masked);
}


