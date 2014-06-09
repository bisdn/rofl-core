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
	rofl::caddress addr(AF_INET, "161.162.163.164");

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
	rofl::caddress addr(AF_INET, "161.162.163.164");

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



