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
		uint8_t*							ofhu_experimenter;
		struct ofp10_vendor_header*			ofhu10_vendor;
		struct ofp12_experimenter_header*	ofhu12_experimenter;
		// TODO: OF1.3
	} ofhu;

#define ofh_experimenter   		ofhu.ofhu_experimenter
#define ofh10_vendor			ofhu.ofhu10_vendor
#define ofh12_experimenter 		ofhu.ofhu12_experimenter
// TODO: OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_experimenter(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t experimenter_type = 0,
			uint32_t experimenter_flags = 0,
			uint8_t *data = (uint8_t*)data,
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
};

} // end of namespace rofl

#endif /* COFMSG_EXPERIMENTER_H_ */
