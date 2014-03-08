#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "clldpattrs_test.h"

using namespace rofl::protocol::lldp;

CPPUNIT_TEST_SUITE_REGISTRATION( clldpattrsTest );

#if defined DEBUG
//#undef DEBUG
#endif

void
clldpattrsTest::setUp()
{
}



void
clldpattrsTest::tearDown()
{
}



void
clldpattrsTest::testLLDPAttrs()
{
	clldpattrs attrs;

	attrs.add_chassis_id().set_sub_type() = LLDPCHIDST_CHASSIS;
	rofl::cmemory chas_id(4);
	chas_id[0] = 0x01;
	chas_id[1] = 0x02;
	chas_id[2] = 0x03;
	chas_id[3] = 0x04;
	attrs.set_chassis_id().set_body() = chas_id;

	attrs.add_port_id().set_sub_type() = LLDPPRTIDST_PORT;
	rofl::cmemory port_id(4);
	port_id[0] = 0x01;
	port_id[1] = 0x02;
	port_id[2] = 0x03;
	port_id[3] = 0x04;
	attrs.set_port_id().set_body() = port_id;

	attrs.add_ttl().set_ttl() = 0x5678;

	attrs.add_end();

	rofl::cmemory a_mem(attrs.length());
	attrs.pack(a_mem.somem(), a_mem.memlen());

	std::cerr << "attrs:" << attrs << std::endl;
	std::cerr << "GGGGGGGGGGGGGGGGGGGGGGGGG" << std::endl;
	std::cerr << "a_mem:" << a_mem << std::endl;
	std::cerr << "GGGGGGGGGGGGGGGGGGGGGGGGG" << std::endl;

	CPPUNIT_ASSERT(attrs.length() == 20);

	// 02 05 01 01 02 03 04 04   05 02 01 02 03 04 06 02   56 78 00 00

	rofl::cmemory mem(20);
	mem[0] = 0x02;
	mem[1] = 0x05;
	mem[2] = 0x01;
	mem[3] = 0x01;
	mem[4] = 0x02;
	mem[5] = 0x03;
	mem[6] = 0x04;
	mem[7] = 0x04;
	mem[8] = 0x05;
	mem[9] = 0x02;
	mem[10] = 0x01;
	mem[11] = 0x02;
	mem[12] = 0x03;
	mem[13] = 0x04;
	mem[14] = 0x06;
	mem[15] = 0x02;
	mem[16] = 0x56;
	mem[17] = 0x78;
	mem[18] = 0x00;
	mem[19] = 0x00;

	CPPUNIT_ASSERT(a_mem == mem);

	clldpattrs clone;

	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << "clone:" << clone;
}


