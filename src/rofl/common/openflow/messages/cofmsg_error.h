/*
 * cofmsg_error.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_ERROR_H_
#define COFMSG_ERROR_H_ 1

#include "cofmsg.h"
#include "rofl/common/openflow/openflow.h"

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
			uint8_t of_version = 0,
			uint32_t xid = 0,
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

} // end of namespace rofl

#endif /* COFMSG_ERROR_H_ */
