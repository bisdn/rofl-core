#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofaction_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofaction_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofaction_test::setUp()
{
}



void
cofaction_test::tearDown()
{
}



void
cofaction_test::testAction()
{
	rofl::openflow::cofaction action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_type(0xa1a2);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	rofl::openflow::cofaction clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() == action.get_type());
	CPPUNIT_ASSERT(clone.get_length() == action.get_length());
	CPPUNIT_ASSERT(clone.length() == sizeof(struct rofl::openflow::ofp_action));
}



void
cofaction_test::testActionOutput()
{
	uint32_t port_no = 0xa1a2a3a4;
	uint16_t max_len = 0xb1b2;

	rofl::openflow::cofaction_output action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_port_no(port_no);
	action.set_max_len(max_len);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() == rofl::openflow::OFPAT_OUTPUT);
	CPPUNIT_ASSERT(action.get_length() == sizeof(struct rofl::openflow13::ofp_action_output));
	CPPUNIT_ASSERT(action.length() == sizeof(struct rofl::openflow13::ofp_action_output));
	CPPUNIT_ASSERT(action.get_port_no() == port_no);
	CPPUNIT_ASSERT(action.get_max_len() == max_len);

	rofl::openflow::cofaction_output clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow::OFPAT_OUTPUT);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_action_output));
	CPPUNIT_ASSERT(clone.length() == sizeof(struct rofl::openflow13::ofp_action_output));
	CPPUNIT_ASSERT(clone.get_port_no() == port_no);
	CPPUNIT_ASSERT(clone.get_max_len() == max_len);
}



void
cofaction_test::testActionOutput10()
{
	uint16_t port_no = 0xa1a2;
	uint16_t max_len = 0xb1b2;

	rofl::openflow::cofaction_output action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_port_no(port_no);
	action.set_max_len(max_len);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() == rofl::openflow::OFPAT_OUTPUT);
	CPPUNIT_ASSERT(action.get_length() == sizeof(struct rofl::openflow10::ofp_action_output));
	CPPUNIT_ASSERT(action.length() == sizeof(struct rofl::openflow10::ofp_action_output));
	CPPUNIT_ASSERT(action.get_port_no() == port_no);
	CPPUNIT_ASSERT(action.get_max_len() == max_len);

	rofl::openflow::cofaction_output clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow::OFPAT_OUTPUT);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow10::ofp_action_output));
	CPPUNIT_ASSERT(clone.length() == sizeof(struct rofl::openflow10::ofp_action_output));
	CPPUNIT_ASSERT(clone.get_port_no() == port_no);
	CPPUNIT_ASSERT(clone.get_max_len() == max_len);
}



void
cofaction_test::testActionSetVlanVid()
{
	uint16_t vid = 0xa1a2;

	rofl::openflow::cofaction_set_vlan_vid action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_vlan_vid(vid);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_VLAN_VID);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_vlan_vid));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_vlan_vid));
	CPPUNIT_ASSERT(action.get_vlan_vid() == vid);

	rofl::openflow::cofaction_set_vlan_vid clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_VLAN_VID);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_vlan_vid));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_vlan_vid));
	CPPUNIT_ASSERT(clone.get_vlan_vid() == vid);

	rofl::openflow::cofaction_set_vlan_vid clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}




void
cofaction_test::testActionSetVlanPcp()
{
	uint8_t pcp = 0xa1;

	rofl::openflow::cofaction_set_vlan_pcp action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_vlan_pcp(pcp);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_VLAN_PCP);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_vlan_vid));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_vlan_vid));
	CPPUNIT_ASSERT(action.get_vlan_pcp() == pcp);

	rofl::openflow::cofaction_set_vlan_pcp clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_VLAN_PCP);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_vlan_vid));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_vlan_vid));
	CPPUNIT_ASSERT(clone.get_vlan_pcp() == pcp);

	rofl::openflow::cofaction_set_vlan_pcp clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}




