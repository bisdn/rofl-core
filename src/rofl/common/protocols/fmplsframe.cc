/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fmplsframe.h"

using namespace rofl;

fmplsframe::fmplsframe(
		uint8_t *data,
		size_t datalen) :
	fframe(data, datalen),
	mpls_hdr(0)
{
	initialize();
}



fmplsframe::~fmplsframe()
{
	// do _NOT_ delete or deallocate (data,datalen) here!
}



void
fmplsframe::reset(
		uint8_t *data, size_t datalen)
{
	fframe::reset(data, datalen);
	initialize();
}



void
fmplsframe::initialize()
{
	mpls_hdr = (struct mpls_hdr_t*)soframe();
}



bool
fmplsframe::complete()
{
	if (framelen() < sizeof(struct mpls_hdr_t))
		return false;

	initialize();

	return true;
}


size_t
fmplsframe::need_bytes()
{
	if (framelen() < sizeof(struct mpls_hdr_t))
		return (sizeof(struct mpls_hdr_t) - framelen());

	initialize();

	// TODO: use length field from Ethernet 802.3 header, for now just Ethernet II

	return 0;
}


void
fmplsframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct mpls_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(soframe() + sizeof(struct mpls_hdr_t), data, datalen);
}


uint8_t*
fmplsframe::payload() const throw (eFrameNoPayload)
{
	if (framelen() <= sizeof(struct mpls_hdr_t))
		throw eFrameNoPayload();

	//initialize(); // commented out 2012-12-13

	return (soframe() + sizeof(struct mpls_hdr_t));
}


size_t
fmplsframe::payloadlen() const throw (eFrameNoPayload)
{
	if (framelen() <= sizeof(struct mpls_hdr_t))
		throw eFrameNoPayload();

	return (framelen() - sizeof(struct mpls_hdr_t));
}


void
fmplsframe::validate(uint16_t total_len) throw (eMplsFrameTooShort)
{
	//initialize(); // commented out 2012-12-13

	if (framelen() < (sizeof(struct mpls_hdr_t)))
	{
		throw eMplsFrameTooShort();
	}

	// TODO: check on minimum length of 64 bytes?
}


const char*
fmplsframe::c_str()
{
	cvastring vas;

	info.assign(vas("[fmplsframe(%p) label:0x%x tc:%d s:%d ttl:%d  fframe: %s]",
			this,
			get_mpls_label(),
			get_mpls_tc(),
			get_mpls_bos(),
			get_mpls_ttl(),
			fframe::c_str()));

	return info.c_str();
}


void
fmplsframe::set_mpls_label(uint32_t label)
{
	mpls_hdr->label[0] =  (label & 0x000ff000) >> 12;
	mpls_hdr->label[1] =  (label & 0x00000ff0) >>  4;
	mpls_hdr->label[2] = ((label & 0x0000000f) <<  4) | (mpls_hdr->label[2] & 0x0f);
}


uint32_t
fmplsframe::get_mpls_label()
{
	uint32_t label =
			(mpls_hdr->label[0] << 12) +
			(mpls_hdr->label[1] <<  4) +
			((mpls_hdr->label[2] & 0xf0) >>  4);
	return label;
}


void
fmplsframe::set_mpls_tc(uint8_t tc)
{
	mpls_hdr->label[2] = ((tc & 0x07) << 1) + (mpls_hdr->label[2] & 0xf1);
}


uint8_t
fmplsframe::get_mpls_tc()
{
	return ((mpls_hdr->label[2] & 0x0e) >> 1);
}


void
fmplsframe::dec_mpls_ttl()
{
	mpls_hdr->ttl--;
}


void
fmplsframe::set_mpls_ttl(uint8_t ttl)
{
	mpls_hdr->ttl = ttl;
}


uint8_t
fmplsframe::get_mpls_ttl()
{
	return mpls_hdr->ttl;
}


void
fmplsframe::set_mpls_bos(bool flag)
{
	if (flag)
		mpls_hdr->label[2] |= 0x01;
	else
		mpls_hdr->label[2] &= ~0xfe;
}


bool
fmplsframe::get_mpls_bos()
{
	return ((mpls_hdr->label[2] & 0x01) ? true : false);
}




