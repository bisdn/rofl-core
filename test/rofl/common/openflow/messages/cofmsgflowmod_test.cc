#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgflowmod_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgflowmodTest );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmsgflowmodTest::setUp()
{
}



void
cofmsgflowmodTest::tearDown()
{
}



void
cofmsgflowmodTest::testMsgFlowMod()
{
	rofl::openflow::cofmsg_flow_mod msg;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == msg.get_version());

	msg.set_version(rofl::openflow13::OFP_VERSION);

	std::cerr << "msg:" << std::endl << msg;

	rofl::cmemory packed(msg.length());

	msg.pack(packed.somem(), packed.memlen());

	std::cerr << "packed:" << std::endl << packed;

	rofl::openflow::cofmsg_flow_mod clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.memlen());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow::OFPT_FLOW_MOD);
	CPPUNIT_ASSERT(clone.get_flowmod().get_instructions().get_instructions().size() == 0);
	CPPUNIT_ASSERT(clone.get_flowmod().length() == 48);
}