void
cofaction_test::testActionStripVlan()
{
	rofl::openflow::cofaction_strip_vlan action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_STRIP_VLAN);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_header));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_header));

	rofl::openflow::cofaction_strip_vlan clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_STRIP_VLAN);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_header));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_header));

	rofl::openflow::cofaction_strip_vlan clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}




void
cofaction_test::testActionSetDlSrc()
{
	rofl::cmacaddr addr("a1:a2:a3:a4:a5:a6");

	rofl::openflow::cofaction_set_dl_src action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_dl_src(addr);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_DL_SRC);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_dl_addr));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_dl_addr));
	CPPUNIT_ASSERT(action.get_dl_src() 	== addr);

	rofl::openflow::cofaction_set_dl_src clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_DL_SRC);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_dl_addr));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_dl_addr));
	CPPUNIT_ASSERT(clone.get_dl_src()	== addr);

	rofl::openflow::cofaction_set_dl_src clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}




void
cofaction_test::testActionSetDlDst()
{
	rofl::cmacaddr addr("a1:a2:a3:a4:a5:a6");

	rofl::openflow::cofaction_set_dl_dst action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_dl_dst(addr);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_DL_DST);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_dl_addr));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_dl_addr));
	CPPUNIT_ASSERT(action.get_dl_dst() 	== addr);

	rofl::openflow::cofaction_set_dl_dst clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_DL_DST);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_dl_addr));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_dl_addr));
	CPPUNIT_ASSERT(clone.get_dl_dst()	== addr);

	rofl::openflow::cofaction_set_dl_dst clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}




void
cofaction_test::testActionSetNwSrc()
{
	rofl::caddress_in4 addr("161.162.163.164");

	rofl::openflow::cofaction_set_nw_src action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_nw_src(addr);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_NW_SRC);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(action.get_nw_src() 	== addr);

	rofl::openflow::cofaction_set_nw_src clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_NW_SRC);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(clone.get_nw_src()	== addr);

	rofl::openflow::cofaction_set_nw_src clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}



void
cofaction_test::testActionSetNwDst()
{
	rofl::caddress_in4 addr("161.162.163.164");

	rofl::openflow::cofaction_set_nw_dst action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_nw_dst(addr);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_NW_DST);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(action.get_nw_dst() 	== addr);

	rofl::openflow::cofaction_set_nw_dst clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_NW_DST);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(clone.get_nw_dst()	== addr);

	rofl::openflow::cofaction_set_nw_dst clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}



void
cofaction_test::testActionSetNwTos()
{
	uint8_t nw_tos = 0xa1;

	rofl::openflow::cofaction_set_nw_tos action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_nw_tos(nw_tos);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_NW_TOS);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(action.get_nw_tos() 	== nw_tos);

	rofl::openflow::cofaction_set_nw_tos clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_NW_TOS);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_nw_addr));
	CPPUNIT_ASSERT(clone.get_nw_tos()	== nw_tos);

	rofl::openflow::cofaction_set_nw_tos clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}



void
cofaction_test::testActionSetTpSrc()
{
	uint16_t tp_src = 0xa1a2;

	rofl::openflow::cofaction_set_tp_src action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_tp_src(tp_src);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_TP_SRC);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_tp_port));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_tp_port));
	CPPUNIT_ASSERT(action.get_tp_src() 	== tp_src);

	rofl::openflow::cofaction_set_tp_src clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_TP_SRC);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_tp_port));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_tp_port));
	CPPUNIT_ASSERT(clone.get_tp_src()	== tp_src);

	rofl::openflow::cofaction_set_tp_src clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}



void
cofaction_test::testActionSetTpDst()
{
	uint16_t tp_dst = 0xa1a2;

	rofl::openflow::cofaction_set_tp_dst action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_tp_dst(tp_dst);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_TP_DST);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_tp_port));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_tp_port));
	CPPUNIT_ASSERT(action.get_tp_dst() 	== tp_dst);

	rofl::openflow::cofaction_set_tp_dst clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_TP_DST);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_tp_port));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_tp_port));
	CPPUNIT_ASSERT(clone.get_tp_dst()	== tp_dst);

	rofl::openflow::cofaction_set_tp_dst clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}



