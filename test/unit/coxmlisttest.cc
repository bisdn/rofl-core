#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "coxmlisttest.h"
#include <stdlib.h>

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( coxmlistTest );


void
coxmlistTest::setUp()
{
	packet 			= new coxmlist();
	rule_nonstrict	= new coxmlist();
	rule_strict 	= new coxmlist();
	rule_mismatch 	= new coxmlist();

	packet->insert(coxmatch_ofb_eth_dst(cmacaddr("00:11:11:11:11:11")));
	packet->insert(coxmatch_ofb_eth_src(cmacaddr("00:22:22:22:22:22")));
	packet->insert(coxmatch_ofb_eth_type(0x0800));
	packet->insert(coxmatch_ofb_ipv4_src(caddress(AF_INET, "1.1.1.1")));
	packet->insert(coxmatch_ofb_ipv4_dst(caddress(AF_INET, "2.2.2.2")));
	packet->insert(coxmatch_ofb_ip_proto(17/*UDP*/));
	packet->insert(coxmatch_ofb_udp_src(1111));
	packet->insert(coxmatch_ofb_udp_dst(2222));

	//std::cerr << "packet=" << std::endl << *packet << std::endl;

	rule_nonstrict->insert(coxmatch_ofb_eth_dst(cmacaddr("00:11:11:11:11:11")));
	rule_nonstrict->insert(coxmatch_ofb_ipv4_dst(caddress(AF_INET, "2.2.2.2")));
	rule_nonstrict->insert(coxmatch_ofb_eth_type(0x0800));

	//std::cerr << "rule_nonstrict=" << std::endl << *rule_nonstrict << std::endl;

	rule_strict->insert(coxmatch_ofb_eth_dst(cmacaddr("00:11:11:11:11:11")));
	rule_strict->insert(coxmatch_ofb_eth_src(cmacaddr("00:22:22:22:22:22")));
	rule_strict->insert(coxmatch_ofb_eth_type(0x0800));
	rule_strict->insert(coxmatch_ofb_ipv4_src(caddress(AF_INET, "1.1.1.1")));
	rule_strict->insert(coxmatch_ofb_ipv4_dst(caddress(AF_INET, "2.2.2.2")));
	rule_strict->insert(coxmatch_ofb_ip_proto(17/*UDP*/));
	rule_strict->insert(coxmatch_ofb_udp_src(1111));
	rule_strict->insert(coxmatch_ofb_udp_dst(2222));

	//std::cerr << "rule_strict=" << std::endl << *rule_strict << std::endl;

	rule_mismatch->insert(coxmatch_ofb_eth_dst(cmacaddr("00:11:22:33:44:55")));

	//std::cerr << "rule_mismatch=" << std::endl << *rule_mismatch << std::endl;
}



void
coxmlistTest::tearDown()
{
	delete rule_mismatch;
	delete rule_strict;
	delete rule_nonstrict;
	delete packet;
}



void
coxmlistTest::testIsPartOf()
{
	//fprintf(stderr, "testIsPartOf: packet.is_part_of(rule_nonstrict)\n");
	exact_hits = 0;
	wildcard_hits = 0;
	missed = 0;
	CPPUNIT_ASSERT(packet->is_part_of(*rule_nonstrict, exact_hits, wildcard_hits, missed));

	//fprintf(stderr, "testIsPartOf: packet.is_part_of(rule_strict)\n");
	exact_hits = 0;
	wildcard_hits = 0;
	missed = 0;
	CPPUNIT_ASSERT(packet->is_part_of(*rule_strict, exact_hits, wildcard_hits, missed));

	//fprintf(stderr, "testIsPartOf: packet.is_part_of(rule_mismatch)\n");
	exact_hits = 0;
	wildcard_hits = 0;
	missed = 0;
	CPPUNIT_ASSERT(not packet->is_part_of(*rule_mismatch, exact_hits, wildcard_hits, missed));
}


void
coxmlistTest::testContains()
{
	//fprintf(stderr, "testContains: rule_nonstrict.contains(packet, false)\n");
	CPPUNIT_ASSERT(rule_nonstrict->contains(*packet, false));

	//fprintf(stderr, "testContains: rule_nonstrict.contains(packet, true)\n");
	CPPUNIT_ASSERT(not rule_nonstrict->contains(*packet, true));

	//fprintf(stderr, "testContains: rule_strict.contains(packet, false)\n");
	CPPUNIT_ASSERT(rule_strict->contains(*packet, false));

	//fprintf(stderr, "testContains: rule_strict.contains(packet, true)\n");
	CPPUNIT_ASSERT(rule_strict->contains(*packet, true));

	//fprintf(stderr, "testContains: rule_mismatch.contains(packet, false)\n");
	CPPUNIT_ASSERT(not rule_mismatch->contains(*packet, false));

	//fprintf(stderr, "testContains: rule_mismatch.contains(packet, true)\n");
	CPPUNIT_ASSERT(not rule_mismatch->contains(*packet, true));
}



