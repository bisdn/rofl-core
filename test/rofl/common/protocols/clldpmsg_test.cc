/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "clldpmsg_test.h"

using namespace rofl::protocol::lldp;

CPPUNIT_TEST_SUITE_REGISTRATION( clldpmsgTest );

#if defined DEBUG
//#undef DEBUG
#endif

void
clldpmsgTest::setUp()
{
}



void
clldpmsgTest::tearDown()
{
}



void
clldpmsgTest::testLLDPMsg()
{
	clldpmsg msg;

	msg.set_eth_src() = rofl::cmacaddr("00:01:02:03:04:05");
	msg.set_eth_dst() = rofl::cmacaddr("01:80:c2:00:00:00");
	rofl::cmemory chassis_id(4);
	for (unsigned int i = 0; i < 4; i++) {
		chassis_id[i] = i;
	}
	msg.set_attrs().add_chassis_id().set_body() = chassis_id;

	rofl::cmemory port_id(4);
	for (unsigned int i = 0; i < 4; i++) {
		port_id[i] = i;
	}
	msg.set_attrs().add_port_id().set_body() = port_id;

	msg.set_attrs().add_ttl().set_ttl() = 0x1234;

	msg.set_attrs().add_end();

	rofl::cmemory mem(msg.length());
	msg.pack(mem.somem(), mem.memlen());

	std::cerr << "msg:" << std::endl << msg;
	std::cerr << "mem:" << std::endl << mem;

	CPPUNIT_ASSERT(mem.memlen() == 34);
}


