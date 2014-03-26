#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coxmatches_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( coxmatches_test );

#if defined DEBUG
#undef DEBUG
#endif

void
coxmatches_test::setUp()
{
}



void
coxmatches_test::tearDown()
{
}



void
coxmatches_test::testPack()
{
	rofl::openflow::coxmatches matches;

	matches.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST).set_u48value(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
	matches.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_SRC_MASK).set_u48value(rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));
	matches.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_SRC_MASK).set_u48mask(rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));
	matches.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK).set_u16value(0x3132);
	matches.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK).set_u16mask(0x4142);
	matches.set_match(rofl::openflow::OXM_TLV_BASIC_IP_DSCP).set_u8value(0xa1);
	matches.set_match(rofl::openflow::OXM_TLV_BASIC_UDP_SRC).set_u16value(0x1112);
	// MPLS

	rofl::cmemory mem(matches.length());
	matches.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(45);
	// ETH_DST
	test[0] = 0x80;
	test[1] = 0x00;
	test[2] = 0x06; // (ETH_DST == 3) << 1, has-mask == 0
	test[3] = 0x06;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	// ETH_SRC_MASK
	test[10] = 0x80;
	test[11] = 0x00;
	test[12] = 0x09; // (ETH_SRC == 4) << 1, has-mask == 1
	test[13] = 0x0c;
	test[14] = 0xc1;
	test[15] = 0xc2;
	test[16] = 0xc3;
	test[17] = 0xc4;
	test[18] = 0xc5;
	test[19] = 0xc6;
	test[20] = 0xd1;
	test[21] = 0xd2;
	test[22] = 0xd3;
	test[23] = 0xd4;
	test[24] = 0xd5;
	test[25] = 0xd6;
	// VLAN_VID
	test[26] = 0x80;
	test[27] = 0x00;
	test[28] = 0x0d; // (VLAN_VID == 6) << 1, has-mask == 1
	test[29] = 0x04;
	test[30] = 0x31;
	test[31] = 0x32;
	test[32] = 0x41;
	test[33] = 0x42;
	// IP_DSCP
	test[34] = 0x80;
	test[35] = 0x00;
	test[36] = 0x10; // (IP_DSCP == 8) << 1, has-mask == 0
	test[37] = 0x01;
	test[38] = 0xa1;
	// UDP_SRC
	test[39] = 0x80;
	test[40] = 0x00;
	test[41] = 0x1e; // (UDP_SRC == 15) << 1, has-mask == 0
	test[42] = 0x02;
	test[43] = 0x11;
	test[44] = 0x12;

	//std::cerr << "mem:" << std::endl << mem;
	//std::cerr << "test:" << std::endl << test;

	CPPUNIT_ASSERT(matches.get_matches().size() == 5);
	CPPUNIT_ASSERT(matches.length() == 45);
	CPPUNIT_ASSERT(mem == test);
}



void
coxmatches_test::testUnPack()
{
	rofl::openflow::coxmatches matches;

	rofl::cmemory test(45);
	// ETH_DST
	test[0] = 0x80;
	test[1] = 0x00;
	test[2] = 0x06; // (ETH_DST == 3) << 1, has-mask == 0
	test[3] = 0x06;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	// ETH_SRC_MASK
	test[10] = 0x80;
	test[11] = 0x00;
	test[12] = 0x09; // (ETH_SRC == 4) << 1, has-mask == 1
	test[13] = 0x0c;
	test[14] = 0xc1;
	test[15] = 0xc2;
	test[16] = 0xc3;
	test[17] = 0xc4;
	test[18] = 0xc5;
	test[19] = 0xc6;
	test[20] = 0xd1;
	test[21] = 0xd2;
	test[22] = 0xd3;
	test[23] = 0xd4;
	test[24] = 0xd5;
	test[25] = 0xd6;
	// VLAN_VID
	test[26] = 0x80;
	test[27] = 0x00;
	test[28] = 0x0d; // (VLAN_VID == 6) << 1, has-mask == 1
	test[29] = 0x04;
	test[30] = 0x31;
	test[31] = 0x32;
	test[32] = 0x41;
	test[33] = 0x42;
	// IP_DSCP
	test[34] = 0x80;
	test[35] = 0x00;
	test[36] = 0x10; // (IP_DSCP == 8) << 1, has-mask == 0
	test[37] = 0x01;
	test[38] = 0xa1;
	// UDP_SRC
	test[39] = 0x80;
	test[40] = 0x00;
	test[41] = 0x1e; // (UDP_SRC == 15) << 1, has-mask == 0
	test[42] = 0x02;
	test[43] = 0x11;
	test[44] = 0x12;

	matches.unpack(test.somem(), test.memlen());

	//std::cerr << "test:" << std::endl << test;
	//std::cerr << "matches:" << std::endl << matches;

	CPPUNIT_ASSERT(matches.get_matches().size() == 5);
	CPPUNIT_ASSERT(matches.length() == 45);
}



