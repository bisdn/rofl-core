#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "fipv6frametest.h"
#include <stdlib.h>

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( fipv6frameTest );


void
fipv6frameTest::setUp()
{
	mem = new cmemory(sizeof(struct fipv6frame::ipv6_hdr_t));
	(*(mem))[0] = 0x6a; // version = 6, tc[higher nibble]=5
	(*(mem))[1] = 0xa3; // tc[lower nibble]=4, flow-label=3
	(*(mem))[2] = 0x33; // flowlabel
	(*(mem))[3] = 0x33; // flowlabel
	(*(mem))[4] = 0x00; // payload length
	(*(mem))[5] = 0x00; // payload length
	(*(mem))[6] = 0x41; // next header = IPv6
	(*(mem))[7] = 0x40; // hop limit = 64
	for (int i = 8; i < 24; i++) {
		(*(mem))[i] = 0x11; // source address
	}
	for (int i = 24; i < 40; i++) {
		(*(mem))[i] = 0x22; // destination address
	}

	ipv6 = new fipv6frame(mem->somem(), mem->memlen());

	ipv6_src = new caddress(AF_INET6, "eeee:eeee:eeee:eeee:eeee:eeee:eeee:eeee");
	ipv6_dst = new caddress(AF_INET6, "dddd:dddd:dddd:dddd:dddd:dddd:dddd:dddd");
}



void
fipv6frameTest::tearDown()
{
	delete ipv6_dst;
	delete ipv6_src;
	delete ipv6;
	delete mem;
}



void
fipv6frameTest::testVersion()
{
	uint8_t version = 0xa;
	ipv6->set_version(version); // version number = 10

	//printf("ipv6: %s\n", ipv6->c_str());

	CPPUNIT_ASSERT((((*(mem))[0] & 0b11110000) >> 4) == version);
	CPPUNIT_ASSERT(ipv6->get_version() == version);
}



void
fipv6frameTest::testTrafficClass()
{
	uint8_t tc = 0x77;
	ipv6->set_traffic_class(tc); // traffic class = 0x77

	//printf("ipv6: %s\n", ipv6->c_str());

	CPPUNIT_ASSERT(((((*mem)[0] & 0b00001111) << 4) + (((*mem)[1] & 0b11110000) >> 4)) == tc);
	CPPUNIT_ASSERT(ipv6->get_traffic_class() == tc);
}



void
fipv6frameTest::testFlowLabel()
{
	uint32_t flabel = 0x66778899;
	ipv6->set_flow_label(flabel); // flabel = 0x00078899 (only 20bits are used here)

	//printf("ipv6: %s\n", ipv6->c_str());

	uint32_t mlabel = (((*mem)[1] & 0b00001111) << 16) + ((*mem)[2] << 8) + ((*mem)[3] << 0);

	CPPUNIT_ASSERT(mlabel == (flabel & 0x000fffff));
	CPPUNIT_ASSERT(ipv6->get_flow_label() == (flabel & 0x000fffff));
}



void
fipv6frameTest::testPayloadLength()
{
	uint16_t plen = 0xbbbb;
	ipv6->set_payload_length(plen); // payload length = 0xbbbb

	//printf("ipv6: %s\n", ipv6->c_str());

	uint16_t mlen = ((*mem)[4] << 8) + ((*mem)[5] << 0);

	CPPUNIT_ASSERT(mlen == plen);
	CPPUNIT_ASSERT(ipv6->get_payload_length() == plen);
}


void
fipv6frameTest::testNextHeader()
{
	uint8_t nxthdr = 41;
	ipv6->set_next_header(nxthdr); // next header = 41 (IPv6)

	//printf("ipv6: %s\n", ipv6->c_str());

	uint16_t mnxthdr = (*mem)[6];

	CPPUNIT_ASSERT(mnxthdr == nxthdr);
	CPPUNIT_ASSERT(ipv6->get_next_header() == nxthdr);
}


void
fipv6frameTest::testHopLimit()
{
	uint8_t hoplimit = 4;
	ipv6->set_hop_limit(hoplimit); // hop limit = 4

	//printf("ipv6: %s\n", ipv6->c_str());

	uint16_t mhoplimit = (*mem)[7];

	CPPUNIT_ASSERT(mhoplimit == hoplimit);
	CPPUNIT_ASSERT(ipv6->get_hop_limit() == hoplimit);
}



void
fipv6frameTest::testSrcAddress()
{
	ipv6->set_ipv6_src(*ipv6_src);

	printf("ipv6: %s\n", ipv6->c_str());

	CPPUNIT_ASSERT(not memcmp(ipv6->ipv6_hdr->src, ipv6_src->ca_s6addr->sin6_addr.s6_addr, 16));
	//CPPUNIT_ASSERT(ipv6->get_ipv6_src() == *ipv6_src);
}



void
fipv6frameTest::testDstAddress()
{
	ipv6->set_ipv6_dst(*ipv6_dst);

	printf("ipv6: %s\n", ipv6->c_str());

	CPPUNIT_ASSERT(not memcmp(ipv6->ipv6_hdr->dst, ipv6_dst->ca_s6addr->sin6_addr.s6_addr, 16));
	//CPPUNIT_ASSERT(ipv6->get_ipv6_dst() == *ipv6_dst);
}



