#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgtablefeatures_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgtablefeaturesTest );

#if defined DEBUG
//#undef DEBUG
#endif

void
cofmsgtablefeaturesTest::setUp()
{
}



void
cofmsgtablefeaturesTest::tearDown()
{
}



void
cofmsgtablefeaturesTest::testDefaultConstructor()
{
	rofl::cofmsg_table_features_request request;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == request.get_version());
	rofl::cofmsg_table_features_reply reply;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == reply.get_version());
}



void
cofmsgtablefeaturesTest::testCopyConstructor()
{
	{
		rofl::cofmsg_table_features_request msg(rofl::openflow13::OFP_VERSION);
		msg.set_xid(0x01020304);
		msg.set_tables().add_table(1).set_properties().add_tfp_apply_actions().add_action(rofl::openflow13::OFPAT_SET_QUEUE);

		rofl::cmemory mmsg(msg.length());
		msg.pack(mmsg.somem(), mmsg.memlen());

#ifdef DEBUG
	std::cerr << "msg:" << std::endl << msg;
	std::cerr << "mmsg:" << std::endl << mmsg;
#endif

		rofl::cofmsg_table_features_request clone(rofl::openflow13::OFP_VERSION);
#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif

		clone.unpack(mmsg.somem(), mmsg.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif
	}

	std::cerr << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;

	{
		rofl::cofmsg_table_features_reply msg(rofl::openflow13::OFP_VERSION);
		msg.set_xid(0x01020304);
		msg.set_tables().add_table(1).set_properties().add_tfp_apply_actions().add_action(rofl::openflow13::OFPAT_SET_QUEUE);

		rofl::cmemory mmsg(msg.length());
		msg.pack(mmsg.somem(), mmsg.memlen());

#ifdef DEBUG
	std::cerr << "msg:" << std::endl << msg;
	std::cerr << "mmsg:" << std::endl << mmsg;
#endif

		rofl::cofmsg_table_features_reply clone(rofl::openflow13::OFP_VERSION);
#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif


		clone.unpack(mmsg.somem(), mmsg.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif
	}
}





