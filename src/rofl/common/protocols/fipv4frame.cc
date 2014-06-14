/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fipv4frame.h"

using namespace rofl;

fipv4frame::fipv4frame(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		ipv4_hdr(0),
		ipv4data(0),
		ipv4datalen(0)
{
	if(data)
		initialize();
}



fipv4frame::~fipv4frame()
{

}



void
fipv4frame::initialize()
{
	ipv4_hdr = (struct ipv4_hdr_t*)soframe();

	//validate();

	if (get_ipv4_version() == 4) // non-empty frame
	{
		if ((be16toh(ipv4_hdr->length) > sizeof(struct ipv4_hdr_t)) &&
				((get_ipv4_ihl() * sizeof(uint32_t)) < framelen()) )
		{
			ipv4data = ((uint8_t*)soframe()) + get_ipv4_ihl() * sizeof(uint32_t);
			ipv4datalen = framelen() - get_ipv4_ihl() * sizeof(uint32_t);
		}
		else
		{
			ipv4data = NULL;
			ipv4datalen = 0;
		}
	}
	else  // frame is empty, payloadlen is overall length - standard ipv4 header
	{
		ipv4data = ipv4_hdr->data;
		ipv4datalen = framelen() - (5 * sizeof(uint32_t));
	}
}


bool
fipv4frame::complete()
{
	if (framelen() < sizeof(struct ipv4_hdr_t))
		return false;

	if (framelen() < (get_ipv4_ihl() * sizeof(uint32_t)))
		return false;

	return true;
}


size_t
fipv4frame::need_bytes()
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct ipv4_hdr_t))
		return (sizeof(struct ipv4_hdr_t) - framelen());

	if (framelen() < be16toh(ipv4_hdr->length))
		return (be16toh(ipv4_hdr->length) - framelen());

	return 0; // just to make gcc happy
}




void
fipv4frame::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct ipv4_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(ipv4data, data, datalen);
	ipv4datalen = datalen;
}


uint8_t*
fipv4frame::payload() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!ipv4data)
		throw eFrameNoPayload();
	return ipv4data;
}


size_t
fipv4frame::payloadlen() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!ipv4datalen)
		throw eFrameNoPayload();
	return ipv4datalen;
}



void
fipv4frame::validate(uint16_t total_len) throw (eFrameInvalidSyntax)
{
	if (!complete())
		throw eFrameInvalidSyntax();

	// FIXME: verify ipv4 checksum?
}


const char*
fipv4frame::c_str()
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
#endif

	std::stringstream s_dst; s_dst << get_ipv4_dst();
	std::stringstream s_src; s_src << get_ipv4_src();

	info.assign(vas("[fipv4frame(%p) dst:%s src:%s length:%d vers:%d dscp:%d ecn:%d ihl:%d proto:%d ttl:%d ]",
			this,
			s_dst.str().c_str(),
			s_src.str().c_str(),
			be16toh(ipv4_hdr->length),
			get_ipv4_version(),
			get_ipv4_dscp(),
			get_ipv4_ecn(),
			get_ipv4_ihl(),
			get_ipv4_proto(),
			get_ipv4_ttl()
			));

	return info.c_str();
}


void
fipv4frame::ipv4_calc_checksum()
{
	initialize();

	size_t datalen = sizeof(struct ipv4_hdr_t);

	// force header checksum to 0x0000
	ipv4_hdr->checksum = htobe16(0x0000);

	// pointer on 16bit words
	uint16_t *word16 = (uint16_t*)ipv4_hdr;
	// number of 16bit words
	int wnum = (datalen / (sizeof(uint16_t)));
	// sum
	uint32_t sum = 0;

	for (int i = 0; i < wnum; i++)
	{
		uint32_t tmp = (uint32_t)(be16toh(word16[i]));
		sum += tmp;
		//fprintf(stderr, "word16[%d]=0x%08x sum()=0x%08x\n", i, tmp, sum);
	}
	//fprintf(stderr, "   sum(1)=0x%x\n", sum);

	uint16_t res16 = (sum & 0x0000ffff) + ((sum & 0xffff0000) >> 16);

	//fprintf(stderr, " res16(1)=0x%x\n", res16);

	ipv4_hdr->checksum = htobe16(~res16);

	//fprintf(stderr, "~res16(1)=0x%x\n", be16toh(ipv4_hdr->checksum));
}


