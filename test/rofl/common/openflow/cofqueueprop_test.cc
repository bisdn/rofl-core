#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofqueueprop_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofqueueprop_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofqueueprop_test::setUp()
{
}



void
cofqueueprop_test::tearDown()
{
}



void
cofqueueprop_test::testQueueProp()
{
	rofl::openflow::cofqueue_prop prop;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == prop.get_version());
}



void
cofqueueprop_test::testQueuePropMinRate()
{
	uint16_t min_rate = 0xa1a2;

	rofl::openflow::cofqueue_prop_min_rate prop;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == prop.get_version());

	prop.set_version(rofl::openflow13::OFP_VERSION);
	prop.set_min_rate(min_rate);

	rofl::cmemory packed(prop.length());
	prop.pack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(prop.get_version() == rofl::openflow13::OFP_VERSION);
	CPPUNIT_ASSERT(prop.get_property() == rofl::openflow13::OFPQT_MIN_RATE);
	CPPUNIT_ASSERT(prop.get_min_rate() == min_rate);
	CPPUNIT_ASSERT(prop.get_length() == sizeof(struct rofl::openflow13::ofp_queue_prop_min_rate));

	//std::cerr << "prop:" << std::endl << prop;
	//std::cerr << "packed:" << std::endl << packed;

	rofl::openflow::cofqueue_prop_min_rate clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	//std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_version() == rofl::openflow13::OFP_VERSION);
	CPPUNIT_ASSERT(clone.get_property() == rofl::openflow13::OFPQT_MIN_RATE);
	CPPUNIT_ASSERT(clone.get_min_rate() == min_rate);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_queue_prop_min_rate));
}



void
cofqueueprop_test::testQueuePropMaxRate()
{
	uint16_t max_rate = 0xa1a2;

	rofl::openflow::cofqueue_prop_max_rate prop;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == prop.get_version());

	prop.set_version(rofl::openflow13::OFP_VERSION);
	prop.set_max_rate(max_rate);

	rofl::cmemory packed(prop.length());
	prop.pack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(prop.get_version() == rofl::openflow13::OFP_VERSION);
	CPPUNIT_ASSERT(prop.get_property() == rofl::openflow13::OFPQT_MAX_RATE);
	CPPUNIT_ASSERT(prop.get_max_rate() == max_rate);
	CPPUNIT_ASSERT(prop.get_length() == sizeof(struct rofl::openflow13::ofp_queue_prop_max_rate));

	//std::cerr << "prop:" << std::endl << prop;
	//std::cerr << "packed:" << std::endl << packed;

	rofl::openflow::cofqueue_prop_max_rate clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	//std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_version() == rofl::openflow13::OFP_VERSION);
	CPPUNIT_ASSERT(clone.get_property() == rofl::openflow13::OFPQT_MAX_RATE);
	CPPUNIT_ASSERT(clone.get_max_rate() == max_rate);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_queue_prop_max_rate));
}



void
cofqueueprop_test::testQueuePropExperimenter()
{
	uint32_t exp_id = 0xa1a2a3a4;
	rofl::cmemory exp_body(12);
	for (unsigned int i = 0; i < exp_body.memlen(); i++) {
		exp_body[i] = i;
	}


	rofl::openflow::cofqueue_prop_experimenter prop;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == prop.get_version());

	prop.set_version(rofl::openflow13::OFP_VERSION);
	prop.set_exp_id(exp_id);
	prop.set_exp_body() = exp_body;

	rofl::cmemory packed(prop.length());
	prop.pack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(prop.get_version() == rofl::openflow13::OFP_VERSION);
	CPPUNIT_ASSERT(prop.get_property() == rofl::openflow13::OFPQT_EXPERIMENTER);
	CPPUNIT_ASSERT(prop.get_exp_body() == exp_body);
	CPPUNIT_ASSERT(prop.get_length() == sizeof(struct rofl::openflow13::ofp_queue_prop_experimenter) + exp_body.memlen());

	//std::cerr << "prop:" << std::endl << prop;
	//std::cerr << "packed:" << std::endl << packed;

	rofl::openflow::cofqueue_prop_experimenter clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	//std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_version() == rofl::openflow13::OFP_VERSION);
	CPPUNIT_ASSERT(clone.get_property() == rofl::openflow13::OFPQT_EXPERIMENTER);
	CPPUNIT_ASSERT(clone.get_exp_body() == exp_body);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_queue_prop_experimenter) + exp_body.memlen());
}