void
cofaction_test::testActionEnqueue()
{
	uint16_t port_no = 0xa1a2;
	uint32_t queue_id = 0xb1b2b3b4;

	rofl::openflow::cofaction_enqueue action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_port_no(port_no);
	action.set_queue_id(queue_id);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow10::OFPAT_ENQUEUE);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_enqueue));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_enqueue));
	CPPUNIT_ASSERT(action.get_port_no() == port_no);
	CPPUNIT_ASSERT(action.get_queue_id()== queue_id);

	rofl::openflow::cofaction_enqueue clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow10::OFPAT_ENQUEUE);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_enqueue));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_enqueue));
	CPPUNIT_ASSERT(clone.get_port_no() 	== port_no);
	CPPUNIT_ASSERT(clone.get_queue_id()	== queue_id);

	rofl::openflow::cofaction_enqueue clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}



void
cofaction_test::testActionVendor()
{
	uint32_t exp_id = 0xa1a2a3a4;
	unsigned int exp_body_len = 12;
	rofl::cmemory exp_body(exp_body_len);
	for (unsigned int i = 0; i < exp_body_len; i++) {
		exp_body[i] = 0xb0 + i;
	}

	size_t total_length = exp_body_len;
	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}

	rofl::openflow::cofaction_vendor action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow10::OFP_VERSION);
	action.set_exp_id(exp_id);
	action.set_exp_body() = exp_body;

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow10::OFPAT_VENDOR);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_vendor_header) + total_length);
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow10::ofp_action_vendor_header) + total_length);
	CPPUNIT_ASSERT(action.get_exp_id() 	== exp_id);
	CPPUNIT_ASSERT(action.get_exp_body()== exp_body);

	rofl::openflow::cofaction_vendor clone(rofl::openflow10::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow10::OFPAT_VENDOR);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow10::ofp_action_vendor_header) + total_length);
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow10::ofp_action_vendor_header) + total_length);
	CPPUNIT_ASSERT(clone.get_exp_id() 	== exp_id);
	for (unsigned int i = 0; i < exp_body_len; i++) {
		clone.get_exp_body()[i] = 0xb0 + i;
	}

	rofl::openflow::cofaction_vendor clone2(rofl::openflow13::OFP_VERSION);
	try {
		clone2.unpack(packed.somem(), packed.length());
		CPPUNIT_ASSERT(false);
	} catch (rofl::eBadVersion& e) {}
}



void
cofaction_test::testActionCopyTtlOut()
{
	rofl::openflow::cofaction_copy_ttl_out action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() == rofl::openflow::OFPAT_COPY_TTL_OUT);
	CPPUNIT_ASSERT(action.get_length() == sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(action.length() == sizeof(struct rofl::openflow13::ofp_action_header));

	rofl::openflow::cofaction_copy_ttl_out clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow::OFPAT_COPY_TTL_OUT);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(clone.length() == sizeof(struct rofl::openflow13::ofp_action_header));
}




void
cofaction_test::testActionCopyTtlIn()
{
	rofl::openflow::cofaction_copy_ttl_in action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() == rofl::openflow::OFPAT_COPY_TTL_IN);
	CPPUNIT_ASSERT(action.get_length() == sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(action.length() == sizeof(struct rofl::openflow13::ofp_action_header));

	rofl::openflow::cofaction_copy_ttl_in clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow::OFPAT_COPY_TTL_IN);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(clone.length() == sizeof(struct rofl::openflow13::ofp_action_header));
}



void
cofaction_test::testActionSetMplsTtl()
{
	uint8_t mpls_ttl = 0xa1;

	rofl::openflow::cofaction_set_mpls_ttl action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_mpls_ttl(mpls_ttl);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() == rofl::openflow::OFPAT_SET_MPLS_TTL);
	CPPUNIT_ASSERT(action.get_length() == sizeof(struct rofl::openflow13::ofp_action_mpls_ttl));
	CPPUNIT_ASSERT(action.length() == sizeof(struct rofl::openflow13::ofp_action_mpls_ttl));
	CPPUNIT_ASSERT(action.get_mpls_ttl() == mpls_ttl);

	rofl::openflow::cofaction_set_mpls_ttl clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow::OFPAT_SET_MPLS_TTL);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_action_mpls_ttl));
	CPPUNIT_ASSERT(clone.length() == sizeof(struct rofl::openflow13::ofp_action_mpls_ttl));
	CPPUNIT_ASSERT(clone.get_mpls_ttl() == mpls_ttl);
}



