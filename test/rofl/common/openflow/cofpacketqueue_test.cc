#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofpacketqueue_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofpacketqueue_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofpacketqueue_test::setUp()
{
}



void
cofpacketqueue_test::tearDown()
{
}



void
cofpacketqueue_test::testPacketQueue()
{
	uint32_t port_no = 0xa1a2a3a4;
	uint32_t queue_id = 0xb1b2b3b4;
	uint16_t min_rate = 0xc1c2;
	uint16_t max_rate = 0xd1d2;

	rofl::openflow::cofpacket_queue queue;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == queue.get_version());

	queue.set_version(rofl::openflow13::OFP_VERSION);
	queue.set_port_no(port_no);
	queue.set_queue_id(queue_id);
	queue.set_queue_props().set_queue_prop_min_rate().set_min_rate(min_rate);
	queue.set_queue_props().set_queue_prop_max_rate().set_max_rate(max_rate);

	rofl::cmemory packed(queue.length());
	queue.pack(packed.somem(), packed.memlen());

	//std::cerr << "queue:" << std::endl << queue;
	//std::cerr << "packed:" << std::endl << packed;

	rofl::openflow::cofpacket_queue clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	//std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_port_no() == port_no);
	CPPUNIT_ASSERT(clone.get_queue_id() == queue_id);
	CPPUNIT_ASSERT(clone.get_queue_props().get_queue_prop_min_rate().get_min_rate() == queue.get_queue_props().get_queue_prop_min_rate().get_min_rate());
	CPPUNIT_ASSERT(clone.get_queue_props().get_queue_prop_max_rate().get_max_rate() == queue.get_queue_props().get_queue_prop_max_rate().get_max_rate());
}





