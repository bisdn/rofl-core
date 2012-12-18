
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FMPLSFRAME_H
#define FMPLSFRAME_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "../openflow/openflow12.h"
#include <endian.h>

#ifndef htobe16
#include "../endian_conversion.h"
#endif


#ifdef __cplusplus
}
#endif

#include "../fframe.h"
#include "../cmemory.h"
#include "../cvastring.h"

namespace rofl
{

class eMplsFrameBase 			: public eFrameBase {};
class eMplsFrameInvalidSyntax 	: public eMplsFrameBase, public eFrameInvalidSyntax {}; // invalid syntax
class eMplsFrameTooShort		: public eMplsFrameInvalidSyntax {};

/** MPLS frame
 *
 *
 */
class fmplsframe : public fframe {
public: // static

	/* Ethernet constants and definitions */

	// VLAN ethernet types
	enum mpls_ether_t {
		MPLS_ETHER = 0x8847,
		MPLS_ETHER_UPSTREAM = 0x8848,
	};

	// MPLS header
	struct mpls_hdr_t {
		uint8_t label[3];
		uint8_t ttl;
	} __attribute__((packed));

public: // methods


	/** reset this frame to a new buffer pointing to at (data, datalen)
	 *
	 */
	virtual void
	reset(
			uint8_t *data, size_t datalen);


	/** constructor
	 *
	 */
	fmplsframe(
			uint8_t* data,
			size_t datalen);


	/** destructor
	 *
	 */
	virtual
	~fmplsframe();


public: // overloaded from fframe

	/** returns boolean value indicating completeness of the packet
	 */
	virtual bool
	complete();

	/** returns the number of bytes this packet expects from the socket next
	 */
	virtual size_t
	need_bytes();

	/** validate (frame structure)
	 *
	 */
	virtual void
	validate(uint16_t total_len = 0) throw (eMplsFrameTooShort);

	/** initialize (set eth_hdr, pppoe_hdr)
	 *
	 */
	virtual void
	initialize();

	/** insert payload
	 *
	 */
	virtual void
	payload_insert(
			uint8_t *data, size_t datalen) throw (eFrameOutOfRange);

	/** get payload
	 *
	 */
	virtual uint8_t*
	payload() const throw (eFrameNoPayload);

	/** get payload length
	 *
	 */
	virtual size_t
	payloadlen() const throw (eFrameNoPayload);

	/** dump info
	 *
	 */
	virtual const char*
	c_str();


public:

	/**
	 */
	void
	set_mpls_label(uint32_t label);

	/**
	 */
	uint32_t
	get_mpls_label();

	/**
	 */
	void
	set_mpls_tc(uint8_t tc);

	/**
	 */
	uint8_t
	get_mpls_tc();

	/**
	 */
	void
	dec_mpls_ttl();

	/**
	 */
	void
	set_mpls_ttl(uint8_t ttl);

	/**
	 */
	uint8_t
	get_mpls_ttl();

	/** set bottom of stack bit
	 */
	void
	set_mpls_bos(bool flag = true);

	/**
	 */
	bool
	get_mpls_bos();


public: // data structures

	// pointer to ethernet header
	struct mpls_hdr_t *mpls_hdr;

private: // methods

private: // data structures

	std::string info;
};


#if 0
class cmplsheader : public cmemory {
public:
	cmplsheader(
			uint32_t label = 0,
			uint8_t  tc = 0,
			uint8_t  s = 0,
			uint8_t  ttl = 0) :
		cmemory(sizeof(struct fmplsframe::mpls_hdr_t))
	{
		mpls_hdr = (struct fmplsframe::mpls_hdr_t*)somem();

		mpls_hdr->label = label;
		mpls_hdr->tc = tc;
		mpls_hdr->s = s;
		mpls_hdr->ttl = ttl;
	}
	virtual
	~cmplsheader() {};
	cmplsheader& operator= (cmplsheader const& m)
	{
		if (this == &m)
			return *this;
		cmemory::operator= (m);
		mpls_hdr = (struct fmplsframe::mpls_hdr_t*)somem();
		return *this;
	};
	struct fmplsframe::mpls_hdr_t *mpls_hdr;

};
#endif

}; // end of namespace

#endif
