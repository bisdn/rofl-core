#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofinstruction_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofinstruction_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofinstruction_test::setUp()
{
}



void
cofinstruction_test::tearDown()
{
}



void
cofinstruction_test::test_cofinstruction()
{
	rofl::openflow::cofinstruction inst;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == inst.get_version());

	rofl::cmemory body(12);
	for (unsigned int i = 0; i < body.memlen(); i++) {
		body[i] = i;
	}

	inst = rofl::openflow::cofinstruction(rofl::openflow13::OFP_VERSION, 0xa1a2, body);

	//std::cerr << "inst:" << std::endl << inst;

	rofl::cmemory packed(inst.length());
	inst.pack(packed.somem(), packed.memlen());

	//std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(packed.memlen() == 16);
	CPPUNIT_ASSERT(packed[0] == 0xa1);
	CPPUNIT_ASSERT(packed[1] == 0xa2);
	CPPUNIT_ASSERT(packed[2] == 0x00);
	CPPUNIT_ASSERT(packed[3] == 0x10);

	rofl::openflow::cofinstruction clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(clone.get_body() == body);
	CPPUNIT_ASSERT(clone.get_type() == 0xa1a2);
	CPPUNIT_ASSERT(clone.get_length() == 16);
}



void
cofinstruction_test::test_cofinstruction_goto_table()
{
	rofl::openflow::cofinstruction_goto_table inst;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == inst.get_version());

	uint8_t table_id = 8;

	inst = rofl::openflow::cofinstruction_goto_table(rofl::openflow13::OFP_VERSION, table_id);

	//std::cerr << "inst:" << std::endl << inst;

	rofl::cmemory packed(inst.length());
	inst.pack(packed.somem(), packed.memlen());

	//std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(packed.memlen() == sizeof(struct rofl::openflow13::ofp_instruction_goto_table));
	CPPUNIT_ASSERT(packed[0] == 0x00);
	CPPUNIT_ASSERT(packed[1] == rofl::openflow13::OFPIT_GOTO_TABLE);
	CPPUNIT_ASSERT(packed[2] == 0x00);
	CPPUNIT_ASSERT(packed[3] == sizeof(struct rofl::openflow13::ofp_instruction_goto_table));
	CPPUNIT_ASSERT(packed[4] == table_id);

	rofl::openflow::cofinstruction_goto_table clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow13::OFPIT_GOTO_TABLE);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_instruction_goto_table));
	CPPUNIT_ASSERT(clone.get_table_id() == table_id);
}



void
cofinstruction_test::test_cofinstruction_apply_actions()
{
	rofl::cindex index(0);

	rofl::openflow::cofinstruction_apply_actions inst;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == inst.get_version());

	rofl::openflow::cofactions actions(rofl::openflow13::OFP_VERSION);
	actions.add_action_copy_ttl_in(index++);
	actions.add_action_output(index++).set_port_no(0x01);

	inst = rofl::openflow::cofinstruction_apply_actions(rofl::openflow13::OFP_VERSION, actions);

	//std::cerr << "inst:" << std::endl << inst;

	rofl::cmemory packed(inst.length());
	inst.pack(packed.somem(), packed.memlen());

	//std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(packed.memlen() == sizeof(struct rofl::openflow13::ofp_instruction_actions) + actions.length());
	CPPUNIT_ASSERT(packed[0] == 0x00);
	CPPUNIT_ASSERT(packed[1] == rofl::openflow13::OFPIT_APPLY_ACTIONS);
	CPPUNIT_ASSERT(packed[2] == 0x00);
	CPPUNIT_ASSERT(packed[3] == sizeof(struct rofl::openflow13::ofp_instruction_actions) + actions.length());

	rofl::openflow::cofinstruction_apply_actions clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow13::OFPIT_APPLY_ACTIONS);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_instruction_actions) + actions.length());
	CPPUNIT_ASSERT(actions == clone.get_actions());
}



