/*
 * cofmsg_error.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_ERROR_H_
#define COFMSG_ERROR_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/openflow_common.h"
#include "rofl/common/openflow/openflow.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_error :
	public cofmsg
{
private:

	cmemory body;

	union {
		uint8_t									*emu_error_msg;
		struct openflow10::ofp_error_msg		*emu10_error_msg;
		struct openflow12::ofp_error_msg		*emu12_error_msg;
		struct openflow13::ofp_error_msg		*emu13_error_msg;
	} emu;

#define err_msg		emu.emu_error_msg
#define err10_msg 	emu.emu10_error_msg
#define err12_msg 	emu.emu12_error_msg
#define err13_msg 	emu.emu13_error_msg

public:


	/** constructor
	 *
	 */
	cofmsg_error(
			uint8_t of_version,
			uint32_t xid,
			uint16_t err_type = 0,
			uint16_t err_code = 0,
			uint8_t* data = 0,
			size_t datalen = 0);


	/**
	 *
	 */
	cofmsg_error(
			cofmsg_error const& error);


	/**
	 *
	 */
	cofmsg_error&
	operator= (
			cofmsg_error const& error);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_error();


	/**
	 *
	 */
	cofmsg_error(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual uint8_t*
	resize(size_t len);


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();

public:

	/**
	 *
	 */
	uint16_t
	get_err_type() const;

	/**
	 *
	 */
	void
	set_err_type(uint16_t type);

	/**
	 *
	 */
	uint16_t
	get_err_code() const;

	/**
	 *
	 */
	void
	set_err_code(uint16_t code);

	/**
	 *
	 */
	cmemory&
	get_body();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_error const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_error >" << std::endl;

		os << std::hex;
		uint32_t error_id = ((uint32_t)msg.get_err_type() << 16) | msg.get_err_code();
		switch ((int32_t)error_id) {
		case OFPETC_HELLO_FAILED_INCOMPATIBLE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " HELLO-FAILED-INCOMPATIBLE >" << std::endl;
		} break;
		case OFPETC_HELLO_FAILED_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " HELLO-FAILED-EPERM >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BAD_VERSION: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BAD-VERSION >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BAD_TYPE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BAD-TYPE >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BAD_STAT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BAD-STAT >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BAD_EXPERIMENTER: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BAD-EXPERIMENTER >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BAD_EXP_TYPE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BAD-EXP-TYPE >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-EPERM >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BAD_LEN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BAD-LEN >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BUFFER_EMPTY: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BUFFER-EMPTY >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BUFFER_UNKNOWN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BUFFER-UNKNOWN >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BAD_TABLE_ID: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BAD-TABLE-ID >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_IS_SLAVE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-IS-SLAVE >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BAD_PORT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BAD-PORT >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_BAD_PACKET: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-BAD-PACKET >" << std::endl;
		} break;
		case OFPETC_BAD_REQUEST_MULTIPART_BUFFER_OVERFLOW: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-REQUEST-MULTIPART-BUFFER-OVERFLOW >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_TYPE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-TYPE >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_LEN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-LEN >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_EXPERIMENTER: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-EXPERIMENTER >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_EXP_TYPE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-EXP-TYPE >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_OUT_PORT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-OUT-PORT >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_ARGUMENT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-ARGUMENT >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-EPERM >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_TOO_MANY: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-TOO-MANY >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_QUEUE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-QUEUE >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_OUT_GROUP: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-OUT-GROUP >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_MATCH_INCONSISTENT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-MATCH-INCONSISTENT >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_UNSUPPORTED_ORDER: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-UNSUPPORTED-ORDER >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_TAG: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-TAG >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_SET_TYPE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-SET-TYPE >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_SET_LEN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-SET-LEN >" << std::endl;
		} break;
		case OFPETC_BAD_ACTION_BAD_SET_ARGUMENT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-ACTION-BAD-SET-ARGUMENT >" << std::endl;
		} break;
		case OFPETC_BAD_INSTRUCTION_UNKNOWN_INST: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-INSTRUCTION-UNKNOWN-INST >" << std::endl;
		} break;
		case OFPETC_BAD_INSTRUCTION_UNSUP_INST: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-INSTRUCTION-UNSUP-INST >" << std::endl;
		} break;
		case OFPETC_BAD_INSTRUCTION_BAD_TABLE_ID: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-INSTRUCTION-BAD-TABLE-ID >" << std::endl;
		} break;
		case OFPETC_BAD_INSTRUCTION_UNSUP_METADATA: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-INSTRUCTION-UNSUP-METADATA >" << std::endl;
		} break;
		case OFPETC_BAD_INSTRUCTION_UNSUP_METADATA_MASK: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-INSTRUCTION-UNSUP-METADATA-MASK >" << std::endl;
		} break;
		case OFPETC_BAD_INSTRUCTION_BAD_EXPERIMENTER: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-INSTRUCTION-BAD-EXPERIMENTER >" << std::endl;
		} break;
		case OFPETC_BAD_INSTRUCTION_BAD_EXP_TYPE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-INSTRUCTION-BAD-EXP-TYPE >" << std::endl;
		} break;
		case OFPETC_BAD_INSTRUCTION_BAD_LEN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-INSTRUCTION-BAD-LEN >" << std::endl;
		} break;
		case OFPETC_BAD_INSTRUCTION_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-INSTRUCTION-EPERM >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_TYPE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-TYPE >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_LEN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-LEN >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_TAG: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-TAG >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_DL_ADDR_MASK: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-DL-ADDR-MASK >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_NW_ADDR_MASK: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-NW-ADDR-MASK >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_WILDCARDS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-WILDCARDS >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_FIELD: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-FIELD >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_VALUE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-VALUE >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_MASK: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-MASK >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_BAD_PREREQ: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-BAD-PREREQ >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_DUP_FIELD: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-DUP-FIELD >" << std::endl;
		} break;
		case OFPETC_BAD_MATCH_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " BAD-MATCH-EPERM >" << std::endl;
		} break;
		case OFPETC_FLOW_MOD_UNKNOWN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " FLOW-MOD-UNKNOWN >" << std::endl;
		} break;
		case OFPETC_FLOW_MOD_TABLE_FULL: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " FLOW-MOD-TABLE-FULL >" << std::endl;
		} break;
		case OFPETC_FLOW_MOD_BAD_TABLE_ID: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " FLOW-MOD-BAD-TABLE-ID >" << std::endl;
		} break;
		case OFPETC_FLOW_MOD_OVERLAP: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " FLOW-MOD-OVERLAP >" << std::endl;
		} break;
		case OFPETC_FLOW_MOD_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " FLOW-MOD-EPERM >" << std::endl;
		} break;
		case OFPETC_FLOW_MOD_BAD_TIMEOUT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " FLOW-MOD-BAD-TIMEOUT >" << std::endl;
		} break;
		case OFPETC_FLOW_MOD_BAD_COMMAND: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " FLOW-MOD-BAD-COMMAND >" << std::endl;
		} break;
		case OFPETC_FLOW_MOD_BAD_FLAGS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " FLOW-MOD-BAD-FLAGS >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_GROUP_EXISTS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-GROUP-EXISTS >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_INVALID_GROUP: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-INVALID-GROUP >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_WEIGHT_UNSUPPORTED: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-WEIGHT-UNSUPPORTED >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_OUT_OF_GROUPS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-OUT-OF-GROUPS >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_OUT_OF_BUCKETS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-OUT-OF-BUCKETS >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_CHAINING_UNSUPPORTED: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-CHAINING-UNSUPPORTED >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_WATCH_UNSUPPORTED: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-WATCH-UNSUPPORTED >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_LOOP: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-LOOP >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_UNKNOWN_GROUP: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-UNKNOWN-GROUP >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_CHAINED_GROUP: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-CHAINED-GROUP >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_BAD_TYPE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-BAD-TYPE >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_BAD_COMMAND: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-BAD-COMMAND >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_BAD_BUCKET: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-BAD-BUCKET >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_BAD_WATCH: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-BAD-WATCH >" << std::endl;
		} break;
		case OFPETC_GROUP_MOD_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " GROUP-MOD-EPERM >" << std::endl;
		} break;
		case OFPETC_PORT_MOD_BAD_PORT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " PORT-MOD-BAD-PORT >" << std::endl;
		} break;
		case OFPETC_PORT_MOD_BAD_HW_ADDR: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " PORT-MOD-BAD-HW-ADDR >" << std::endl;
		} break;
		case OFPETC_PORT_MOD_BAD_CONFIG: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " PORT-MOD-BAD-CONFIG >" << std::endl;
		} break;
		case OFPETC_PORT_MOD_BAD_ADVERTISE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " PORT-MOD-BAD-ADVERTISE >" << std::endl;
		} break;
		case OFPETC_PORT_MOD_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " PORT-MOD-EPERM >" << std::endl;
		} break;
		case OFPETC_TABLE_MOD_BAD_TABLE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " TABLE-MOD-BAD-TABLE >" << std::endl;
		} break;
		case OFPETC_TABLE_MOD_BAD_CONFIG: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " TABLE-MOD-BAD-CONFIG >" << std::endl;
		} break;
		case OFPETC_TABLE_MOD_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " TABLE-MOD-EPERM >" << std::endl;
		} break;
		case OFPETC_QUEUE_OP_BAD_PORT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " QUEUE-OP-BAD-PORT >" << std::endl;
		} break;
		case OFPETC_QUEUE_OP_BAD_QUEUE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " QUEUE-OP-BAD-QUEUE >" << std::endl;
		} break;
		case OFPETC_QUEUE_OP_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " QUEUE-OP-EPERM >" << std::endl;
		} break;
		case OFPETC_SWITCH_CONFIG_BAD_FLAGS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " SWITCH-CONFIG-BAD-FLAGS >" << std::endl;
		} break;
		case OFPETC_SWITCH_CONFIG_BAD_LEN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " SWITCH-CONFIG-BAD-LEN >" << std::endl;
		} break;
		case OFPETC_SWITCH_CONFIG_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " SWITCH-CONFIG-EPERM >" << std::endl;
		} break;
		case OFPETC_ROLE_REQUEST_STALE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " ROLE-REQUEST-STALE >" << std::endl;
		} break;
		case OFPETC_ROLE_REQUEST_UNSUP: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " ROLE-REQUEST-UNSUP >" << std::endl;
		} break;
		case OFPETC_ROLE_REQUEST_BAD_ROLE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " ROLE-REQUEST-BAD-ROLE >" << std::endl;
		} break;
		case OFPETC_METER_MOD_UNKNOWN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-UNKNOWN >" << std::endl;
		} break;
		case OFPETC_METER_MOD_METER_EXISTS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-METER-EXISTS >" << std::endl;
		} break;
		case OFPETC_METER_MOD_INVALID_METER: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-INVALID-METER >" << std::endl;
		} break;
		case OFPETC_METER_MOD_UNKNOWN_METER: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-UNKNOWN-METER >" << std::endl;
		} break;
		case OFPETC_METER_MOD_BAD_COMMAND: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-BAD-COMMAND >" << std::endl;
		} break;
		case OFPETC_METER_MOD_BAD_FLAGS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-BAD-FLAGS >" << std::endl;
		} break;
		case OFPETC_METER_MOD_BAD_RATE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-BAD-RATE >" << std::endl;
		} break;
		case OFPETC_METER_MOD_BAD_BURST: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-BAD-BURST >" << std::endl;
		} break;
		case OFPETC_METER_MOD_BAD_BAND: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-BAD-BAND >" << std::endl;
		} break;
		case OFPETC_METER_MOD_BAD_BAND_VALUE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-BAD-BAND-VALUE >" << std::endl;
		} break;
		case OFPETC_METER_MOD_OUT_OF_METERS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-OUT-OF-METERS >" << std::endl;
		} break;
		case OFPETC_METER_MOD_OUT_OF_BANDS: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " METER-MOD-OUT-OF-BANDS >" << std::endl;
		} break;
		case OFPETC_TABLE_FEATURES_BAD_TABLE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " TABLE-FEATURES-BAD-TABLE >" << std::endl;
		} break;
		case OFPETC_TABLE_FEATURES_BAD_METADATA: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " TABLE-FEATURES-BAD-METADATA >" << std::endl;
		} break;
		case OFPETC_TABLE_FEATURES_BAD_TYPE: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " TABLE-FEATURES-BAD-TYPE >" << std::endl;
		} break;
		case OFPETC_TABLE_FEATURES_BAD_LEN: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " TABLE-FEATURES-BAD-LEN >" << std::endl;
		} break;
		case OFPETC_TABLE_FEATURES_BAD_ARGUMENT: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " TABLE-FEATURES-BAD-ARGUMENT >" << std::endl;
		} break;
		case OFPETC_TABLE_FEATURES_EPERM: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " TABLE-FEATURES-EPERM >" << std::endl;
		} break;
		case OFPETC_EXPERIMENTER: {
			os << indent(4) << "<error-id: 0x" << (int)error_id << " EXPERIMENTER >" << std::endl;
		} break;
		default: {
			os << indent(4) << "<err-type:" << (int)msg.get_err_type() << " >" << std::endl;
			os << indent(4) << "<err-code:" << (int)msg.get_err_code() << " >" << std::endl;
		};
		}
		os << std::dec;
		indent i(6);
		os << msg.body;
		return os;
	};
};



