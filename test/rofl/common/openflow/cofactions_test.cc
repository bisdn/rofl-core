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
	rofl::openflow::cofactions actions;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == actions.ofp_version);

	actions.ofp_version = rofl::openflow13::OFP_VERSION;
	actions.append_action_output(1, 1518);
	actions.append_action_set_field(rofl::openflow::coxmatch_ofb_eth_dst("a0:a1:a2:a3:a4:a5:a6"));
	actions.append_action_set_field(rofl::openflow::coxmatch_ofb_eth_src("b0:b1:b2:b3:b4:b5:b6"));
	actions.append_action_set_field(rofl::openflow::coxmatch_ofb_eth_type(0xc1c2));

	std::cerr << "actions:" << std::endl << actions;

	rofl::openflow::cofactions clone;

	clone = actions;

	std::cerr << "clone:" << std::endl << clone;

	rofl::openflow::cofactions clone2(rofl::openflow13::OFP_VERSION);

	for (std::list<rofl::openflow::cofaction*>::iterator
			it = actions.begin(); it != actions.end(); ++it) {
		std::cerr << "appending action:" << std::endl << *(*it);
		clone2.append_action(*(*it));
	}

	std::cerr << "clone2:" << std::endl << clone2;
}