void
coxmatches_test::testAddMatch()
{
	rofl::openflow::coxmatches matches;

	matches.add_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST).set_u48value(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));

	try {
		CPPUNIT_ASSERT(matches.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST) == rofl::openflow::coxmatch_ofb_eth_dst(rofl::cmacaddr("b1:b2:b3:b4:b5:b6")));
	} catch (rofl::openflow::eOxmNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	CPPUNIT_ASSERT(matches.get_matches().size() == 1);
	CPPUNIT_ASSERT(matches.add_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST) != rofl::openflow::coxmatch_ofb_eth_dst(rofl::cmacaddr("b1:b2:b3:b4:b5:b6")));
	CPPUNIT_ASSERT(matches.add_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST) == rofl::openflow::coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_DST));
	CPPUNIT_ASSERT(matches.get_matches().find(rofl::openflow::OXM_TLV_BASIC_ETH_DST & 0xfffffe00) != matches.get_matches().end());

	matches.add_match(rofl::openflow::coxmatch_ofb_eth_dst(rofl::cmacaddr("c1:c2:c3:c4:c5:c6"), rofl::cmacaddr("d1:d2:d3:d4:d5:d6")));

	CPPUNIT_ASSERT(matches.get_matches().size() == 1);
	CPPUNIT_ASSERT(matches.get_matches().find(rofl::openflow::OXM_TLV_BASIC_ETH_DST & 0xfffffe00) != matches.get_matches().end());
}



void
coxmatches_test::testDropMatch()
{
	rofl::openflow::coxmatches matches;

	matches.add_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST).set_u48value(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));

	CPPUNIT_ASSERT(matches.get_matches().size() == 1);
	CPPUNIT_ASSERT(matches.get_matches().find(rofl::openflow::OXM_TLV_BASIC_ETH_DST & 0xfffffe00) != matches.get_matches().end());

	matches.drop_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST);

	CPPUNIT_ASSERT(matches.get_matches().size() == 0);

	matches.add_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST).set_u48value(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));

	CPPUNIT_ASSERT(matches.get_matches().size() == 1);
	CPPUNIT_ASSERT(matches.get_matches().find(rofl::openflow::OXM_TLV_BASIC_ETH_DST & 0xfffffe00) != matches.get_matches().end());

	matches.drop_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK);

	CPPUNIT_ASSERT(matches.get_matches().size() == 0);
}



void
coxmatches_test::testSetMatch()
{
	rofl::openflow::coxmatches matches;

	matches.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48value(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
	matches.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48mask(rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));

	rofl::cmemory test(16);
	test[0] = 0x80;
	test[1] = 0x00;
	test[2] = 0x07;
	test[3] = 0x0c;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	test[10] = 0xc1;
	test[11] = 0xc2;
	test[12] = 0xc3;
	test[13] = 0xc4;
	test[14] = 0xc5;
	test[15] = 0xc6;

	CPPUNIT_ASSERT(matches.length() == 16);

	rofl::cmemory mem(matches.length());
	matches.pack(mem.somem(), mem.memlen());

	CPPUNIT_ASSERT(test == mem);

	CPPUNIT_ASSERT(matches.get_matches().size() == 1);

	try {
		matches.get_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
	} catch (rofl::openflow::eOxmNotFound& e) {
		CPPUNIT_ASSERT(false);
	}
}



void
coxmatches_test::testGetMatch()
{
	rofl::openflow::coxmatches matches;

	try {
		matches.get_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOxmNotFound& e) {}

	try {
		matches.get_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOxmNotFound& e) {}

	matches.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST);

	try {
		matches.get_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
	} catch (rofl::openflow::eOxmNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		matches.get_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK);
	} catch (rofl::openflow::eOxmNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	CPPUNIT_ASSERT(matches.get_matches().size() == 1);

	matches.drop_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST);

	try {
		matches.get_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOxmNotFound& e) {}

	try {
		matches.get_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK);
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eOxmNotFound& e) {}

	CPPUNIT_ASSERT(matches.get_matches().size() == 0);
}



