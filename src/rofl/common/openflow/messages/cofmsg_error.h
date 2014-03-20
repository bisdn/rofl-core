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
	virtual uint8_t*
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

} // end of namespace rofl

#endif /* COFMSG_ERROR_H_ */