void
cofaction_test::testActionDecMplsTtl()
{
	rofl::openflow::cofaction_dec_mpls_ttl action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() == rofl::openflow::OFPAT_DEC_MPLS_TTL);
	CPPUNIT_ASSERT(action.get_length() == sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(action.length() == sizeof(struct rofl::openflow13::ofp_action_header));

	rofl::openflow::cofaction_dec_mpls_ttl clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow::OFPAT_DEC_MPLS_TTL);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(clone.length() == sizeof(struct rofl::openflow13::ofp_action_header));
}



void
cofaction_test::testActionPushVlan()
{
	uint16_t eth_type = 0xa1a2;

	rofl::openflow::cofaction_push_vlan action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_eth_type(eth_type);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_PUSH_VLAN);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(action.get_eth_type()== eth_type);

	rofl::openflow::cofaction_push_vlan clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_PUSH_VLAN);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(clone.get_eth_type() == eth_type);
}



void
cofaction_test::testActionPopVlan()
{
	rofl::openflow::cofaction_pop_vlan action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_POP_VLAN);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_header));

	rofl::openflow::cofaction_pop_vlan clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_POP_VLAN);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_header));
}



void
cofaction_test::testActionPushMpls()
{
	uint16_t eth_type = 0xa1a2;

	rofl::openflow::cofaction_push_mpls action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_eth_type(eth_type);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_PUSH_MPLS);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(action.get_eth_type()== eth_type);

	rofl::openflow::cofaction_push_mpls clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_PUSH_MPLS);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(clone.get_eth_type() == eth_type);
}



void
cofaction_test::testActionPopMpls()
{
	uint16_t eth_type = 0xa1a2;

	rofl::openflow::cofaction_pop_mpls action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_eth_type(eth_type);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_POP_MPLS);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_pop_mpls));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_pop_mpls));
	CPPUNIT_ASSERT(action.get_eth_type()== eth_type);

	rofl::openflow::cofaction_pop_mpls clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_POP_MPLS);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_pop_mpls));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_pop_mpls));
	CPPUNIT_ASSERT(clone.get_eth_type() == eth_type);
}



void
cofaction_test::testActionGroup()
{
	uint32_t group_id = 0xa1a2a3a4;

	rofl::openflow::cofaction_group action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_group_id(group_id);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_GROUP);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_group));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_group));
	CPPUNIT_ASSERT(action.get_group_id()== group_id);

	rofl::openflow::cofaction_group clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_GROUP);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_group));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_group));
	CPPUNIT_ASSERT(clone.get_group_id()	== group_id);
}



void
cofaction_test::testActionSetNwTtl()
{
	uint8_t nw_ttl = 0xa1;

	rofl::openflow::cofaction_set_nw_ttl action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_nw_ttl(nw_ttl);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_NW_TTL);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_nw_ttl));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_nw_ttl));
	CPPUNIT_ASSERT(action.get_nw_ttl()	== nw_ttl);

	rofl::openflow::cofaction_set_nw_ttl clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_NW_TTL);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_nw_ttl));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_nw_ttl));
	CPPUNIT_ASSERT(clone.get_nw_ttl()	== nw_ttl);
}



void
cofaction_test::testActionDecNwTtl()
{
	rofl::openflow::cofaction_dec_nw_ttl action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_DEC_NW_TTL);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_header));

	rofl::openflow::cofaction_dec_nw_ttl clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_DEC_NW_TTL);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_header));
}



