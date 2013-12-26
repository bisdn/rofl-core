/*
 * cofmsg_flow_removed.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FLOW_REMOVED_H_
#define COFMSG_FLOW_REMOVED_H_ 1

#include "cofmsg.h"
#include "rofl/common/openflow/cofmatch.h"

namespace rofl
{

/**
 *
 */
class cofmsg_flow_removed :
	public cofmsg
{
private:

	cofmatch			match;

	union {
		uint8_t*						ofhu_flow_removed;
		struct openflow10::ofp_flow_removed*		ofhu10_flow_removed;
		struct openflow12::ofp_flow_removed*		ofhu12_flow_removed;
		struct openflow13::ofp_flow_removed*		ofhu13_flow_removed;
	} ofhu;

#define ofh_flow_removed   ofhu.ofhu_flow_removed
#define ofh10_flow_removed ofhu.ofhu10_flow_removed
#define ofh12_flow_removed ofhu.ofhu12_flow_removed
#define ofh13_flow_removed ofhu.ofhu13_flow_removed

#define OFP10_FLOW_REMOVED_STATIC_HDR_LEN	88
#define OFP12_FLOW_REMOVED_STATIC_HDR_LEN	48	// length without struct openflow12::ofp_match
#define OFP13_FLOW_REMOVED_STATIC_HDR_LEN	48	// length without struct openflow13::ofp_match

public:


	/** constructor
	 *
	 */
	cofmsg_flow_removed(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint64_t cookie = 0,
			uint16_t priority = 0,
			uint8_t  reason = 0,
			uint8_t  table_id = 0,
			uint32_t duration_sec = 0,
			uint32_t duration_nsec = 0,
			uint16_t idle_timeout = 0,
			uint16_t hard_timeout = 0,
			uint64_t packet_count = 0,
			uint64_t byte_count = 0,
			cofmatch const& match = cofmatch());


	/**
	 *
	 */
	cofmsg_flow_removed(
			cofmsg_flow_removed const& flow_removed);


	/**
	 *
	 */
	cofmsg_flow_removed&
	operator= (
			cofmsg_flow_removed const& flow_removed);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_flow_removed();


	/**
	 *
	 */
	cofmsg_flow_removed(cmemory *memarea);


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
	uint64_t
	get_cookie() const;

	/**
	 *
	 */
	void
	set_cookie(uint64_t cookie);

	/**
	 *
	 */
	uint16_t
	get_priority() const;

	/**
	 *
	 */
	void
	set_priority(uint64_t priority);

	/**
	 *
	 */
	uint8_t
	get_reason() const;

	/**
	 *
	 */
	void
	set_reason(uint8_t reason);

	/**
	 *
	 */
	uint8_t
	get_table_id() const;

	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id);

	/**
	 *
	 */
	uint32_t
	get_duration_sec() const;

	/**
	 *
	 */
	void
	set_duration_sec(uint32_t duration_sec);

	/**
	 *
	 */
	uint32_t
	get_duration_nsec() const;

	/**
	 *
	 */
	void
	set_duration_nsec(uint32_t duration_nsec);

	/**
	 *
	 */
	uint16_t
	get_idle_timeout() const;

	/**
	 *
	 */
	void
	set_idle_timeout(uint16_t idle_timeout);

	/**
	 *
	 */
	uint16_t
	get_hard_timeout() const;

	/**
	 *
	 */
	void
	set_hard_timeout(uint16_t hard_timeout);

	/**
	 *
	 */
	uint64_t
	get_packet_count() const;

	/**
	 *
	 */
	void
	set_packet_count(uint64_t packet_count);

	/**
	 *
	 */
	uint64_t
	get_byte_count() const;

	/**
	 *
	 */
	void
	set_byte_count(uint64_t byte_count);

	/**
	 *
	 */
	cofmatch&
	get_match();


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_flow_removed const& msg) {
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_flow_removed >" << std::endl;
			os << indent(4) << "<cookie:0x" << std::hex << (int)msg.get_cookie() << std::dec << " >" << std::endl;
			os << indent(4) << "<priority:0x" << std::hex << (int)msg.get_priority() << std::dec << " >" << std::endl;
			os << indent(4) << "<reason:" << (int)msg.get_reason() << " >" << std::endl;
			os << indent(4) << "<table-id:" << (int)msg.get_table_id() << " >" << std::endl;
			os << indent(4) << "<duration-sec:" << (int)msg.get_duration_sec() << " >" << std::endl;
			os << indent(4) << "<duration-nsec:" << (int)msg.get_duration_nsec() << " >" << std::endl;
			os << indent(4) << "<idle-timeout:" << (int)msg.get_idle_timeout() << " >" << std::endl;
			os << indent(4) << "<hard-timeout:" << (int)msg.get_hard_timeout() << " >" << std::endl;
			os << indent(4) << "<packet-count:" << (int)msg.get_packet_count() << " >" << std::endl;
			os << indent(4) << "<byte-count:" << (int)msg.get_byte_count() << " >" << std::endl;
			os << indent(4) << "<matches: >" << std::endl;
			indent i(6);
			os << msg.match;
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_FLOW_REMOVED_H_ */
