/*
 * cofmsg_port_mod.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PORT_MOD_H_
#define COFMSG_PORT_MOD_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_port_mod :
	public cofmsg
{
private:

	union {
		uint8_t*					ofhu_port_mod;
		struct openflow10::ofp_port_mod*		ofhu10_port_mod;
		struct openflow12::ofp_port_mod*		ofhu12_port_mod;
		struct openflow13::ofp_port_mod*		ofhu13_port_mod;
	} ofhu;

#define ofh_port_mod   ofhu.ofhu_port_mod
#define ofh10_port_mod ofhu.ofhu10_port_mod
#define ofh12_port_mod ofhu.ofhu12_port_mod
#define ofh13_port_mod ofhu.ofhu13_port_mod

public:


	/** constructor
	 *
	 */
	cofmsg_port_mod(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t port_no = 0,
			cmacaddr const& hwaddr = cmacaddr("00:00:00:00:00:00"),
			uint32_t config = 0,
			uint32_t mask = 0,
			uint32_t advertise = 0);


	/**
	 *
	 */
	cofmsg_port_mod(
			cofmsg_port_mod const& port_mod);


	/**
	 *
	 */
	cofmsg_port_mod&
	operator= (
			cofmsg_port_mod const& port_mod);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_port_mod();


	/**
	 *
	 */
	cofmsg_port_mod(cmemory *memarea);


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
	uint32_t
	get_port_no() const;

	/**
	 *
	 */
	void
	set_port_no(uint32_t port_no);

	/**
	 *
	 */
	cmacaddr
	get_hwaddr() const;

	/**
	 *
	 */
	void
	set_hwaddr(cmacaddr const& hwaddr);

	/**
	 *
	 */
	uint32_t
	get_config() const;

	/**
	 *
	 */
	void
	set_config(uint32_t config);

	/**
	 *
	 */
	uint32_t
	get_mask() const;

	/**
	 *
	 */
	void
	set_mask(uint32_t mask);

	/**
	 *
	 */
	uint32_t
	get_advertise() const;

	/**
	 *
	 */
	void
	set_advertise(uint32_t advertise);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_port_mod const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_port_mod >" << std::endl;
		os << indent(2) << "<port-no:" 		<< (int)msg.get_port_no() 		<< " >" << std::endl;
		os << indent(2) << "<hw-addr:" 		<< msg.get_hwaddr() 			<< " >" << std::endl;
		os << indent(2) << "<config:" 		<< std::hex << (int)msg.get_config() 	<< std::dec << " >" << std::endl;
		os << indent(2) << "<mask:" 		<< std::hex << (int)msg.get_mask() 		<< std::dec << " >" << std::endl;
		os << indent(2) << "<advertise:" 	<< std::hex << (int)msg.get_advertise() << std::dec << " >" << std::endl;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_PORT_MOD_H_ */
