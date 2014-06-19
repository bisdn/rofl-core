/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fipv6frame.h"

using namespace rofl;

fipv6frame::fipv6frame(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		ipv6_hdr(0),
		ipv6data(0),
		ipv6datalen(0)
{
	if(data)
		initialize();
}



fipv6frame::~fipv6frame()
{

}



fipv6ext&
fipv6frame::get_ext_hdr(enum ipv6_ext_t type) throw (eIPv6FrameNotFound)
{
	if (ipv6exts.find(type) == ipv6exts.end()) {
		throw eIPv6FrameNotFound();
	}
	return ipv6exts[type];
}



void
fipv6frame::initialize() throw (eIPv6FrameInval)
{
	ipv6exts.clear();

	if (framelen() < sizeof(struct ipv6_hdr_t)) {
		throw eIPv6FrameInval();
	}

	ipv6_hdr = (struct ipv6_hdr_t*)soframe();

	if (get_version() != IPV6_VERSION) {
		return; // ok, no valid IP version number, so we assume, this header is empty, skip all parsing below
	}


	// loop variables
	uint8_t nxthdr = ipv6_hdr->nxthdr;
	struct fipv6ext::ipv6_ext_hdr_t *ipv6ext_hdr = (struct fipv6ext::ipv6_ext_hdr_t*)(ipv6_hdr + 1);	// next (potential) IPv6 extension header
	int reslen = framelen() - sizeof(struct ipv6_hdr_t); // remaining length without static IPv6 header

	while (true) {
		// any IPv6 extension headers?
		switch (nxthdr) {
		case IPPROTO_IPV6_NONXT: {
			// do nothing
		} return;
		case IPPROTO_IPV6_HOPOPT:
		case IPPROTO_IPV6_ROUTE:
		case IPPROTO_IPV6_FRAG:
		case IPPROTO_IPV6_OPTS:
		case IPPROTO_IPV6_MIPV6: {

			// sanity check 1: remaining length must be at least 8 bytes (=1 block) in each extension header
			if (reslen < 8) {
				throw eIPv6FrameInval();
			}

			// real length of header extension in bytes
			int extlen = 8 * (ipv6ext_hdr->len + 1);

			// sanity check 2: remaining length must be at least 8 bytes * (value + 1) from ipv6_ext_hdr->len
			if (reslen < extlen) {
				throw eIPv6FrameInval();
			}

			ipv6exts[(enum ipv6_ext_t)(nxthdr)] = fipv6ext(ipv6ext_hdr, extlen);

			// move forward
			nxthdr = ipv6ext_hdr->nxthdr;
			ipv6ext_hdr = (struct fipv6ext::ipv6_ext_hdr_t*)(((uint8_t*)ipv6ext_hdr) + extlen);
			reslen -= extlen;

		} break;
		default: {
			ipv6data = (uint8_t*)ipv6ext_hdr;
			ipv6datalen = reslen;
		} return;
		}
	}
}



bool
fipv6frame::complete()
{
	if (framelen() < sizeof(struct ipv6_hdr_t))
		return false;

	if (framelen() < (sizeof(struct ipv6_hdr_t) + get_payload_length()))
		return false;

	return true;
}



size_t
fipv6frame::need_bytes()
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct ipv6_hdr_t))
		return (sizeof(struct ipv6_hdr_t) - framelen());

	if (framelen() < (sizeof(struct ipv6_hdr_t) + get_payload_length()))
		return (sizeof(struct ipv6_hdr_t) + get_payload_length() - framelen());

	return 0; // just to make gcc happy
}




void
fipv6frame::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	throw eNotImplemented();
}


uint8_t*
fipv6frame::payload() const throw (eFrameNoPayload)
{
	if (!ipv6datalen)
		throw eFrameNoPayload();
	return ipv6data;
}


size_t
fipv6frame::payloadlen() const throw (eFrameNoPayload)
{
	if (!ipv6datalen)
		throw eFrameNoPayload();
	return ipv6datalen;
}



void
fipv6frame::validate(uint16_t total_len) throw (eFrameInvalidSyntax)
{
	if (!complete())
		throw eFrameInvalidSyntax();

	// FIXME: verify ipv4 checksum?
}



void
fipv6frame::ipv6_calc_checksum()
{
	initialize();

	throw eNotImplemented();
#if 0
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
#endif
}



void
fipv6frame::set_version(uint8_t version)
{
	ipv6_hdr->bytes[0] = (ipv6_hdr->bytes[0] & 0x0F) + ((version & 0x0F) << 4);
}



uint8_t
fipv6frame::get_version()
{
	return (uint8_t)((ipv6_hdr->bytes[0] & 0xF0) >> 4);
}



void
fipv6frame::set_traffic_class(uint8_t tc)
{
	ipv6_hdr->bytes[0] = (ipv6_hdr->bytes[0] & 0xF0) + ((tc & 0xF0) >> 4);
	ipv6_hdr->bytes[1] = (ipv6_hdr->bytes[1] & 0x0F) + ((tc & 0x0F) << 4);
}