void
coxmatches_test::testHasMatch()
{
	rofl::openflow::coxmatches matches;


	if (matches.has_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) {
		CPPUNIT_ASSERT(false);
	}

	if (matches.has_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK)) {
		CPPUNIT_ASSERT(false);
	}

	matches.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST);

	if (not matches.has_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) {
		CPPUNIT_ASSERT(false);
	}

	if (not matches.has_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK)) {;
		CPPUNIT_ASSERT(false);
	}

	CPPUNIT_ASSERT(matches.get_matches().size() == 1);

	matches.drop_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST);


	if (matches.has_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) {
		CPPUNIT_ASSERT(false);
	}

	if (matches.has_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK)) {
		CPPUNIT_ASSERT(false);
	}

	CPPUNIT_ASSERT(matches.get_matches().size() == 0);
}



void
coxmatches_test::testStrictMatching()
{
	// TODO
	return;

	uint16_t exact_hits = 0, wildcard_hits = 0, missed = 0;

	rofl::openflow::coxmatches left;

	left.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48value(rofl::cmacaddr("a1:a2:a3:a4:a5:a6"));
	left.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48mask(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
	left.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_SRC).set_u48value(rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));
	left.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_VID).set_u16value(0x3132);
	left.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP).set_u8value(0x3);
	left.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE).set_u16value(0x0800);
	left.set_match(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK).set_u32value(rofl::caddress(AF_INET, "10.1.1.0"));
	left.set_match(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK).set_u32mask(rofl::caddress(AF_INET, "255.255.255.0"));
	left.set_match(rofl::openflow::OXM_TLV_BASIC_IP_PROTO).set_u8value(17);
	left.set_match(rofl::openflow::OXM_TLV_BASIC_UDP_DST).set_u16value(80);

	rofl::openflow::coxmatches right;

	// right is all wildcard, left does not contain entire namespace
	CPPUNIT_ASSERT(not left.contains(right, true));
	// right is all wildcard, left is part of it, but strict checking means => right does not contain left
	CPPUNIT_ASSERT(not right.contains(left, true));

	right.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48value(rofl::cmacaddr("a1:a2:a3:a4:a5:a6"));
	right.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48mask(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
	//std::cerr << "left:" << std::endl << left;
	//std::cerr << "right:" << std::endl << right;
	CPPUNIT_ASSERT(not left.contains(right, true));
	CPPUNIT_ASSERT(left.is_part_of(right, exact_hits, wildcard_hits, missed));
	//std::cerr << "exact: " << exact_hits << " wildcard: " << wildcard_hits << " missed: " << missed << std::endl;
	CPPUNIT_ASSERT(not right.contains(left, true));
	CPPUNIT_ASSERT(not right.is_part_of(left, exact_hits, wildcard_hits, missed));
	//std::cerr << "exact: " << exact_hits << " wildcard: " << wildcard_hits << " missed: " << missed << std::endl;



	right.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_SRC).set_u48value(rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));
	CPPUNIT_ASSERT(not left.contains(right, true));
	CPPUNIT_ASSERT(right.contains(left, true));

	right.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_VID).set_u16value(0x3132);
	CPPUNIT_ASSERT(not left.contains(right, true));
	CPPUNIT_ASSERT(right.contains(left, true));

	right.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP).set_u8value(0x3);
	CPPUNIT_ASSERT(not left.contains(right, true));
	CPPUNIT_ASSERT(right.contains(left, true));

	right.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE).set_u16value(0x0800);
	CPPUNIT_ASSERT(not left.contains(right, true));
	CPPUNIT_ASSERT(right.contains(left, true));

	right.set_match(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK).set_u32value(rofl::caddress(AF_INET, "10.1.1.0"));
	right.set_match(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK).set_u32mask(rofl::caddress(AF_INET, "255.255.255.0"));
	CPPUNIT_ASSERT(not left.contains(right, true));
	CPPUNIT_ASSERT(right.contains(left, true));

	right.set_match(rofl::openflow::OXM_TLV_BASIC_IP_PROTO).set_u8value(17);
	CPPUNIT_ASSERT(not left.contains(right, true));
	CPPUNIT_ASSERT(right.contains(left, true));

	right.set_match(rofl::openflow::OXM_TLV_BASIC_UDP_DST).set_u16value(80);
	CPPUNIT_ASSERT(not left.contains(right, true));
	CPPUNIT_ASSERT(right.contains(left, true));


}



void
coxmatches_test::testNonStrictMatching()
{
	// TODO
}




