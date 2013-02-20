/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ficmpv6frame.h"

using namespace rofl;

ficmpv6frame::ficmpv6frame(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		icmpv6_hdr(0),
		data(0),
		datalen(0)
{
	initialize();
}



ficmpv6frame::ficmpv6frame(
		size_t len) :
		fframe(len)
{
	initialize();
}



ficmpv6frame::~ficmpv6frame()
{

}



void
ficmpv6frame::initialize()
{
	icmpv6_hdr = (struct icmpv6_hdr_t*)soframe();
	if (framelen() > sizeof(struct icmpv6_hdr_t))
	{
		data = icmpv6_hdr->data;
		datalen = framelen() - sizeof(struct icmpv6_hdr_t);
	}
	else
	{
		data = NULL;
		datalen = 0;
	}
}


bool
ficmpv6frame::complete()
{
	initialize();

	if (framelen() < sizeof(struct icmpv6_hdr_t))
		return false;

	return true;
}


size_t
ficmpv6frame::need_bytes()
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct icmpv6_hdr_t))
		return (sizeof(struct icmpv6_hdr_t) - framelen());

	return 0; // just to make gcc happy
}


void
ficmpv6frame::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct icmpv6_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(icmpv6_hdr->data, data, datalen);
	this->datalen = datalen;
}


uint8_t*
ficmpv6frame::payload() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!data)
		throw eFrameNoPayload();
	return data;
}


size_t
ficmpv6frame::payloadlen() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!datalen)
		throw eFrameNoPayload();
	return datalen;
}


void
ficmpv6frame::validate(uint16_t total_len) throw (eICMPv6FrameTooShort)
{
	initialize();

	if (!complete())
	{
		throw eICMPv6FrameTooShort();
	}

	// TODO: verify checksum here?
}


const char*
ficmpv6frame::c_str()
{
	cvastring vas;

	info.assign(vas("[ficmpv6frame(%p) type[%d] code[%d] checksum[0x%x] %s]",
			this,
			icmpv6_hdr->type,
			icmpv6_hdr->code,
			be16toh(icmpv6_hdr->checksum),
			fframe::c_str() ));

	return info.c_str();
}



void
ficmpv6frame::icmpv6_calc_checksum()
{
	initialize();

	throw eNotImplemented();
#if 0
	icmp_hdr->checksum = htobe16(0x0000);

	// ICMPv4 header
	//

	// sum
	uint32_t sum = 0;

	// pointer on 16bit words
	uint16_t *word16 = (uint16_t*)icmp_hdr;

	// number of 16bit words
	int wnum = (sizeof(struct icmpv4_hdr_t) / sizeof(uint16_t));

	// header loop
	for (int i = 0; i < wnum; i++)
	{
		uint32_t tmp = (uint32_t)(be16toh(word16[i]));
		sum += tmp;
		//fprintf(stderr, "word16[%d]=0x%08x sum()=0x%08x\n", i, tmp, sum);
	}

	// TODO: checksum must also cover data portion of ICMP message!

	uint16_t res16 = (sum & 0x0000ffff) + ((sum & 0xffff0000) >> 16);

	//fprintf(stderr, " res16(1)=0x%x\n", res16);

	icmp_hdr->checksum = htobe16(~res16);

	//fprintf(stderr, "~res16(1)=0x%x\n", be16toh(udp_hdr->checksum));
#endif
}


uint8_t
ficmpv6frame::get_icmpv6_code()
{
	return icmpv6_hdr->code;
}


void
ficmpv6frame::set_icmpv6_code(uint8_t code)
{
	icmpv6_hdr->code = code;
}


uint8_t
ficmpv6frame::get_icmpv6_type()
{
	return icmpv6_hdr->type;
}


void
ficmpv6frame::set_icmpv6_type(uint8_t type)
{
	icmpv6_hdr->type = type;
}