void
cofinstruction_test::test_cofinstruction_write_actions()
{
	rofl::cindex index(0);

	rofl::openflow::cofinstruction_write_actions inst;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == inst.get_version());

	rofl::openflow::cofactions actions(rofl::openflow13::OFP_VERSION);
	actions.add_action_copy_ttl_in(index++);
	actions.add_action_output(index++).set_port_no(0x01);

	inst = rofl::openflow::cofinstruction_write_actions(rofl::openflow13::OFP_VERSION, actions);

	//std::cerr << "inst:" << std::endl << inst;

	rofl::cmemory packed(inst.length());
	inst.pack(packed.somem(), packed.memlen());

	//std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(packed.memlen() == sizeof(struct rofl::openflow13::ofp_instruction_actions) + actions.length());
	CPPUNIT_ASSERT(packed[0] == 0x00);
	CPPUNIT_ASSERT(packed[1] == rofl::openflow13::OFPIT_WRITE_ACTIONS);
	CPPUNIT_ASSERT(packed[2] == 0x00);
	CPPUNIT_ASSERT(packed[3] == sizeof(struct rofl::openflow13::ofp_instruction_actions) + actions.length());

	rofl::openflow::cofinstruction_write_actions clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow13::OFPIT_WRITE_ACTIONS);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_instruction_actions) + actions.length());
	CPPUNIT_ASSERT(actions == clone.get_actions());
}



void
cofinstruction_test::test_cofinstruction_clear_actions()
{
	rofl::openflow::cofinstruction_clear_actions inst;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == inst.get_version());

	inst = rofl::openflow::cofinstruction_clear_actions(rofl::openflow13::OFP_VERSION);

	//std::cerr << "inst:" << std::endl << inst;

	rofl::cmemory packed(inst.length());
	inst.pack(packed.somem(), packed.memlen());

	//std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(packed.memlen() == sizeof(struct rofl::openflow13::ofp_instruction_actions));
	CPPUNIT_ASSERT(packed[0] == 0x00);
	CPPUNIT_ASSERT(packed[1] == rofl::openflow13::OFPIT_CLEAR_ACTIONS);
	CPPUNIT_ASSERT(packed[2] == 0x00);
	CPPUNIT_ASSERT(packed[3] == sizeof(struct rofl::openflow13::ofp_instruction_actions));

	rofl::openflow::cofinstruction_clear_actions clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow13::OFPIT_CLEAR_ACTIONS);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_instruction_actions));
}



void
cofinstruction_test::test_cofinstruction_write_metadata()
{
	rofl::openflow::cofinstruction_write_metadata inst;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == inst.get_version());

	uint64_t metadata = 0xa1a2a3a4a5a6a7a8;
	uint64_t metadata_mask = 0xb1b2b3b4b5b6b7b8;

	inst = rofl::openflow::cofinstruction_write_metadata(rofl::openflow13::OFP_VERSION, metadata, metadata_mask);

	//std::cerr << "inst:" << std::endl << inst;

	rofl::cmemory packed(inst.length());
	inst.pack(packed.somem(), packed.memlen());

	//std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(packed.memlen() == sizeof(struct rofl::openflow13::ofp_instruction_write_metadata));
	CPPUNIT_ASSERT(packed[0] == 0x00);
	CPPUNIT_ASSERT(packed[1] == rofl::openflow13::OFPIT_WRITE_METADATA);
	CPPUNIT_ASSERT(packed[2] == 0x00);
	CPPUNIT_ASSERT(packed[3] == sizeof(struct rofl::openflow13::ofp_instruction_write_metadata));
	CPPUNIT_ASSERT(packed[8] == 0xa1);
	CPPUNIT_ASSERT(packed[9] == 0xa2);
	CPPUNIT_ASSERT(packed[10] == 0xa3);
	CPPUNIT_ASSERT(packed[11] == 0xa4);
	CPPUNIT_ASSERT(packed[12] == 0xa5);
	CPPUNIT_ASSERT(packed[13] == 0xa6);
	CPPUNIT_ASSERT(packed[14] == 0xa7);
	CPPUNIT_ASSERT(packed[15] == 0xa8);
	CPPUNIT_ASSERT(packed[16] == 0xb1);
	CPPUNIT_ASSERT(packed[17] == 0xb2);
	CPPUNIT_ASSERT(packed[18] == 0xb3);
	CPPUNIT_ASSERT(packed[19] == 0xb4);
	CPPUNIT_ASSERT(packed[20] == 0xb5);
	CPPUNIT_ASSERT(packed[21] == 0xb6);
	CPPUNIT_ASSERT(packed[22] == 0xb7);
	CPPUNIT_ASSERT(packed[23] == 0xb8);

	rofl::openflow::cofinstruction_write_metadata clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow13::OFPIT_WRITE_METADATA);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_instruction_write_metadata));
	CPPUNIT_ASSERT(clone.get_metadata() == metadata);
	CPPUNIT_ASSERT(clone.get_metadata_mask() == metadata_mask);
}



