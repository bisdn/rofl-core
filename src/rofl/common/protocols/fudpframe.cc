/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fudpframe.h"

using namespace rofl;

fudpframe::fudpframe(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		udp_hdr(0),
		data(0),
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
fudpframe::payload() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!data)
		throw eFrameNoPayload();
	return data;
}


size_t
fudpframe::payloadlen() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!datalen)
		throw eFrameNoPayload();
	return datalen;
}





void
fudpframe::validate(uint16_t total_len) throw (eUdpFrameTooShort)
{
	initialize();

	if (!complete())
	{
		throw eUdpFrameTooShort();
	}

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
	int wnum;
	uint32_t sum = 0; //sum
	uint16_t *word16;
	uint16_t res16;
	
	initialize();

	//Set 0 to checksum
	udp_hdr->checksum = 0x0;

	/*
	* part -I- (IPv4 pseudo header)
	*/

	word16 = (uint16_t*)(void*)&ip_src.ca_s4addr->sin_addr.s_addr;
	sum += be16toh(*(word16+1));
	sum += be16toh(*(word16));

	word16 = (uint16_t*)(void*)&ip_dst.ca_s4addr->sin_addr.s_addr;
	sum += be16toh(*(word16+1));
	sum += be16toh(*(word16));
	sum += ip_proto;
	
	sum += length; 

	/*
	* part -II- (UDP header + payload)
	*/
	
	// pointer on 16bit words
	// number of 16bit words
	word16 = (uint16_t*)udp_hdr;
	wnum = (length/*datalen*/ / sizeof(uint16_t));

	for (int i = 0; i < wnum; i++){
		sum += (uint32_t)(be16toh(word16[i]));
	}

	res16 = (sum & 0x0000ffff) + ((sum & 0xffff0000) >> 16);

	udp_hdr->checksum = htobe16(~res16);

//	fprintf(stderr," %x \n", udp_hdr->checksum);
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

