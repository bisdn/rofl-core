/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "clldpattr_test.h"

using namespace rofl::protocol::lldp;

CPPUNIT_TEST_SUITE_REGISTRATION( clldpattrTest );

#if defined DEBUG
//#undef DEBUG
#endif

void
clldpattrTest::setUp()
{
}



void
clldpattrTest::tearDown()
{
}



void
clldpattrTest::testTypeField()
{
	clldpattr attr;
	attr.set_type(0x7f);

	std::cerr << "attr:" << attr << std::endl;

	rofl::cmemory mem(attr.length());
	attr.pack(mem.somem(), mem.memlen());

	CPPUNIT_ASSERT(mem[0] == 0xfe);
	CPPUNIT_ASSERT(attr.get_type() == 0x7f);
	CPPUNIT_ASSERT(mem[1] == 0x00);
	CPPUNIT_ASSERT(attr.get_length() == 0x00);
}



void
clldpattrTest::testLengthField()
{
	clldpattr attr;
	attr.set_body() = rofl::cmemory(0x1ff);
	attr.set_length(0x1ff);

	std::cerr << "attr:" << attr << std::endl;

	rofl::cmemory mem(attr.length());
	attr.pack(mem.somem(), mem.memlen());

	std::cerr << "mem:" << mem << std::endl;

	CPPUNIT_ASSERT(mem[0] == 0x01);
	CPPUNIT_ASSERT(attr.get_type() == 0x00);
	CPPUNIT_ASSERT(mem[1] == 0xff);
	CPPUNIT_ASSERT(attr.get_length() == 0x1ff);
}



void
clldpattrTest::testDefaultConstructor()
{
	clldpattr attr;

	CPPUNIT_ASSERT(attr.length() == sizeof(struct lldp_tlv_hdr_t));
	CPPUNIT_ASSERT(attr.get_type() == 0);
	CPPUNIT_ASSERT(attr.get_length() == 0);
}



void
clldpattrTest::testCopyConstructor()
{
	clldpattr attr;

	attr.set_type(0x58);
	attr.set_body().resize(7);
	for (unsigned int i = 0; i < 7; i++) {
		attr.set_body()[i] = i;
	}
	rofl::cmemory body = attr.get_body();
	attr.pack();

	clldpattr clone(attr);

	std::cerr << "attr:" << attr;
	std::cerr << "clone:" << clone;

	CPPUNIT_ASSERT(clone.get_type() == 0x58);
	CPPUNIT_ASSERT(clone.get_length() == 7);
	CPPUNIT_ASSERT(clone.get_body() == body);
}



void
clldpattrTest::testPackUnpack()
{
	clldpattr attr;

	attr.set_type(0x58);
	attr.set_body().resize(7);
	for (unsigned int i = 0; i < 7; i++) {
		attr.set_body()[i] = i;
	}

	rofl::cmemory a_mem(attr.length());

	attr.pack(a_mem.somem(), a_mem.memlen());

	rofl::cmemory b_mem(9);
	b_mem[0] = 0xb0;
	b_mem[1] = 0x07;
	b_mem[2] = 0x00;
	b_mem[3] = 0x01;
	b_mem[4] = 0x02;
	b_mem[5] = 0x03;
	b_mem[6] = 0x04;
	b_mem[7] = 0x05;
	b_mem[8] = 0x06;

	std::cerr << "a_mem:" << a_mem;
	std::cerr << "b_mem:" << b_mem;

	CPPUNIT_ASSERT(a_mem == b_mem);

	clldpattr clone;
	clone.unpack(b_mem.somem(), b_mem.memlen());

	std::cerr << "a_attr:" << attr;
	std::cerr << "b_attr:" << clone;

	CPPUNIT_ASSERT(attr == clone);
}


void
clldpattrTest::testId()
{
	clldpattr_id attr(LLDPTT_CHASSIS_ID, 9);
	std::cerr << "attr:" << std::endl << attr;

	rofl::cmemory mem(9);

	mem[0] = (rofl::protocol::lldp::LLDPTT_CHASSIS_ID << 1);
	mem[1] = 0x07;
	mem[2] = 0x34;
	mem[3] = 0x65;
	mem[4] = 0x65;
	mem[5] = 0x65;
	mem[6] = 0x65;
	mem[7] = 0x65;
	mem[8] = 0x65;

	attr.unpack(mem.somem(), mem.memlen());
	std::cerr << "attr:" << std::endl << attr;

	CPPUNIT_ASSERT(attr.get_type() == rofl::protocol::lldp::LLDPTT_CHASSIS_ID);
	CPPUNIT_ASSERT(attr.get_length() == 7);
}



void
clldpattrTest::testTTL()
{
	clldpattr_ttl attr(sizeof(struct rofl::protocol::lldp::lldp_tlv_ttl_hdr_t));
	rofl::cmemory mem(4);
	mem[0] = (rofl::protocol::lldp::LLDPTT_TTL << 1);
	mem[1] = 0x04;
	mem[2] = 0x12;
	mem[3] = 0xff;

	attr.unpack(mem.somem(), mem.memlen());
	std::cerr << "attr:" << std::endl << attr;

	CPPUNIT_ASSERT(attr.get_type() == rofl::protocol::lldp::LLDPTT_TTL);
	CPPUNIT_ASSERT(attr.get_length() == sizeof(struct rofl::protocol::lldp::lldp_tlv_ttl_hdr_t));
	CPPUNIT_ASSERT(attr.length() == sizeof(struct rofl::protocol::lldp::lldp_tlv_ttl_hdr_t));
	CPPUNIT_ASSERT(attr.get_ttl() == 0x12ff);
}


void
clldpattrTest::testDesc()
{
	clldpattr_desc attr(LLDPTT_PORT_DESC, 6);
	rofl::cmemory mem(6);
	mem[0] = (rofl::protocol::lldp::LLDPTT_PORT_DESC << 1);
	mem[1] = 0x09;
	mem[2] = 0x61;
	mem[3] = 0x62;
	mem[4] = 0x63;
	mem[5] = 0x64;

	attr.unpack(mem.somem(), mem.memlen());

	std::cerr << "attr:" << std::endl << attr;

	std::string a_str("abcd");

	CPPUNIT_ASSERT(attr.get_desc() == a_str);
}


void
clldpattrTest::testSysCaps()
{
	clldpattr_system_caps attr(sizeof(struct rofl::protocol::lldp::lldp_tlv_sys_caps_hdr_t));
	rofl::cmemory mem(7);
	mem[0] = (rofl::protocol::lldp::LLDPTT_SYSTEM_CAPS << 1);
	mem[1] = sizeof(struct rofl::protocol::lldp::lldp_tlv_sys_caps_hdr_t);
	mem[2] = 0x08; // chassis-id
	mem[3] = 0x12;
	mem[4] = 0xff;
	mem[5] = 0x33;
	mem[6] = 0xff;

	attr.unpack(mem.somem(), mem.memlen());

	std::cerr << "attr:" << std::endl << attr;

	CPPUNIT_ASSERT(attr.get_chassis_id() == 0x08);
	CPPUNIT_ASSERT(attr.get_available_caps() == 0x12ff);
	CPPUNIT_ASSERT(attr.get_enabled_caps() == 0x33ff);
}




