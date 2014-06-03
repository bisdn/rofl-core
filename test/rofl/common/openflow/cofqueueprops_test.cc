#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofqueueprops_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofqueueprops_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofqueueprops_test::setUp()
{
}



void
cofqueueprops_test::tearDown()
{
}



void
cofqueueprops_test::testQueueProps()
{
	uint16_t min_rate = 0xa1a2;
	uint16_t max_rate = 0xb1b2;
	uint32_t exp_id = 0xc1c2c3c4;
	rofl::cmemory exp_body(12);
	for (unsigned int i = 0; i < exp_body.memlen(); i++) {
		exp_body[i] = i;
	}

	rofl::openflow::cofqueue_props props;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == props.get_version());

	props.set_version(rofl::openflow13::OFP_VERSION);

	props.add_queue_prop_min_rate().set_min_rate(min_rate);
	props.add_queue_prop_max_rate().set_max_rate(max_rate);
	props.add_queue_prop_experimenter().set_exp_id(exp_id);
	props.add_queue_prop_experimenter().set_exp_body() = exp_body;

	rofl::cmemory packed(props.length());
	props.pack(packed.somem(), packed.memlen());

	//std::cerr << "props:" << std::endl << props;
	//std::cerr << "packed:" << std::endl << packed;

	rofl::openflow::cofqueue_props clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	//std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_properties().size() == 3);
	CPPUNIT_ASSERT(clone.length() == props.length());
	CPPUNIT_ASSERT(clone.get_queue_prop_min_rate().get_min_rate() == min_rate);
	CPPUNIT_ASSERT(clone.get_queue_prop_max_rate().get_max_rate() == max_rate);
}






