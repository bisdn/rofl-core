/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FFRAME_H
#define FFRAME_H 1

#include <bitset>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include <string>

#include "cerror.h"
#include "cmemory.h"
#include "cvastring.h"
#include "rofl/platform/unix/csyslog.h"

class eFrameBase : public cerror {}; // error base class fframe
class eFrameNoPayload : public eFrameBase {}; // no payload in frame
class eFrameOutOfRange : public eFrameBase {}; // too many data to copy
class eFrameInvalidSyntax : public eFrameBase {}; // invalid syntax
class eFrameInval : public eFrameBase {}; // invalid parameter


/** base class for all framing related classes
 * these classes are used for mixin'g in an existing memory area
 * parsing functionality for specific packet formats, e.g. OpenFlow,
 * Ethernet, VLAN, IPv4, etc.
 */
class fframe :
	public csyslog
{
/*
 * data structure
 */
private:

		enum fframe_flag_t {
			FFRAME_FLAG_MEM = (1 << 0), 		// fframe is self-containing memory area for frame
		};

		cmemory 				 mem; 			// frame container, if none was specified in constructor => this is used for creating new frames from scratch
		uint8_t 				*data; 			// data area
		size_t 					 datalen; 		// data area length
		uint16_t 				 total_len; 	// real length of packet
		std::string 			 info; 			// info string
		std::bitset<32> 		 flags;


public:

#define DEFAULT_FFRAME_SIZE 128


		fframe 					*next;			// next fframe in a list or 0
		fframe					*prev;			// previous fframe in a list or 0





/*
 * methods
 */
public:

	/** constructor for parsing existing frames
	 *
	 */
	fframe(
			uint8_t* _data = NULL,
			size_t _datalen = 0,
			uint16_t total_len = 0);

	/** constructor for creating new frames
	 *
	 */
	fframe(
			size_t len = DEFAULT_FFRAME_SIZE);

	/** destructor
	 *
	 */
	virtual
	~fframe();

	/** reset this frame to a new buffer pointing to at (data, datalen)
	 *
	 */
	virtual void
	reset(uint8_t *_data, size_t _datalen, uint16_t _total_len);

	/** copy constructor
	 *
	 */
	fframe(const fframe& frame);

	/** assignment operator
	 *
	 */
	fframe& operator= (const fframe& frame);

	/** index operator
	 *
	 */
	virtual uint8_t&
	operator[] (
			size_t index) throw (eFrameOutOfRange);

	/** returns boolean value indicating completeness of the packet
	 */
	virtual bool
	complete() { return true; };

	/** returns the number of bytes this packet expects from the socket next
	 */
	virtual size_t
	need_bytes() { return 0; };

	/** validate (frame structure)
	 *
	 */
	virtual void
	validate(uint16_t total_len = 0) throw (eFrameInvalidSyntax) {};

	/** initialize (set eth_hdr, pppoe_hdr)
	 *
	 */
	virtual void
	initialize() {};

	/** insert payload
	 *
	 */
	virtual void
	payload_insert(
			uint8_t *data, size_t datalen) throw (eFrameOutOfRange) {};


	/** get payload
	 *
	 */
	virtual uint8_t*
	payload() const throw (eFrameNoPayload)
	{
		return soframe();
	};


	/** get payload length
	 *
	 */
	virtual size_t
	payloadlen() const throw (eFrameNoPayload)
	{
		return framelen();
	};


	/** get total payload length
	 *
	 */
	virtual uint16_t
	totalpayloadlen() const throw (eFrameNoPayload)
	{
		return total_len;
	};


	/** dump info
	 *
	 */
	virtual const char*
	c_str();


	/** get start of data area
	 *
	 */
	virtual uint8_t*
	soframe() const
	{
		return data;
	};


	/** get length of data area
	 *
	 */
	virtual size_t
	framelen() const
	{
		return datalen;
	};


	/**
	 *
	 */
	virtual uint16_t
	totallen() const
	{
		return total_len;
	};


	/** return bool whether frame is empty or not
	 *
	 */
	bool
	empty() const
	{
		return (0 == datalen);
	};
};



#endif
