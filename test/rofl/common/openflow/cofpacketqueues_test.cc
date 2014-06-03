#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofpacketqueues_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofpacketqueues_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofpacketqueues_test::setUp()
{
}



void
cofpacketqueues_test::tearDown()
{
}



void
cofpacketqueues_test::testPacketQueues()
{
	uint32_t port_no = 0xa1a2a300;
	uint32_t queue_id = 0xb1b2b300;
	uint16_t min_rate = 0xc1c2;
	uint16_t max_rate = 0xd1d2;

	rofl::openflow::cofpacket_queues queues;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == queues.get_version());

	queues.set_version(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < 2; i++) {
		for (unsigned int j = 0; j < 2; j++) {
			queues.set_pqueue(port_no + i, queue_id + j).set_queue_props().set_queue_prop_min_rate().set_min_rate(min_rate);
			queues.set_pqueue(port_no + i, queue_id + j).set_queue_props().set_queue_prop_max_rate().set_max_rate(max_rate);
		}
	}

	rofl::cmemory packed(queues.length());
	queues.pack(packed.somem(), packed.memlen());

	//std::cerr << "queues:" << std::endl << queues;
	//std::cerr << "packed:" << std::endl << packed;

	rofl::openflow::cofpacket_queues clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	//std::cerr << "clone:" << std::endl << clone;

	for (unsigned int i = 0; i < 2; i++) {
		for (unsigned int j = 0; j < 2; j++) {
			CPPUNIT_ASSERT(clone.get_pqueue(port_no + i, queue_id + j).get_port_no() == port_no + i);
			CPPUNIT_ASSERT(clone.get_pqueue(port_no + i, queue_id + j).get_queue_id() == queue_id + j);
			CPPUNIT_ASSERT(clone.get_pqueue(port_no + i, queue_id + j).get_queue_props().get_queue_prop_min_rate().get_min_rate() == queues.get_pqueue(port_no + i, queue_id + j).get_queue_props().get_queue_prop_min_rate().get_min_rate());
			CPPUNIT_ASSERT(clone.get_pqueue(port_no + i, queue_id + j).get_queue_props().get_queue_prop_max_rate().get_max_rate() == queues.get_pqueue(port_no + i, queue_id + j).get_queue_props().get_queue_prop_max_rate().get_max_rate());
		}
	}
}





