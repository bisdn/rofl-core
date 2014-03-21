/*
 * cofmsg_experimenter.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_EXPERIMENTER_H_
#define COFMSG_EXPERIMENTER_H_ 1

#include "cofmsg.h"

namespace rofl
{

/**
 *
 */
class cofmsg_experimenter :
	public cofmsg
{
private:

	cmemory				body;	// for experimental statistics messages

	union {
		uint8_t*										ofhu_experimenter;
		struct openflow10::ofp_vendor_header*			ofhu10_vendor;
		struct openflow12::ofp_experimenter_header*		ofhu12_experimenter;
		struct openflow13::ofp_experimenter_header*		ofhu13_experimenter;
	} ofhu;

#define ofh_experimenter   		ofhu.ofhu_experimenter
#define ofh10_vendor			ofhu.ofhu10_vendor
#define ofh12_experimenter 		ofhu.ofhu12_experimenter
#define ofh13_experimenter 		ofhu.ofhu13_experimenter


public:


	/** constructor
	 *
	 */
	cofmsg_experimenter(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t experimenter_type = 0,
			uint32_t experimenter_flags = 0,
			uint8_t *data = (uint8_t*)0,
			size_t datalen = 0);


	/**
	 *
	 */
	cofmsg_experimenter(
			cofmsg_experimenter const& experimenter);


	/**
	 *
	 */
	cofmsg_experimenter&
	operator= (
			cofmsg_experimenter const& experimenter);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_experimenter();


	/**
	 *
	 */
	cofmsg_experimenter(cmemory *memarea);


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
	get_experimenter_id() const;

	/**
	 *
	 */
	void
	set_experimenter_id(uint32_t exp_id);

	/**
	 *
	 */
	uint32_t
	get_experimenter_type() const;

	/**
	 *
	 */
	void
	set_experimenter_type(uint32_t exp_type);

	/**
	 *
	 */
	cmemory&
	get_body();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_experimenter const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_experimenter >" << std::endl;
		os << indent(4) << "<exp-id:" << (int)msg.get_experimenter_id() << " >" << std::endl;
		switch (msg.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << indent(4) << "<exp-type:" << (int)msg.get_experimenter_type() << " >" << std::endl;
		} break;
		default: {
			// do nothing
		};
		}
		indent i(4);
		os << msg.body;
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_EXPERIMENTER_H_ */
