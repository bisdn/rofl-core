/*
 * cofmsg_group_mod.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_GROUP_MOD_H_
#define COFMSG_GROUP_MOD_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofbuckets.h"
#include "rofl/common/openflow/cofgroupmod.h"

namespace rofl
{

/**
 *
 */
class cofmsg_group_mod :
	public cofmsg
{
private:

	cofbuckets			buckets;

	union {
		uint8_t*					ofhu_group_mod;
		struct openflow12::ofp_group_mod*		ofhu12_group_mod;
		struct openflow13::ofp_group_mod*		ofhu13_group_mod;
	} ofhu;

#define ofh_group_mod   ofhu.ofhu_group_mod
#define ofh12_group_mod ofhu.ofhu12_group_mod
#define ofh13_group_mod ofhu.ofhu13_group_mod

public:


	/**
	 *
	 */
	cofmsg_group_mod(
			uint8_t of_version,
			uint32_t xid,
			cgroupentry const& ge);


	/** constructor
	 *
	 */
	cofmsg_group_mod(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t command = 0,
			uint8_t  group_type = 0,
			uint32_t group_id = 0,
			cofbuckets const& buckets = cofbuckets());


	/**
	 *
	 */
	cofmsg_group_mod(
			cofmsg_group_mod const& group_mod);


	/**
	 *
	 */
	cofmsg_group_mod&
	operator= (
			cofmsg_group_mod const& group_mod);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_group_mod();


	/**
	 *
	 */
	cofmsg_group_mod(cmemory *memarea);


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
	uint16_t
	get_command() const;

	/**
	 *
	 */
	void
	set_command(uint16_t command);

	/**
	 *
	 */
	uint8_t
	get_group_type() const;

	/**
	 *
	 */
	void
	set_group_type(uint8_t group_type);

	/**
	 *
	 */
	uint32_t
	get_group_id() const;

	/**
	 *
	 */
	void
	set_group_id(uint32_t group_id);

	/**
	 *
	 */
	cofbuckets&
	get_buckets();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_group_mod const& msg) {
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_group_mod >" << std::endl;
		switch (msg.get_command()) {
			case rofl::openflow12::OFPGC_ADD: {
				os << indent(4) << "<command: -ADD- >" << std::endl;
			} break;
			case rofl::openflow12::OFPGC_MODIFY: {
				os << indent(4) << "<command: -MODIFY- >" << std::endl;
			} break;
			case rofl::openflow12::OFPGC_DELETE: {
				os << indent(4) << "<command: -DELETE- >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<command: -UNKNOWN- >" << std::endl;
			};
			}
		os << indent(4) << "<group-type:" 	<< (int)msg.get_group_type() 	<< " >" << std::endl;
		os << indent(4) << "<group-id:" 	<< (int)msg.get_group_id() 		<< " >" << std::endl;
		os << indent(4) << msg.buckets;
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_GROUP_MOD_H_ */
