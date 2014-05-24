/*
 * cofmsg_meter.h
 *
 *  Created on: 24.05.2014
 *      Author: andi
 */

#ifndef COFMSG_METER_H
#define COFMSG_METER_H 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofmeterbands.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_meter :
	public cofmsg
{
public:


	/** constructor
	 *
	 */
	cofmsg_meter(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			const rofl::openflow::cofmeter_bands& mbs = rofl::openflow::cofmeter_bands());


	/**
	 *
	 */
	cofmsg_meter(
			const cofmsg_meter& meter);


	/**
	 *
	 */
	cofmsg_meter&
	operator= (
			const cofmsg_meter& meter);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_meter();


	/**
	 *
	 */
	cofmsg_meter(
			rofl::cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();

public:

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
	void
	set_command(uint16_t command) { this->command = command; };

	/**
	 *
	 */
	uint16_t
	get_command() const { return command; };

	/**
	 *
	 */
	void
	set_flags(uint16_t flags) { this->flags = flags; };

	/**
	 *
	 */
	uint16_t
	get_flags() const { return flags; };

	/**
	 *
	 */
	void
	set_meter_id(uint32_t meter_id) { this->meter_id = meter_id; };

	/**
	 *
	 */
	uint32_t
	get_meter_id() const { return meter_id; };

	/**
	 *
	 */
	rofl::openflow::cofmeter_bands&
	set_meter_bands() { return meter_bands; };

	/**
	 *
	 */
	const rofl::openflow::cofmeter_bands&
	get_meter_bands() const { return meter_bands; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_meter const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(0) << "<cofmsg_meter >" << std::endl;
		rofl::indent i(2);
		os << msg.meter_bands;
		return os;
	};

private:

	uint16_t							command;
	uint16_t							flags;
	uint32_t							meter_id;
	rofl::openflow::cofmeter_bands		meter_bands;
};


} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_METER_H_ */
