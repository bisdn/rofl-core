#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofflowmod_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofflowmod_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofflowmod_test::setUp()
{
}



void
cofflowmod_test::tearDown()
{
}



void
cofflowmod_test::testFlowMod10()
{
	rofl::cindex index(0);

	uint16_t port_no = 0xb6b7;
	uint16_t max_len = 0xf5f6;
	rofl::cmacaddr eth_dst("a1:a2:a3:a4:a5:a6");
	rofl::cmacaddr eth_src("b1:b2:b3:b4:b5:b6");
	uint16_t eth_type = 0xc1c2;

	rofl::openflow::cofflowmod flowmod;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == flowmod.get_version());

	flowmod.set_version(rofl::openflow10::OFP_VERSION);

	uint8_t	command 		= rofl::openflow::OFPFC_ADD;
	uint16_t idle_timeout 	= 0xb1b2;
	uint16_t hard_timeout 	= 0xc1c2;
	uint64_t cookie 		= 0xd1d2d3d4d5d6d7d8;
	uint16_t priority 		= 0xf1f2;
	uint32_t buffer_id 		= 0x11121314;
	uint16_t out_port 		= 0x2324;
	uint16_t flags 			= 0x4142;

	flowmod.set_command(command);
	flowmod.set_idle_timeout(idle_timeout);
	flowmod.set_hard_timeout(hard_timeout);
	flowmod.set_cookie(cookie);
	flowmod.set_priority(priority);
	flowmod.set_buffer_id(buffer_id);
	flowmod.set_out_port(out_port);
	flowmod.set_flags(flags);

	flowmod.set_match().set_eth_dst(eth_dst);
	flowmod.set_match().set_eth_src(eth_src);
	flowmod.set_match().set_eth_type(eth_type);

	flowmod.set_actions().add_action_output(index).set_port_no(port_no);
	flowmod.set_actions().set_action_output(index).set_max_len(max_len);

	//std::cerr << "[10] flowmod:" << std::endl << flowmod;

	rofl::cmemory packed(flowmod.length());

	flowmod.pack(packed.somem(), packed.memlen());

	//std::cerr << "[10] packed:" << std::endl << packed;

	rofl::openflow::cofflowmod clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.memlen());

	//std::cerr << "[10] clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_command() 		== command);
	CPPUNIT_ASSERT(clone.get_idle_timeout() == idle_timeout);
	CPPUNIT_ASSERT(clone.get_hard_timeout() == hard_timeout);
	CPPUNIT_ASSERT(clone.get_cookie() 		== cookie);
	CPPUNIT_ASSERT(clone.get_priority() 	== priority);
	CPPUNIT_ASSERT(clone.get_buffer_id() 	== buffer_id);
	CPPUNIT_ASSERT(clone.get_out_port() 	== out_port);
	CPPUNIT_ASSERT(clone.get_flags()		== flags);
	CPPUNIT_ASSERT(clone.get_match().get_eth_dst() == eth_dst);
	CPPUNIT_ASSERT(clone.get_match().get_eth_src() == eth_src);
	CPPUNIT_ASSERT(clone.get_match().get_eth_type() == eth_type);
}



void
cofflowmod_test::testFlowMod13()
{
	rofl::cindex index(0);

	uint32_t port_no = 0xb6b7b8b9;
	uint16_t max_len = 0xf5f6;
	rofl::cmacaddr eth_dst("a1:a2:a3:a4:a5:a6");
	rofl::cmacaddr eth_src("b1:b2:b3:b4:b5:b6");
	uint16_t eth_type = 0xc1c2;

	rofl::openflow::cofflowmod flowmod;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == flowmod.get_version());

	flowmod.set_version(rofl::openflow13::OFP_VERSION);

	uint8_t	command 		= rofl::openflow::OFPFC_ADD;
	uint8_t	table_id 		= 0xa1;
	uint16_t idle_timeout 	= 0xb1b2;
	uint16_t hard_timeout 	= 0xc1c2;
	uint64_t cookie 		= 0xd1d2d3d4d5d6d7d8;
	uint64_t cookie_mask 	= 0xe1e2e3e4e5e6e7e8;
	uint16_t priority 		= 0xf1f2;
	uint32_t buffer_id 		= 0x11121314;
	uint32_t out_port 		= 0x21222324;
	uint32_t out_group 		= 0x31323334;
	uint16_t flags 			= 0x4142;

	flowmod.set_command(command);
	flowmod.set_table_id(table_id);
	flowmod.set_idle_timeout(idle_timeout);
	flowmod.set_hard_timeout(hard_timeout);
	flowmod.set_cookie(cookie);
	flowmod.set_cookie_mask(cookie_mask);
	flowmod.set_priority(priority);
	flowmod.set_buffer_id(buffer_id);
	flowmod.set_out_port(out_port);
	flowmod.set_out_group(out_group);
	flowmod.set_flags(flags);

	flowmod.set_match().set_eth_dst(eth_dst);
	flowmod.set_match().set_eth_src(eth_src);
	flowmod.set_match().set_eth_type(eth_type);

	flowmod.set_instructions().set_inst_goto_table().set_table_id(table_id+1);
	flowmod.set_instructions().set_inst_apply_actions().set_actions().add_action_output(index).set_port_no(port_no);
	flowmod.set_instructions().set_inst_apply_actions().set_actions().add_action_output(index).set_max_len(max_len);

	//std::cerr << "[13] flowmod:" << std::endl << flowmod;

	rofl::cmemory packed(flowmod.length());

	flowmod.pack(packed.somem(), packed.memlen());

	//std::cerr << "[13] packed:" << std::endl << packed;

	rofl::openflow::cofflowmod clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.memlen());

	//std::cerr << "[13] clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_command() 		== command);
	CPPUNIT_ASSERT(clone.get_table_id() 	== table_id);
	CPPUNIT_ASSERT(clone.get_idle_timeout() == idle_timeout);
	CPPUNIT_ASSERT(clone.get_hard_timeout() == hard_timeout);
	CPPUNIT_ASSERT(clone.get_cookie() 		== cookie);
	CPPUNIT_ASSERT(clone.get_cookie_mask() 	== cookie_mask);
	CPPUNIT_ASSERT(clone.get_priority() 	== priority);
	CPPUNIT_ASSERT(clone.get_buffer_id() 	== buffer_id);
	CPPUNIT_ASSERT(clone.get_out_port() 	== out_port);
	CPPUNIT_ASSERT(clone.get_out_group() 	== out_group);
	CPPUNIT_ASSERT(clone.get_flags()		== flags);
	CPPUNIT_ASSERT(clone.get_match().get_eth_dst() == eth_dst);
	CPPUNIT_ASSERT(clone.get_match().get_eth_src() == eth_src);
	CPPUNIT_ASSERT(clone.get_match().get_eth_type() == eth_type);
	CPPUNIT_ASSERT(clone.get_instructions().get_inst_goto_table().get_table_id() == (table_id + 1));
}






