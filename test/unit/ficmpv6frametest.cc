/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "ficmpv6frametest.h"
#include <stdlib.h>

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( ficmpv6frameTest );


void
ficmpv6frameTest::setUp()
{
	mem = new cmemory(sizeof(struct ficmpv6frame::icmpv6_hdr_t) + 60 * sizeof(uint8_t));
	(*mem)[0] = 137; 	// redirect message (type = 137)
	(*mem)[1] = 0; 		// redirect message (code = 0)
	(*mem)[2] = 0x33; 	// checksum
	(*mem)[3] = 0x33; 	// checksum
	(*mem)[4] = 0x00; 	// reserved (mbz)
	(*mem)[5] = 0x00; 	// reserved (mbz)
	(*mem)[6] = 0x00; 	// reserved (mbz)
	(*mem)[7] = 0x00;   // reserved (mbz)
	for (int i = 8; i < 24; i++) {	// target address
		(*mem)[i] = 0xaa;
	}
	for (int i = 24; i < 40; i++) { 	// destination address
		(*mem)[i] = 0xbb;
	}

	/*
	 * target link layer address option (RFC 4861, Section 4.6.1⁾
	 */
	(*mem)[40] = 2;		// target LL address (type = 2)
	(*mem)[41] = 1;		// length in blocks of 8-octets (here = 1*8 = 8bytes)
	for (int i = 42; i < 48; i++) { // MAC address
		(*mem)[i] = 0xdd;
	}

	/*
	 * redirected header
	 */
	(*mem)[48] = 4;		// redirected header (type = 4)
	(*mem)[49] = 2;		// length in blocks of 8-octets (here = 2*8 = 16bytes)
	for (int i = 50; i < 64; i++) {
		(*mem)[i] = 0xee;
	}

	icmpv6 = new ficmpv6frame(mem->somem(), mem->memlen());


	/*
	 * too short frame
	 */

	mem_t = new cmemory(sizeof(struct ficmpv6frame::icmpv6_hdr_t) + 50 * sizeof(uint8_t));
	(*mem_t)[0] = 137; 	// redirect message (type = 137)
	(*mem_t)[1] = 0; 		// redirect message (code = 0)
	(*mem_t)[2] = 0x33; 	// checksum
	(*mem_t)[3] = 0x33; 	// checksum
	(*mem_t)[4] = 0x00; 	// reserved (mbz)
	(*mem_t)[5] = 0x00; 	// reserved (mbz)
	(*mem_t)[6] = 0x00; 	// reserved (mbz)
	(*mem_t)[7] = 0x00;   // reserved (mbz)
	for (int i = 8; i < 24; i++) {	// target address
		(*mem_t)[i] = 0xaa;
	}
	for (int i = 24; i < 40; i++) { 	// destination address
		(*mem_t)[i] = 0xbb;
	}

	/*
	 * target link layer address option (RFC 4861, Section 4.6.1⁾
	 */
	(*mem_t)[40] = 2;		// target LL address (type = 2)
	(*mem_t)[41] = 1;		// length in blocks of 8-octets (here = 1*8 = 8bytes)
	for (int i = 42; i < 48; i++) { // MAC address
		(*mem_t)[i] = 0xdd;
	}

	/*
	 * redirected header (this is shortened by 10 bytes)
	 */
	(*mem_t)[48] = 4;		// redirected header (type = 4)
	(*mem_t)[49] = 2;		// length in blocks of 8-octets (here = 2*8 = 16bytes)
	for (int i = 50; i < 54; i++) {
		(*mem_t)[i] = 0xee;
	}

	icmpv6_too_short = new ficmpv6frame(mem_t->somem(), mem_t->memlen());

	/*
	 * option with lenght field == 0
	 */
	mem_n = new cmemory(*mem); // make a copy

	(*mem_n)[41] = 0; // set length field in target link layer address option to 0

	icmpv6_option_len_null = new ficmpv6frame(mem_n->somem(), mem_n->memlen());
}



void
ficmpv6frameTest::tearDown()
{
	delete icmpv6_option_len_null;
	delete mem_n;
	delete icmpv6_too_short;
	delete mem_t;
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



void
ficmpv6frameTest::testICMPv6ShortFrame()
{
	//printf("icmpv6_too_short: %s\n", icmpv6_too_short->c_str());

	CPPUNIT_ASSERT(icmpv6_too_short->icmpv6opts.size() == 1);
}



void
ficmpv6frameTest::testICMPv6OptionWithLengthNull()
{
	//printf("icmpv6_option_len_null: %s\n", icmpv6_option_len_null->c_str());

	//printf("MAC address: %s\n", icmpv6->get_option(ficmpv6opt::ICMPV6_OPT_LLADDR_TARGET).get_ll_taddr().c_str());

	CPPUNIT_ASSERT(icmpv6_option_len_null->icmpv6opts.size() == 0);
}


