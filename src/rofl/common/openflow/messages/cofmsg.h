/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFMSG_H
#define COFMSG_H 1

#include <set>
#include <map>
#include <string>
#include <vector>
#include <strings.h>
#include <arpa/inet.h>
#include <endian.h>
#ifndef htobe16
	#include "../../endian_conversion.h"
#endif

#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/fframe.h"
#include "rofl/common/cpacket.h"

#include "rofl/common/openflow/openflow.h"
#if 0
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofbuckets.h"
#include "rofl/common/openflow/cofports.h"
#include "rofl/common/openflow/cofport.h"
#include "rofl/common/openflow/cofdescstats.h"
#include "rofl/common/openflow/cofflowstats.h"
#include "rofl/common/openflow/cofaggrstats.h"
#include "rofl/common/openflow/coftablestats.h"
#include "rofl/common/openflow/cofportstats.h"
#include "rofl/common/openflow/cofqueuestats.h"
#include "rofl/common/openflow/cofgroupstats.h"
#include "rofl/common/openflow/cofgroupdescstats.h"
#endif

// forward declarations
class cofbase;

namespace rofl {
namespace openflow {

class eOFpacketBase 		: public RoflException {};
class eOFpacketInval 		: public eOFpacketBase {};
class eOFpacketNoData 		: public eOFpacketBase {};
class eOFpacketHeaderInval 	: public eOFpacketBase {}; // invalid header


/** cofpacket
 * - contains a packet from OpenFlow protocol
 * - provides container for validating an OpenFlow packet
 * - stores during validation actions, matches, instructions, buckets, ofports
 *   in appropriate containers
 * - stores pointer to cofbase entity the packet was received from
 *   (either cfwdelem or cofrpc)
 */
class cofmsg
{
public: // static

	static std::set<cofmsg*> cofpacket_list; //< list of allocated cofpacket instances

private: // static

	static std::string pinfo; //< information string for cofpacket::cofpacket_list

public:

	/** return information string about allocated cofpacket instances
	 */
	static
	const char*
	packet_info(uint8_t ofp_version);

    /** return description for ofp_type
     */
    static const char*
    type2desc(uint8_t ofp_version, uint8_t type);

	typedef struct {
		uint8_t type;
		char desc[64];
	} typedesc_t;


protected: // data structures

	cmemory 					*memarea;			// OpenFlow packet received from socket

	union {
		uint8_t*							ofhu_generic;
		struct rofl::openflow::ofp_header*	ofhu_header;
	} ofh_ofhu;

#define ofh_generic ofh_ofhu.ofhu_generic
#define ofh_header 	ofh_ofhu.ofhu_header

public:

	/**
	 *
	 */
	cofmsg(
			uint8_t ofp_version, uint32_t xid, uint8_t type);

	/** constructor
	 *
	 */
	cofmsg(
			size_t size = sizeof(struct openflow::ofp_header));


	/**
	 *
	 */
	cofmsg(
			cmemory *memarea);


	/** copy constructor
	 *
	 */
	cofmsg(
			cofmsg const& p);


	/** destructor
	 *
	 */
	virtual
	~cofmsg();


	/** assignment operator
	 *
	 */
	cofmsg&
	operator=(
			cofmsg const& p);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/** pack OFpacket content to specified buffer
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/** unpack OFpacket content from specified buffer
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();


	/**
	 */
	virtual uint8_t*
	resize(size_t len);


	/** start of frame
	 *
	 */
	uint8_t*
	soframe() const { return memarea->somem(); };


	/** frame length
	 *
	 */
	size_t
	framelen() const { return memarea->memlen(); };


	/** start of frame
	 *
	 */
	uint8_t*
	sobody() const { return (soframe() + sizeof(struct openflow::ofp_header)); };


	/** frame length
	 *
	 */
	size_t
	bodylen() const { return (framelen() - sizeof(struct openflow::ofp_header)); };


public:




	/**
	 *
	 */
	uint8_t
	get_version() const;


	/**
	 *
	 */
	virtual void
	set_version(uint8_t ofp_version);


	/**
	 *
	 */
	uint16_t
	get_length() const;


	/**
	 *
	 */
	void
	set_length(uint16_t len);


	/**
	 *
	 */
	uint8_t
	get_type() const;


	/**
	 *
	 */
	void
	set_type(uint8_t type);


	/** returns xid field in host byte order from header
	 *
	 */
	uint32_t
	get_xid() const;


	/** sets xid field in header
	 *
	 */
	void
	set_xid(uint32_t xid);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg const& msg) {
		os << indent(0) << "<cofmsg ";
			os << "version: " 	<< (int)msg.get_version() 			<< " ";
			os << "type: " 		<< std::dec << (int)msg.get_type() 	<< " ";
			os << "length: " 	<< (int)msg.get_length() 			<< " ";
			os << "xid: 0x" 	<< std::hex << (unsigned int)msg.get_xid() << std::dec 	<< " ";
		os << ">" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "OFP:{";
		switch (get_version()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			ss << "version: unknown, ";
		} break;
		case rofl::openflow10::OFP_VERSION: {
			ss << "version: 1.0, ";
		} break;
		case rofl::openflow12::OFP_VERSION: {
			ss << "version: 1.2, ";
		} break;
		case rofl::openflow13::OFP_VERSION: {
			ss << "version: 1.3, ";
		} break;
		}
		ss << "type: " << (int)get_type() << ", ";
		ss << "len: " << (int)get_length() << ", ";
		ss << "xid: 0x" << std::hex << (unsigned int)get_xid() << std::dec;
		ss << "} ";
		return ss.str();
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
