/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ftcpframe.h"


ftcpframe::ftcpframe(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		tcp_hdr(0),
		data(0),
		datalen(0)
{
	initialize();
}



ftcpframe::~ftcpframe()
{

}



void
ftcpframe::initialize()
{
	tcp_hdr = (struct tcp_hdr_t*)soframe();
	if (framelen() > sizeof(struct tcp_hdr_t))
	{
		data = soframe() + (tcp_hdr->offset * sizeof(uint32_t));
		datalen = framelen() - (tcp_hdr->offset * sizeof(uint32_t));
	}
	else
	{
		data = NULL;
		datalen = 0;
	}
}


bool
ftcpframe::complete()
{
	initialize();

	if (framelen() < sizeof(struct tcp_hdr_t))
		return false;

	if (framelen() < (tcp_hdr->offset * sizeof(uint32_t)))
		return false;

	return true;
}


size_t
ftcpframe::need_bytes()
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct tcp_hdr_t))
		return (sizeof(struct tcp_hdr_t) - framelen());

	if (framelen() < (tcp_hdr->offset * sizeof(uint32_t)))
		return ((tcp_hdr->offset * sizeof(uint32_t)) - framelen());

	return 0; // just to make gcc happy
}


void
ftcpframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	initialize();

	if (datalen > (framelen() - (tcp_hdr->offset * sizeof(uint32_t))))
	{
		throw eFrameOutOfRange();
	}
	memcpy(this->data, data, datalen);
	this->datalen = datalen;
}


uint8_t*
ftcpframe::payload() throw (eFrameNoPayload)
{
	initialize();
	if (!data)
		throw eFrameNoPayload();
	return data;
}


size_t
ftcpframe::payloadlen() throw (eFrameNoPayload)
{
	initialize();
	if (!datalen)
		throw eFrameNoPayload();
	return datalen;
}


void
ftcpframe::validate() throw (eFrameInvalidSyntax)
{
	initialize();

	if (!complete())
		throw eFrameInvalidSyntax();

	// TODO: verify checksum here?
}


const char*
ftcpframe::c_str()
{
	cvastring vas;

	info.assign(vas("[ftcpframe(%p) sport[%d] dport[%d] seqno[0x%x] ack[0x%x] header-length[%d] checksum[0x%x] %s]",
			this,
			be16toh(tcp_hdr->sport),
			be16toh(tcp_hdr->dport),
			be32toh(tcp_hdr->seqno),
			be32toh(tcp_hdr->ackno),
			tcp_hdr->offset * sizeof(uint32_t),
			be16toh(tcp_hdr->checksum),
			fframe::c_str() ));

	return info.c_str();
}


void
ftcpframe::tcp_calc_checksum(
		caddress const& ip_src,
		caddress const& ip_dst,
		uint8_t ip_proto,
		uint16_t length)
{
	initialize();

	tcp_hdr->checksum = htobe16(0x0000);

	// part -I- (IPv4 pseudo header)
	//

	// create IPv4 pseudo header for UDP checksum calculation
	struct ip_pseudo_hdr_t hdr;
	bzero(&hdr, sizeof(hdr));

	hdr.src 		= ip_src.s4addr->sin_addr.s_addr;
	hdr.dst 		= ip_dst.s4addr->sin_addr.s_addr;
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
	word16 = (uint16_t*)tcp_hdr;
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

	tcp_hdr->checksum = htobe16(~res16);

	//fprintf(stderr, "~res16(1)=0x%x\n", be16toh(tcp_hdr->checksum));
}


uint16_t
ftcpframe::get_sport()
{
	return be16toh(tcp_hdr->sport);
}


void
ftcpframe::set_sport(uint16_t port)
{
	tcp_hdr->sport = htobe16(port);
}


uint16_t
ftcpframe::get_dport()
{
	return be16toh(tcp_hdr->dport);
}


void
ftcpframe::set_dport(uint16_t port)
{
	tcp_hdr->dport = htobe16(port);
}