void
fipv4frame::set_ipv4_src(uint32_t src)
{
	ipv4_hdr->src = htobe32(src);
}


void
fipv4frame::set_ipv4_src(const caddress_in4& src)
{
	ipv4_hdr->src = src.get_addr_nbo();
}


caddress_in4
fipv4frame::get_ipv4_src() const
{
	caddress_in4 src; src.set_addr_nbo(ipv4_hdr->src);
	return src;
}


void
fipv4frame::set_ipv4_dst(uint32_t dst)
{
	ipv4_hdr->dst = htobe32(dst);
}


void
fipv4frame::set_ipv4_dst(const caddress_in4 & dst)
{
	ipv4_hdr->dst = dst.get_addr_nbo();
}


caddress_in4
fipv4frame::get_ipv4_dst() const
{
	caddress_in4 dst; dst.set_addr_nbo(ipv4_hdr->dst);
	return dst;
}

void
fipv4frame::set_ipv4_dscp(uint8_t dscp)
{
	ipv4_hdr->tos = ((dscp & 0x3f) << 2) | (ipv4_hdr->tos & 0x03);
}


uint8_t
fipv4frame::get_ipv4_dscp() const
{
	return (ipv4_hdr->tos >> 2);
}


void
fipv4frame::set_ipv4_ecn(uint8_t ecn)
{
	ipv4_hdr->tos = (ipv4_hdr->tos & 0xfc) | (ecn & 0x03);
}


uint8_t
fipv4frame::get_ipv4_ecn() const
{
	return (ipv4_hdr->tos & 0x03);
}




void
fipv4frame::set_ipv4_ttl(uint8_t ttl)
{
	ipv4_hdr->ttl = ttl;
}



uint8_t
fipv4frame::get_ipv4_ttl()
{
	return ipv4_hdr->ttl;
}


void
fipv4frame::dec_ipv4_ttl()
{
	ipv4_hdr->ttl--;
}


void
fipv4frame::set_ipv4_proto(uint8_t proto)
{
	ipv4_hdr->proto = proto;
}


void
fipv4frame::set_ipv4_ihl(uint8_t ihl)
{
	ipv4_hdr->ihlvers = (ihl & 0x0f) + (ipv4_hdr->ihlvers & 0xf0);
}


uint8_t
fipv4frame::get_ipv4_ihl()
{
	return ((ipv4_hdr->ihlvers & 0x0f));
}


void
fipv4frame::set_ipv4_version(uint8_t version)
{
	ipv4_hdr->ihlvers = (ipv4_hdr->ihlvers & 0x0f) + ((version & 0x0f) << 4);
}


uint8_t
fipv4frame::get_ipv4_version()
{
	return ((ipv4_hdr->ihlvers & 0xf0) >> 4);
}


uint8_t
fipv4frame::get_ipv4_proto()
{
	return ipv4_hdr->proto;
}


uint16_t
fipv4frame::get_ipv4_length()
{
    return be16toh(ipv4_hdr->length);
}


void
fipv4frame::set_ipv4_length(uint16_t length)
{
	ipv4_hdr->length = htobe16(length);
	initialize();
}


bool
fipv4frame::has_DF_bit_set()
{
	return (bool)((be16toh(ipv4_hdr->offset_flags) >> 13)  & bit_dont_fragment);
}


void
fipv4frame::set_DF_bit()
{
	ipv4_hdr->offset_flags = htobe16( be16toh(ipv4_hdr->offset_flags) | (bit_dont_fragment << 13) );
}


void
fipv4frame::clear_DF_bit()
{
	ipv4_hdr->offset_flags = htobe16( be16toh(ipv4_hdr->offset_flags) & ~(bit_dont_fragment << 13) );
}


bool
fipv4frame::has_MF_bit_set()
{
	return (bool)((be16toh(ipv4_hdr->offset_flags) >> 13)  & bit_more_fragments);
}


void
fipv4frame::set_MF_bit()
{
	ipv4_hdr->offset_flags = htobe16( be16toh(ipv4_hdr->offset_flags) | (bit_more_fragments << 13) );
}


void
fipv4frame::clear_MF_bit()
{
	ipv4_hdr->offset_flags = htobe16( be16toh(ipv4_hdr->offset_flags) & ~(bit_more_fragments << 13) );
}
