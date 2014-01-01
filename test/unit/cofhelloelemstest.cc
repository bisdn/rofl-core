#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "cofhelloelemstest.h"
#include <stdlib.h>

CPPUNIT_TEST_SUITE_REGISTRATION( cofhelloelemsTest );

void
cofhelloelemsTest::setUp()
{
	elems = new rofl::openflow::cofhelloelems();
	mem = new rofl::cmemory(4*sizeof(uint64_t));
	struct rofl::openflow13::ofp_hello_elem_header *hello;

	hello = (struct rofl::openflow13::ofp_hello_elem_header*)(mem->somem() +  0);
	hello->type = htobe16(1);
	hello->length = htobe16(12);
	memset(mem->somem() + 4, 0xff, 8);

	hello = (struct rofl::openflow13::ofp_hello_elem_header*)(mem->somem() + 16);
	hello->type = htobe16(2);
	hello->length = htobe16(8);
	memset(mem->somem() + 20, 0xaa, 4);

	hello = (struct rofl::openflow13::ofp_hello_elem_header*)(mem->somem() + 24);
	hello->type = htobe16(3);
	hello->length = htobe16(8);
	memset(mem->somem() + 28, 0xbb, 4);
}



void
cofhelloelemsTest::tearDown()
{
	delete mem;
	delete elems;
}



void
cofhelloelemsTest::testPack()
{

	//CPPUNIT_ASSERT(*rule_nonstrict == oxl);
}



void
cofhelloelemsTest::testUnPack()
{
	std::cerr << *mem << std::endl;
	elems->unpack(mem->somem(), mem->memlen());
	std::cerr << *elems << std::endl;
	//CPPUNIT_ASSERT(*rule_nonstrict == oxl);
}


