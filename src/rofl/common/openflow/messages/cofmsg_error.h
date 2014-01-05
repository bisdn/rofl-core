/*
 * cofmsg_error.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_ERROR_H_
#define COFMSG_ERROR_H_ 1

#include "cofmsg.h"
#include "rofl/common/openflow/openflow_common.h"
#include "rofl/common/openflow/openflow.h"

using namespace rofl::openflow;

namespace rofl
{

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
	virtual void
	resize(size_t len);


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length();


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
		os << indent(4) << "<err-type:" << (int)msg.get_err_type() << " >" << std::endl;
		os << indent(4) << "<err-code:" << (int)msg.get_err_code() << " >" << std::endl;
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
		if (ofp_version < rofl::openflow13::OFP_VERSION)
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
		if (ofp_version < rofl::openflow13::OFP_VERSION)
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
		if (ofp_version < rofl::openflow13::OFP_VERSION)
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
		if (ofp_version < rofl::openflow13::OFP_VERSION)
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
		if (ofp_version < rofl::openflow12::OFP_VERSION)
			throw eBadVersion();
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




#if 0
void
crofbase::send_error_switch_config_failed_bad_flags(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_SWITCH_CONFIG_FAILED; code = openflow12::OFPSCFC_BAD_FLAGS;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_SWITCH_CONFIG_FAILED; code = openflow13::OFPSCFC_BAD_FLAGS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send SwitchConfigFailed/BadFlags for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_switch_config_failed_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_SWITCH_CONFIG_FAILED; code = openflow12::OFPSCFC_BAD_LEN;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_SWITCH_CONFIG_FAILED; code = openflow13::OFPSCFC_BAD_LEN;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send SwitchConfigFailed/BadLen for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_switch_config_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_SWITCH_CONFIG_FAILED; code = openflow13::OFPSCFC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send SwitchConfigFailed/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_unknown(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_UNKNOWN;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_UNKNOWN;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/Unknown for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_table_full(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_ALL_TABLES_FULL;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_TABLE_FULL;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_TABLE_FULL;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/TableFull for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_bad_table_id(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_BAD_TABLE_ID;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_BAD_TABLE_ID;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/BadTableId for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_overlap(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_OVERLAP;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_OVERLAP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_OVERLAP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/Overlap for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_EPERM;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_EPERM;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_bad_timeout(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_BAD_EMERG_TIMEOUT;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_BAD_TIMEOUT;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_BAD_TIMEOUT;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/BadTimeout for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_bad_command(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_BAD_COMMAND;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_BAD_COMMAND;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_BAD_COMMAND;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/BadCommand for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_bad_flags(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_BAD_FLAGS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/BadFlags for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_group_exists(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_GROUP_EXISTS;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_GROUP_EXISTS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/GroupExists for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_inval_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_INVALID_GROUP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_INVALID_GROUP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/InvalGroup for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_weight_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_WEIGHT_UNSUPPORTED;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_WEIGHT_UNSUPPORTED;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/WeightUnsupported for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_out_of_groups(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_OUT_OF_GROUPS;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_OUT_OF_GROUPS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/OutOfGroups for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_out_of_buckets(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_OUT_OF_BUCKETS;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_OUT_OF_BUCKETS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/OutOfBuckets for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_chaining_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_CHAINING_UNSUPPORTED;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_CHAINING_UNSUPPORTED;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/ChainingUnsupported for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_watch_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_WATCH_UNSUPPORTED;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_WATCH_UNSUPPORTED;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/WatchUnsupported for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_loop(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_LOOP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_LOOP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/Loop for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_unknown_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_UNKNOWN_GROUP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_UNKNOWN_GROUP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/UnknownGroup for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_chained_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_CHAINED_GROUP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_CHAINED_GROUP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/ChainedGroup for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_bad_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_BAD_TYPE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_BAD_TYPE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/BadType for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_bad_command(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_BAD_COMMAND;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_BAD_COMMAND;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/BadCommand for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_bad_bucket(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_BAD_BUCKET;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_BAD_BUCKET;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/BadBucket for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_bad_watch(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_BAD_WATCH;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_BAD_WATCH;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/BadWatch for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_EPERM;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_port_mod_failed_bad_port(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_PORT_MOD_FAILED; code = openflow12::OFPPMFC_BAD_PORT;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_PORT_MOD_FAILED; code = openflow13::OFPPMFC_BAD_PORT;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send PortModFailed/BadPort for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_port_mod_failed_bad_hw_addr(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_PORT_MOD_FAILED; code = openflow12::OFPPMFC_BAD_HW_ADDR;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_PORT_MOD_FAILED; code = openflow13::OFPPMFC_BAD_HW_ADDR;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send PortModFailed/BadHwAddr for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_port_mod_failed_bad_config(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_PORT_MOD_FAILED; code = openflow12::OFPPMFC_BAD_CONFIG;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_PORT_MOD_FAILED; code = openflow13::OFPPMFC_BAD_CONFIG;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send PortModFailed/BadConfig for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_port_mod_failed_bad_advertise(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_PORT_MOD_FAILED; code = openflow12::OFPPMFC_BAD_ADVERTISE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_PORT_MOD_FAILED; code = openflow13::OFPPMFC_BAD_ADVERTISE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send PortModFailed/BadAdvertise for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_port_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_PORT_MOD_FAILED; code = openflow13::OFPPMFC_BAD_ADVERTISE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send PortModFailed/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_table_mod_failed_bad_table(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_TABLE_MOD_FAILED; code = openflow12::OFPTMFC_BAD_TABLE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_TABLE_MOD_FAILED; code = openflow13::OFPTMFC_BAD_TABLE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send TableModFailed/BadTable for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_table_mod_failed_bad_config(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_TABLE_MOD_FAILED; code = openflow12::OFPTMFC_BAD_CONFIG;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_TABLE_MOD_FAILED; code = openflow13::OFPTMFC_BAD_CONFIG;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send TableModFailed/BadConfig for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_table_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_TABLE_MOD_FAILED; code = openflow13::OFPTMFC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send TableModFailed/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_role_request_failed_stale(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_ROLE_REQUEST_FAILED; code = openflow12::OFPRRFC_STALE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_ROLE_REQUEST_FAILED; code = openflow13::OFPRRFC_STALE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send RoleRequestFailed/Stale for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_role_request_failed_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_ROLE_REQUEST_FAILED; code = openflow12::OFPRRFC_UNSUP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_ROLE_REQUEST_FAILED; code = openflow13::OFPRRFC_UNSUP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send RoleRequestFailed/Unsupported for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_role_request_failed_bad_role(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_ROLE_REQUEST_FAILED; code = openflow12::OFPRRFC_BAD_ROLE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_ROLE_REQUEST_FAILED; code = openflow13::OFPRRFC_BAD_ROLE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send RoleRequestFailed/BadRole for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}
#endif


} // end of namespace rofl

#endif /* COFMSG_ERROR_H_ */