void
cofaction_test::testActionSetQueue()
{
	uint32_t queue_id = 0xa1a2a3a4;

	rofl::openflow::cofaction_set_queue action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_queue_id(queue_id);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_QUEUE);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_set_queue));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_set_queue));
	CPPUNIT_ASSERT(action.get_queue_id()== queue_id);

	rofl::openflow::cofaction_set_queue clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_QUEUE);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_set_queue));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_set_queue));
	CPPUNIT_ASSERT(clone.get_queue_id() == queue_id);
}



void
cofaction_test::testActionSetField()
{
	rofl::openflow::coxmatch_ofb_eth_src oxm(rofl::caddress_ll("a1:a2:a3:a4:a5:a6"));

	size_t total_length = sizeof(struct rofl::openflow13::ofp_action_set_field) - 4*sizeof(uint8_t) + oxm.length();
	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}

	rofl::openflow::cofaction_set_field action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_oxm(oxm);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_SET_FIELD);
	CPPUNIT_ASSERT(action.get_length() 	== total_length);
	CPPUNIT_ASSERT(action.length() 		== total_length);
	CPPUNIT_ASSERT(action.get_oxm()		== oxm);

	rofl::openflow::cofaction_set_field clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_SET_FIELD);
	CPPUNIT_ASSERT(clone.get_length() 	== total_length);
	CPPUNIT_ASSERT(clone.length() 		== total_length);
	CPPUNIT_ASSERT(clone.get_oxm() 		== oxm);
}



void
cofaction_test::testActionExperimenter()
{
	uint32_t exp_id = 0xa1a2a3a4;
	unsigned int exp_body_len = 12;
	rofl::cmemory exp_body(exp_body_len);
	for (unsigned int i = 0; i < exp_body_len; i++) {
		exp_body[i] = 0xb0 + i;
	}

	size_t total_length = sizeof(struct rofl::openflow13::ofp_action_experimenter_header) + exp_body_len;
	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}

	rofl::openflow::cofaction_experimenter action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_exp_id(exp_id);
	action.set_exp_body() = exp_body;

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow13::OFPAT_EXPERIMENTER);
	CPPUNIT_ASSERT(action.get_length() 	== total_length);
	CPPUNIT_ASSERT(action.length() 		== total_length);
	CPPUNIT_ASSERT(action.get_exp_id() 	== exp_id);
	CPPUNIT_ASSERT(action.get_exp_body()== exp_body);

	rofl::openflow::cofaction_experimenter clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow13::OFPAT_EXPERIMENTER);
	CPPUNIT_ASSERT(clone.get_length() 	== total_length);
	CPPUNIT_ASSERT(clone.length() 		== total_length);
	CPPUNIT_ASSERT(clone.get_exp_id() 	== exp_id);
	for (unsigned int i = 0; i < exp_body_len; i++) {
		clone.get_exp_body()[i] = 0xb0 + i;
	}
}



void
cofaction_test::testActionPushPbb()
{
	uint16_t eth_type = 0xa1a2;

	rofl::openflow::cofaction_push_pbb action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);
	action.set_eth_type(eth_type);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_PUSH_PBB);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(action.get_eth_type()== eth_type);

	rofl::openflow::cofaction_push_pbb clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_PUSH_PBB);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_push));
	CPPUNIT_ASSERT(clone.get_eth_type() == eth_type);
}



void
cofaction_test::testActionPopPbb()
{
	rofl::openflow::cofaction_pop_pbb action;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == action.get_version());

	action.set_version(rofl::openflow13::OFP_VERSION);

	rofl::cmemory packed(action.length());
	action.pack(packed.somem(), packed.length());

	std::cerr << "action:" << std::endl << action;

	std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(action.get_type() 	== rofl::openflow::OFPAT_POP_PBB);
	CPPUNIT_ASSERT(action.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(action.length() 		== sizeof(struct rofl::openflow13::ofp_action_header));

	rofl::openflow::cofaction_pop_pbb clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.length());

	std::cerr << "clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_type() 	== rofl::openflow::OFPAT_POP_PBB);
	CPPUNIT_ASSERT(clone.get_length() 	== sizeof(struct rofl::openflow13::ofp_action_header));
	CPPUNIT_ASSERT(clone.length() 		== sizeof(struct rofl::openflow13::ofp_action_header));
}



