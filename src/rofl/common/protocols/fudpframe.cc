/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fudpframe.h"

fudpframe::fudpframe(
		uint8_t* _data,
		size_t _datalen,
		uint16_t _totallen,
		fframe* _predecessor) :
		fframe(_data, _datalen, _totallen, _predecessor),
		udp_hdr(NULL),
		data(NULL),
		datalen(0)
{
	initialize();
}


fudpframe::~fudpframe()
{

}


void
fudpframe::initialize()
{
	udp_hdr = (struct udp_hdr_t*)soframe();
	if (framelen() > sizeof(struct udp_hdr_t))
	{
		data = udp_hdr->data;
		datalen = framelen() - sizeof(struct udp_hdr_t);
	}
	else
	{
		data = NULL;
		datalen = 0;
	}
}


bool
fudpframe::complete()
{
	initialize();

	if (framelen() < sizeof(struct udp_hdr_t))
		return false;

	if (framelen() < be16toh(udp_hdr->length))
		return false;

	return true;
}


size_t
fudpframe::need_bytes()
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct udp_hdr_t))
		return (sizeof(struct udp_hdr_t) - framelen());

	if (framelen() < be16toh(udp_hdr->length))
		return (be16toh(udp_hdr->length) - framelen());

	return 0; // just to make gcc happy
}


void
fudpframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct udp_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(udp_hdr->data, data, datalen);
	this->datalen = datalen;
}


uint8_t*
fudpframe::payload() throw (eFrameNoPayload)
{
	initialize();
	if (!data)
		throw eFrameNoPayload();
	return data;
}


size_t
fudpframe::payloadlen() throw (eFrameNoPayload)
{
	initialize();
	if (!datalen)
		throw eFrameNoPayload();
	return datalen;
}


uint16_t
fudpframe::totalpayloadlen() throw (eFrameNoPayload)
{
	if (totallen() < sizeof(struct fudpframe::udp_hdr_t))
	{
		throw eFrameNoPayload();
	}
	return (totallen() - sizeof(struct fudpframe::udp_hdr_t));
}


void
fudpframe::validate() throw (eFrameInvalidSyntax)
{
	initialize();

	if (!complete())
		throw eFrameInvalidSyntax();

	// TODO: verify checksum here?
}


const char*
fudpframe::c_str()
{
	cvastring vas;

	info.assign(vas("[fudpframe(%p) sport[%d] dport[%d] length[%d] checksum[0x%x] %s]",
			this,
			be16toh(udp_hdr->sport),
			be16toh(udp_hdr->dport),
			be16toh(udp_hdr->length),
			be16toh(udp_hdr->checksum),
			fframe::c_str() ));

	return info.c_str();
}



void
fudpframe::udp_calc_checksum(
		caddress const& ip_src,
		caddress const& ip_dst,
		uint8_t ip_proto,
		uint16_t length)
{
	initialize();

	caddress src(ip_src);
	caddress dst(ip_dst);

	WRITELOG(CPACKET, DBG, "udp(%p)::udp_calc_checksum() =>\n"
			"\tip-src      : %s\n"
			"\tip-dst      : %s\n"
			"\tip-proto    : %d\n"
			"\tudp-length  : %d\n",
			this,
			src.c_str(),
			dst.c_str(),
			ip_proto,
			length);

	udp_hdr->checksum = htobe16(0x0000);

	// part -I- (IPv4 pseudo header)
	//

	// create IPv4 pseudo header for UDP checksum calculation
	struct ip_pseudo_hdr_t hdr;
	bzero(&hdr, sizeof(hdr));

	hdr.src 		= ip_src.s4addr->sin_addr.s_addr;
	hdr.dst 		= ip_dst.s4addr->sin_addr.s_addr;
	hdr.reserved 	= 0;
	hdr.proto 		= ip_proto;
	hdr.len 		= htobe16(length);

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

	// part -II- (UDP header + payload)
	//

	// pointer on 16bit words
	word16 = (uint16_t*)udp_hdr;
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

	udp_hdr->checksum = htobe16(~res16);

	//fprintf(stderr, "~res16(1)=0x%x\n", be16toh(udp_hdr->checksum));
}


uint16_t
fudpframe::get_sport()
{
	return be16toh(udp_hdr->sport);
}


void
fudpframe::set_sport(uint16_t port)
{
	udp_hdr->sport = htobe16(port);
}


uint16_t
fudpframe::get_dport()
{
	return be16toh(udp_hdr->dport);
}


void
fudpframe::set_dport(uint16_t port)
{
	udp_hdr->dport = htobe16(port);
}


uint16_t
fudpframe::get_length()
{
		return be16toh(udp_hdr->length);
}


void
fudpframe::set_length(uint16_t length)
{
	udp_hdr->length = htobe16(length);
}