void
fipv6frame::set_dscp(uint8_t dscp)
{
	ipv6_hdr->bytes[0] = (ipv6_hdr->bytes[0] & 0xF0) + ((dscp & 0x3C) >> 2);
	ipv6_hdr->bytes[1] = (ipv6_hdr->bytes[1] & 0x3F) + ((dscp & 0x03) << 6);
}

void
fipv6frame::set_ecn(uint8_t ecn)
{
	ipv6_hdr->bytes[1] = (ipv6_hdr->bytes[1] & 0xCF) + ((ecn & 0x03) << 4);
}

uint8_t
fipv6frame::get_traffic_class() const
{
	return (uint8_t)(((ipv6_hdr->bytes[0] & 0x0F) << 4) + ((ipv6_hdr->bytes[1] & 0xF0) >> 4));
}

uint8_t
fipv6frame::get_dscp() const
{
	return (uint8_t)(((ipv6_hdr->bytes[0] & 0x0F) << 2) + ((ipv6_hdr->bytes[1] & 0xC0) >> 6));
}

uint8_t
fipv6frame::get_ecn() const
{
	return (uint8_t)((ipv6_hdr->bytes[1] & 0x30) >> 4);
}

void
fipv6frame::set_flow_label(uint32_t flabel)
{
	ipv6_hdr->bytes[1] = (ipv6_hdr->bytes[1] & 0xF0) + ((flabel & 0x000f0000) >> 16);
	ipv6_hdr->bytes[2] = (flabel & 0x0000ff00) >> 8;
	ipv6_hdr->bytes[3] = (flabel & 0x000000ff) >> 0;
}



uint32_t
fipv6frame::get_flow_label() const
{
	return (uint32_t)(((ipv6_hdr->bytes[1] & 0x0F) << 16) + (ipv6_hdr->bytes[2] << 8) + (ipv6_hdr->bytes[3] << 0));
}



void
fipv6frame::set_payload_length(uint16_t len)
{
	ipv6_hdr->payloadlen = htobe16(len);
}



uint16_t
fipv6frame::get_payload_length()
{
	return be16toh(ipv6_hdr->payloadlen);
}



void
fipv6frame::set_next_header(uint8_t nxthdr)
{
	ipv6_hdr->nxthdr = nxthdr;
}



uint8_t
fipv6frame::get_next_header()
{
	return ipv6_hdr->nxthdr;
}



void
fipv6frame::set_hop_limit(uint8_t hops)
{
	ipv6_hdr->hoplimit = hops;
}


uint8_t
fipv6frame::get_hop_limit()
{
	return ipv6_hdr->hoplimit;
}

void
fipv6frame::dec_hop_limit()
{
	ipv6_hdr->hoplimit--;
}

void
fipv6frame::set_ipv6_src(uint8_t *somem, size_t memlen) throw (eIPv6FrameInval)
{
	if (memlen < IPV6_ADDR_LEN) {
		throw eIPv6FrameInval();
	}
	memcpy(ipv6_hdr->src, somem, IPV6_ADDR_LEN);
	//Convert to network byte order 
	HTONB128(ipv6_hdr->src);
}



void
fipv6frame::set_ipv6_src(cmemory const& src) throw (eIPv6FrameInval)
{
	if (src.memlen() < IPV6_ADDR_LEN) {
		throw eIPv6FrameInval();
	}
	memcpy(ipv6_hdr->src, src.somem(), IPV6_ADDR_LEN);
	//Convert to network byte order 
	HTONB128(ipv6_hdr->src);
}



void
fipv6frame::set_ipv6_src(caddress_in6 const& src)
{
	memcpy(ipv6_hdr->src, src.somem(), IPV6_ADDR_LEN);
}



caddress_in6
fipv6frame::get_ipv6_src() const
{
	caddress_in6 src;
	memcpy(src.somem(), ipv6_hdr->src, IPV6_ADDR_LEN);
	return src;
}



void
fipv6frame::set_ipv6_dst(uint8_t *somem, size_t memlen) throw (eIPv6FrameInval)
{
	if (memlen < IPV6_ADDR_LEN) {
		throw eIPv6FrameInval();
	}
	memcpy(ipv6_hdr->dst, somem, IPV6_ADDR_LEN);
	//Convert to network byte order 
	HTONB128(ipv6_hdr->dst);
}



void
fipv6frame::set_ipv6_dst(cmemory const& dst) throw (eIPv6FrameInval)
{
	if (dst.memlen() < IPV6_ADDR_LEN) {
		throw eIPv6FrameInval();
	}
	memcpy(ipv6_hdr->dst, dst.somem(), IPV6_ADDR_LEN);
	//Convert to network byte order 
	HTONB128(ipv6_hdr->dst);
}



void
fipv6frame::set_ipv6_dst(caddress_in6 const& dst)
{
	memcpy(ipv6_hdr->dst, dst.somem(), IPV6_ADDR_LEN);
}



caddress_in6
fipv6frame::get_ipv6_dst() const
{
	caddress_in6 dst;
	memcpy(dst.somem(), ipv6_hdr->dst, IPV6_ADDR_LEN);
	return dst;
}









