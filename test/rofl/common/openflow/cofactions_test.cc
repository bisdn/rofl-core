#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofactions_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofactions_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofactions_test::setUp()
{
}



void
cofactions_test::tearDown()
{
}



void
cofactions_test::testActions()
{
	unsigned int index = 0;
	rofl::openflow::cofactions actions;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == actions.get_version());

	actions.set_version(rofl::openflow13::OFP_VERSION);
	actions.add_action_output(index).set_port_no(1);
	actions.set_action_output(index++).set_max_len(1518);
	actions.add_action_set_field(index++).set_oxm(rofl::openflow::coxmatch_ofb_eth_dst("a0:a1:a2:a3:a4:a5:a6"));
	actions.add_action_set_field(index++).set_oxm(rofl::openflow::coxmatch_ofb_eth_src("b0:b1:b2:b3:b4:b5:b6"));
	actions.add_action_set_field(index++).set_oxm(rofl::openflow::coxmatch_ofb_eth_type(0xc1c2));

	std::cerr << "actions:" << std::endl << actions;

	rofl::cmemory packed(actions.length());
	actions.pack(packed.somem(), packed.memlen());

	std::cerr << "packed:" << std::endl << packed;

	rofl::openflow::cofactions clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.memlen());

	std::cerr << "clone:" << std::endl << clone;
}