class cofmsg_error_bad_request_bad_len : public cofmsg_error {
public:
	cofmsg_error_bad_request_bad_len(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, data, datalen) {};
	virtual ~cofmsg_error_bad_request_bad_len() {};
};



class cofmsg_error_bad_request_bad_version : public cofmsg_error {
public:
	cofmsg_error_bad_request_bad_version(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION, data, datalen) {};
	virtual ~cofmsg_error_bad_request_bad_version() {};
};



class cofmsg_error_bad_request_bad_type : public cofmsg_error {
public:
	cofmsg_error_bad_request_bad_type(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE, data, datalen) {};
	virtual ~cofmsg_error_bad_request_bad_type() {};
};



class cofmsg_error_bad_request_bad_stat : public cofmsg_error {
public:
	cofmsg_error_bad_request_bad_stat(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BAD_STAT, data, datalen) {};
	virtual ~cofmsg_error_bad_request_bad_stat() {};
};



class cofmsg_error_bad_request_bad_experimenter : public cofmsg_error {
public:
	cofmsg_error_bad_request_bad_experimenter(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER, data, datalen) {};
	virtual ~cofmsg_error_bad_request_bad_experimenter() {};
};



class cofmsg_error_bad_request_bad_exp_type : public cofmsg_error {
public:
	cofmsg_error_bad_request_bad_exp_type(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BAD_EXP_TYPE, data, datalen) {};
	virtual ~cofmsg_error_bad_request_bad_exp_type() {};
};



