/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


/* fsctpframe.cc
 *
 *  Created on: 07.12.2012
 *      Author: andreas
 */

#include <rofl/common/protocols/fsctpframe.h>

using namespace rofl;

fsctpframe::fsctpframe(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		sctp_hdr(0),
		data(0),
		datalen(0)
{
	initialize();
}



fsctpframe::~fsctpframe()
{

}



void
fsctpframe::initialize()
{
	sctp_hdr = (struct sctp_hdr_t*)soframe();
	if (framelen() > sizeof(struct sctp_hdr_t))
	{
		data = NULL;
		datalen = 0;
#if 0
		data = 1st data chunk in SCTP frame?
		datalen = 0;
#endif
	}
	else
	{
		data = NULL;
		datalen = 0;
	}
}


bool
fsctpframe::complete()
{
	initialize();

	if (framelen() < sizeof(struct sctp_hdr_t))
		return false;

	// TODO: check chunks

	return true;
}


size_t
fsctpframe::need_bytes()
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct sctp_hdr_t))
		return (sizeof(struct sctp_hdr_t) - framelen());

	// TODO: check chunks

	return 0; // just to make gcc happy
}


void
fsctpframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	initialize();

	// TODO: append SCTP DATA chunk
}


uint8_t*
fsctpframe::payload() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!data)
		throw eFrameNoPayload();
	return data;
}


size_t
fsctpframe::payloadlen() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!datalen)
		throw eFrameNoPayload();
	return datalen;
}


void
fsctpframe::validate(uint16_t total_len) throw (eSctpFrameTooShort)
{
	initialize();

	if (!complete())
	{
		throw eSctpFrameTooShort();
	}

	// TODO: verify checksum here?
}


const char*
fsctpframe::c_str()
{
	cvastring vas;

	info.assign(vas("[fsctpframe(%p) sport[%d] dport[%d] verification-tag[0x%x] checksum[0x%x] %s]",
			this,
			be16toh(sctp_hdr->sport),
			be16toh(sctp_hdr->dport),
			be32toh(sctp_hdr->verification_tag),
			be32toh(sctp_hdr->checksum),
			fframe::c_str() ));

	return info.c_str();
}


void
fsctpframe::sctp_calc_checksum(
		caddress const& ip_src,
		caddress const& ip_dst,
		uint8_t ip_proto,
		uint16_t length)
{
	initialize();

	sctp_hdr->checksum = htobe16(0x0000);

	// part -I- (IPv4 pseudo header)
	//

	// create IPv4 pseudo header for UDP checksum calculation
	struct ip_pseudo_hdr_t hdr;
	bzero(&hdr, sizeof(hdr));

	hdr.src 		= ip_src.ca_s4addr->sin_addr.s_addr;
	hdr.dst 		= ip_dst.ca_s4addr->sin_addr.s_addr;
	hdr.reserved 	= 0;
	hdr.proto 		= ip_proto;
	hdr.len 		= htobe16(datalen);

	// sum
	uint32_t sum = 0;

	// pointer on 16bit words
	uint16_t *word16 = (uint16_t*)&hdr;
	// number of 16bit words
	int wnum = 6;

	for (int i = 0; i < wnum; i++)
	{
		uint32_t tmp = (uint32_t)(be16toh(word16[i]));
		sum += tmp;
		//fprintf(stderr, "word16[%d]=0x%08x sum()=0x%08x\n", i, tmp, sum);
	}
	//fprintf(stderr, "   sum(1)=0x%x\n", sum);

	// part -II- (TCP header + payload)
	//

	// pointer on 16bit words
	word16 = (uint16_t*)sctp_hdr;
	// number of 16bit words
	wnum = (datalen / sizeof(uint16_t));

	for (int i = 0; i < wnum; i++)
	{
		uint32_t tmp = (uint32_t)(be16toh(word16[i]));
		sum += tmp;
		//fprintf(stderr, "word16[%d]=0x%08x sum()=0x%08x\n", i, tmp, sum);
	}

	uint16_t res16 = (sum & 0x0000ffff) + ((sum & 0xffff0000) >> 16);

	//fprintf(stderr, " res16(1)=0x%x\n", res16);

	sctp_hdr->checksum = htobe16(~res16);

	//fprintf(stderr, "~res16(1)=0x%x\n", be16toh(sctp_hdr->checksum));
}


uint16_t
fsctpframe::get_sport()
{
	return be16toh(sctp_hdr->sport);
}


void
fsctpframe::set_sport(uint16_t port)
{
	sctp_hdr->sport = htobe16(port);
}


uint16_t
fsctpframe::get_dport()
{
	return be16toh(sctp_hdr->dport);
}


void
fsctpframe::set_dport(uint16_t port)
{
	sctp_hdr->dport = htobe16(port);
}


