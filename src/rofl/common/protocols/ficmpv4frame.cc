/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ficmpv4frame.h"

using namespace rofl;

ficmpv4frame::ficmpv4frame(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		icmp_hdr(0),
		data(0),
		datalen(0)
{
	initialize();
}



ficmpv4frame::ficmpv4frame(
		size_t len) :
		fframe(len)
{
	initialize();
}



ficmpv4frame::~ficmpv4frame()
{

}



void
ficmpv4frame::initialize()
{
	icmp_hdr = (struct icmpv4_hdr_t*)soframe();
	if (framelen() > sizeof(struct icmpv4_hdr_t))
	{
		data = icmp_hdr->data;
		datalen = framelen() - sizeof(struct icmpv4_hdr_t);
	}
	else
	{
		data = NULL;
		datalen = 0;
	}
}


bool
ficmpv4frame::complete() const
{
	//initialize();

	if (framelen() < sizeof(struct icmpv4_hdr_t))
		return false;

	return true;
}


size_t
ficmpv4frame::need_bytes() const
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct icmpv4_hdr_t))
		return (sizeof(struct icmpv4_hdr_t) - framelen());

	return 0; // just to make gcc happy
}


void
ficmpv4frame::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct icmpv4_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(icmp_hdr->data, data, datalen);
	this->datalen = datalen;
}


uint8_t*
ficmpv4frame::payload() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!data)
		throw eFrameNoPayload();
	return data;
}


size_t
ficmpv4frame::payloadlen() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!datalen)
		throw eFrameNoPayload();
	return datalen;
}


void
ficmpv4frame::validate(uint16_t total_len) const
{
	//initialize();

	if (!complete())
	{
		throw eICMPv4FrameTooShort();
	}

	// TODO: verify checksum here?
}



void
ficmpv4frame::icmpv4_calc_checksum(uint16_t length)
{
	initialize();

	icmp_hdr->checksum = htobe16(0x0000);

	// ICMPv4 header
	//

	// sum
	uint32_t sum = 0;

	// pointer on 16bit words
	uint16_t *word16 = (uint16_t*)icmp_hdr;

	// number of 16bit words
	//int wnum = (sizeof(struct icmpv4_hdr_t) / sizeof(uint16_t));
    int wnum = (length / sizeof(uint16_t));
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
}


uint8_t
ficmpv4frame::get_icmp_code() const
{
	return icmp_hdr->code;
}


void
ficmpv4frame::set_icmp_code(uint8_t code)
{
	icmp_hdr->code = code;
}


uint8_t
ficmpv4frame::get_icmp_type() const
{
	return icmp_hdr->type;
}


void
ficmpv4frame::set_icmp_type(uint8_t type)
{
	icmp_hdr->type = type;
}