class cofmsg_error_bad_request_eperm : public cofmsg_error {
public:
	cofmsg_error_bad_request_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_EPERM, data, datalen) {};
	virtual ~cofmsg_error_bad_request_eperm() {};
};



class cofmsg_error_bad_request_buffer_empty : public cofmsg_error {
public:
	cofmsg_error_bad_request_buffer_empty(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BUFFER_EMPTY, data, datalen) {};
	virtual ~cofmsg_error_bad_request_buffer_empty() {};
};



class cofmsg_error_bad_request_buffer_unknown : public cofmsg_error {
public:
	cofmsg_error_bad_request_buffer_unknown(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BUFFER_UNKNOWN, data, datalen) {};
	virtual ~cofmsg_error_bad_request_buffer_unknown() {};
};



class cofmsg_error_bad_request_bad_table_id : public cofmsg_error {
public:
	cofmsg_error_bad_request_bad_table_id(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BAD_TABLE_ID, data, datalen) {};
	virtual ~cofmsg_error_bad_request_bad_table_id() {};
};



class cofmsg_error_bad_request_is_slave : public cofmsg_error {
public:
	cofmsg_error_bad_request_is_slave(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE, data, datalen) {};
	virtual ~cofmsg_error_bad_request_is_slave() {};
};



class cofmsg_error_bad_request_bad_port : public cofmsg_error {
public:
	cofmsg_error_bad_request_bad_port(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BAD_PORT, data, datalen) {};
	virtual ~cofmsg_error_bad_request_bad_port() {};
};



