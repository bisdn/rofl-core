#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "ficmpv6frametest.h"
#include <stdlib.h>

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( ficmpv6frameTest );


void
ficmpv6frameTest::setUp()
{
	mem = new cmemory(sizeof(struct ficmpv6frame::icmpv6_hdr_t) + 4 * sizeof(uint8_t));
	(*mem)[0] = 0x00; // Destination unreachable (= 0)
	(*mem)[1] = 0x04; // Port unreachachable (= 4)
	(*mem)[2] = 0x33; // checksum
	(*mem)[3] = 0x33; // checksum
	(*mem)[4] = 0x00; // mbz
	(*mem)[5] = 0x00; // mbz
	(*mem)[6] = 0x00; // mbz
	(*mem)[7] = 0x00;   // mbz

	icmpv6 = new ficmpv6frame(mem->somem(), mem->memlen());
}



void
ficmpv6frameTest::tearDown()
{
	delete icmpv6;
	delete mem;
}



void
ficmpv6frameTest::testICMPv6Type()
{
	uint8_t type = 146; // Mobile Prefix Solicitation
	icmpv6->set_icmpv6_type(type);

	//printf("icmpv6: %s\n", icmpv6->c_str());

	CPPUNIT_ASSERT((*mem)[0] == type);
	CPPUNIT_ASSERT(icmpv6->get_icmpv6_type() == type);
}



void
ficmpv6frameTest::testICMPv6Code()
{
	uint8_t code = 0xaa; // random value
	icmpv6->set_icmpv6_code(code);

	//printf("icmpv6: %s\n", icmpv6->c_str());

	CPPUNIT_ASSERT((*mem)[1] == code);
	CPPUNIT_ASSERT(icmpv6->get_icmpv6_code() == code);
}




