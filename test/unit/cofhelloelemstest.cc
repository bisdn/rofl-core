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
	rofl::openflow::cofhello_elem e1(16);
	e1.set_type(1);
	e1.set_length(12);
	for (unsigned int i = 0; i < 8; i++) {
		e1[i+4] = 0xff;
	}
	rofl::openflow::cofhello_elem e2( 8);
	e2.set_type(2);
	e2.set_length(8);
	for (unsigned int i = 0; i < 4; i++) {
		e2[i+4] = 0xaa;
	}
	rofl::openflow::cofhello_elem e3( 8);
	e3.set_type(3);
	e3.set_length(8);
	for (unsigned int i = 0; i < 4; i++) {
		e3[i+4] = 0xbb;
	}

	(*elems)[1] = new rofl::openflow::cofhello_elem(e1);
	(*elems)[2] = new rofl::openflow::cofhello_elem(e2);
	(*elems)[3] = new rofl::openflow::cofhello_elem(e3);

	//std::cerr << "testPack: elems: " << *elems << std::endl;

	CPPUNIT_ASSERT(elems->length() == 32);

	rofl::cmemory packed(elems->length());

	elems->pack(packed.somem(), packed.memlen());

	//std::cerr << "testPack: packed: " << packed << std::endl;

	CPPUNIT_ASSERT(packed == *mem);
}



void
cofhelloelemsTest::testUnPack()
{
	//std::cerr << "testUnPack: mem: "<< *mem << std::endl;
	elems->unpack(mem->somem(), mem->memlen());
	//std::cerr << "testUnPack: elems: "<< *elems << std::endl;
	CPPUNIT_ASSERT(3 == elems->size());
	CPPUNIT_ASSERT((*elems)[1]->get_type() == 1);
	CPPUNIT_ASSERT((*elems)[2]->get_type() == 2);
	CPPUNIT_ASSERT((*elems)[3]->get_type() == 3);
	CPPUNIT_ASSERT((*elems)[1]->get_length() == 12);
	CPPUNIT_ASSERT((*elems)[2]->get_length() ==  8);
	CPPUNIT_ASSERT((*elems)[3]->get_length() ==  8);
	CPPUNIT_ASSERT(elems->length() == 32);
}