class cofmsg_error_bad_request_bad_packet : public cofmsg_error {
public:
	cofmsg_error_bad_request_bad_packet(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_BAD_PACKET, data, datalen) {};
	virtual ~cofmsg_error_bad_request_bad_packet() {};
};



class cofmsg_error_bad_request_multipart_buffer_overflow : public cofmsg_error {
public:
	cofmsg_error_bad_request_multipart_buffer_overflow(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_REQUEST, OFPBRC_MULTIPART_BUFFER_OVERFLOW, data, datalen) {};
	virtual ~cofmsg_error_bad_request_multipart_buffer_overflow() {};
};



class cofmsg_error_bad_action_bad_type : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_type(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_TYPE, data, datalen) {};
	virtual ~cofmsg_error_bad_action_bad_type() {};
};



class cofmsg_error_bad_action_bad_len : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_len(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_LEN, data, datalen) {};
	virtual ~cofmsg_error_bad_action_bad_len() {};
};



class cofmsg_error_bad_action_bad_experimenter : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_experimenter(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_EXPERIMENTER, data, datalen) {};
	virtual ~cofmsg_error_bad_action_bad_experimenter() {};
};



class cofmsg_error_bad_action_bad_experimenter_type : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_experimenter_type(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_EXP_TYPE, data, datalen) {};
	virtual ~cofmsg_error_bad_action_bad_experimenter_type() {};
};



class cofmsg_error_bad_action_bad_out_port : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_out_port(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT, data, datalen) {};
	virtual ~cofmsg_error_bad_action_bad_out_port() {};
};



class cofmsg_error_bad_action_bad_argument : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_argument(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT, data, datalen) {};
	virtual ~cofmsg_error_bad_action_bad_argument() {};
};



class cofmsg_error_bad_action_eperm : public cofmsg_error {
public:
	cofmsg_error_bad_action_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_EPERM, data, datalen) {};
	virtual ~cofmsg_error_bad_action_eperm() {};
};



class cofmsg_error_bad_action_too_many : public cofmsg_error {
public:
	cofmsg_error_bad_action_too_many(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_TOO_MANY, data, datalen) {};
	virtual ~cofmsg_error_bad_action_too_many() {};
};



class cofmsg_error_bad_action_bad_queue : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_queue(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_QUEUE, data, datalen) {};
	virtual ~cofmsg_error_bad_action_bad_queue() {};
};



