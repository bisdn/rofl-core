/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fcapwapframe.h"

using namespace rofl;

#define GET_CAPWAP_HEADER_FIELD(hdr, mask, shift)		\
	((be32toh(*(uint32_t *)(hdr) & (mask))) >> (shift))

#define SET_CAPWAP_HEADER_FIELD(hdr, value, mask, shift)		\
	*(uint32_t *)(hdr) = (*(uint32_t *)(hdr) & ~(mask)) | htobe32((value) << (shift))

fcapwapframe::fcapwapframe(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		capwapdata(0),
		capwapdatalen(0)
{
	initialize();
}



fcapwapframe::~fcapwapframe()
{

}



void
fcapwapframe::initialize()
{
	capwapdata = NULL;
	capwapdatalen = 0;
	capwap_hdr = soframe();

	if (framelen() < CAPWAP_HEADER_LEN)
		return;

	if (get_capwap_hlen() > framelen())
		return;

	if (get_capwap_hlen() < CAPWAP_HEADER_LEN)
		set_capwap_hlen(CAPWAP_HEADER_LEN);

	capwapdata = ((uint8_t*)soframe()) + get_capwap_hlen();
	capwapdatalen = framelen() - get_capwap_hlen();
}


bool
fcapwapframe::complete()
{
	return true;
}


size_t
fcapwapframe::need_bytes()
{
	return 0; // just to make gcc happy
}

#if 0
void
fcapwapframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct capwap_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(capwapdata, data, datalen);
	capwapdatalen = datalen;
}
#endif


uint8_t*
fcapwapframe::payload() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!capwapdata)
		throw eFrameNoPayload();
	return capwapdata;
}


size_t
fcapwapframe::payloadlen() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!capwapdatalen)
		throw eFrameNoPayload();
	return capwapdatalen;
}


void
fcapwapframe::validate(uint16_t total_len) throw (eFrameInvalidSyntax)
{
	if (!complete())
		throw eFrameInvalidSyntax();
}


const char*
fcapwapframe::c_str()
{
	cvastring vas;
#if 0
	uint8_t ihlvers;        // IP header length in 32bit words,
							// TODO: check for options and padding
	uint8_t tos;
	uint16_t length;
	uint16_t ident;
	uint16_t offset_flags;
	uint8_t ttl;
	uint8_t proto;
	uint16_t checksum;
	uint32_t src;
	uint32_t dst;

	info.assign(vas("[fcapwapframe(%p) dst:%s src:%s length:%d vers:%d dscp:%d ecn:%d ihl:%d proto:%d ttl:%d ]",
			this,
			get_capwap_dst().addr_c_str(),
			get_capwap_src().addr_c_str(),
			be16toh(capwap_hdr->length),
			get_capwap_version(),
			get_capwap_dscp(),
			get_capwap_ecn(),
			get_capwap_ihl(),
			get_capwap_proto(),
			get_capwap_ttl()
			));
#endif
	info.assign("not implementd");

	return info.c_str();
}


void
fcapwapframe::set_capwap_rid(uint8_t rid)
{
	SET_CAPWAP_HEADER_FIELD(capwap_hdr, rid, CAPWAP_RID_MASK, CAPWAP_RID_SHIFT);
}


uint8_t
fcapwapframe::get_capwap_rid()
{
	return GET_CAPWAP_HEADER_FIELD(capwap_hdr, CAPWAP_RID_MASK, CAPWAP_RID_SHIFT);
}

void
fcapwapframe::set_capwap_flags(uint16_t flags)
{
	SET_CAPWAP_HEADER_FIELD(capwap_hdr, flags, CAPWAP_F_MASK, 0);
}

uint16_t
fcapwapframe::get_capwap_flags()
{
	return GET_CAPWAP_HEADER_FIELD(capwap_hdr, CAPWAP_F_MASK, 0);
}

void
fcapwapframe::set_capwap_wbid(uint8_t wbid)
{
	SET_CAPWAP_HEADER_FIELD(capwap_hdr, wbid, CAPWAP_WBID_MASK, CAPWAP_WBID_SHIFT);
}


uint8_t
fcapwapframe::get_capwap_wbid()
{
	return GET_CAPWAP_HEADER_FIELD(capwap_hdr, CAPWAP_WBID_MASK, CAPWAP_WBID_SHIFT);
}

uint16_t
fcapwapframe::get_capwap_length()
{
    return capwapdatalen;
}


void
fcapwapframe::set_capwap_hlen(uint8_t hlen)
{
	SET_CAPWAP_HEADER_FIELD(capwap_hdr, hlen / 4, CAPWAP_HLEN_MASK, CAPWAP_HLEN_SHIFT);
}

uint8_t
fcapwapframe::get_capwap_hlen()
{
	return GET_CAPWAP_HEADER_FIELD(capwap_hdr, CAPWAP_HLEN_MASK, CAPWAP_HLEN_SHIFT) * 4;
}