void
cofinstruction_test::test_cofinstruction_meter()
{
	rofl::openflow::cofinstruction_meter inst;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == inst.get_version());

	uint32_t meter_id = 0xa1a2a3a4;

	inst = rofl::openflow::cofinstruction_meter(rofl::openflow13::OFP_VERSION, meter_id);

	//std::cerr << "inst:" << std::endl << inst;

	rofl::cmemory packed(inst.length());
	inst.pack(packed.somem(), packed.memlen());

	//std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(packed.memlen() == sizeof(struct rofl::openflow13::ofp_instruction_meter));
	CPPUNIT_ASSERT(packed[0] == 0x00);
	CPPUNIT_ASSERT(packed[1] == rofl::openflow13::OFPIT_METER);
	CPPUNIT_ASSERT(packed[2] == 0x00);
	CPPUNIT_ASSERT(packed[3] == sizeof(struct rofl::openflow13::ofp_instruction_meter));
	CPPUNIT_ASSERT(packed[4] == 0xa1);
	CPPUNIT_ASSERT(packed[5] == 0xa2);
	CPPUNIT_ASSERT(packed[6] == 0xa3);
	CPPUNIT_ASSERT(packed[7] == 0xa4);

	rofl::openflow::cofinstruction_meter clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow13::OFPIT_METER);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_instruction_meter));
	CPPUNIT_ASSERT(clone.get_meter_id() == meter_id);
}



void
cofinstruction_test::test_cofinstruction_experimenter()
{
	rofl::openflow::cofinstruction_experimenter inst;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == inst.get_version());

	uint32_t exp_id = 0xa1a2a3a4;

	rofl::cmemory body(12);
	for (unsigned int i = 0; i < body.memlen(); i++) {
		body[i] = i;
	}

	inst = rofl::openflow::cofinstruction_experimenter(rofl::openflow13::OFP_VERSION, exp_id, body);

	//std::cerr << "inst:" << std::endl << inst;

	rofl::cmemory packed(inst.length());
	inst.pack(packed.somem(), packed.memlen());

	//std::cerr << "packed:" << std::endl << packed;

	CPPUNIT_ASSERT(packed.memlen() == sizeof(struct rofl::openflow13::ofp_instruction_experimenter) + body.memlen());
	CPPUNIT_ASSERT(packed[0] == 0xff);
	CPPUNIT_ASSERT(packed[1] == 0xff);
	CPPUNIT_ASSERT(packed[2] == 0x00);
	CPPUNIT_ASSERT(packed[3] == sizeof(struct rofl::openflow13::ofp_instruction_meter) + body.memlen());
	CPPUNIT_ASSERT(packed[4] == 0xa1);
	CPPUNIT_ASSERT(packed[5] == 0xa2);
	CPPUNIT_ASSERT(packed[6] == 0xa3);
	CPPUNIT_ASSERT(packed[7] == 0xa4);

	rofl::openflow::cofinstruction_experimenter clone(rofl::openflow13::OFP_VERSION);
	clone.unpack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(clone.get_type() == rofl::openflow13::OFPIT_EXPERIMENTER);
	CPPUNIT_ASSERT(clone.get_length() == sizeof(struct rofl::openflow13::ofp_instruction_experimenter) + body.memlen());
	CPPUNIT_ASSERT(clone.get_exp_id() == exp_id);
	CPPUNIT_ASSERT(clone.get_body() == body);
}


