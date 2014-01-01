#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "cofhelloelemversionbitmaptest.h"
#include <stdlib.h>

CPPUNIT_TEST_SUITE_REGISTRATION( cofhelloelemversionbitmapTest );

void
cofhelloelemversionbitmapTest::setUp()
{
	elem = new rofl::openflow::cofhello_elem_versionbitmap();
	mem = new rofl::cmemory(16);
	struct rofl::openflow13::ofp_hello_elem_versionbitmap *hello =
			(struct rofl::openflow13::ofp_hello_elem_versionbitmap*)(mem->somem());
	hello->type = htobe16(rofl::openflow13::OFPHET_VERSIONBITMAP);
	hello->length = htobe16(12); // bitmap[0] and bitmap[1], following 4 bytes are padding
	uint32_t bitmap0 = 0;
	bitmap0 |= (1 << rofl::openflow10::OFP_VERSION);
	bitmap0 |= (1 << rofl::openflow12::OFP_VERSION);
	bitmap0 |= (1 << rofl::openflow13::OFP_VERSION);
	hello->bitmaps[0] = htobe32(bitmap0);
	uint32_t bitmap1 = 0;
	bitmap1 |= (1 << (40 - 32)); // fictious OFP version 40
	hello->bitmaps[1] = htobe32(bitmap1);
}



void
cofhelloelemversionbitmapTest::tearDown()
{
	delete mem;
	delete elem;
}



void
cofhelloelemversionbitmapTest::testPack()
{
	elem->add_ofp_version(rofl::openflow10::OFP_VERSION);
	elem->add_ofp_version(rofl::openflow12::OFP_VERSION);
	elem->add_ofp_version(rofl::openflow13::OFP_VERSION);
	elem->add_ofp_version(40);
	rofl::cmemory packed(elem->length());
	elem->pack(packed.somem(), packed.memlen());

	//std::cerr << "testPack: elem: " << *elem << std::endl;
	//std::cerr << "testPack: packed: " << packed << std::endl;
	//std::cerr << "testPack: mem: " << *mem << std::endl;

	CPPUNIT_ASSERT(*mem == packed);
}



void
cofhelloelemversionbitmapTest::testUnPack()
{
	//std::cerr << "testUnPack: mem: " << *mem << std::endl;
	elem->unpack(mem->somem(), mem->memlen());
	//std::cerr << "testUnPack: elem: " << *elem << std::endl;
	CPPUNIT_ASSERT(rofl::openflow13::OFPHET_VERSIONBITMAP == elem->get_type());
	CPPUNIT_ASSERT(12 == elem->get_length());
	CPPUNIT_ASSERT(16 == elem->length());
	CPPUNIT_ASSERT(elem->has_ofp_version(rofl::openflow10::OFP_VERSION));
	CPPUNIT_ASSERT(elem->has_ofp_version(rofl::openflow12::OFP_VERSION));
	CPPUNIT_ASSERT(elem->has_ofp_version(rofl::openflow13::OFP_VERSION));
	CPPUNIT_ASSERT(not elem->has_ofp_version(2));
	CPPUNIT_ASSERT(not elem->has_ofp_version(39));
	CPPUNIT_ASSERT(elem->has_ofp_version(40));
	CPPUNIT_ASSERT(not elem->has_ofp_version(41));
}