class cofmsg_error_bad_action_bad_out_group : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_out_group(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_GROUP, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_action_bad_out_group() {};
};



class cofmsg_error_bad_action_match_inconsistent : public cofmsg_error {
public:
	cofmsg_error_bad_action_match_inconsistent(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_MATCH_INCONSISTENT, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_action_match_inconsistent() {};
};



class cofmsg_error_bad_action_unsupported_order : public cofmsg_error {
public:
	cofmsg_error_bad_action_unsupported_order(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_UNSUPPORTED_ORDER, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_action_unsupported_order() {};
};



class cofmsg_error_bad_action_bad_tag : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_tag(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_TAG, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_action_bad_tag() {};
};



class cofmsg_error_bad_action_bad_set_len : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_set_len(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_SET_LEN, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_action_bad_set_len() {};
};



class cofmsg_error_bad_action_bad_set_type : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_set_type(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_SET_TYPE, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_action_bad_set_type() {};
};



class cofmsg_error_bad_action_bad_set_argument : public cofmsg_error {
public:
	cofmsg_error_bad_action_bad_set_argument(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_ACTION, OFPBAC_BAD_SET_ARGUMENT, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_action_bad_set_argument() {};
};



class cofmsg_error_bad_inst_unknown_inst : public cofmsg_error {
public:
	cofmsg_error_bad_inst_unknown_inst(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_INSTRUCTION, OFPBIC_UNKNOWN_INST, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_inst_unknown_inst() {};
};



class cofmsg_error_bad_inst_unsup_inst : public cofmsg_error {
public:
	cofmsg_error_bad_inst_unsup_inst(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_INSTRUCTION, OFPBIC_UNSUP_INST, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_inst_unsup_inst() {};
};



class cofmsg_error_bad_inst_bad_table_id : public cofmsg_error {
public:
	cofmsg_error_bad_inst_bad_table_id(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_INSTRUCTION, OFPBIC_BAD_TABLE_ID, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_inst_bad_table_id() {};
};



class cofmsg_error_bad_inst_unsup_metadata : public cofmsg_error {
public:
	cofmsg_error_bad_inst_unsup_metadata(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_INSTRUCTION, OFPBIC_UNSUP_METADATA, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_inst_unsup_metadata() {};
};



class cofmsg_error_bad_inst_unsup_metadata_mask : public cofmsg_error {
public:
	cofmsg_error_bad_inst_unsup_metadata_mask(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_INSTRUCTION, OFPBIC_UNSUP_METADATA_MASK, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_inst_unsup_metadata_mask() {};
};



class cofmsg_error_bad_inst_bad_experimenter : public cofmsg_error {
public:
	cofmsg_error_bad_inst_bad_experimenter(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_INSTRUCTION, OFPBIC_BAD_EXPERIMENTER, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_inst_bad_experimenter() {};
};



class cofmsg_error_bad_inst_bad_exp_type : public cofmsg_error {
public:
	cofmsg_error_bad_inst_bad_exp_type(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_INSTRUCTION, OFPBIC_BAD_EXP_TYPE, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_inst_bad_exp_type() {};
};



class cofmsg_error_bad_inst_bad_len : public cofmsg_error {
public:
	cofmsg_error_bad_inst_bad_len(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_INSTRUCTION, OFPBIC_BAD_LEN, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_inst_bad_len() {};
};



class cofmsg_error_bad_inst_eperm : public cofmsg_error {
public:
	cofmsg_error_bad_inst_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_INSTRUCTION, OFPBIC_EPERM, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_inst_eperm() {};
};



class cofmsg_error_bad_match_bad_type : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_type(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_TYPE, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_type() {};
};



class cofmsg_error_bad_match_bad_len : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_len(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_LEN, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_len() {};
};



class cofmsg_error_bad_match_bad_tag : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_tag(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_TAG, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_tag() {};
};



class cofmsg_error_bad_match_bad_dladdr_mask : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_dladdr_mask(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_DL_ADDR_MASK, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_dladdr_mask() {};
};



class cofmsg_error_bad_match_bad_nwaddr_mask : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_nwaddr_mask(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_NW_ADDR_MASK, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_nwaddr_mask() {};
};



class cofmsg_error_bad_match_bad_wildcards : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_wildcards(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_WILDCARDS, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_wildcards() {};
};



class cofmsg_error_bad_match_bad_field : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_field(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_FIELD, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_field() {};
};



class cofmsg_error_bad_match_bad_value : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_value(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_VALUE, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_value() {};
};



class cofmsg_error_bad_match_bad_mask : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_mask(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_MASK, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_mask() {};
};



class cofmsg_error_bad_match_bad_prereq : public cofmsg_error {
public:
	cofmsg_error_bad_match_bad_prereq(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_BAD_PREREQ, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_bad_prereq() {};
};



class cofmsg_error_bad_match_dup_field : public cofmsg_error {
public:
	cofmsg_error_bad_match_dup_field(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_DUP_FIELD, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_dup_field() {};
};



class cofmsg_error_bad_match_eperm : public cofmsg_error {
public:
	cofmsg_error_bad_match_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_BAD_MATCH, OFPBMC_EPERM, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_bad_match_eperm() {};
};



class cofmsg_error_hello_failed_incompatible : public cofmsg_error {
public:
	cofmsg_error_hello_failed_incompatible(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_HELLO_FAILED, OFPHFC_INCOMPATIBLE, data, datalen) {
	};
	virtual ~cofmsg_error_hello_failed_incompatible() {};
};



class cofmsg_error_hello_failed_eperm : public cofmsg_error {
public:
	cofmsg_error_hello_failed_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_HELLO_FAILED, OFPHFC_EPERM, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_hello_failed_eperm() {};
};



class cofmsg_error_switch_config_failed_bad_flags : public cofmsg_error {
public:
	cofmsg_error_switch_config_failed_bad_flags(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_SWITCH_CONFIG_FAILED, OFPSCFC_BAD_FLAGS, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_switch_config_failed_bad_flags() {};
};



class cofmsg_error_switch_config_failed_bad_len : public cofmsg_error {
public:
	cofmsg_error_switch_config_failed_bad_len(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_SWITCH_CONFIG_FAILED, OFPSCFC_BAD_LEN, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_switch_config_failed_bad_len() {};
};



class cofmsg_error_switch_config_failed_eperm : public cofmsg_error {
public:
	cofmsg_error_switch_config_failed_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_SWITCH_CONFIG_FAILED, OFPSCFC_EPERM, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_switch_config_failed_eperm() {};
};



class cofmsg_error_flow_mod_failed_unknown : public cofmsg_error {
public:
	cofmsg_error_flow_mod_failed_unknown(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_FLOW_MOD_FAILED, OFPFMFC_UNKNOWN, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_flow_mod_failed_unknown() {};
};



class cofmsg_error_flow_mod_failed_table_full : public cofmsg_error {
public:
	cofmsg_error_flow_mod_failed_table_full(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, 0, 0, data, datalen) {
		switch (ofp_version) {
		case rofl::openflow10::OFP_VERSION: {
			set_err_type(rofl::openflow10::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow10::OFPFMFC_ALL_TABLES_FULL);
		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION:
		default: {
			set_err_type(rofl::openflow12::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow12::OFPFMFC_TABLE_FULL);
		};
		}
	};
	virtual ~cofmsg_error_flow_mod_failed_table_full() {};
};



class cofmsg_error_flow_mod_failed_bad_table_id : public cofmsg_error {
public:
	cofmsg_error_flow_mod_failed_bad_table_id(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_FLOW_MOD_FAILED, OFPFMFC_BAD_TABLE_ID, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_flow_mod_failed_bad_table_id() {};
};



class cofmsg_error_flow_mod_failed_overlap : public cofmsg_error {
public:
	cofmsg_error_flow_mod_failed_overlap(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, 0, 0, data, datalen) {
		switch (ofp_version) {
		case rofl::openflow10::OFP_VERSION: {
			set_err_type(rofl::openflow10::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow10::OFPFMFC_OVERLAP);
		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION:
		default: {
			set_err_type(rofl::openflow12::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow12::OFPFMFC_OVERLAP);
		};
		}
	};
	virtual ~cofmsg_error_flow_mod_failed_overlap() {};
};



class cofmsg_error_flow_mod_failed_eperm : public cofmsg_error {
public:
	cofmsg_error_flow_mod_failed_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, 0, 0, data, datalen) {
		switch (ofp_version) {
		case rofl::openflow10::OFP_VERSION: {
			set_err_type(rofl::openflow10::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow10::OFPFMFC_EPERM);
		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION:
		default: {
			set_err_type(rofl::openflow12::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow12::OFPFMFC_EPERM);
		};
		}
	};
	virtual ~cofmsg_error_flow_mod_failed_eperm() {};
};



class cofmsg_error_flow_mod_failed_bad_timeout : public cofmsg_error {
public:
	cofmsg_error_flow_mod_failed_bad_timeout(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, 0, 0, data, datalen) {
		switch (ofp_version) {
		case rofl::openflow10::OFP_VERSION: {
			set_err_type(rofl::openflow10::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow10::OFPFMFC_BAD_EMERG_TIMEOUT);
		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION:
		default: {
			set_err_type(rofl::openflow12::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow12::OFPFMFC_BAD_TIMEOUT);
		};
		}
	};
	virtual ~cofmsg_error_flow_mod_failed_bad_timeout() {};
};



class cofmsg_error_flow_mod_failed_bad_command : public cofmsg_error {
public:
	cofmsg_error_flow_mod_failed_bad_command(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, 0, 0, data, datalen) {
		switch (ofp_version) {
		case rofl::openflow10::OFP_VERSION: {
			set_err_type(rofl::openflow10::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow10::OFPFMFC_BAD_COMMAND);
		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION:
		default: {
			set_err_type(rofl::openflow12::OFPET_FLOW_MOD_FAILED);
			set_err_code(rofl::openflow12::OFPFMFC_BAD_COMMAND);
		};
		}
	};
	virtual ~cofmsg_error_flow_mod_failed_bad_command() {};
};



class cofmsg_error_flow_mod_failed_unsupported : public cofmsg_error {
public:
	cofmsg_error_flow_mod_failed_unsupported(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, rofl::openflow10::OFPET_FLOW_MOD_FAILED, rofl::openflow10::OFPFMFC_UNSUPPORTED, data, datalen) {
		if (ofp_version > rofl::openflow10::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_flow_mod_failed_unsupported() {};
};



class cofmsg_error_flow_mod_failed_bad_flags : public cofmsg_error {
public:
	cofmsg_error_flow_mod_failed_bad_flags(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_FLOW_MOD_FAILED, OFPFMFC_BAD_FLAGS, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_flow_mod_failed_bad_flags() {};
};



class cofmsg_error_group_mod_failed_group_exists : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_group_exists(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_GROUP_EXISTS, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_group_exists() {};
};



class cofmsg_error_group_mod_failed_inval_group : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_inval_group(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_INVALID_GROUP, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_inval_group() {};
};



class cofmsg_error_group_mod_failed_weight_unsupported : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_weight_unsupported(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_WEIGHT_UNSUPPORTED, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_weight_unsupported() {};
};



class cofmsg_error_group_mod_failed_out_of_groups : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_out_of_groups(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_OUT_OF_GROUPS, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_out_of_groups() {};
};



class cofmsg_error_group_mod_failed_out_of_buckets : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_out_of_buckets(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_OUT_OF_BUCKETS, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_out_of_buckets() {};
};



class cofmsg_error_group_mod_failed_chaining_unsupported : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_chaining_unsupported(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_CHAINING_UNSUPPORTED, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_chaining_unsupported() {};
};



class cofmsg_error_group_mod_failed_watch_unsupported : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_watch_unsupported(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_WATCH_UNSUPPORTED, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_watch_unsupported() {};
};



class cofmsg_error_group_mod_failed_loop : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_loop(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_LOOP, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_loop() {};
};



class cofmsg_error_group_mod_failed_unknown_group : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_unknown_group(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_UNKNOWN_GROUP, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_unknown_group() {};
};



class cofmsg_error_group_mod_failed_chained_group : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_chained_group(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_CHAINED_GROUP, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_chained_group() {};
};



class cofmsg_error_group_mod_failed_bad_type : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_bad_type(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_BAD_TYPE, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_bad_type() {};
};



class cofmsg_error_group_mod_failed_bad_command : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_bad_command(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_BAD_COMMAND, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_bad_command() {};
};



class cofmsg_error_group_mod_failed_bad_bucket : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_bad_bucket(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_BAD_BUCKET, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_bad_bucket() {};
};



class cofmsg_error_group_mod_failed_bad_watch : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_bad_watch(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_BAD_WATCH, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_bad_watch() {};
};



class cofmsg_error_group_mod_failed_eperm : public cofmsg_error {
public:
	cofmsg_error_group_mod_failed_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_GROUP_MOD_FAILED, OFPGMFC_EPERM, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_group_mod_failed_eperm() {};
};



class cofmsg_error_port_mod_failed_bad_port : public cofmsg_error {
public:
	cofmsg_error_port_mod_failed_bad_port(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_PORT_MOD_FAILED, OFPPMFC_BAD_PORT, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_port_mod_failed_bad_port() {};
};



class cofmsg_error_port_mod_failed_bad_hwaddr : public cofmsg_error {
public:
	cofmsg_error_port_mod_failed_bad_hwaddr(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_PORT_MOD_FAILED, OFPPMFC_BAD_HW_ADDR, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_port_mod_failed_bad_hwaddr() {};
};



class cofmsg_error_port_mod_failed_bad_config : public cofmsg_error {
public:
	cofmsg_error_port_mod_failed_bad_config(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_PORT_MOD_FAILED, OFPPMFC_BAD_CONFIG, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_port_mod_failed_bad_config() {};
};



class cofmsg_error_port_mod_failed_bad_advertise : public cofmsg_error {
public:
	cofmsg_error_port_mod_failed_bad_advertise(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_PORT_MOD_FAILED, OFPPMFC_BAD_ADVERTISE, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_port_mod_failed_bad_advertise() {};
};



class cofmsg_error_port_mod_failed_eperm : public cofmsg_error {
public:
	cofmsg_error_port_mod_failed_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_PORT_MOD_FAILED, OFPPMFC_EPERM, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_port_mod_failed_eperm() {};
};



class cofmsg_error_table_mod_failed_bad_table : public cofmsg_error {
public:
	cofmsg_error_table_mod_failed_bad_table(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_TABLE_MOD_FAILED, OFPTMFC_BAD_TABLE, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_table_mod_failed_bad_table() {};
};



class cofmsg_error_table_mod_failed_bad_config : public cofmsg_error {
public:
	cofmsg_error_table_mod_failed_bad_config(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_TABLE_MOD_FAILED, OFPTMFC_BAD_CONFIG, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_table_mod_failed_bad_config() {};
};



class cofmsg_error_table_mod_failed_eperm : public cofmsg_error {
public:
	cofmsg_error_table_mod_failed_eperm(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_TABLE_MOD_FAILED, OFPTMFC_EPERM, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_table_mod_failed_eperm() {};
};



class cofmsg_error_role_request_failed_stale : public cofmsg_error {
public:
	cofmsg_error_role_request_failed_stale(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_ROLE_REQUEST_FAILED, OFPRRFC_STALE, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_role_request_failed_stale() {};
};



class cofmsg_error_role_request_failed_unsupported : public cofmsg_error {
public:
	cofmsg_error_role_request_failed_unsupported(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_ROLE_REQUEST_FAILED, OFPRRFC_UNSUP, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_role_request_failed_unsupported() {};
};



class cofmsg_error_role_request_failed_bad_role : public cofmsg_error {
public:
	cofmsg_error_role_request_failed_bad_role(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_ROLE_REQUEST_FAILED, OFPRRFC_BAD_ROLE, data, datalen) {
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_role_request_failed_bad_role() {};
};



class cofmsg_error_meter_mod_unknown : public cofmsg_error {
public:
	cofmsg_error_meter_mod_unknown(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_UNKNOWN, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_unknown() {};
};



class cofmsg_error_meter_mod_meter_exists : public cofmsg_error {
public:
	cofmsg_error_meter_mod_meter_exists(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_METER_EXISTS, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_meter_exists() {};
};



class cofmsg_error_meter_mod_invalid_meter : public cofmsg_error {
public:
	cofmsg_error_meter_mod_invalid_meter(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_INVALID_METER, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_invalid_meter() {};
};



class cofmsg_error_meter_mod_unknown_meter : public cofmsg_error {
public:
	cofmsg_error_meter_mod_unknown_meter(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_UNKNOWN_METER, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_unknown_meter() {};
};



class cofmsg_error_meter_mod_bad_command : public cofmsg_error {
public:
	cofmsg_error_meter_mod_bad_command(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_BAD_COMMAND, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_bad_command() {};
};



class cofmsg_error_meter_mod_bad_flags : public cofmsg_error {
public:
	cofmsg_error_meter_mod_bad_flags(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_BAD_FLAGS, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_bad_flags() {};
};



class cofmsg_error_meter_mod_bad_rate : public cofmsg_error {
public:
	cofmsg_error_meter_mod_bad_rate(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_BAD_RATE, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_bad_rate() {};
};



class cofmsg_error_meter_mod_bad_burst : public cofmsg_error {
public:
	cofmsg_error_meter_mod_bad_burst(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_BAD_BURST, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_bad_burst() {};
};



class cofmsg_error_meter_mod_bad_band : public cofmsg_error {
public:
	cofmsg_error_meter_mod_bad_band(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_BAD_BAND, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_bad_band() {};
};



class cofmsg_error_meter_mod_bad_band_value : public cofmsg_error {
public:
	cofmsg_error_meter_mod_bad_band_value(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_BAD_BAND_VALUE, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_bad_band_value() {};
};



class cofmsg_error_meter_mod_out_of_meters : public cofmsg_error {
public:
	cofmsg_error_meter_mod_out_of_meters(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_OUT_OF_METERS, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_out_of_meters() {};
};



class cofmsg_error_meter_mod_out_of_bands : public cofmsg_error {
public:
	cofmsg_error_meter_mod_out_of_bands(
				uint8_t ofp_version,
				uint32_t xid,
				uint8_t* data = 0,
				size_t datalen = 0) :
					cofmsg_error(ofp_version, xid, OFPET_METER_MOD_FAILED, OFPMMFC_OUT_OF_BANDS, data, datalen) {
		if (ofp_version < rofl::openflow13::OFP_VERSION)
			throw eBadVersion();
	};
	virtual ~cofmsg_error_meter_mod_out_of_bands() {};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_ERROR_H_ */
