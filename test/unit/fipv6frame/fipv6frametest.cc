#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "fipv6frametest.h"
#include <stdlib.h>

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( fipv6frameTest );


void
fipv6frameTest::setUp()
{
	mem_A = new cmemory(sizeof(struct fipv6frame::ipv6_hdr_t));
	*(mem_A)[0] = 0x65; // version = 6, tc[higher nibble]=5
	*(mem_A)[1] = 0x43; // tc[lower nibble]=4, flow-label=3
	*(mem_A)[2] = 0x33; // flowlabel
	*(mem_A)[3] = 0x33; // flowlabel
	*(mem_A)[4] = 0x00; // payload length
	*(mem_A)[5] = 0x00; // payload length
	*(mem_A)[6] = 0x41; // next header = IPv6
	*(mem_A)[7] = 0x40; // hop limit = 64
	for (int i = 8; i < 24; i++) {
		*(mem_A)[i] = 0x11; // source address
	}
	for (int i = 24; i < 40; i++) {
		*(mem_A)[i] = 0x22; // destination address
	}

	ipv6_A = new fipv6frame(mem_A->somem(), mem_A->memlen());
}



void
fipv6frameTest::tearDown()
{
	delete ipv6_A;
	delete mem_A;
}



void
fipv6frametest::testVersion()
{
	uint8_t version = 0xa;
	ipv6_A->set_version(version); // version number = 10

	CPPUNIT_ASSERT(((*(mem_A)[0] & 0b11110000) >> 4) == version);
	CPPUNIT_ASSERT(ipv6_A->get_version() == version);
#if 0
	version = 6;
	ipv6_A->set_version(version); // version number = 6
#endif
}



void
fipv6frametest::testTrafficClass()
{

}



